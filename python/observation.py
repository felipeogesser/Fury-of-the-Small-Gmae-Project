import numpy as np

from actions import ORDER_FEATURE_DIM
from config import DEST_GRID_X, DEST_GRID_Y, ENUM_SIZES, MAX_BATTALIONS, ROUTING_LIFECYCLE
from geometry import crowding, facing_vectors, footprint, mirror_facing, sector

RAW_FIELDS = [
    "morale",
    "fatigue",
    "speed",
    "meele_attack",
    "meele_defense",
    "strength",
    "rally_timer",
    "charge_timer",
    "morale_modifiers",
    "range",
    "charge_bonus",
    "armor",
    "max_hp",
    "attack_speed",
    "start_entity_count",
    "formation_x",
    "formation_y",
]

BOOL_FIELDS = [
    "is_active",
    "is_rallying",
    "is_charging",
    "is_engaging",
    "is_engaged",
    "is_controllable",
    "is_present",
    "is_headless",
    "is_bracing",
    "is_shielded",
    "is_ranged",
    "is_large",
    "is_armor_piercing",
    "is_anti_large",
]

ENUM_FIELDS = [
    "battalion_type",
    "headless_movement_mode",
    "fatigue_state",
    "morale_state",
    "lifecycle_state",
    "movement_state",
    "movement_pace",
    "flank_exposure",
    "flank_attack",
]

RAW_INDEX = {name: index for index, name in enumerate(RAW_FIELDS)}
SPATIAL_DIM = 9
RATIO_DIM = 5
ONEHOT_DIM = sum(ENUM_SIZES[name] + 1 for name in ENUM_FIELDS)
ENTITY_DIM = SPATIAL_DIM + RATIO_DIM + len(RAW_FIELDS) + len(BOOL_FIELDS) + ONEHOT_DIM + ORDER_FEATURE_DIM
ENEMY_PAIR_DIM = 20
FRIEND_PAIR_DIM = 8
GLOBAL_DIM = 7
DEST_CELLS = DEST_GRID_X * DEST_GRID_Y
INDICES = np.arange(MAX_BATTALIONS)


class RunningNorm:

    def __init__(self, size, clip=5.0):
        self.mean = np.zeros(size, dtype=np.float64)
        self.var = np.ones(size, dtype=np.float64)
        self.count = 1e-4
        self.clip = clip

    def update(self, values):
        if values.shape[0] == 0:
            return
        batch_mean = values.mean(axis=0)
        batch_var = values.var(axis=0)
        batch_count = values.shape[0]
        delta = batch_mean - self.mean
        total = self.count + batch_count
        self.mean = self.mean + delta * batch_count / total
        m2 = self.var * self.count + batch_var * batch_count + delta ** 2 * self.count * batch_count / total
        self.var = m2 / total
        self.count = total

    def normalize(self, values):
        normalized = (values - self.mean) / np.sqrt(self.var + 1e-8)
        return np.clip(normalized, -self.clip, self.clip).astype(np.float32)

    def state_dict(self):
        import torch
        return {
            "mean": torch.tensor(self.mean),
            "var": torch.tensor(self.var),
            "count": float(self.count),
        }

    def load_state_dict(self, state):
        self.mean = state["mean"].numpy().astype(np.float64)
        self.var = state["var"].numpy().astype(np.float64)
        self.count = float(state["count"])


def safe(values):
    return np.nan_to_num(values, nan=0.0, posinf=0.0, neginf=0.0)


def snapshot_army(static, dynamic, count):
    snapshot = {}
    for group in (static, dynamic):
        for name, view in group.items():
            values = np.zeros(MAX_BATTALIONS, dtype=np.float64)
            values[:count] = view[:count]
            snapshot[name] = values
    return snapshot


def present_mask(snapshot, count):
    return (snapshot["is_present"] > 0.5) & (INDICES < count)


def army_total_hp(snapshot, count):
    return float(np.clip(safe(snapshot["hp"][:count]), 0.0, None).sum())


def raw_matrix(snapshot):
    return safe(np.stack([snapshot[name] for name in RAW_FIELDS], axis=1))


def _one_hot(values, size):
    indices = np.clip(safe(values).astype(np.int64) + 1, 0, size)
    out = np.zeros((MAX_BATTALIONS, size + 1), dtype=np.float32)
    out[INDICES, indices] = 1.0
    return out


