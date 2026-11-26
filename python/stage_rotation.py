import os
import random

from generate_stages import generate_layout, write_layout

class StageRotation:

    def __init__(self, path, battles_per_layout, min_battalions, max_battalions, seed=None):
        self.path = path
        self.battles_per_layout = max(battles_per_layout, 1)
        self.min_battalions = min_battalions
        self.max_battalions = max_battalions
        self.rng = random.Random(seed)
        self.battles_on_layout = 0

    def new_layout(self):
        grid = None
        for _ in range(1000):
            grid = generate_layout(self.rng, self.min_battalions, self.max_battalions)
            if grid is not None:
                break
        if grid is None:
            raise RuntimeError("stage generator could not produce a valid layout")
        directory = os.path.dirname(self.path)
        if directory:
            os.makedirs(directory, exist_ok=True)
        temporary = self.path + ".tmp"
        write_layout(temporary, grid)
        os.replace(temporary, self.path)
        self.battles_on_layout = 0

    def on_battle_end(self):
        self.battles_on_layout += 1
        if self.battles_on_layout >= self.battles_per_layout:
            self.new_layout()