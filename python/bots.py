import numpy as np

from config import (
    ACTION_ADVANCE,
    ACTION_ATTACK,
    ACTION_FORM_LINE,
    APPROACH_FRONT,
    APPROACH_LEFT,
    APPROACH_REAR,
    APPROACH_RIGHT,
    MAX_BATTALIONS,
    PACE_RUN,
    PACE_WALK,
    TYPE_CAVALRY,
)
from geometry import finite_positions, footprint, present_of, sector

ENGAGE_DISTANCE = 700.0
CAVALRY_REACT = 600.0


def empty_actions():
    return {key: np.zeros(MAX_BATTALIONS, dtype=np.int64) for key in ("type", "pace", "dest", "target", "approach", "screen")}


def _distances(x, y, ex, ey, enemy_present):
    distance = np.hypot(ex[None, :] - x[:, None], ey[None, :] - y[:, None])
    return np.where(enemy_present[None, :], distance, np.inf)


class RushBot:
    """Every battalion charges its nearest enemy: the blob this project is trying to train away from."""

    name = "rush"

    def act(self, own, opponent, info):
        count = info.battalion_count
        x, y = finite_positions(own, info)
        ex, ey = finite_positions(opponent, info)
        distance = _distances(x, y, ex, ey, present_of(opponent, count))
        actions = empty_actions()
        actions["type"][:count] = ACTION_ATTACK
        actions["pace"][:count] = 1
        actions["approach"][:count] = APPROACH_FRONT
        actions["target"][:count] = distance[:count, :count].argmin(axis=1)
        return actions


class LineBot:
    """Forms a line and holds it; once the enemy is close each battalion attacks the enemy in front of it."""

    name = "line"
    flanks = False

    def act(self, own, opponent, info):
        count = info.battalion_count
        x, y = finite_positions(own, info)
        ex, ey = finite_positions(opponent, info)
        present = present_of(own, count)
        enemy_present = present_of(opponent, count)
        distance = _distances(x, y, ex, ey, enemy_present)
        actions = empty_actions()
        actions["type"][:count] = ACTION_FORM_LINE
        if not enemy_present.any():
            return actions
        cavalry = own["battalion_type"][:count].astype(np.int64) == TYPE_CAVALRY
        ranged = own["is_ranged"][:count] > 0.5
        close = distance[:count, :count].min(axis=1) < ENGAGE_DISTANCE
        enemy_cavalry = opponent["battalion_type"][:count].astype(np.int64) == TYPE_CAVALRY
        claimed = np.zeros(MAX_BATTALIONS, dtype=bool)
        for i in np.argsort(distance[:count, :count].min(axis=1)):
            if not present[i]:
                continue
            row = distance[i, :count].copy()
            if ranged[i]:
                if row.min() < own["range"][i]:
                    actions["type"][i] = ACTION_ATTACK
                    actions["target"][i] = int(row.argmin())
                continue
            if cavalry[i]:
                if self.flanks:
                    self._flank(i, actions, own, opponent, x, y, ex, ey, enemy_present, enemy_cavalry, count)
                else:
                    near_cavalry = np.where(enemy_cavalry & enemy_present[:count], row, np.inf)
                    if near_cavalry.min() < CAVALRY_REACT:
                        actions["type"][i] = ACTION_ATTACK
                        actions["target"][i] = int(near_cavalry.argmin())
                continue
            if close[i]:
                # this makes a battalion choose a target which has not been chosen by anyone else yet,
                # so that battalions avoid pilling up into one single target. it also helps maintain battle line
                free = np.where(claimed[:count], row * 3.0, row)
                j = int(free.argmin())
                claimed[j] = True
                actions["type"][i] = ACTION_ATTACK
                actions["target"][i] = j
                actions["pace"][i] = 1
        return actions

    def _flank(self, i, actions, own, opponent, x, y, ex, ey, enemy_present, enemy_cavalry, count):
        horses = np.where(enemy_present[:count] & enemy_cavalry)[0]
        if horses.size:
            targets, approach = horses, APPROACH_FRONT
        else:
            targets = np.where(enemy_present[:count] & (opponent["is_ranged"][:count] < 0.5))[0]
            approach = APPROACH_REAR
            if targets.size == 0:
                targets = np.where(enemy_present[:count])[0]
        j = int(targets[np.argmin(np.hypot(ex[targets] - x[i], ey[targets] - y[i]))])
        actions["type"][i] = ACTION_ATTACK
        actions["target"][i] = j
        actions["pace"][i] = 1
        actions["approach"][i] = approach


class FlankBot(LineBot):
    """The line, plus cavalry that swings round to the enemy's rear."""

    name = "flank"
    flanks = True