def _entity(snapshot, count, info, mirror, normalizer, order_features):
    present = present_mask(snapshot, count)
    target_valid = np.isfinite(snapshot["target_x"]) & np.isfinite(snapshot["target_y"])
    x = safe(snapshot["x"])
    y = safe(snapshot["y"])
    target_x = safe(snapshot["target_x"])
    target_y = safe(snapshot["target_y"])
    facing = safe(snapshot["facing"])
    if mirror:
        x = info.left + info.right - x
        target_x = info.left + info.right - target_x
        facing = mirror_facing(facing)
    half_width, half_depth, radius = footprint(snapshot)
    forward_x, forward_y = facing_vectors(facing)
    xn = (x - info.left) / info.width
    yn = (y - info.top) / info.height
    target_xn = np.where(target_valid, (target_x - info.left) / info.width, 0.0)
    target_yn = np.where(target_valid, (target_y - info.top) / info.height, 0.0)
    spatial = np.stack([
        xn, yn, target_xn, target_yn, target_valid.astype(np.float64),
        forward_x, forward_y, half_width / info.width, half_depth / info.width,
    ], axis=1)
    max_hp = np.maximum(safe(snapshot["max_hp"]), 1e-6)
    start = np.maximum(safe(snapshot["start_entity_count"]), 1.0)
    hp_ratio = np.clip(safe(snapshot["hp"]) / max_hp, 0.0, 5.0)
    ratios = np.stack([
        hp_ratio,
        safe(snapshot["alive_entity_count"]) / start,
        safe(snapshot["dead_entity_count"]) / start,
        safe(snapshot["withdrawed_entity_count"]) / start,
        safe(snapshot["unit_alive_count"]) / start,
    ], axis=1)
    raw = normalizer.normalize(raw_matrix(snapshot))
    bools = np.stack([snapshot[name] for name in BOOL_FIELDS], axis=1)
    onehots = [_one_hot(snapshot[name], ENUM_SIZES[name]) for name in ENUM_FIELDS]
    if order_features is None:
        order_features = np.zeros((MAX_BATTALIONS, ORDER_FEATURE_DIM), dtype=np.float32)
    features = np.concatenate([spatial, ratios, raw, bools] + onehots + [order_features], axis=1).astype(np.float32)
    features[~present] = 0.0
    return {
        "features": features,
        "present": present,
        "controllable": snapshot["is_controllable"] > 0.5,
        "x": x,
        "y": y,
        "facing": facing,
        "half_width": half_width,
        "half_depth": half_depth,
        "radius": radius,
        "forward_x": forward_x,
        "forward_y": forward_y,
        "hp_ratio": hp_ratio,
        "raw": raw,
        "target": safe(snapshot["target_battalion_idx"]).astype(np.int64),
        "large": snapshot["is_large"] > 0.5,
        "anti_large": snapshot["is_anti_large"] > 0.5,
        "ranged": snapshot["is_ranged"] > 0.5,
        "routing": safe(snapshot["lifecycle_state"]).astype(np.int64) == ROUTING_LIFECYCLE,
    }


def _raw_column(entity, name):
    return entity["raw"][:, RAW_INDEX[name]]


def _bearing(a, b):
    """Direction from each a to each b, in a's own frame: cos is ahead, sin is to its right."""
    dx = b["x"][None, :] - a["x"][:, None]
    dy = b["y"][None, :] - a["y"][:, None]
    distance = np.maximum(np.sqrt(dx * dx + dy * dy), 1e-6)
    ahead = (dx * a["forward_x"][:, None] + dy * a["forward_y"][:, None]) / distance
    right = (dy * a["forward_x"][:, None] - dx * a["forward_y"][:, None]) / distance
    return dx, dy, distance, ahead, right


