import argparse
import signal
import sys
import time
import traceback

from config import TrainConfig
from seeding import battle_seed, run_seed
from shm_client import SharedMemoryClient
from shm_layout import EpisodeResult

class StopFlag:

    def __init__(self):
        self.requested = False
        signal.signal(signal.SIGTERM, self._handle)
        signal.signal(signal.SIGINT, self._handle)

    def _handle(self, signum, frame):
        self.requested = True


def parse_arguments():
    parser = argparse.ArgumentParser()
    parser.add_argument("--shm", required=True)
    parser.add_argument("--mode", choices=["train", "play"], required=True)
    parser.add_argument("--checkpoint", required=True)
    parser.add_argument("--deterministic", action="store_true")
    return parser.parse_args()


def run(client, runner, rotation, stop):
    current_episode = None
    while not stop.requested and not getattr(runner, "wants_stop", False):
        if client.c_shutdown():
            break
        if not client.battle_active():
            if not client.c_alive():
                break
            time.sleep(0.01)
            continue
        episode = client.episode_index()
        if episode != current_episode:
            runner.begin_episode(client.refresh_battle(), battle_seed(run_seed(), episode))
            current_episode = episode
        step = client.wait_for_step(lambda: stop.requested)
        if step is None:
            continue
        result = client.episode_result()
        runner.on_step(client, result)
        if result != EpisodeResult.RUNNING and rotation is not None:
            rotation.on_battle_end()
        client.set_py_step(step)


def main():
    arguments = parse_arguments()
    stop = StopFlag()
    client = SharedMemoryClient(arguments.shm)
    runner = None
    rotation = None
    exit_code = 0
    try:
        if arguments.mode == "train":
            from parallel import actor_from_environment
            from runners import TrainRunner
            from stage_rotation import StageRotation
            config = TrainConfig()
            runner = actor_from_environment(config) or TrainRunner(arguments.checkpoint, config)
            rotation = StageRotation(config.stage_path, config.battles_per_layout, config.stage_min_battalions, config.stage_max_battalions, seed=run_seed())
            rotation.new_layout()
        else:
            from runners import PlayRunner
            runner = PlayRunner(arguments.checkpoint, arguments.deterministic, TrainConfig())
        client.set_ready()
        run(client, runner, rotation, stop)
    except Exception:
        traceback.print_exc()
        exit_code = 1
    finally:
        if runner is not None:
            try:
                runner.on_exit()
            except Exception:
                traceback.print_exc()
                exit_code = 1
        client.set_shutdown()
        client.close()
    sys.exit(exit_code)


if __name__ == "__main__":
    main()