DEPLOY_DECISIONS = 5        # 10 s to form the line before anything else
ENGAGE_GAP = 300.0          # infantry attacks once an enemy is this close, edge to edge
PIN_GAP = 120.0             # an enemy this close to one of our battalions is pinned in melee
CAVALRY_THREAT = 900.0      # enemy cavalry this close to our archers or riders gets met by our cavalry
SPEAR_GUARD = 500.0         # spearmen step out at enemy cavalry this close to them or to the archers
ARCHER_ESCORT = 400.0       # enemy archers with infantry closer than this are escorted
ARCHER_PATIENCE = 45        # 90 s: an army with more archers waits that long for the enemy to come
STICKY = 1.5                # keep a target unless a new one is this much closer


class CommanderBot:
    """The behaviour the network should end up with: a cohesive line that advances together,
    one infantry battalion per enemy so the line never piles onto a single target, archers
    shooting from the second rank, spears guarding against horses, and cavalry that counters
    cavalry, hunts loose archers and hits pinned enemies in the rear.

    Infantry walks into contact: a walking attack carries no charge, but it does not tire the
    men either. Cavalry runs, because its charge bonus is what makes a rear attack decisive."""

    name = "commander"

    def __init__(self):
        self.episode = None

    def _reset(self, info):
        self.episode = info.episode_index
        self.decisions = 0
        self.targets = {}
        self.approaches = {}

    def act(self, own, opponent, info):
        if info.episode_index != self.episode:
            self._reset(info)
        self.decisions += 1
        count = info.battalion_count
        x, y = finite_positions(own, info)
        ex, ey = finite_positions(opponent, info)
        present = present_of(own, count)[:count]
        enemy_present = present_of(opponent, count)[:count]
        _, _, radius = footprint(own)
        enemy_half_width, enemy_half_depth, enemy_radius = footprint(opponent)
        actions = empty_actions()
        actions["type"][:count] = ACTION_FORM_LINE
        if not enemy_present.any():
            return actions

        kind = own["battalion_type"][:count].astype(np.int64)
        ranged = own["is_ranged"][:count] > 0.5
        cavalry = (kind == TYPE_CAVALRY) & ~ranged
        spears = (own["is_anti_large"][:count] > 0.5) & ~ranged & ~cavalry
        infantry = ~ranged & ~cavalry
        enemy_kind = opponent["battalion_type"][:count].astype(np.int64)
        enemy_ranged = (opponent["is_ranged"][:count] > 0.5) & enemy_present
        enemy_cavalry = (enemy_kind == TYPE_CAVALRY) & ~enemy_ranged & enemy_present
        enemy_spears = (opponent["is_anti_large"][:count] > 0.5) & enemy_present
        enemy_routing = opponent["lifecycle_state"][:count].astype(np.int64) == 2

        distance = _distances(x, y, ex, ey, present_of(opponent, count))[:count, :count]
        gap = distance - radius[:count, None] - enemy_radius[None, :count]
        friend_gap = np.hypot(x[:count, None] - ex[None, :count], y[:count, None] - ey[None, :count])
        friend_gap = friend_gap - radius[:count, None] - enemy_radius[None, :count]
        pinned = enemy_present & (np.where(present[:, None] & infantry[:, None], friend_gap, np.inf).min(axis=0) < PIN_GAP)

        deploying = self.decisions <= DEPLOY_DECISIONS
        outranging = ranged[present].sum() > enemy_ranged.sum()
        waiting = deploying or (outranging and self.decisions <= ARCHER_PATIENCE)
        line_kind = ACTION_FORM_LINE if waiting else ACTION_ADVANCE
        front_contact = np.where(present & infantry, gap.min(axis=1), np.inf).min() < ENGAGE_GAP

        # infantry: one battalion per enemy, nearest first, sticking to a target it already has
        claimed = np.zeros(count, dtype=np.int64)
        order = np.argsort(np.where(present & infantry, gap.min(axis=1), np.inf))
        for i in order:
            if not (present[i] and infantry[i]):
                continue
            if spears[i]:
                guard = self._spear_guard(i, x, y, ex, ey, ranged & present, enemy_cavalry, distance)
                if guard >= 0:
                    self._attack(actions, i, guard, PACE_WALK, APPROACH_FRONT)
                    continue
            if gap[i].min() >= ENGAGE_GAP and not (front_contact and gap[i].min() < 2.0 * ENGAGE_GAP):
                actions["type"][i] = line_kind
                continue
            cost = np.where(enemy_present & ~enemy_routing, gap[i], np.inf) + 400.0 * claimed
            if not np.isfinite(cost).any():
                cost = np.where(enemy_present, gap[i], np.inf)
            j = self._sticky(i, cost)
            claimed[j] += 1
            approach = self._open_side(i, j, x, y, ex, ey, opponent, enemy_half_width, enemy_half_depth) if claimed[j] > 1 else APPROACH_FRONT
            self._attack(actions, i, j, PACE_WALK, approach)

        # archers keep their place in the line and shoot whatever comes in range
        for i in np.where(present & ranged)[0]:
            row = np.where(enemy_present & ~enemy_routing, distance[i], np.inf)
            if row.min() < own["range"][i]:
                self._attack(actions, i, int(row.argmin()), PACE_WALK, APPROACH_FRONT)
            else:
                actions["type"][i] = line_kind

        for i in np.where(present & cavalry)[0]:
            self._cavalry(i, actions, x, y, ex, ey, ranged & present, distance, enemy_cavalry, enemy_ranged,
                          enemy_spears, enemy_routing, pinned, infantry, opponent, enemy_half_width, enemy_half_depth,
                          line_kind, deploying, enemy_present)
        return actions

    def _attack(self, actions, i, j, pace, approach):
        actions["type"][i] = ACTION_ATTACK
        actions["target"][i] = j
        actions["pace"][i] = pace
        actions["approach"][i] = approach
        self.targets[i] = j

    def _sticky(self, i, cost):
        best = int(np.argmin(cost))
        previous = self.targets.get(i)
        if previous is not None and previous < cost.shape[0] and np.isfinite(cost[previous]) and cost[previous] <= STICKY * max(cost[best], 1.0):
            return previous
        return best

    def _open_side(self, i, j, x, y, ex, ey, opponent, half_width, half_depth):
        # a second battalion on the same enemy goes for the side it is already nearest
        _, side = sector(x[i], y[i], ex[j], ey[j], opponent["facing"][j], half_width[j], half_depth[j])
        return APPROACH_LEFT if side >= 0 else APPROACH_RIGHT

    def _spear_guard(self, i, x, y, ex, ey, archers, enemy_cavalry, distance):
        if not enemy_cavalry.any():
            return -1
        near_me = np.where(enemy_cavalry, distance[i], np.inf)
        near_archers = np.full(enemy_cavalry.shape[0], np.inf)
        for a in np.where(archers)[0]:
            near_archers = np.minimum(near_archers, np.where(enemy_cavalry, np.hypot(ex[:near_archers.shape[0]] - x[a], ey[:near_archers.shape[0]] - y[a]), np.inf))
        threat = np.minimum(near_me, near_archers)
        if threat.min() > SPEAR_GUARD:
            return -1
        return int(threat.argmin())

    def _cavalry(self, i, actions, x, y, ex, ey, archers, distance, enemy_cavalry, enemy_ranged, enemy_spears,
                 enemy_routing, pinned, infantry, opponent, half_width, half_depth, line_kind, deploying, enemy_present):
        count = distance.shape[0]
        previous = self.targets.get(i)
        if previous is not None and previous < count and enemy_present[previous] and not enemy_routing[previous]:
            # riders committed to a charge see it through
            self._attack(actions, i, previous, PACE_RUN, self.approaches.get(i, APPROACH_FRONT))
            return
        self.targets.pop(i, None)
        # 1. enemy cavalry near our archers or our riders
        guard_points = list(np.where(archers)[0]) + [i]
        threat = np.full(count, np.inf)
        for g in guard_points:
            threat = np.minimum(threat, np.where(enemy_cavalry, np.hypot(ex[:count] - x[g], ey[:count] - y[g]), np.inf))
        if threat.min() < CAVALRY_THREAT:
            self._commit(actions, i, int(threat.argmin()), APPROACH_FRONT)
            return
        if deploying:
            actions["type"][i] = ACTION_FORM_LINE
            return
        # 2. archers without infantry close by
        loose = enemy_ranged & ~enemy_routing
        if loose.any():
            escort = np.full(count, np.inf)
            for k in np.where(enemy_present & ~enemy_ranged & ~enemy_cavalry)[0]:
                escort = np.minimum(escort, np.hypot(ex[:count] - ex[k], ey[:count] - ey[k]))
            loose = loose & (escort > ARCHER_ESCORT)
            if loose.any():
                self._commit(actions, i, int(np.where(loose, distance[i], np.inf).argmin()), APPROACH_FRONT)
                return
        # 3. hammer: the rear of an enemy pinned by our infantry, spearmen last
        candidates = pinned & ~enemy_cavalry & ~enemy_routing
        if candidates.any():
            cost = np.where(candidates, distance[i], np.inf) * np.where(enemy_spears, 2.0, 1.0)
            self._commit(actions, i, int(cost.argmin()), APPROACH_REAR)
            return
        actions["type"][i] = line_kind

    def _commit(self, actions, i, j, approach):
        self._attack(actions, i, j, PACE_RUN, approach)
        self.approaches[i] = approach


BOTS = [RushBot(), LineBot(), FlankBot(), CommanderBot()]
