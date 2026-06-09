import sys
import os
import json
import time
import random
from concurrent.futures import ProcessPoolExecutor

sys.path.append(os.path.join(os.path.dirname(__file__), "build"))

import tft_engine as tft
from envs.tft_env import TFTEnvironment
from envs.population_manager import PopulationManager

def load_champions_data():
    json_path = os.path.join(os.path.dirname(__file__), "data", "toy_set.json")
    with open(json_path, 'r', encoding='utf-8') as f:
        py_dict = json.load(f)
    return py_dict, json_path

def simulate_lobby(agent_genes_list, py_dict, json_path):
    import tft_engine as tft
    from envs.tft_env import TFTEnvironment
    from envs.genetic_agent import GeneticAgent
    
    cpp_db = tft.loadChampionsFromJson(json_path)
    
    env = TFTEnvironment(py_dict, cpp_db, num_players=8)
    
    agents = [GeneticAgent(py_dict, genes) for genes in agent_genes_list]
    
    env.reset()
    episodio_encerrado = False
    
    while not episodio_encerrado:
        actions = []
        for i in range(8):
            if env.alive_status[i]:
                actions.append(agents[i].get_action(i, env))
            else:
                actions.append(0)
        
        env.step(actions)
        
        if sum(1 for alive in env.alive_status if alive) <= 1 or env.current_stage >= 8:
            episodio_encerrado = True

    scores = []
    for i, player in enumerate(env.players):
        score = max(0, player.getHp()) + (player.getLevel() * 10)
        scores.append(score)
        
    return scores

def run_training():
    print("--- INICIANDO TREINAMENTO DO ALGORITMO GENÉTICO ---")
    
    py_dict, json_path = load_champions_data()
    
    POP_SIZE = 24
    GENERATIONS = 10
    MATCHES_PER_GEN = 10
    
    manager = PopulationManager(py_dict, pop_size=POP_SIZE, mutation_rate=0.08, sigma=0.1)
    
    for gen in range(1, GENERATIONS + 1):
        gen_start_time = time.time()
        print(f"\n[Geração {gen}/{GENERATIONS}] Simulando partidas...")
        
        fitness_scores = [0.0] * POP_SIZE
        
        lobbies_to_run = []
        indices_mapping = []
        
        for _ in range(MATCHES_PER_GEN):
            indices = list(range(POP_SIZE))
            random.shuffle(indices)
            
            for i in range(0, POP_SIZE, 8):
                lobby_indices = indices[i:i+8]
                lobby_genes = [manager.population[idx].genes for idx in lobby_indices]
                lobbies_to_run.append(lobby_genes)
                indices_mapping.append(lobby_indices)

        results = []
        with ProcessPoolExecutor() as executor:
            futures = [executor.submit(simulate_lobby, lobby, py_dict, json_path) for lobby in lobbies_to_run]
            for future in futures:
                results.append(future.result())

        for lobby_scores, lobby_indices in zip(results, indices_mapping):
            for score, original_idx in zip(lobby_scores, lobby_indices):
                fitness_scores[original_idx] += score

        best_idx = fitness_scores.index(max(fitness_scores))
        best_agent = manager.population[best_idx]
        avg_score = sum(fitness_scores) / POP_SIZE
        
        print(f"-> Tempo da Geração: {time.time() - gen_start_time:.2f}s")
        print(f"-> Score Médio: {avg_score:.1f} | Melhor Score: {fitness_scores[best_idx]:.1f}")
        print(f"-> GENOMA DO CAMPEÃO:")
        print(f"   Economia: Guarda {best_agent.genes['min_gold']:.0f} Gold | Pânico: {best_agent.genes['hp_panic_threshold']:.0f} HP | Alvo Estrelas: {best_agent.genes['target_basl']:.2f}")
        
        with open(os.path.join("data", "best_genetic_agent.json"), "w") as f:
            json.dump(best_agent.genes, f, indent=4)
            
        if gen < GENERATIONS:
            manager.evolve(fitness_scores)

    print("\n--- TREINAMENTO CONCLUÍDO ---")
    print("O melhor genoma foi salvo em data/best_genetic_agent.json")

if __name__ == "__main__":
    run_training()