import sys
import os
import json
import random
import time

sys.path.append(os.path.join(os.path.dirname(__file__), "build"))

from envs.tft_env import TFTEnvironment
from agents.heuristic_agent import HeuristicAgent
import tft_engine as tft

def load_champions_data():
    json_path = os.path.join(os.path.dirname(__file__), "data", "toy_set.json")
    with open(json_path, 'r', encoding='utf-8') as f:
        py_dict = json.load(f)
    cpp_db = tft.loadChampionsFromJson(json_path)
    return py_dict, cpp_db

def run_heuristic_test():
    print("--- INICIANDO TESTE: HEURÍSTICA VS ALEATÓRIOS ---")
    
    py_dict, cpp_db = load_champions_data()
    env = TFTEnvironment(py_dict, cpp_db, num_players=8)
    
    heuristic = HeuristicAgent(py_dict)
    
    observations = env.reset()
    episodio_encerrado = False
    turnos_jogados = 0
    last_stage = env.current_stage
    last_round = env.current_round
    
    start_time = time.time()

    while not episodio_encerrado:
        actions = []
        for i in range(8):
            if env.alive_status[i]:
                if i == 0:
                    actions.append(heuristic.get_action(i, env))
                else:
                    if random.random() < 0.10:
                        actions.append(0)
                    else:
                        actions.append(random.randint(1, 1080))
            else:
                actions.append(0)
        
        observations, rewards, dones, info = env.step(actions)
        turnos_jogados += 1
        
        if env.current_round != last_round or env.current_stage != last_stage:
            last_stage = env.current_stage
            last_round = env.current_round
        
        if all(dones) or sum(1 for alive in env.alive_status if alive) <= 1:
            episodio_encerrado = True
            
        if env.current_stage >= 6:
            episodio_encerrado = True

    end_time = time.time()
    
    print("\n--- PARTIDA ENCERRADA ---")
    print(f"Tempo de simulação: {(end_time - start_time):.4f} segundos")
    
    print("\nRanking de Sobrevivência (HP Final):")
    for i, player in enumerate(env.players):
        tipo = "Heurística (Dark Star)" if i == 0 else "Aleatório"
        print(f"Jogador {i} [{tipo}]: {max(0, player.getHp())} HP | Nível {player.getLevel()} | Ouro: {player.getGold()}")

if __name__ == "__main__":
    run_heuristic_test()