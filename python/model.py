import numpy as np
import torch
import torch.nn as nn
from torch.distributions import Categorical

from config import (
    ACTION_ATTACK,
    ACTION_COUNT,
    ACTION_FORM_LINE,
    ACTION_MOVE,
    ACTION_SCREEN,
    APPROACH_COUNT,
)

NEGATIVE = -1e9
OBS_KEYS = ["friend", "enemy", "epair", "fpair", "glob", "fmask", "emask", "cmask"]
ACTION_KEYS = ["type", "pace", "dest", "target", "approach", "screen"]
MOVING_KINDS = (ACTION_MOVE, ACTION_ATTACK, ACTION_FORM_LINE, ACTION_SCREEN)


def mlp(input_dim, hidden_dim, output_dim):
    return nn.Sequential(
        nn.Linear(input_dim, hidden_dim),
        nn.ReLU(),
        nn.Linear(hidden_dim, hidden_dim),
        nn.ReLU(),
        nn.Linear(hidden_dim, output_dim),
    )


class BattalionPolicy(nn.Module):
    """Entity transformer: every battalion of both armies is a token, so each one decides
    knowing where all the others stand. Heads read each friendly token; targets and screened
    friends are scored pairwise with the geometric pair features."""

    def __init__(self, entity_dim, enemy_pair_dim, friend_pair_dim, global_dim, dest_cells, hidden=128, heads=4, layers=3):
        super().__init__()
        self.dims = {
            "entity": entity_dim,
            "enemy_pair": enemy_pair_dim,
            "friend_pair": friend_pair_dim,
            "global": global_dim,
            "dest": dest_cells,
            "hidden": hidden,
            "heads": heads,
            "layers": layers,
        }
        self.friend_input = nn.Linear(entity_dim, hidden)
        self.enemy_input = nn.Linear(entity_dim, hidden)
        self.global_input = nn.Linear(global_dim, hidden)
        layer = nn.TransformerEncoderLayer(hidden, heads, dim_feedforward=2 * hidden, dropout=0.0, batch_first=True, norm_first=True)
        self.encoder = nn.TransformerEncoder(layer, layers, enable_nested_tensor=False)
        self.output_norm = nn.LayerNorm(hidden)
        self.type_head = mlp(hidden, hidden, ACTION_COUNT)
        self.pace_head = nn.Linear(hidden, 2)
        self.dest_head = mlp(hidden, hidden, dest_cells)
        self.approach_head = mlp(2 * hidden + enemy_pair_dim, hidden, APPROACH_COUNT)
        self.target_scorer = mlp(2 * hidden + enemy_pair_dim, hidden, 1)
        self.friend_scorer = mlp(2 * hidden + friend_pair_dim, hidden, 1)
        self.value_head = mlp(2 * hidden, hidden, 1)

    def forward(self, friend, enemy, epair, fpair, glob, fmask, emask):
        own_count = friend.shape[1]
        tokens = torch.cat([
            self.global_input(glob).unsqueeze(1),
            self.friend_input(friend),
            self.enemy_input(enemy),
        ], dim=1)
        always = torch.ones_like(fmask[:, :1])
        padding = ~torch.cat([always, fmask, emask], dim=1)
        encoded = self.output_norm(self.encoder(tokens, src_key_padding_mask=padding))
        encoded = encoded.masked_fill(padding.unsqueeze(-1), 0.0)
        context = encoded[:, 0]
        unit = encoded[:, 1:1 + own_count]
        foe = encoded[:, 1 + own_count:]
        enemy_count = foe.shape[1]

        type_logits = self.type_head(unit)
        no_enemy = ~emask.any(dim=-1, keepdim=True)
        no_friend = fmask.sum(dim=-1, keepdim=True) < 2
        type_logits = type_logits.clone()
        type_logits[..., ACTION_ATTACK] = type_logits[..., ACTION_ATTACK].masked_fill(no_enemy, NEGATIVE)
        type_logits[..., ACTION_SCREEN] = type_logits[..., ACTION_SCREEN].masked_fill(no_friend, NEGATIVE)

        pair_input = torch.cat([
            unit.unsqueeze(2).expand(-1, -1, enemy_count, -1),
            foe.unsqueeze(1).expand(-1, own_count, -1, -1),
            epair,
        ], dim=-1)
        target_logits = self.target_scorer(pair_input).squeeze(-1).masked_fill(~emask.unsqueeze(1), NEGATIVE)

        approach_logits = self.approach_head(pair_input)

        friend_input = torch.cat([
            unit.unsqueeze(2).expand(-1, -1, own_count, -1),
            unit.unsqueeze(1).expand(-1, own_count, -1, -1),
            fpair,
        ], dim=-1)
        eye = torch.eye(own_count, dtype=torch.bool, device=unit.device).unsqueeze(0)
        friend_valid = fmask.unsqueeze(1) & ~eye
        friend_logits = self.friend_scorer(friend_input).squeeze(-1).masked_fill(~friend_valid, NEGATIVE)

        value = self.value_head(torch.cat([unit, context.unsqueeze(1).expand(-1, own_count, -1)], dim=-1)).squeeze(-1)
        return {
            "type": type_logits,
            "pace": self.pace_head(unit),
            "dest": self.dest_head(unit),
            "target": target_logits,
            "approach": approach_logits,
            "screen": friend_logits,
        }, value


