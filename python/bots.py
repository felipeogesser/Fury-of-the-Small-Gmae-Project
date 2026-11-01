import numpy as np

from config import (
    ACTION_ATTACK,
    ACTION_FORM_LINE,
        APPROACH_FRONT,
    APPROACH_REAR,
    MAX_BATTALIONS,
    TYPE_CAVALRY,
    )
from geometry import finite_positions, present_of

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


BOTS = [RushBot(), LineBot(), FlankBot()]
