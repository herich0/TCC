import gymnasium as gym
from gymnasium import spaces
import numpy as np
import json
import os
import random
from .tft_env import TFTEnvironment
from .genetic_agent import GeneticAgent
from agents.heuristic_agent import HeuristicAgent

class TFTGymEnv(gym.Env):
    def __init__(self, py_dict, cpp_db, genetic_genes_path="data/top_30_genetic_agents.json"):
        super().__init__()
        self.py_dict = py_dict
        self.cpp_db = cpp_db
        
        self.action_space = spaces.Discrete(17)
        
        temp_env = TFTEnvironment(py_dict, cpp_db, num_players=8)
        temp_obs = temp_env.reset()
        obs_size = len(temp_obs[0])
        
        self.observation_space = spaces.Box(low=-10.0, high=10.0, shape=(obs_size,), dtype=np.float32)
        
        with open(genetic_genes_path, 'r') as f:
            self.bot_genes_list = json.load(f)
            
        self.pdl_rewards = [36.0, 25.0, 15.0, 10.0, -10.0, -20.0, -40.0, -70.0]
            
    def reset(self, seed=None, options=None):
        super().reset(seed=seed)
        
        self.env = TFTEnvironment(self.py_dict, self.cpp_db, num_players=8)
        self.bots = []
        
        lobbies_genes = random.sample(self.bot_genes_list, 7)
        for genes in lobbies_genes:
            self.bots.append(GeneticAgent(self.py_dict, genes))
        
        obs = self.env.reset()
        self.last_hp = 100
        self.last_micro_tick = (self.env.current_stage * 10) + self.env.current_round
        
        info = {}
        return np.array(obs[0], dtype=np.float32), info

    def step(self, action):
        reward = 0.0
        force_pass = False
        action = int(action)
        
        player = self.env.players[0]
        shop = self.env.shops[0]
        bench = player.getBench()
        gold = player.getGold()

        if action == 1: # Comprar XP
            if gold < 4 or player.getLevel() == 9:
                reward -= 0.05
                force_pass = True
                
        elif action == 2: 
            if gold < 2:
                reward -= 0.05
                force_pass = True
                
        elif action >= 3 and action <= 7: 
            shop_idx = action - 3
            champ_name = shop[shop_idx]
            has_space = any(c is None for c in bench)
            
            if champ_name == "Empty" or not has_space:
                reward -= 0.05
                force_pass = True
            else:
                cost = self.py_dict.get(champ_name, {}).get("cost", 1)
                if gold < cost:
                    reward -= 0.05
                    force_pass = True
                    
        elif action >= 8 and action <= 16:
            bench_idx = action - 8
            if bench[bench_idx] is None:
                reward -= 0.05
                force_pass = True

        if force_pass:
            action = 0 
            
        actions = [action]
        for i in range(1, 8):
            if self.env.alive_status[i]:
                actions.append(self.bots[i-1].get_action(i, self.env))
            else:
                actions.append(0)
                
        self.env.step(actions)
        
        obs = self.env._get_all_observations()
        state = np.array(obs[0], dtype=np.float32)
        
        current_hp = player.getHp()
        current_macro_tick = (self.env.current_stage * 10) + self.env.current_round
        
        if current_macro_tick != self.last_macro_tick:
            if current_hp == self.last_hp:
                reward += 2.0 
                
            self.last_hp = current_hp
            self.last_macro_tick = current_macro_tick
            
        terminated = current_hp <= 0
        truncated = self.env.current_stage >= 8
        
        if terminated or truncated:
            alive_count = sum(1 for a in self.env.alive_status if a)
            placement = alive_count + 1 if terminated else 1
            
            reward += self.pdl_rewards[placement - 1]
            
        info = {"placement": sum(1 for a in self.env.alive_status if a) if terminated else 1}
        
        return state, reward, terminated, truncated, info