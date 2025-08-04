"""
Python implementation of REINFORCE on cartpole.
"""

import gymnasium as gym
import torch
import torch.nn as nn
import torch.nn.functional as F


class Policy(nn.Module):
    def __init__(self):
        super().__init__()
        self.net = nn.Sequential(nn.Linear(4, 64), nn.Tanh(), nn.Linear(64, 2))

    def forward(self, x):
        return self.net(x)


gamma = 0.99
env = gym.make("CartPole-v1")
policy = Policy()
optimizer = torch.optim.Adam(policy.parameters(), lr=1e-2)

for episode in range(1000):
    observations = []
    actions = []
    rewards = []
    log_probs = []

    # obs = [cart_position, cart_velocity, pole_angle, pole_angular_velocity]
    obs, _ = env.reset()
    done = False

    # Rollout
    while not done:
        obs_tensor = torch.tensor(obs, dtype=torch.float32).unsqueeze(0)
        # logits = policy(obs_tensor)
        # probs = F.softmax(logits, dim=-1)
        # dist = torch.distributions.Categorical(probs)
        dist = torch.distributions.Categorical(logits=policy(obs_tensor))

        action = dist.sample()
        log_prob = dist.log_prob(action)

        next_obs, reward, terminated, truncated, _ = env.step(action.item())
        done = terminated or truncated

        observations.append(obs_tensor)
        actions.append(action)
        rewards.append(reward)
        log_probs.append(log_prob)

        obs = next_obs

    # Calculate discounted returns
    returns = []
    G = 0
    for r in reversed(rewards):
        G = r + gamma * G
        returns.append(G)
    returns = list(reversed(returns))

    # Convert lists into tensors
    log_probs_tensor = torch.stack(log_probs)
    returns_tensor = torch.tensor(returns, dtype=torch.float32)

    # Normalize returns
    returns_tensor = (returns_tensor - returns_tensor.mean()) / (
        returns_tensor.std() + 1e-8
    )

    # Compute loss
    loss = -(log_probs_tensor * returns_tensor).mean()

    # Update policy
    optimizer.zero_grad()
    loss.backward()
    optimizer.step()

    print(f"Episode {episode + 1} | Length: {len(rewards)} Loss: {loss.item():.8f}")
