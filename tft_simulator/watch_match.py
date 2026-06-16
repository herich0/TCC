import sys
import os
import json
import numpy as np
from stable_baselines3 import PPO

build_dir = os.path.join(os.path.dirname(__file__), "build")
sys.path.append(build_dir)
sys.path.append(os.path.join(build_dir, "Release"))
import tft_engine as tft
from envs.tft_gym_env import TFTGymEnv

def print_lobby_status(env_core, stage, round_num):
    print(f"\n{'='*50}")
    print(f" RADIOGRAFIA DO LOBBY - ESTÁGIO {stage}-{round_num}")
    print(f"{'='*50}")
    
    players_data = []
    for i, p in enumerate(env_core.players):
        hp = max(0, p.getHp())
        lvl = p.getLevel()
        gold = p.getGold()
        
        board = env_core.boards[i]
        units = []
        for y in range(4):
            for x in range(7):
                champ = board.getChampion(x, y)
                if champ:
                    units.append(f"{champ.getName()}({champ.getStarLevel()}*)")
        
        # Identificação de quem é quem
        if i == 0:
            role = "PPO (Rede Neural)"
        elif i == 7:
            role = "Heurístico (Esbanjador)"
        else:
            role = f"Genético (AG {i})"
            
        players_data.append({
            'id': i, 'role': role, 'hp': hp, 'lvl': lvl, 'gold': gold, 'units': units
        })
        
    players_data.sort(key=lambda x: (x['hp'], x['lvl']), reverse=True)
    
    for rank, data in enumerate(players_data):
        status = "VIVO" if data['hp'] > 0 else "MORTO"
        print(f"#{rank+1} - {data['role']} [{status}]: {data['hp']} HP | Lvl {data['lvl']} | {data['gold']} Gold")
        board_str = ", ".join(data['units']) if data['units'] else "Nenhuma unidade"
        print(f"    Tabuleiro: {board_str}")
    print("="*50)

def watch_trained_agent():
    print("Carregando banco de dados...")
    json_path = os.path.join("data", "toy_set.json")
    with open(json_path, 'r', encoding='utf-8') as f:
        py_dict = json.load(f)
    cpp_db = tft.loadChampionsFromJson(json_path)
    
    print("Iniciando Ambiente...")
    env = TFTGymEnv(py_dict, cpp_db, "data/top_30_genetic_agents.json")
    
    print("Carregando o Cérebro do PPO...")
    model = PPO.load("data/ppo_tft_agent.zip")
    
    obs, info = env.reset()
    done = False
    
    env_core = env.env  # Acessa a engine C++ por trás do Gym
    last_round_printed = 0
    
    print("\n--- INÍCIO DA PARTIDA DE EXIBIÇÃO ---")
    
    while not done:
        # A IA toma a decisão baseada na observação
        action, _states = model.predict(obs, deterministic=True)
        
        # O ambiente avança um passo
        obs, reward, terminated, truncated, info = env.step(action)
        done = terminated or truncated
        
        current_stage = env_core.current_stage
        current_round = env_core.current_round
        
        # Imprime o status do Lobby a cada 5 rodadas ou quando a partida acabar
        if current_round % 5 == 0 and current_round != last_round_printed:
            print_lobby_status(env_core, current_stage, current_round)
            last_round_printed = current_round
            
        if done:
            print("\n\n" + "#"*50)
            print(" MATCH OVER - RESULTADO FINAL")
            print("#"*50)
            print_lobby_status(env_core, current_stage, current_round)
            print(f"\nPosição Final do PPO: {info.get('placement', 'Desconhecida')}")
            break

if __name__ == "__main__":
    watch_trained_agent()