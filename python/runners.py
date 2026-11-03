import glob
import os
import random
import time

import numpy as np
import torch

from actions import OrderBook
from bots import BOTS
from checkpoint import load_checkpoint, save_checkpoint
from model import BattalionPolicy, select_actions
from observation import (
    DEST_CELLS,
    ENEMY_PAIR_DIM,
    ENTITY_DIM,
    FRIEND_PAIR_DIM,
    GLOBAL_DIM,
    RAW_FIELDS,
    RunningNorm,
    build_observations,
    snapshot_army,
)
from config import APPROACH_LEFT, APPROACH_RIGHT
from ppo import DemoBuffer, RolloutBuffer, imitation_scale, ppo_update
from seeding import seed_torch
from rewards import RewardTracker, shaping_scale
from shm_layout import EpisodeResult

OUTCOMES = ("win", "loss", "draw", "timeout")


def build_model(cfg):
    return BattalionPolicy(ENTITY_DIM, ENEMY_PAIR_DIM, FRIEND_PAIR_DIM, GLOBAL_DIM, DEST_CELLS, cfg.hidden, cfg.heads, cfg.layers)


def dims_match(saved, model):
    return dict(saved) == dict(model.dims)


def read_snapshots(client, info):
    return [snapshot_army(client.static[army], client.dynamic[army], info.battalion_count) for army in (0, 1)]


def tick_fraction(info, steps):
    return min(steps * info.ticks_per_decision / info.max_ticks, 1.0)


def outcome_for(result, side):
    if result == EpisodeResult.DRAW:
        return "draw"
    if result == EpisodeResult.TIMEOUT:
        return "timeout"
    won = (result == EpisodeResult.PLAYER_WON) == (side == 0)
    return "win" if won else "loss"


def training_device():
    if torch.cuda.is_available():
        try:
            torch.zeros(1, device="cuda")
            return torch.device("cuda")
        except RuntimeError:
            pass
    return torch.device("cpu")


def control_mask(observation):
    return observation["cmask"]


def to_network_frame(actions, mirror):
    """Bot orders are in world coordinates; the network sees side 1 mirrored, so its left and right swap."""
    converted = {key: np.array(value, dtype=np.int64) for key, value in actions.items()}
    if mirror:
        approach = converted["approach"]
        left = approach == APPROACH_LEFT
        right = approach == APPROACH_RIGHT
        approach[left] = APPROACH_RIGHT
        approach[right] = APPROACH_LEFT
    return converted


class Battle:
    """Order books of both armies and the decision clock of one battle."""

    def __init__(self, info, decision_steps):
        self.info = info
        self.decision_steps = decision_steps
        self.books = [OrderBook(), OrderBook()]
        self.steps = 0

    def decision_due(self):
        return self.steps % self.decision_steps == 0

    def observe(self, snapshots, normalizer, update_stats):
        features = [book.features(self.info.battalion_count) for book in self.books]
        return build_observations(snapshots, self.info, tick_fraction(self.info, self.steps), normalizer, update_stats, features)

    def apply(self, side, actions, mask, mirror):
        return self.books[side].apply(actions, self.info, mirror, mask)

    def carry_out(self, client, snapshots):
        for side in (0, 1):
            self.books[side].step(client.orders[side], snapshots[side], snapshots[1 - side], self.info)
        self.steps += 1


class OpponentPool:
    """Frozen past versions of the learner, so it keeps beating its old self and not only its current self."""

    def __init__(self, directory, cfg, size):
        self.directory = directory
        self.cfg = cfg
        self.size = size
        self.model = build_model(cfg)
        self.model.eval()
        self.snapshots = []
        self.refresh()

    def refresh(self):
        """Read the pool directory again: a parallel worker picks up what the learner added or removed."""
        on_disk = sorted(glob.glob(os.path.join(self.directory, "pool_*.pt")))
        known = dict(self.snapshots)
        snapshots = []
        for path in on_disk:
            if path in known:
                snapshots.append((path, known[path]))
                continue
            try:
                payload = load_checkpoint(path)
            except (OSError, RuntimeError, EOFError):
                continue
            if payload.get("dims") == dict(self.model.dims):
                snapshots.append((path, payload["model"]))
        self.snapshots = snapshots[-self.size:]

    def add(self, learner, update_count):
        state = {key: value.detach().cpu().clone() for key, value in learner.state_dict().items()}
        path = os.path.join(self.directory, f"pool_{update_count:06d}.pt")
        save_checkpoint(path, {"model": state, "dims": dict(learner.dims)})
        self.snapshots.append((path, state))
        while len(self.snapshots) > self.size:
            old_path, _ = self.snapshots.pop(0)
            if os.path.exists(old_path):
                os.remove(old_path)

    def sample(self, rng):
        path, state = rng.choice(self.snapshots)
        self.model.load_state_dict(state)
        return os.path.basename(path)


