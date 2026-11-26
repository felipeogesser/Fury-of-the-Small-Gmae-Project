import numpy as np

MELEE_DAMAGE_MULTIPLIER = 4.0
ARROW_DAMAGE_MULTIPLIER = 1.25
ARCHER_MELEE_SWING = 1.0
ARROW_SHIELD_BLOCK = 0.5
LARGE_TARGET_DAMAGE = 1.5
ANTI_LARGE_DAMAGE = 1.5
ARMOR_K = 100.0
BASE_HIT = 35.0
MIN_HIT = 8.0
MAX_HIT = 90.0


def _flag(snapshot, name):
    return snapshot[name] > 0.5


def _armor_factor(attacker, defender):
    armor = defender["armor"][None, :] * np.where(_flag(attacker, "is_armor_piercing")[:, None], 0.5, 1.0)
    return ARMOR_K / (armor + ARMOR_K)


def melee_rate(attacker, defender):
    ranged = _flag(attacker, "is_ranged")[:, None]
    large = _flag(defender, "is_large")[None, :]
    anti_large = _flag(attacker, "is_anti_large")[:, None]
    damage = attacker["strength"][:, None] * np.where(ranged, 1.0, MELEE_DAMAGE_MULTIPLIER)
    damage = damage * np.where(large, LARGE_TARGET_DAMAGE, 1.0) * _armor_factor(attacker, defender)
    damage = damage * np.where(large & anti_large, ANTI_LARGE_DAMAGE, 1.0)
    chance = np.clip(BASE_HIT + attacker["meele_attack"][:, None] - defender["meele_defense"][None, :], MIN_HIT, MAX_HIT) / 100.0
    swing = np.where(ranged, ARCHER_MELEE_SWING, np.maximum(attacker["attack_speed"][:, None], 1e-3))
    fighters = np.minimum(attacker["formation_y"], attacker["alive_entity_count"])[:, None]
    return damage * chance / swing * fighters


def arrow_rate(attacker, defender):
    damage = attacker["strength"][:, None] * ARROW_DAMAGE_MULTIPLIER * _armor_factor(attacker, defender)
    damage = damage * np.where(_flag(defender, "is_shielded")[None, :], 1.0 - ARROW_SHIELD_BLOCK, 1.0)
    reload = np.maximum(attacker["attack_speed"][:, None], 1e-3)
    return damage / reload * attacker["alive_entity_count"][:, None]


def advantage(own, enemy):
    own_hp = np.maximum(own["hp"], 1.0)
    enemy_hp = np.maximum(enemy["hp"], 1.0)
    dealt = melee_rate(own, enemy) / enemy_hp[None, :]
    taken = melee_rate(enemy, own).T / own_hp[:, None]
    melee = np.log(np.maximum(dealt, 1e-9) / np.maximum(taken, 1e-9))
    arrows = arrow_rate(own, enemy) / enemy_hp[None, :]
    present = enemy["is_present"] > 0.5
    typical = np.exp(np.log(np.maximum(arrows, 1e-9))[:, present].mean(axis=1, keepdims=True)) if present.any() else 1.0
    shooting = np.log(np.maximum(arrows, 1e-9) / np.maximum(typical, 1e-9))
    return np.where(_flag(own, "is_ranged")[:, None], shooting, melee)
