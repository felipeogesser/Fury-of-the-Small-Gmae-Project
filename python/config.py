from dataclasses import dataclass, field

MAX_BATTALIONS = 20
DEST_GRID_X = 8
DEST_GRID_Y = 6

# macro orders the network chooses; actions.py turns them into C orders every step
ACTION_KEEP = 0
ACTION_HOLD = 1
ACTION_MOVE = 2
ACTION_ATTACK = 3
ACTION_FORM_LINE = 4
ACTION_SCREEN = 5
ACTION_ADVANCE = 6
ACTION_COUNT = 7
ACTION_NAMES = ["keep", "hold", "move", "attack", "form_line", "screen", "advance"]

# side of the target an attack goes for, in the target's own frame
APPROACH_FRONT = 0
APPROACH_LEFT = 1
APPROACH_RIGHT = 2
APPROACH_REAR = 3
APPROACH_COUNT = 4

# C order types (ai_order_types.h)
ORDER_HOLD = 0
ORDER_MOVE = 1
ORDER_ATTACK = 2

PACE_WALK = 0
PACE_RUN = 1

# battalion_types.h
TYPE_TEMPLAR = 0
TYPE_CAVALRY = 1
TYPE_GUARDSMEN = 2
TYPE_SWORDSMEN = 3
TYPE_ARCHERS = 4

# formation geometry, same as unit_table.c
FILE_SPACING = 28.0
RANK_SPACING = 32.0
GENERAL_LEAD = 36.0

FLANK_SIDE = 1
FLANK_REAR = 2

ENUM_SIZES = {
    "battalion_type": 8,
    "headless_movement_mode": 4,
    "fatigue_state": 6,
    "morale_state": 6,
    "lifecycle_state": 4,
    "movement_state": 6,
    "movement_pace": 2,
    "flank_exposure": 3,
    "flank_attack": 3,
}

ROUTING_LIFECYCLE = 2


@dataclass
class RewardConfig:
    hp_trade: float = 1.0 # enemy hp fraction lost minus own hp fraction lost
    rout: float = 0.4
    win: float = 1.0
    timeout: float = -0.25
    time: float = 0.002
    switch_cost: float = 0.01
    # per battalion: these are what teach cohesion instead of blobbing
    crowding: float = 0.012 # per decision, per unit of footprint overlap with a friendly battalion
    flank_attack: float = 0.003 # per decision spent hitting a side (x1) or the rear (x2)
    flank_exposed: float = 0.002 # per decision an own battalion is hit in a side (x1) or the rear (x2)
    isolation: float = 0.001 # per decision a non-cavalry battalion stands far from every friend
    isolation_radii: float = 6.0 # footprint radii to the nearest friend before isolation counts
    anneal_updates: int = 3000
    floor: float = 0.5


@dataclass
class OpponentConfig:
    # chance per battle of each opponent kind; the rest is self-play with both sides learning
    pool: float = 0.25
    scripted: float = 0.25
    pool_every_updates: int = 25
    pool_size: int = 12 # kept next to the checkpoint, in pool/
    # how often each bot is picked when a scripted battle comes up
    bot_weights: dict = field(default_factory=lambda: {"commander": 0.55, "flank": 0.2, "line": 0.1, "rush": 0.15})


@dataclass
class ImitationConfig:
    # the network copies the teacher bot's orders, strongly at first and less as it learns to win on its own
    teacher: str = "commander"
    coef: float = 0.5
    anneal_updates: int = 1500     # the imitation weight falls linearly to floor over this many updates
    floor: float = 0.0
    buffer_size: int = 20000       # teacher decisions kept, oldest dropped first
    minibatch_size: int = 128


@dataclass
class ParallelConfig:
    workers: int = 16
    chunk_size: int = 128
    max_lag: int = 1
    steps_per_update: int = 8192
    minibatch_size: int = 512


@dataclass
class TrainConfig:
    battles_per_layout: int = 30
    stage_path: str = "stages/current.csv"
    stage_min_battalions: int = 4
    stage_max_battalions: int = 12
    decision_steps: int = 8
    hidden: int = 128
    heads: int = 4
    layers: int = 3
    learning_rate: float = 3e-4
    gamma: float = 0.99
    gae_lambda: float = 0.95
    clip: float = 0.2
    value_coef: float = 0.5
    entropy_coef: float = 0.01
    max_grad_norm: float = 0.5
    epochs: int = 4
    target_kl: float = 0.2
    minibatch_size: int = 128
    steps_per_update: int = 2048
    save_every_updates: int = 10
    log_every_episodes: int = 20
    rewards: RewardConfig = field(default_factory=RewardConfig)
    opponents: OpponentConfig = field(default_factory=OpponentConfig)
    imitation: ImitationConfig = field(default_factory=ImitationConfig)
    parallel: ParallelConfig = field(default_factory=ParallelConfig)