class PlayRunner:

    def __init__(self, checkpoint_path, deterministic, cfg):
        if not os.path.exists(checkpoint_path):
            raise FileNotFoundError(f"play mode needs a trained checkpoint at {checkpoint_path}")
        payload = load_checkpoint(checkpoint_path)
        self.model = build_model(cfg)
        if not dims_match(payload["dims"], self.model):
            raise RuntimeError(f"checkpoint dimensions {dict(payload['dims'])} do not match the current code {self.model.dims}")
        self.model.load_state_dict(payload["model"])
        self.model.eval()
        self.normalizer = RunningNorm(len(RAW_FIELDS))
        self.normalizer.load_state_dict(payload["normalizer"])
        self.deterministic = deterministic
        self.cfg = cfg
        self.battle = None
        print(f"playing with {checkpoint_path}", flush=True)

    def begin_episode(self, info, seed=None):
        seed_torch(seed)
        self.battle = Battle(info, self.cfg.decision_steps)

    def on_step(self, client, result):
        if result != EpisodeResult.RUNNING:
            return
        battle = self.battle
        snapshots = read_snapshots(client, battle.info)
        if battle.decision_due():
            observations = battle.observe(snapshots, self.normalizer, False)
            actions, _, _ = select_actions(self.model, observations, self.deterministic)
            for side in (0, 1):
                battle.apply(side, actions[side], control_mask(observations[side]), side == 1)
        battle.carry_out(client, snapshots)

    def on_exit(self):
        pass


