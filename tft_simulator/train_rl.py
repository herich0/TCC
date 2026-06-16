import sys
import os
import json
import datetime
import numpy as np
import matplotlib.pyplot as plt
from stable_baselines3 import PPO
from stable_baselines3.common.vec_env import DummyVecEnv
from stable_baselines3.common.monitor import Monitor
from stable_baselines3.common.callbacks import BaseCallback

sys.path.append(os.path.join(os.path.dirname(__file__), "build", "Release"))
import tft_engine as tft
from envs.tft_gym_env import TFTGymEnv

class TCCLoggerCallback(BaseCallback):
    def __init__(self, log_dir, verbose=0):
        super().__init__(verbose)
        self.log_dir = log_dir
        self.episode_rewards = []
        self.episode_placements = []
        self.current_rewards = 0.0

    def _on_step(self):
        self.current_rewards += float(self.locals["rewards"][0])
        
        if self.locals["dones"][0]:
            self.episode_rewards.append(self.current_rewards)
            info = self.locals["infos"][0]
            if "placement" in info:
                self.episode_placements.append(int(info["placement"]))
            self.current_rewards = 0.0
        return True

    def _on_training_end(self):
        timestamp = datetime.datetime.now().strftime("%Y%m%d_%H%M%S")
        data_file = os.path.join(self.log_dir, f"rl_metrics_{timestamp}.json")
        
        metrics = {
            "episode_rewards": self.episode_rewards,
            "episode_placements": self.episode_placements
        }
        
        with open(data_file, "w") as f:
            json.dump(metrics, f, indent=4)
            
        plt.figure(figsize=(12, 5))
        
        plt.subplot(1, 2, 1)
        plt.plot(self.episode_rewards, alpha=0.6, color='blue')
        if len(self.episode_rewards) >= 50:
            plt.plot(np.convolve(self.episode_rewards, np.ones(50)/50, mode='valid'), color='darkblue', linewidth=2)
        plt.title('Recompensa por Episódio')
        plt.xlabel('Episódio')
        plt.ylabel('Recompensa')
        
        plt.subplot(1, 2, 2)
        plt.plot(self.episode_placements, alpha=0.4, color='red')
        if len(self.episode_placements) >= 50:
            plt.plot(np.convolve(self.episode_placements, np.ones(50)/50, mode='valid'), color='darkred', linewidth=2)
        plt.title('Colocação (1º ao 8º)')
        plt.xlabel('Episódio')
        plt.ylabel('Posição Final')
        plt.gca().invert_yaxis() 
        
        plot_file = os.path.join(self.log_dir, f"rl_training_plot_{timestamp}.png")
        plt.savefig(plot_file)
        plt.close()

def load_champions_data():
    json_path = os.path.join(os.path.dirname(__file__), "data", "toy_set.json")
    with open(json_path, 'r', encoding='utf-8') as f:
        py_dict = json.load(f)
    return py_dict, json_path

def run_rl_training():
    print("--- INICIANDO TREINAMENTO PPO (REINFORCEMENT LEARNING) ---")
    
    py_dict, json_path = load_champions_data()
    cpp_db = tft.loadChampionsFromJson(json_path)
    
    env = TFTGymEnv(py_dict, cpp_db)
    env = Monitor(env)
    vec_env = DummyVecEnv([lambda: env])
    
    log_dir = "data"
    os.makedirs(log_dir, exist_ok=True)
    tcc_logger = TCCLoggerCallback(log_dir=log_dir)
    
    model = PPO("MlpPolicy", vec_env, verbose=1, n_steps=2048, ent_coef=0.005, tensorboard_log="./tft_tensorboard/")
    
    print("Treinando a Rede Neural... (Pressione Ctrl+C para interromper e salvar gráficos)")
    try:
        model.learn(total_timesteps=10000000, callback=tcc_logger) 
    except KeyboardInterrupt:
        print("\nTreinamento interrompido. Processando dados...")
        
    tcc_logger._on_training_end()
    model.save("data/ppo_tft_agent")
    print("\nModelo salvo com sucesso em data/ppo_tft_agent.zip")

if __name__ == "__main__":
    run_rl_training()