def _forward(model, obs):
    return model(obs["friend"], obs["enemy"], obs["epair"], obs["fpair"], obs["glob"], obs["fmask"], obs["emask"])


def _distributions(logits, target):
    index = target[..., None, None].expand(-1, -1, 1, logits["approach"].shape[-1])
    chosen = dict(logits, approach=torch.gather(logits["approach"], 2, index).squeeze(2))
    return {key: Categorical(logits=chosen[key]) for key in ACTION_KEYS}


def _uses(kind):
    moving = torch.zeros_like(kind, dtype=torch.float32)
    for k in MOVING_KINDS:
        moving = moving + (kind == k).float()
    return {
        "type": torch.ones_like(moving),
        "pace": moving,
        "dest": (kind == ACTION_MOVE).float(),
        "target": (kind == ACTION_ATTACK).float(),
        "approach": (kind == ACTION_ATTACK).float(),
        "screen": (kind == ACTION_SCREEN).float(),
    }


def _log_prob(dists, actions):
    uses = _uses(actions["type"])
    return sum(dists[key].log_prob(actions[key]) * uses[key] for key in ACTION_KEYS)


def evaluate_actions(model, obs, actions):
    logits, value = _forward(model, obs)
    dists = _distributions(logits, actions["target"])
    probs = dists["type"].probs
    moving = sum(probs[..., k] for k in MOVING_KINDS)
    entropy = (
        dists["type"].entropy()
        + moving * dists["pace"].entropy()
        + probs[..., ACTION_MOVE] * dists["dest"].entropy()
        + probs[..., ACTION_ATTACK] * (dists["target"].entropy() + dists["approach"].entropy())
        + probs[..., ACTION_SCREEN] * dists["screen"].entropy()
    )
    return _log_prob(dists, actions), entropy, value


def to_tensors(observations, device):
    batch = {}
    for key in OBS_KEYS:
        stacked = np.stack([observation[key] for observation in observations])
        dtype = torch.bool if stacked.dtype == np.bool_ else torch.float32
        batch[key] = torch.as_tensor(stacked, dtype=dtype, device=device)
    return batch


@torch.no_grad()
def select_actions(model, observations, deterministic):
    obs = to_tensors(observations, "cpu")
    logits, value = _forward(model, obs)
    pick = (lambda d: d.probs.argmax(dim=-1)) if deterministic else (lambda d: d.sample())
    target = pick(Categorical(logits=logits["target"]))
    dists = _distributions(logits, target)
    actions = {key: pick(dists[key]) for key in ACTION_KEYS}
    actions["target"] = target
    log_prob = _log_prob(dists, actions)
    per_side = [{key: actions[key][index].numpy() for key in ACTION_KEYS} for index in range(len(observations))]
    return per_side, log_prob.numpy(), value.numpy()