class Actor:
    """Plays training battles with a CPU copy of the network and collects what the learner needs:
    decision windows, teacher demonstrations and one log entry per learning side and battle."""

    def __init__(self, cfg, normalizer, pool, demos, flush_size, flush, report):
        self.cfg = cfg
        self.model = build_model(cfg)
        self.model.eval()
        self.normalizer = normalizer
        self.pool = pool
        self.demos = demos
        self.flush_size = flush_size
        self.flush = flush
        self.report = report
        self.buffer = RolloutBuffer()
        self.rng = random.Random()
        self.update_count = 0
        self.battle = None
        self.tracker = None
        self.mode = None
        self.learning_sides = ()
        self.opponent = None
        self.advisors = {}

    def load(self, state, update_count):
        self.model.load_state_dict(state)
        self.update_count = update_count

    def begin_episode(self, info, seed=None):
        if seed is not None:
            self.rng.seed(seed)
        seed_torch(seed)
        self.buffer.drop_pending()
        self.battle = Battle(info, self.cfg.decision_steps)
        self.tracker = RewardTracker(self.cfg.rewards, info)
        roll = self.rng.random()
        opponents = self.cfg.opponents
        if roll < opponents.scripted:
            weights = opponents.bot_weights
            self.opponent = self.rng.choices(BOTS, weights=[weights.get(bot.name, 0.0) for bot in BOTS])[0]
            self.mode = self.opponent.name
        elif roll < opponents.scripted + opponents.pool and self.pool.snapshots:
            self.opponent = self.pool
            self.pool.sample(self.rng)
            self.mode = "pool"
        else:
            self.opponent = None
            self.mode = "self"
        self.learning_sides = (0, 1) if self.opponent is None else (self.rng.randrange(2),)
        teacher = next(bot for bot in BOTS if bot.name == self.cfg.imitation.teacher)
        self.advisors = {side: type(teacher)() for side in self.learning_sides}

    def on_step(self, client, result):
        battle = self.battle
        snapshots = read_snapshots(client, battle.info)
        if result != EpisodeResult.RUNNING or battle.decision_due():
            self.tracker.step(snapshots, shaping_scale(self.cfg.rewards, self.update_count))
        if result != EpisodeResult.RUNNING:
            for side in self.learning_sides:
                self.buffer.finalize(side, self.tracker.take(side, result), True)
            self._record_episode(result)
            return
        if battle.decision_due():
            self._decide(battle, snapshots)
        battle.carry_out(client, snapshots)

    def _decide(self, battle, snapshots):
        for side in (0, 1):
            reward = self.tracker.take(side)
            if side in self.learning_sides:
                self.buffer.finalize(side, reward, False)
        observations = battle.observe(snapshots, self.normalizer, True)
        learner_obs = [observations[side] for side in self.learning_sides]
        actions, log_probs, values = select_actions(self.model, learner_obs, False)
        imitating = imitation_scale(self.cfg.imitation, self.update_count) > 0.0
        if imitating:
            for side in self.learning_sides:
                advice = self.advisors[side].act(snapshots[side], snapshots[1 - side], battle.info)
                self.demos.add(observations[side], to_network_frame(advice, side == 1))
        for index, side in enumerate(self.learning_sides):
            switched = battle.apply(side, actions[index], control_mask(observations[side]), side == 1)
            self.tracker.charge_switches(side, switched, control_mask(observations[side]))
            self.buffer.add_pending(side, {
                "obs": observations[side],
                "actions": actions[index],
                "log_prob": log_probs[index],
                "value": values[index],
            })
        if self.opponent is not None:
            side = 1 - self.learning_sides[0]
            if self.opponent is self.pool:
                opponent_actions, _, _ = select_actions(self.pool.model, [observations[side]], False)
                battle.apply(side, opponent_actions[0], control_mask(observations[side]), side == 1)
            else:
                bot_actions = self.opponent.act(snapshots[side], snapshots[1 - side], battle.info)
                battle.apply(side, bot_actions, control_mask(observations[side]), False)
                if self.opponent.name == self.cfg.imitation.teacher and imitating:
                    self.demos.add(observations[side], to_network_frame(bot_actions, side == 1))
        if self.buffer.size() >= self.flush_size:
            batch = self.buffer.build(self.cfg.gamma, self.cfg.gae_lambda)
            if batch is not None:
                self.flush(batch)

    def _record_episode(self, result):
        entries = []
        for side in self.learning_sides:
            entry = {"mode": self.mode, "outcome": outcome_for(result, side), "decisions": self.battle.steps // self.cfg.decision_steps}
            entry.update(self.tracker.summary(side))
            entries.append(entry)
        self.report(entries)


class Learner:
    """The trained network with its optimizer and normalizer, the opponent pool, the demonstrations and the log."""

    def __init__(self, checkpoint_path, cfg):
        self.checkpoint_path = checkpoint_path
        self.cfg = cfg
        self.device = training_device()
        self.model = build_model(cfg).to(self.device)
        self.optimizer = torch.optim.Adam(self.model.parameters(), lr=cfg.learning_rate)
        self.normalizer = RunningNorm(len(RAW_FIELDS))
        self.demos = DemoBuffer(cfg.imitation.buffer_size)
        self.update_count = 0
        self.episode_count = 0
        if os.path.exists(checkpoint_path):
            payload = load_checkpoint(checkpoint_path)
            if dims_match(payload["dims"], self.model):
                self.model.load_state_dict(payload["model"])
                self.optimizer.load_state_dict(payload["optimizer"])
                self.normalizer.load_state_dict(payload["normalizer"])
                self.update_count = int(payload["update_count"])
                self.episode_count = int(payload["episode_count"])
                print(f"resumed from {checkpoint_path} at update {self.update_count}", flush=True)
            else:
                aside = f"{checkpoint_path}.incompatible-{int(time.time())}"
                os.replace(checkpoint_path, aside)
                print(f"{checkpoint_path} was built for another network, moved it to {aside} and starting fresh", flush=True)
        directory = os.path.dirname(checkpoint_path) or "."
        self.pool = OpponentPool(os.path.join(directory, "pool"), cfg, cfg.opponents.pool_size)
        self.log_path = os.path.join(directory, "training_log.csv")
        self.window = []
        self.last_update_time = time.monotonic()
        print(f"training on {self.device}, pool of {len(self.pool.snapshots)} past networks", flush=True)

    def weights(self):
        return {key: value.detach().cpu().clone() for key, value in self.model.state_dict().items()}

    def record(self, entries):
        self.episode_count += 1
        self.window.extend(entries)
        if self.episode_count % self.cfg.log_every_episodes == 0:
            self._log_window()

    def _log_window(self):
        if not self.window:
            return
        parts = []
        rows = []
        for mode in sorted({entry["mode"] for entry in self.window}):
            entries = [entry for entry in self.window if entry["mode"] == mode]
            counts = {outcome: sum(1 for entry in entries if entry["outcome"] == outcome) for outcome in OUTCOMES}
            win_rate = counts["win"] / len(entries)
            parts.append(f"{mode} {counts['win']}/{counts['loss']}/{counts['timeout']}")
            rows.append((mode, len(entries), win_rate))
        crowding = np.mean([entry["crowding"] for entry in self.window])
        flank = np.mean([entry["flank"] for entry in self.window])
        exposed = np.mean([entry["exposed"] for entry in self.window])
        switching = np.mean([entry["switching"] for entry in self.window])
        decisions = np.mean([entry["decisions"] for entry in self.window])
        print(
            f"episodes {self.episode_count} updates {self.update_count} | win/loss/timeout {', '.join(parts)} | "
            f"crowding {crowding:.3f} flanking {flank:.3f} flanked {exposed:.3f} switching {switching:.3f} decisions {decisions:.0f}",
            flush=True,
        )
        new_file = not os.path.exists(self.log_path)
        with open(self.log_path, "a") as log:
            if new_file:
                log.write("episode,update,mode,battles,win_rate,crowding,flanking,flanked,switching\n")
            for mode, battles, win_rate in rows:
                log.write(f"{self.episode_count},{self.update_count},{mode},{battles},{win_rate:.3f},{crowding:.4f},{flank:.4f},{exposed:.4f},{switching:.4f}\n")
        self.window.clear()

    def update(self, batch, note=""):
        update_started = time.monotonic()
        self.model.train()
        imitation = imitation_scale(self.cfg.imitation, self.update_count)
        stats = ppo_update(self.model, self.optimizer, batch, self.cfg, self.device, self.demos, imitation)
        update_seconds = time.monotonic() - update_started
        self.update_count += 1
        cycle_seconds = time.monotonic() - self.last_update_time
        self.last_update_time = time.monotonic()
        print(
            f"update {self.update_count} samples {batch['value'].shape[0]} "
            f"policy {stats['policy_loss']:.4f} value {stats['value_loss']:.4f} "
            f"entropy {stats['entropy']:.3f} kl {stats['approx_kl']:.4f} epochs {stats['epochs']:.2f} "
            f"shaping {shaping_scale(self.cfg.rewards, self.update_count):.2f} "
            f"imitation {stats['imitation']:.3f} x{imitation:.2f} ({len(self.demos)} demos) | "
            f"cycle {cycle_seconds:.1f}s update {update_seconds:.1f}s {note}",
            flush=True,
        )
        if self.update_count % self.cfg.opponents.pool_every_updates == 0:
            self.pool.add(self.model, self.update_count)
        if self.update_count % self.cfg.save_every_updates == 0:
            self.save()

    def save(self):
        save_checkpoint(self.checkpoint_path, {
            "model": self.model.state_dict(),
            "optimizer": self.optimizer.state_dict(),
            "normalizer": self.normalizer.state_dict(),
            "dims": dict(self.model.dims),
            "update_count": self.update_count,
            "episode_count": self.episode_count,
        })
        print(f"checkpoint saved to {self.checkpoint_path}", flush=True)


class TrainRunner:
    """Training inside the game's own agent: one process plays the battles and learns from them."""

    def __init__(self, checkpoint_path, cfg):
        self.learner = Learner(checkpoint_path, cfg)
        self.actor = Actor(cfg, self.learner.normalizer, self.learner.pool, self.learner.demos,
                           cfg.steps_per_update, self._update, self.learner.record)
        self.actor.load(self.learner.weights(), self.learner.update_count)
        self.step_seconds = 0.0
        self.step_count = 0

    def begin_episode(self, info, seed=None):
        self.actor.begin_episode(info, seed)

    def on_step(self, client, result):
        started = time.monotonic()
        self.actor.on_step(client, result)
        self.step_seconds += time.monotonic() - started
        self.step_count += 1

    def _update(self, batch):
        self.learner.update(batch, f"python per step {1000.0 * self.step_seconds / max(self.step_count, 1):.1f}ms")
        self.actor.load(self.learner.weights(), self.learner.update_count)
        self.step_seconds = 0.0
        self.step_count = 0

    def on_exit(self):
        self.actor.buffer.drop_pending()
        self.learner.save()
