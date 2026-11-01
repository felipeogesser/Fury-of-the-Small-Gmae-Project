import numpy as np
import torch
import torch.nn as nn

from model import ACTION_KEYS, OBS_KEYS, evaluate_actions


class RolloutBuffer:
    """Decision windows of the learning armies; reward, value and advantage are per battalion."""

    def __init__(self):
        self.items = [[], []]
        self.pending = [None, None]

    def add_pending(self, side, transition):
        self.pending[side] = transition

    def finalize(self, side, reward, done):
        transition = self.pending[side]
        if transition is None:
            return
        transition["reward"] = reward
        transition["done"] = done
        self.items[side].append(transition)
        self.pending[side] = None

    def drop_pending(self):
        self.pending = [None, None]

    def size(self):
        return len(self.items[0]) + len(self.items[1])

    def build(self, gamma, lam):
        collected = []
        for side in (0, 1):
            items = self.items[side]
            if not items:
                continue
            bootstrap = self.pending[side]
            advantage = np.zeros_like(items[-1]["value"])
            for t in reversed(range(len(items))):
                following = items[t + 1] if t + 1 < len(items) else bootstrap
                if following is None or items[t]["done"]:
                    next_value = np.zeros_like(advantage)
                    alive = np.zeros_like(advantage)
                else:
                    # a battalion that is gone in the next window ends its own episode here
                    alive = following["obs"]["fmask"].astype(np.float32)
                    next_value = following["value"] * alive
                delta = items[t]["reward"] + gamma * next_value - items[t]["value"]
                advantage = delta + gamma * lam * alive * advantage
                items[t]["advantage"] = advantage.astype(np.float32)
                items[t]["return"] = (advantage + items[t]["value"]).astype(np.float32)
            collected.extend(items)
        self.items = [[], []]
        if not collected:
            return None
        return _collate(collected)


def _collate(items):
    batch = {}
    for key in OBS_KEYS:
        batch[key] = np.stack([item["obs"][key] for item in items])
    for key in ACTION_KEYS:
        batch[key] = np.stack([item["actions"][key] for item in items]).astype(np.int64)
    for key in ("log_prob", "value", "advantage", "return"):
        batch[key] = np.stack([item[key] for item in items]).astype(np.float32)
    return batch


def ppo_update(model, optimizer, batch, cfg, device):
    count = batch["value"].shape[0]
    control = batch["cmask"]
    advantage = batch["advantage"]
    if control.sum() > 1:
        mean = advantage[control].mean()
        std = advantage[control].std()
        advantage = (advantage - mean) / (std + 1e-8)
    tensors = {}
    for key in OBS_KEYS:
        dtype = torch.bool if batch[key].dtype == np.bool_ else torch.float32
        tensors[key] = torch.as_tensor(batch[key], dtype=dtype, device=device)
    for key in ACTION_KEYS:
        tensors[key] = torch.as_tensor(batch[key], dtype=torch.int64, device=device)
    old_log_prob = torch.as_tensor(batch["log_prob"], device=device)
    advantage_tensor = torch.as_tensor(advantage, dtype=torch.float32, device=device)
    returns = torch.as_tensor(batch["return"], device=device)
    totals = {"policy_loss": 0.0, "value_loss": 0.0, "entropy": 0.0, "approx_kl": 0.0}
    batches = 0
    for _ in range(cfg.epochs):
        permutation = torch.randperm(count, device=device)
        for start in range(0, count, cfg.minibatch_size):
            index = permutation[start:start + cfg.minibatch_size]
            obs = {key: tensors[key][index] for key in OBS_KEYS}
            actions = {key: tensors[key][index] for key in ACTION_KEYS}
            log_prob, entropy, value = evaluate_actions(model, obs, actions)
            mask = obs["cmask"].float()
            denominator = mask.sum().clamp(min=1.0)
            ratio = torch.exp((log_prob - old_log_prob[index]) * mask)
            minibatch_advantage = advantage_tensor[index]
            surrogate = torch.min(ratio * minibatch_advantage, torch.clamp(ratio, 1.0 - cfg.clip, 1.0 + cfg.clip) * minibatch_advantage)
            policy_loss = -(surrogate * mask).sum() / denominator
            entropy_mean = (entropy * mask).sum() / denominator
            value_mask = obs["fmask"].float()
            value_loss = (((value - returns[index]) ** 2) * value_mask).sum() / value_mask.sum().clamp(min=1.0)
            loss = policy_loss + cfg.value_coef * value_loss - cfg.entropy_coef * entropy_mean
            optimizer.zero_grad()
            loss.backward()
            nn.utils.clip_grad_norm_(model.parameters(), cfg.max_grad_norm)
            optimizer.step()
            with torch.no_grad():
                approx_kl = (((ratio - 1.0) - torch.log(ratio)) * mask).sum() / denominator
            totals["policy_loss"] += policy_loss.item()
            totals["value_loss"] += value_loss.item()
            totals["entropy"] += entropy_mean.item()
            totals["approx_kl"] += approx_kl.item()
            batches += 1
    return {key: value / max(batches, 1) for key, value in totals.items()}
