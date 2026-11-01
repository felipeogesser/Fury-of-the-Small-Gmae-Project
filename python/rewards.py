import numpy as np

from config import FLANK_REAR, MAX_BATTALIONS, ROUTING_LIFECYCLE, TYPE_CAVALRY
from geometry import crowding, finite_positions, footprint, nearest_distance, present_of
from observation import army_total_hp, safe
from shm_layout import EpisodeResult


def shaping_scale(cfg, update_count):
    progress = min(update_count / max(cfg.anneal_updates, 1), 1.0)
    return max(cfg.floor, 1.0 - progress * (1.0 - cfg.floor))


def terminal_reward(result, side, cfg):
    if result == EpisodeResult.PLAYER_WON:
        return cfg.win if side == 0 else -cfg.win
    if result == EpisodeResult.ENEMY_WON:
        return -cfg.win if side == 0 else cfg.win
    if result == EpisodeResult.TIMEOUT:
        return cfg.timeout
    return 0.0


class RewardTracker:
    """Collects one reward per battalion over a decision window, both armies at once.

    The army shares the hp trade, routs and the result; each battalion gets its own crowding,
    flank and isolation terms, which is what gives a single battalion credit for keeping its place."""

    def __init__(self, cfg, info, decision_steps):
        self.cfg = cfg
        self.count = info.battalion_count
        self.info = info
        self.step_weight = 1.0 / max(decision_steps, 1)
        self.initial_hp = None
        self.previous_fraction = None
        self.previous_routing = None
        self.pending = [np.zeros(MAX_BATTALIONS), np.zeros(MAX_BATTALIONS)]
        self.stats = {"crowding": [0.0, 0.0], "flank": [0.0, 0.0], "exposed": [0.0, 0.0], "samples": [0, 0]}

    def step(self, snapshots, scale):
        count = self.count
        totals = [army_total_hp(snapshot, count) for snapshot in snapshots]
        routing = [safe(snapshot["lifecycle_state"][:count]).astype(np.int64) == ROUTING_LIFECYCLE for snapshot in snapshots]
        if self.initial_hp is None:
            self.initial_hp = [max(total, 1e-6) for total in totals]
            self.previous_fraction = [totals[army] / self.initial_hp[army] for army in (0, 1)]
            self.previous_routing = routing
        fraction = [totals[army] / self.initial_hp[army] for army in (0, 1)]
        cfg = self.cfg
        for side in (0, 1):
            other = 1 - side
            own_loss = self.previous_fraction[side] - fraction[side]
            enemy_loss = self.previous_fraction[other] - fraction[other]
            broke = float((routing[other] & ~self.previous_routing[other]).sum())
            lost = float((routing[side] & ~self.previous_routing[side]).sum())
            team = cfg.hp_trade * (enemy_loss - own_loss) + cfg.rout * (broke - lost)
            self.pending[side] += team + scale * self.step_weight * self._individual(snapshots[side], side)
        self.previous_fraction = fraction
        self.previous_routing = routing

    def _individual(self, snapshot, side):
        cfg = self.cfg
        count = self.count
        present = present_of(snapshot, count)
        x, y = finite_positions(snapshot, self.info)
        _, _, radius = footprint(snapshot)
        overlap = crowding(x, y, radius, present).sum(axis=1)
        flank_attack = safe(snapshot["flank_attack"]).clip(0, FLANK_REAR)
        flank_exposure = safe(snapshot["flank_exposure"]).clip(0, FLANK_REAR)
        nearest, _ = nearest_distance(x, y, x, y, present, exclude_self=True)
        cavalry = safe(snapshot["battalion_type"]).astype(np.int64) == TYPE_CAVALRY
        engaged = snapshot["is_engaged"] > 0.5
        isolated = present & ~cavalry & ~engaged & np.isfinite(nearest) & (nearest > cfg.isolation_radii * radius)
        reward = (
            -cfg.crowding * overlap
            + cfg.flank_attack * flank_attack
            - cfg.flank_exposed * flank_exposure
            - cfg.isolation * isolated
        )
        reward[~present] = 0.0
        if present.any():
            self.stats["crowding"][side] += float(overlap[present].mean())
            self.stats["flank"][side] += float((flank_attack[present] > 0).mean())
            self.stats["exposed"][side] += float((flank_exposure[present] > 0).mean())
            self.stats["samples"][side] += 1
        return reward

    def take(self, side, result=None):
        """Reward of the window that just ended for one army, plus the result once the battle is over."""
        reward = self.pending[side].copy()
        if result is not None and result != EpisodeResult.RUNNING:
            reward += terminal_reward(result, side, self.cfg)
        self.pending[side][:] = 0.0
        return reward.astype(np.float32)

    def summary(self, side):
        samples = max(self.stats["samples"][side], 1)
        return {key: self.stats[key][side] / samples for key in ("crowding", "flank", "exposed")}
