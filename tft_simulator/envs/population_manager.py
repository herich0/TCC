import random
import numpy as np
from .genetic_agent import GeneticAgent

class PopulationManager:
    def __init__(self, py_dict, pop_size=8, mutation_rate=0.05, sigma=0.1):
        self.py_dict = py_dict
        self.pop_size = pop_size
        self.mutation_rate = mutation_rate
        self.sigma = sigma
        self.population = [GeneticAgent(py_dict) for _ in range(pop_size)]

    def crossover(self, parent1, parent2):
        alpha = random.uniform(0, 1)
        child_genes = {}
        
        child_genes["min_gold"] = alpha * parent1.genes["min_gold"] + (1 - alpha) * parent2.genes["min_gold"]
        child_genes["hp_panic_threshold"] = alpha * parent1.genes["hp_panic_threshold"] + (1 - alpha) * parent2.genes["hp_panic_threshold"]
        child_genes["target_basl"] = alpha * parent1.genes["target_basl"] + (1 - alpha) * parent2.genes["target_basl"]
        child_genes["buy_threshold"] = alpha * parent1.genes["buy_threshold"] + (1 - alpha) * parent2.genes["buy_threshold"]
        
        child_genes["traits"] = {}
        for t in parent1.genes["traits"].keys():
            child_genes["traits"][t] = alpha * parent1.genes["traits"][t] + (1 - alpha) * parent2.genes["traits"][t]
            
        return GeneticAgent(self.py_dict, child_genes)

    def mutate(self, agent):
        if random.random() < self.mutation_rate:
            agent.genes["min_gold"] += np.random.normal(0, self.sigma * 50)
            agent.genes["min_gold"] = max(0.0, min(50.0, agent.genes["min_gold"]))
            
        if random.random() < self.mutation_rate:
            agent.genes["hp_panic_threshold"] += np.random.normal(0, self.sigma * 40)
            agent.genes["hp_panic_threshold"] = max(0.0, min(100.0, agent.genes["hp_panic_threshold"]))
            
        if random.random() < self.mutation_rate:
            agent.genes["target_basl"] += np.random.normal(0, self.sigma * 2)
            agent.genes["target_basl"] = max(1.0, min(3.0, agent.genes["target_basl"]))

        if random.random() < self.mutation_rate:
            agent.genes["buy_threshold"] += np.random.normal(0, self.sigma * 3)
            agent.genes["buy_threshold"] = max(0.0, min(5.0, agent.genes["buy_threshold"]))

        for t in agent.genes["traits"].keys():
            if random.random() < self.mutation_rate:
                agent.genes["traits"][t] += np.random.normal(0, self.sigma)
                agent.genes["traits"][t] = max(0.0, min(1.0, agent.genes["traits"][t]))

    def evolve(self, fitness_scores):
        sorted_indices = np.argsort(fitness_scores)[::-1]
        elite1 = self.population[sorted_indices[0]]
        elite2 = self.population[sorted_indices[1]]
        
        new_population = [elite1, elite2]
        
        while len(new_population) < self.pop_size:
            p1 = self.population[random.choices(sorted_indices[:4])[0]]
            p2 = self.population[random.choices(sorted_indices[:4])[0]]
            
            child = self.crossover(p1, p2)
            self.mutate(child)
            new_population.append(child)
            
        self.population = new_population