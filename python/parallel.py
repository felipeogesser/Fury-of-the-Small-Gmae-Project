"""Parallel training: one learner on the GPU, many games playing on the CPU.

Run from the repository root:

    .venv/bin/python python/parallel.py --workers 8

Each worker is a windowless ./game --ai in checkpoints/workers/<n>. The game starts its own python agent as
usual; that agent sees CGAME_LEARNER and plays as an actor for this learner instead of training on its own.
Actors send their decision windows, teacher demonstrations, battle results and normalizer statistics; the
learner runs PPO on all of it and sends the new weights back. Ctrl-C stops the workers and saves."""

import argparse
import os
import queue
import secrets
import signal
import subprocess
import sys
import threading
import time
from multiprocessing.connection import Client, Listener

import numpy as np
import torch

from config import TrainConfig
from observation import RAW_FIELDS, RunningNorm
from runners import Actor, Learner, OpponentPool

LINKED = ("assets", ".venv", "python", "game", "battlefield_formation_layout.ini", "inventory.json", "tutorial.csv", "tutorial.json")


class TrackedNorm(RunningNorm):
    """The worker's normalizer: normalizes with the learner's statistics and keeps what it saw since the last send."""

    def __init__(self, size):
        super().__init__(size)
        self.delta = self._empty(size)

    @staticmethod
    def _empty(size):
        delta = RunningNorm(size)
        delta.var = np.zeros(size, dtype=np.float64)
        delta.count = 0.0
        return delta

    def update(self, values):
        super().update(values)
        self.delta.update(values)

    def take_delta(self):
        delta = self.delta
        self.delta = self._empty(self.mean.shape[0])
        return delta.mean, delta.var, delta.count

    def adopt(self, mean, var, count):
        self.mean = np.array(mean, dtype=np.float64)
        self.var = np.array(var, dtype=np.float64)
        self.count = float(count)


class DemoSink:

    def __init__(self):
        self.items = []

    def add(self, obs, actions):
        self.items.append((obs, actions))

    def take(self):
        items = self.items
        self.items = []
        return items


class ActorRunner:
    """The game's agent when a parallel learner runs: plays, ships experience in chunks, takes the new weights."""

    def __init__(self, cfg, address, authkey, pool_directory):
        torch.set_num_threads(1)
        self.cfg = cfg
        self.wants_stop = False
        self.connection = Client(address, authkey=authkey)
        self.normalizer = TrackedNorm(len(RAW_FIELDS))
        self.demos = DemoSink()
        self.episodes = []
        self.pool = OpponentPool(pool_directory, cfg, cfg.opponents.pool_size)
        self.actor = Actor(cfg, self.normalizer, self.pool, self.demos, cfg.parallel.chunk_size, self._send, self.episodes.append)
        self.version = None
        self.chunk_version = None
        self.connection.send({"hello": os.getpid()})
        self._apply(self.connection.recv())
        self.chunk_version = self.version

    def begin_episode(self, info):
        self.actor.begin_episode(info)

    def on_step(self, client, result):
        self.actor.on_step(client, result)

    def _send(self, batch):
        self.connection.send({
            "version": self.chunk_version,
            "batch": batch,
            "demos": self.demos.take(),
            "episodes": self.episodes[:],
            "norm": self.normalizer.take_delta(),
        })
        self.episodes.clear()
        self.chunk_version = self.version
        self._apply(self.connection.recv())

    def _apply(self, reply):
        if reply.get("stop"):
            self.wants_stop = True
            return
        self.normalizer.adopt(*reply["norm"])
        if "weights" in reply:
            previous = self.actor.update_count
            self.actor.load({key: torch.from_numpy(value) for key, value in reply["weights"].items()}, reply["update_count"])
            self.version = reply["version"]
            every = self.cfg.opponents.pool_every_updates
            if previous // every != reply["update_count"] // every:
                self.pool.refresh()

    def on_exit(self):
        try:
            self.connection.close()
        except OSError:
            pass


def actor_from_environment(cfg):
    address = os.environ.get("CGAME_LEARNER")
    if not address:
        return None
    host, port = address.rsplit(":", 1)
    return ActorRunner(cfg, (host, int(port)), bytes.fromhex(os.environ["CGAME_AUTHKEY"]), os.environ["CGAME_POOL"])


