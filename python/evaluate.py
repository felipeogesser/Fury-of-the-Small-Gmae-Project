import os

import numpy as np

from bots import BOTS
from checkpoint import load_checkpoint
from model import select_actions
from observation import RAW_FIELDS, RunningNorm
from rewards import RewardTracker
from runners import Battle, build_model, control_mask, outcome_for, read_snapshots
from shm_layout import EpisodeResult

EVAL_HEADER = "update,opponent,battles,wins,losses,draws,win_rate,seconds,crowding,switching,matchup\n"


class Evaluator:

    def __init__(self, cfg, snapshot_path, log_path, update):
        payload = load_checkpoint(snapshot_path)
        self.model = build_model(cfg)
        self.model.load_state_dict(payload["model"])
        self.model.eval()
        self.normalizer = RunningNorm(len(RAW_FIELDS))
        self.normalizer.load_state_dict(payload["normalizer"])
        self.cfg = cfg
        self.log_path = log_path
        self.update = update
        names = cfg.evaluation.opponents or [bot.name for bot in BOTS]
        self.opponents = [next(bot for bot in BOTS if bot.name == name) for name in names]
        self.per_opponent = cfg.evaluation.battles_per_opponent
        self.played = 0
        self.results = {bot.name: [] for bot in self.opponents}
        self.wants_stop = False
        self.battle = None

    def begin_episode(self, info, seed=None):
        self.battle = Battle(info, self.cfg.decision_steps)
        self.tracker = RewardTracker(self.cfg.rewards, info)
        self.opponent = type(self.opponents[self.played // self.per_opponent])()
        self.net_side = self.played % 2

    def on_step(self, client, result):
        if self.wants_stop:
            return
        battle = self.battle
        snapshots = read_snapshots(client, battle.info)
        net, bot = self.net_side, 1 - self.net_side
        if result != EpisodeResult.RUNNING or battle.decision_due():
            self.tracker.step(snapshots, 1.0, battle.books)
        if result != EpisodeResult.RUNNING:
            self._record(result, battle)
            return
        if battle.decision_due():
            observations = battle.observe(snapshots, self.normalizer, False)
            actions, _, _ = select_actions(self.model, [observations[net]], True)
            switched = battle.apply(net, actions[0], control_mask(observations[net]), net == 1)
            self.tracker.charge_switches(net, switched, control_mask(observations[net]))
            bot_actions = self.opponent.act(snapshots[bot], snapshots[net], battle.info)
            battle.apply(bot, bot_actions, control_mask(observations[bot]), False)
        battle.carry_out(client, snapshots)

    def _record(self, result, battle):
        seconds = battle.steps * battle.info.ticks_per_decision * battle.info.tick_dt
        entry = {"outcome": outcome_for(result, self.net_side), "seconds": seconds}
        entry.update(self.tracker.summary(self.net_side))
        self.results[self.opponent.name].append(entry)
        self.played += 1
        if self.played >= self.per_opponent * len(self.opponents):
            self._write()
            self.wants_stop = True

    def _write(self):
        new_file = not os.path.exists(self.log_path)
        with open(self.log_path, "a") as log:
            if new_file:
                log.write(EVAL_HEADER)
            for name, entries in self.results.items():
                outcomes = [entry["outcome"] for entry in entries]
                wins = outcomes.count("win")
                losses = outcomes.count("loss")
                draws = len(outcomes) - wins - losses
                mean = {key: float(np.mean([entry[key] for entry in entries])) for key in ("seconds", "crowding", "switching", "matchup")}
                log.write(f"{self.update},{name},{len(entries)},{wins},{losses},{draws},{wins / max(len(entries), 1):.3f},"
                          f"{mean['seconds']:.0f},{mean['crowding']:.4f},{mean['switching']:.4f},{mean['matchup']:.4f}\n")

    def on_exit(self):
        pass


def evaluator_from_environment(cfg):
    snapshot = os.environ.get("CGAME_EVAL")
    if not snapshot:
        return None
    return Evaluator(cfg, snapshot, os.environ["CGAME_EVAL_LOG"], int(os.environ.get("CGAME_EVAL_UPDATE", "0")))
