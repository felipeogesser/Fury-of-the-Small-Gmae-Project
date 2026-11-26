import argparse
import math
import os
import random

GRID_X = 5
GRID_Y = 6
FRONT = GRID_X - 1

GENERAL_GUARDSMEN = 1
GENERAL_SWORDSMEN = 2
GENERAL_TEMPLAR = 3
GENERAL_CAVALRY = 4
GENERAL_ARCHERS = 5

INFANTRY = {GENERAL_GUARDSMEN, GENERAL_SWORDSMEN, GENERAL_TEMPLAR}


def composition(rng, total):
    cavalry = rng.randint(0, int(total * 0.3))
    archers = rng.randint(0, int(total * 0.25))
    infantry = total - cavalry - archers
    line = rng.randint(math.ceil(infantry * 0.5), infantry) if infantry > 0 else 0
    swordsmen = infantry - line
    templar = rng.randint(0, math.ceil(line * 0.4)) if line > 0 else 0
    guardsmen = line - templar
    return guardsmen, templar, swordsmen, cavalry, archers


def distance_from_center(y, center):
    return abs(y + 0.5 - center)


def cavalry_cells(rng, center, mode):
    rows = list(range(GRID_Y))
    if mode == "top":
        rows.sort(key=lambda y: (y, rng.random()))
    elif mode == "bottom":
        rows.sort(key=lambda y: (-y, rng.random()))
    else:
        rows.sort(key=lambda y: (-distance_from_center(y, center), rng.random()))
    cells = []
    if mode == "reserve":
        center_rows = sorted(range(GRID_Y), key=lambda y: (distance_from_center(y, center), rng.random()))
        for x in (1, 0, 2):
            cells.extend((x, y) for y in center_rows)
        return cells
    for x in (FRONT, FRONT - 1, FRONT - 2):
        cells.extend((x, y) for y in rows[:2])
    for x in range(FRONT, -1, -1):
        cells.extend((x, y) for y in rows[2:])
    return cells


def archer_cells(rng, center, screen):
    # screening archers stand in front of the line, otherwise they shoot from behind it
    columns = list(range(FRONT, -1, -1)) if screen else list(range(FRONT - 1, -1, -1)) + [FRONT]
    cells = []
    for x in columns:
        rows = sorted(range(GRID_Y), key=lambda y: (distance_from_center(y, center), rng.random()))
        cells.extend((x, y) for y in rows)
    return cells


def place(grid, cells, general, count):
    placed = 0
    for x, y in cells:
        if placed == count:
            break
        if grid[x][y] == 0:
            grid[x][y] = general
            placed += 1
    return placed


def infantry_cells(rng, center):
    cells = []
    for x in range(FRONT, -1, -1):
        rows = sorted(range(GRID_Y), key=lambda y: (distance_from_center(y, center), rng.random()))
        cells.extend((x, y) for y in rows)
    return cells


def generate_layout(rng, min_battalions, max_battalions):
    total = rng.randint(min_battalions, max_battalions)
    guardsmen, templar, swordsmen, cavalry, archers = composition(rng, total)
    grid = [[0] * GRID_Y for _ in range(GRID_X)]
    center = GRID_Y / 2.0 + rng.choice([-1.0, -0.5, 0.0, 0.0, 0.5, 1.0])

    roll = rng.random()
    if roll < 0.15:
        cavalry_mode = "reserve"
    elif roll < 0.3:
        cavalry_mode = "top"
    elif roll < 0.45:
        cavalry_mode = "bottom"
    else:
        cavalry_mode = "flanks"

    placed = 0
    remaining_cavalry = cavalry
    for x, y in cavalry_cells(rng, center, cavalry_mode):
        if remaining_cavalry == 0:
            break
        if grid[x][y] == 0:
            grid[x][y] = GENERAL_CAVALRY
            remaining_cavalry -= 1
            placed += 1

    screen = rng.random() < 0.2
    if screen:
        placed += place(grid, archer_cells(rng, center, screen), GENERAL_ARCHERS, archers)

    line_units = [GENERAL_TEMPLAR] * templar + [GENERAL_GUARDSMEN] * guardsmen
    if rng.random() < 0.3:
        rng.shuffle(line_units)
    infantry_units = line_units + [GENERAL_SWORDSMEN] * swordsmen

    free_cells = sum(1 for x in range(GRID_X) for y in range(GRID_Y) if grid[x][y] == 0)
    spare = free_cells - len(infantry_units)
    gap_rate = 0.12 if spare > 4 else 0.0

    index = 0
    for x, y in infantry_cells(rng, center):
        if index == len(infantry_units):
            break
        if grid[x][y] != 0:
            continue
        if x == FRONT and spare > 0 and rng.random() < gap_rate:
            spare -= 1
            continue
        grid[x][y] = infantry_units[index]
        index += 1
        placed += 1

    if not screen:
        placed += place(grid, archer_cells(rng, center, screen), GENERAL_ARCHERS, archers)

    if rng.random() < 0.5:
        grid = [list(reversed(column)) for column in grid]

    if placed != total:
        return None
    if infantry_units and not any(grid[FRONT][y] in INFANTRY for y in range(GRID_Y)):
        return None
    return grid


def write_layout(path, grid):
    lines = [",".join(str(grid[x][y]) for x in range(GRID_X)) for y in range(GRID_Y)]
    with open(path, "w", newline="\n") as file:
        file.write("\n".join(lines) + "\n")


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--count", type=int, default=500)
    parser.add_argument("--out", default="stages/generated")
    parser.add_argument("--seed", type=int, default=None)
    parser.add_argument("--min-battalions", type=int, default=4)
    parser.add_argument("--max-battalions", type=int, default=12)
    arguments = parser.parse_args()

    if not 1 <= arguments.min_battalions <= arguments.max_battalions <= min(20, GRID_X * GRID_Y):
        raise SystemExit("battalion range must be within 1 and 20, with min <= max")

    rng = random.Random(arguments.seed)
    os.makedirs(arguments.out, exist_ok=True)
    for name in os.listdir(arguments.out):
        if name.endswith(".csv"):
            os.remove(os.path.join(arguments.out, name))

    seen = set()
    attempts = 0
    while len(seen) < arguments.count and attempts < arguments.count * 50:
        attempts += 1
        grid = generate_layout(rng, arguments.min_battalions, arguments.max_battalions)
        if grid is None:
            continue
        key = tuple(tuple(column) for column in grid)
        if key in seen:
            continue
        seen.add(key)
        write_layout(os.path.join(arguments.out, f"stage_{len(seen) - 1:04d}.csv"), grid)

    print(f"wrote {len(seen)} layouts to {arguments.out}")


if __name__ == "__main__":
    main()