import numpy as np

from config import (
    ACTION_ATTACK,
    ACTION_COUNT,
    ACTION_FORM_LINE,
    ACTION_HOLD,
    ACTION_KEEP,
    ACTION_MOVE,
    ACTION_SCREEN,
    APPROACH_COUNT,
    APPROACH_FRONT,
    APPROACH_LEFT,
    APPROACH_REAR,
    APPROACH_RIGHT,
    DEST_GRID_X,
    DEST_GRID_Y,
    GENERAL_LEAD,
    MAX_BATTALIONS,
    ORDER_ATTACK,
    ORDER_HOLD,
    ORDER_MOVE,
    PACE_RUN,
    PACE_WALK,
    TYPE_CAVALRY,
)
from geometry import facing_vectors, finite_positions, footprint, left_of, present_of, sector

PACE_VALUES = np.array([PACE_WALK, PACE_RUN], dtype=np.int32)
ORDER_FEATURE_DIM = ACTION_COUNT + APPROACH_COUNT + 2

ARRIVE_RADIUS = 50.0 # general this close to his destination has arrived
LINE_GAP = 60.0 # space between neighbouring battalions in a line
LINE_SECOND_RANK_GAP = 80.0 # archers stand this far behind the line's back rank
FLANK_MARGIN = 150.0 # how wide of the target a flanking battalion swings
FLANK_GIVE_UP_STEPS = 160 # 40 s: a flank that has not got round attacks straight in
SCREEN_GAP = 60.0 # space between a screen and the battalion it covers
SCREEN_INTERCEPT = 250.0 # a screen charges threats that come this close to it
FRONT_COS = 0.5 # same sectors as flank.c
REAR_COS = -0.5
FACE_DISTANCE = 1000.0 # hold destinations sit this far ahead, only their direction matters


def _unit(dx, dy, fallback_x=1.0, fallback_y=0.0):
    length = np.hypot(dx, dy)
    if length < 1e-6:
        return fallback_x, fallback_y
    return dx / length, dy / length