class LearnerServer:
    """Takes chunks from the actors on one thread each and trains on them on the main thread."""

    def __init__(self, cfg, checkpoint_path):
        self.cfg = cfg
        self.learner = Learner(checkpoint_path, cfg)
        self.authkey = secrets.token_bytes(16)
        self.listener = Listener(("127.0.0.1", 0), authkey=self.authkey)
        self.inbox = queue.Queue()
        self.lock = threading.Lock()
        self.stopping = threading.Event()
        self._publish_norm()
        self._publish_weights()
        self.samples = 0
        self.dropped = 0
        threading.Thread(target=self._accept, daemon=True).start()

    @property
    def address(self):
        host, port = self.listener.address
        return f"{host}:{port}"

    def _publish_weights(self):
        weights = {key: value.numpy() for key, value in self.learner.weights().items()}
        with self.lock:
            self.weights = weights
            self.version = self.learner.update_count

    def _publish_norm(self):
        norm = self.learner.normalizer
        with self.lock:
            self.norm = (norm.mean.copy(), norm.var.copy(), norm.count)

    def _reply(self, version):
        if self.stopping.is_set():
            return {"stop": True}
        with self.lock:
            reply = {"norm": self.norm}
            if version != self.version:
                reply.update(weights=self.weights, version=self.version, update_count=self.version)
        return reply

    def _accept(self):
        while True:
            try:
                connection = self.listener.accept()
            except OSError:
                return
            threading.Thread(target=self._serve, args=(connection,), daemon=True).start()

    def _serve(self, connection):
        try:
            connection.recv()
            connection.send(self._reply(None))
            while True:
                message = connection.recv()
                self.inbox.put(message)
                connection.send(self._reply(message["version"]))
        except (EOFError, OSError):
            pass
        finally:
            connection.close()

    def train(self, should_stop, note):
        pending = []
        pending_samples = 0
        while not should_stop():
            try:
                message = self.inbox.get(timeout=1.0)
            except queue.Empty:
                continue
            self.learner.normalizer.merge(*message["norm"])
            self._publish_norm()
            for obs, actions in message["demos"]:
                self.learner.demos.add(obs, actions)
            for entries in message["episodes"]:
                self.learner.record(entries)
            batch = message["batch"]
            count = batch["value"].shape[0]
            if self.learner.update_count - message["version"] > self.cfg.parallel.max_lag:
                self.dropped += count
                continue
            pending.append(batch)
            pending_samples += count
            self.samples += count
            if pending_samples < self.cfg.steps_per_update:
                continue
            merged = {key: np.concatenate([part[key] for part in pending]) for key in pending[0]}
            pending.clear()
            pending_samples = 0
            self.learner.update(merged, note())
            self._publish_weights()

    def close(self):
        self.stopping.set()
        try:
            self.listener.close()
        except OSError:
            pass


def prepare_worker(root, index):
    directory = os.path.join(root, "checkpoints", "workers", f"{index:02d}")
    os.makedirs(os.path.join(directory, "stages"), exist_ok=True)
    for name in LINKED:
        link = os.path.join(directory, name)
        if not os.path.lexists(link):
            os.symlink(os.path.join(root, name), link)
    return directory


class Workers:

    def __init__(self, root, count, server):
        self.root = root
        self.server = server
        self.directories = [prepare_worker(root, index) for index in range(count)]
        self.processes = [None] * count
        self.env = dict(
            os.environ,
            SDL_VIDEODRIVER="dummy",
            CGAME_LEARNER=server.address,
            CGAME_AUTHKEY=server.authkey.hex(),
            CGAME_POOL=os.path.join(root, "checkpoints", "pool"),
            OMP_NUM_THREADS="1",
            MKL_NUM_THREADS="1",
        )
        self.restarts = 0

    def start(self, index):
        directory = self.directories[index]
        log = open(os.path.join(directory, "game.log"), "ab")
        self.processes[index] = subprocess.Popen(["./game", "--ai"], cwd=directory, env=self.env,
                                                 stdout=log, stderr=subprocess.STDOUT, start_new_session=True)
        log.close()

    def keep_running(self):
        for index, process in enumerate(self.processes):
            if process is None:
                self.start(index)
            elif process.poll() is not None:
                print(f"worker {index} exited with code {process.returncode}, restarting (see {self.directories[index]}/game.log)", flush=True)
                self.restarts += 1
                self.start(index)

    def alive(self):
        return sum(1 for process in self.processes if process is not None and process.poll() is None)

    def stop(self, timeout):
        deadline = time.monotonic() + timeout
        while self.alive() and time.monotonic() < deadline:
            time.sleep(0.5)
        for process in self.processes:
            if process is not None and process.poll() is None:
                process.terminate()
        for process in self.processes:
            if process is not None:
                try:
                    process.wait(timeout=5)
                except subprocess.TimeoutExpired:
                    process.kill()


def main():
    parser = argparse.ArgumentParser()
    cfg = TrainConfig()
    cfg.steps_per_update = cfg.parallel.steps_per_update
    cfg.minibatch_size = cfg.parallel.minibatch_size
    cfg.imitation.minibatch_size = cfg.parallel.minibatch_size
    parser.add_argument("--workers", type=int, default=cfg.parallel.workers)
    parser.add_argument("--checkpoint", default="checkpoints/latest.pt")
    parser.add_argument("--updates", type=int, default=0, help="stop after this many updates (0: run until Ctrl-C)")
    arguments = parser.parse_args()
    root = os.getcwd()
    if not os.path.exists(os.path.join(root, "game")):
        sys.exit("run this from the repository root, next to ./game")

    stop = threading.Event()
    signal.signal(signal.SIGINT, lambda signum, frame: stop.set())
    signal.signal(signal.SIGTERM, lambda signum, frame: stop.set())

    server = LearnerServer(cfg, arguments.checkpoint)
    workers = Workers(root, arguments.workers, server)
    target = server.learner.update_count + arguments.updates
    started = time.monotonic()
    print(f"{arguments.workers} workers, learner at {server.address}", flush=True)

    def should_stop():
        if stop.is_set() or (arguments.updates > 0 and server.learner.update_count >= target):
            return True
        workers.keep_running()
        return False

    def note():
        rate = server.samples / max(time.monotonic() - started, 1e-6)
        return f"| {workers.alive()} workers {rate:.0f} samples/s, {server.dropped} stale dropped, {workers.restarts} restarts"

    try:
        server.train(should_stop, note)
    finally:
        print("stopping workers", flush=True)
        server.close()
        workers.stop(timeout=60)
        server.learner.save()


if __name__ == "__main__":
    main()
