import numpy as np

from config import FILE_SPACING, MAX_BATTALIONS, RANK_SPACING

QUARTER = np.pi / 2.0


def facing_vectors(facing):
    angle = facing * QUARTER
    return np.cos(angle), np.sin(angle)


def mirror_facing(facing):
    return np.mod(2.0 - facing, 4.0)


def left_of(forward_x, forward_y):
    return forward_y, -forward_x


def _cached(snapshot, key, compute):
    """Derived arrays are worked out once per snapshot; read-only, since every caller shares them."""
    value = snapshot.get(key)
    if value is None:
        value = compute()
        for array in value:
            array.setflags(write=False)
        snapshot[key] = value
    return value


def footprint(snapshot):
    def compute():
        half_depth = 0.5 * RANK_SPACING * np.maximum(snapshot["formation_x"], 1.0)
        half_width = 0.5 * FILE_SPACING * np.maximum(snapshot["formation_y"], 1.0)
        return half_width, half_depth, np.sqrt(half_depth * half_width)
    return _cached(snapshot, "_footprint", compute)


def sector(px, py, tx, ty, facing, half_width, half_depth):
    """Where points (px, py) sit around targets (tx, ty), same rule as flank.c.

    Returns cos of the scaled bearing (1 front, -1 rear) and the lateral sign (1 on the target's left)."""
    fx, fy = facing_vectors(facing)
    lx, ly = left_of(fx, fy)
    dx = px - tx
    dy = py - ty
    along = (dx * fx + dy * fy) / np.maximum(half_depth, 1e-6)
    across = (dx * lx + dy * ly) / np.maximum(half_width, 1e-6)
    length = np.sqrt(along * along + across * across)
    cos = np.where(length > 1e-6, along / np.maximum(length, 1e-6), 1.0)
    return cos, np.sign(across)


def crowding(x, y, radius, present):
    """Overlap of every battalion's footprint with its friends: 0 apart, 1 on top of each other."""
    dx = x[:, None] - x[None, :]
    dy = y[:, None] - y[None, :]
    distance = np.sqrt(dx * dx + dy * dy)
    reach = radius[:, None] + radius[None, :]
    overlap = np.clip(1.0 - distance / np.maximum(reach, 1e-6), 0.0, 1.0)
    mask = present[:, None] & present[None, :]
    np.fill_diagonal(mask, False)
    return np.where(mask, overlap, 0.0)


def nearest_distance(x, y, other_x, other_y, other_present, exclude_self=False):
    dx = other_x[None, :] - x[:, None]
    dy = other_y[None, :] - y[:, None]
    distance = np.sqrt(dx * dx + dy * dy)
    distance = np.where(other_present[None, :], distance, np.inf)
    if exclude_self:
        np.fill_diagonal(distance, np.inf)
    index = distance.argmin(axis=1) if distance.shape[1] else np.zeros(x.shape[0], dtype=np.int64)
    return distance[np.arange(x.shape[0]), index], index


def finite_positions(snapshot, info):
    """Positions with the missing ones (NaN or inf in the C table) moved to the map centre."""
    def compute():
        center_x = (info.left + info.right) / 2.0
        center_y = (info.top + info.bottom) / 2.0
        return np.where(snapshot["x_valid"], snapshot["x"], center_x), np.where(snapshot["y_valid"], snapshot["y"], center_y)
    return _cached(snapshot, "_positions", compute)


def present_of(snapshot, count):
    return _cached(snapshot, "_present", lambda: ((snapshot["is_present"] > 0.5) & (np.arange(MAX_BATTALIONS) < count),))[0]