class OrderBook:
    """Macro orders of one army, carried out every C step until the network replaces them."""

    def __init__(self):
        self.kind = np.full(MAX_BATTALIONS, ACTION_HOLD, dtype=np.int64)
        self.pace = np.zeros(MAX_BATTALIONS, dtype=np.int64)
        self.target = np.zeros(MAX_BATTALIONS, dtype=np.int64)
        self.approach = np.zeros(MAX_BATTALIONS, dtype=np.int64)
        self.friend = np.zeros(MAX_BATTALIONS, dtype=np.int64)
        self.dest_x = np.zeros(MAX_BATTALIONS, dtype=np.float64)
        self.dest_y = np.zeros(MAX_BATTALIONS, dtype=np.float64)
        self.age = np.zeros(MAX_BATTALIONS, dtype=np.int64)
        self.committed = np.zeros(MAX_BATTALIONS, dtype=bool)

    def apply(self, actions, info, mirror, mask):
        """actions use the network's frame: mirrored x for side 1, so cells and left/right flip back here."""
        for i in range(info.battalion_count):
            kind = int(actions["type"][i])
            if not mask[i] or kind == ACTION_KEEP:
                continue
            approach = int(actions["approach"][i])
            if mirror and approach in (APPROACH_LEFT, APPROACH_RIGHT):
                approach = APPROACH_RIGHT if approach == APPROACH_LEFT else APPROACH_LEFT
            dest_x, dest_y = self.dest_x[i], self.dest_y[i]
            if kind == ACTION_MOVE:
                cell = int(actions["dest"][i])
                u = ((cell % DEST_GRID_X) + 0.5) / DEST_GRID_X
                v = ((cell // DEST_GRID_X) + 0.5) / DEST_GRID_Y
                dest_x = info.left + u * info.width
                if mirror:
                    dest_x = info.left + info.right - dest_x
                dest_y = info.top + v * info.height
            if self._is_same_order(i, kind, actions, approach, dest_x, dest_y):
                self.pace[i] = int(actions["pace"][i])
                continue
            self.kind[i] = kind
            self.pace[i] = int(actions["pace"][i])
            self.target[i] = int(actions["target"][i])
            self.friend[i] = int(actions["screen"][i])
            self.approach[i] = approach
            self.dest_x[i] = dest_x
            self.dest_y[i] = dest_y
            self.age[i] = 0
            self.committed[i] = False

    def _is_same_order(self, i, kind, actions, approach, dest_x, dest_y):
        if kind != self.kind[i]:
            return False
        if kind == ACTION_ATTACK:
            return int(actions["target"][i]) == self.target[i] and approach == self.approach[i]
        if kind == ACTION_SCREEN:
            return int(actions["screen"][i]) == self.friend[i]
        if kind == ACTION_MOVE:
            return dest_x == self.dest_x[i] and dest_y == self.dest_y[i]
        return True

    def features(self, count):
        """What each battalion is currently doing, for the observation; KEEP only makes sense with it."""
        out = np.zeros((MAX_BATTALIONS, ORDER_FEATURE_DIM), dtype=np.float32)
        rows = np.arange(count)
        out[rows, self.kind[:count]] = 1.0
        out[rows, ACTION_COUNT + self.approach[:count]] = (self.kind[:count] == ACTION_ATTACK)
        out[rows, ACTION_COUNT + APPROACH_COUNT] = np.minimum(self.age[:count] / 240.0, 1.0)
        out[rows, ACTION_COUNT + APPROACH_COUNT + 1] = self.committed[:count]
        return out

    def step(self, views, own, opponent, info):
        count = info.battalion_count
        x, y = finite_positions(own, info)
        ex, ey = finite_positions(opponent, info)
        present = present_of(own, count)
        enemy_present = present_of(opponent, count)
        half_width, half_depth, _ = footprint(own)
        enemy_half_width, enemy_half_depth, _ = footprint(opponent)
        fx, fy = facing_vectors(own["facing"])
        lead = GENERAL_LEAD + half_depth

        order_type = np.full(MAX_BATTALIONS, ORDER_HOLD, dtype=np.int32)
        pace = np.full(MAX_BATTALIONS, PACE_WALK, dtype=np.int32)
        target = np.zeros(MAX_BATTALIONS, dtype=np.uint32)
        dest_x = x.astype(np.float32)
        dest_y = y.astype(np.float32)

        def nearest_enemy(px, py):
            if not enemy_present.any():
                return -1
            distance = np.where(enemy_present, np.hypot(ex - px, ey - py), np.inf)
            return int(distance.argmin())

        def hold(i, face_x, face_y):
            order_type[i] = ORDER_HOLD
            dest_x[i] = x[i] + face_x * FACE_DISTANCE
            dest_y[i] = y[i] + face_y * FACE_DISTANCE

        def hold_facing_enemy(i):
            j = nearest_enemy(x[i], y[i])
            if j < 0:
                hold(i, fx[i], fy[i])
                return
            hold(i, *_unit(ex[j] - x[i], ey[j] - y[i], fx[i], fy[i]))

        def general_has_arrived(i, general_x, general_y):
            return np.hypot(x[i] + fx[i] * lead[i] - general_x, y[i] + fy[i] * lead[i] - general_y) < ARRIVE_RADIUS

        def take_position(i, slot_x, slot_y, face_x, face_y):
            general_x = slot_x + face_x * lead[i]
            general_y = slot_y + face_y * lead[i]
            if general_has_arrived(i, general_x, general_y) or np.hypot(x[i] - slot_x, y[i] - slot_y) < ARRIVE_RADIUS:
                hold(i, face_x, face_y)
                return
            order_type[i] = ORDER_MOVE
            dest_x[i] = general_x
            dest_y[i] = general_y

        def attack(i, j):
            order_type[i] = ORDER_ATTACK
            target[i] = j
            dest_x[i] = ex[j]
            dest_y[i] = ey[j]

        line = self._line_slots(own, x, y, ex, ey, enemy_present, present, half_width, half_depth, count)

        for i in range(count):
            if not present[i]:
                continue
            self.age[i] += 1
            pace[i] = PACE_VALUES[self.pace[i]]
            kind = self.kind[i]

            if kind == ACTION_MOVE:
                if general_has_arrived(i, self.dest_x[i], self.dest_y[i]):
                    hold_facing_enemy(i)
                else:
                    order_type[i] = ORDER_MOVE
                    dest_x[i] = self.dest_x[i]
                    dest_y[i] = self.dest_y[i]

            elif kind == ACTION_ATTACK:
                j = int(self.target[i])
                if j >= count or not enemy_present[j]:
                    self.kind[i] = ACTION_HOLD
                    hold_facing_enemy(i)
                    continue
                if not self.committed[i] and self._flank_waypoint(i, j, x, y, ex, ey, opponent, enemy_half_width, enemy_half_depth, half_depth, order_type, dest_x, dest_y):
                    continue
                self.committed[i] = True
                attack(i, j)

            elif kind == ACTION_FORM_LINE and i in line:
                take_position(i, *line[i])

            elif kind == ACTION_SCREEN:
                j = int(self.friend[i])
                if j == i or j >= count or not present[j]:
                    self.kind[i] = ACTION_HOLD
                    hold_facing_enemy(i)
                    continue
                threat = nearest_enemy(x[j], y[j])
                if threat < 0:
                    hold(i, fx[i], fy[i])
                    continue
                if np.hypot(ex[threat] - x[i], ey[threat] - y[i]) < SCREEN_INTERCEPT + half_depth[i] + enemy_half_depth[threat]:
                    attack(i, threat)
                    continue
                face_x, face_y = _unit(ex[threat] - x[j], ey[threat] - y[j])
                gap = half_depth[j] + half_depth[i] + SCREEN_GAP
                take_position(i, x[j] + face_x * gap, y[j] + face_y * gap, face_x, face_y)

            else:
                hold_facing_enemy(i)

        views["type"][:] = order_type
        views["pace"][:] = pace
        views["target"][:] = target
        views["dest_x"][:] = dest_x
        views["dest_y"][:] = dest_y

    def _flank_waypoint(self, i, j, x, y, ex, ey, opponent, enemy_half_width, enemy_half_depth, own_half_depth, order_type, dest_x, dest_y):
        """Steers i round to the requested side of j; returns False once it is there and should charge."""
        approach = int(self.approach[i])
        if approach == APPROACH_FRONT or self.age[i] > FLANK_GIVE_UP_STEPS:
            return False
        facing = opponent["facing"][j]
        cos, side = sector(x[i], y[i], ex[j], ey[j], facing, enemy_half_width[j], enemy_half_depth[j])
        wanted_side = 1.0 if approach == APPROACH_LEFT else -1.0
        if approach == APPROACH_REAR and cos <= REAR_COS:
            return False
        if approach != APPROACH_REAR and FRONT_COS > cos > REAR_COS and side == wanted_side:
            return False

        tfx, tfy = facing_vectors(facing)
        tlx, tly = left_of(tfx, tfy)
        if approach == APPROACH_REAR:
            wanted_side = side if side != 0 else 1.0
        side_reach = enemy_half_width[j] + own_half_depth[i] + FLANK_MARGIN
        rear_reach = enemy_half_depth[j] + own_half_depth[i] + FLANK_MARGIN
        if approach == APPROACH_REAR and cos < FRONT_COS:
            wx = ex[j] - tfx * rear_reach
            wy = ey[j] - tfy * rear_reach
        elif approach == APPROACH_REAR:
            wx = ex[j] + tlx * wanted_side * side_reach - tfx * rear_reach
            wy = ey[j] + tly * wanted_side * side_reach - tfy * rear_reach
        else:
            wx = ex[j] + tlx * wanted_side * side_reach
            wy = ey[j] + tly * wanted_side * side_reach
        order_type[i] = ORDER_MOVE
        dest_x[i] = wx
        dest_y[i] = wy
        return True

    def _line_slots(self, own, x, y, ex, ey, enemy_present, present, half_width, half_depth, count):
        """Slots of one line, spaced by frontage, for every battalion on FORM_LINE.

        Infantry stands in the middle ordered as it already stands, cavalry on the wings and
        archers in a second rank behind, all facing the enemy's centre."""
        members = [i for i in range(count) if present[i] and self.kind[i] == ACTION_FORM_LINE]
        if not members:
            return {}
        cx = float(np.mean(x[members]))
        cy = float(np.mean(y[members]))
        if enemy_present.any():
            forward = _unit(float(ex[enemy_present].mean()) - cx, float(ey[enemy_present].mean()) - cy)
        else:
            fx, fy = facing_vectors(own["facing"][members])
            forward = _unit(float(fx.mean()), float(fy.mean()))
        lateral = (-forward[1], forward[0])

        def offset(i):
            return (x[i] - cx) * lateral[0] + (y[i] - cy) * lateral[1]

        ranged = [i for i in members if own["is_ranged"][i] > 0.5]
        cavalry = [i for i in members if i not in ranged and int(own["battalion_type"][i]) == TYPE_CAVALRY]
        infantry = sorted([i for i in members if i not in ranged and i not in cavalry], key=offset)
        cavalry = sorted(cavalry, key=offset)
        low_wing = [i for i in cavalry if offset(i) < 0.0]
        high_wing = [i for i in cavalry if offset(i) >= 0.0]
        front = low_wing + infantry + high_wing
        back = sorted(ranged, key=offset)
        if not front:
            front, back = back, []

        cx = float(np.mean(x[front]))
        cy = float(np.mean(y[front]))
        slots = {}
        front_depth = max(half_depth[i] for i in front)
        for rank, depth in ((front, 0.0), (back, front_depth * 2.0 + LINE_SECOND_RANK_GAP)):
            if not rank:
                continue
            total = sum(2.0 * half_width[i] for i in rank) + LINE_GAP * (len(rank) - 1)
            cursor = -total / 2.0
            for i in rank:
                along = cursor + half_width[i]
                cursor += 2.0 * half_width[i] + LINE_GAP
                slot_x = cx + lateral[0] * along - forward[0] * depth
                slot_y = cy + lateral[1] * along - forward[1] * depth
                slots[i] = (slot_x, slot_y, forward[0], forward[1])
        return slots