def _enemy_pair(friend, enemy, info):
    dx, dy, distance, ahead, right = _bearing(friend, enemy)
    my_sector, my_side = sector(
        friend["x"][:, None], friend["y"][:, None],
        enemy["x"][None, :], enemy["y"][None, :], enemy["facing"][None, :],
        enemy["half_width"][None, :], enemy["half_depth"][None, :],
    )
    their_sector, _ = sector(
        enemy["x"][None, :], enemy["y"][None, :],
        friend["x"][:, None], friend["y"][:, None], friend["facing"][:, None],
        friend["half_width"][:, None], friend["half_depth"][:, None],
    )
    gap = (distance - friend["radius"][:, None] - enemy["radius"][None, :]) / info.width
    is_my_target = (friend["target"][:, None] == INDICES[None, :]).astype(np.float64)
    targets_me = (enemy["target"][None, :] == INDICES[:, None]).astype(np.float64)
    pair = np.stack([
        dx / info.width,
        dy / info.width,
        distance / info.width,
        gap,
        ahead,
        right,
        my_sector,
        my_side,
        their_sector,
        friend["hp_ratio"][:, None] - enemy["hp_ratio"][None, :],
        _raw_column(friend, "morale")[:, None] - _raw_column(enemy, "morale")[None, :],
        _raw_column(friend, "meele_attack")[:, None] - _raw_column(enemy, "meele_defense")[None, :],
        _raw_column(friend, "meele_defense")[:, None] - _raw_column(enemy, "meele_attack")[None, :],
        _raw_column(friend, "speed")[:, None] - _raw_column(enemy, "speed")[None, :],
        _raw_column(friend, "fatigue")[:, None] - _raw_column(enemy, "fatigue")[None, :],
        (friend["anti_large"][:, None] & enemy["large"][None, :]).astype(np.float64),
        (friend["large"][:, None] & enemy["anti_large"][None, :]).astype(np.float64),
        np.broadcast_to(enemy["routing"][None, :], dx.shape).astype(np.float64),
        is_my_target,
        targets_me,
    ], axis=-1).astype(np.float32)
    mask = friend["present"][:, None] & enemy["present"][None, :]
    pair[~mask] = 0.0
    return pair


def _friend_pair(friend, info):
    dx, dy, distance, ahead, right = _bearing(friend, friend)
    overlap = crowding(friend["x"], friend["y"], friend["radius"], friend["present"])
    gap = (distance - friend["radius"][:, None] - friend["radius"][None, :]) / info.width
    pair = np.stack([
        dx / info.width,
        dy / info.width,
        distance / info.width,
        gap,
        ahead,
        right,
        overlap,
        np.broadcast_to(friend["ranged"][None, :], dx.shape).astype(np.float64),
    ], axis=-1).astype(np.float32)
    mask = friend["present"][:, None] & friend["present"][None, :]
    np.fill_diagonal(mask, False)
    pair[~mask] = 0.0
    return pair


def build_observations(snapshots, info, tick_fraction, normalizer, update_stats, order_features):
    count = info.battalion_count
    if update_stats:
        rows = [raw_matrix(snapshot)[present_mask(snapshot, count)] for snapshot in snapshots]
        normalizer.update(np.concatenate(rows, axis=0))
    totals = [army_total_hp(snapshot, count) for snapshot in snapshots]
    observations = []
    for side in (0, 1):
        mirror = side == 1
        friend = _entity(snapshots[side], count, info, mirror, normalizer, order_features[side])
        enemy = _entity(snapshots[1 - side], count, info, mirror, normalizer, None)
        hp_sum = totals[side] + totals[1 - side]
        share = totals[side] / hp_sum if hp_sum > 0.0 else 0.5
        overlap = crowding(friend["x"], friend["y"], friend["radius"], friend["present"]).sum(axis=1)
        glob = np.array([
            tick_fraction,
            share,
            friend["present"].sum() / MAX_BATTALIONS,
            enemy["present"].sum() / MAX_BATTALIONS,
            count / MAX_BATTALIONS,
            float(overlap[friend["present"]].mean()) if friend["present"].any() else 0.0,
            float(enemy["routing"][enemy["present"]].mean()) if enemy["present"].any() else 0.0,
        ], dtype=np.float32)
        observations.append({
            "friend": friend["features"],
            "enemy": enemy["features"],
            "epair": _enemy_pair(friend, enemy, info),
            "fpair": _friend_pair(friend, info),
            "glob": glob,
            "fmask": friend["present"],
            "emask": enemy["present"],
            "cmask": friend["present"] & friend["controllable"],
        })
    return observations
