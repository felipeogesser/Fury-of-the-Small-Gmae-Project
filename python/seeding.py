import os

import torch

SEED_VARIABLE = "CGAME_SEED"
MASK = (1 << 64) - 1


def run_seed():
    value = os.environ.get(SEED_VARIABLE)
    return int(value) if value else None


def _splitmix64(x):
    z = (x + 0x9E3779B97F4A7C15) & MASK
    z = ((z ^ (z >> 30)) * 0xBF58476D1CE4E5B9) & MASK
    z = ((z ^ (z >> 27)) * 0x94D049BB133111EB) & MASK
    return z ^ (z >> 31)


def battle_seed(seed, battle_index):
    if seed is None:
        return None
    return _splitmix64(seed ^ _splitmix64(battle_index))


def seed_torch(seed):
    if seed is not None:
        torch.manual_seed(seed)
