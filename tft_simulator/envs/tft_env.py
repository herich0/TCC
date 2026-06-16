import numpy as np
import random
import tft_engine as tft
from .state_encoder import StateEncoder
from .action_decoder import ActionDecoder

class TFTEnvironment:
    def __init__(self, py_dict, cpp_db, num_players=8):
        self.num_players = num_players
        self.py_dict = py_dict
        self.cpp_db = cpp_db
        
        self.players = []
        self.boards = []
        self.shops = []
        self.global_pool = None
        
        self.current_stage = 1
        self.current_round = 1
        self.players_done_planning = []
        self.alive_status = []
        
        self.state_encoder = StateEncoder(py_dict)
        self.action_decoder = ActionDecoder()
        
        self.available_champions = list(py_dict.keys())

    def reset(self):
        self.global_pool = tft.GlobalPool(self.cpp_db)
        self.players = [tft.Player() for _ in range(self.num_players)]
        
        # Instancia as matrizes de tabuleiro usando a assinatura do seu construtor
        self.boards = [tft.Board(7, 4) for _ in range(self.num_players)]
        self.shops = [self._generate_random_shop() for _ in range(self.num_players)]
        
        self.current_stage = 1
        self.current_round = 1
        self.players_done_planning = [False] * self.num_players
        self.alive_status = [True] * self.num_players
        
        for player in self.players:
            player.addGold(10)
            
        return self._get_all_observations()

    def _generate_random_shop(self):
        # Gera 5 cartas aleatórias abstraindo probabilidades finas por nível nesta fase inicial
        return [random.choice(self.available_champions) for _ in range(5)]

    def step(self, actions):
        rewards = [0.0] * self.num_players
        
        for i, action in enumerate(actions):
            if not self.alive_status[i] or self.players_done_planning[i]:
                continue
                
            result = self.action_decoder.decode_and_execute(
                self.players[i], 
                self.boards[i],
                self.shops[i],
                self.global_pool,
                self.cpp_db,
                self.py_dict,
                action
            )
            
            if result == "ROLL_SHOP":
                if self.players[i].getGold() >= 2:
                    self.players[i].addGold(-2)
                    self.shops[i] = self._generate_random_shop()
                    success = True
                else:
                    success = False
            else:
                success = result
            
            if not success:
                continue
                
            if action == 0:
                self.players_done_planning[i] = True

        if all(p_done or not p_alive for p_done, p_alive in zip(self.players_done_planning, self.alive_status)):
            self._resolve_macro_turn()

        dones = [not alive for alive in self.alive_status]
        
        return self._get_all_observations(), rewards, dones, {}
    
    def _resolve_macro_turn(self):
        alive_players = [i for i, alive in enumerate(self.alive_status) if alive]

        if len(alive_players) <= 1:
            return

        for p_idx in alive_players:
            self.players[p_idx].autoDeploy(self.boards[p_idx], 1)
        
        random.shuffle(alive_players)
        matches = []
        
        while len(alive_players) >= 2:
            p1 = alive_players.pop(0)
            p2 = alive_players.pop(0)
            matches.append((p1, p2, True)) 
            

        if len(alive_players) == 1:
            p1 = alive_players.pop(0)
            ghost = random.choice([i for i, alive in enumerate(self.alive_status) if alive and i != p1])
            matches.append((p1, ghost, False))

        engine = tft.CombatEngine()

        for p1_idx, p2_idx, is_p2_real in matches:
            arena = tft.Board(7, 8)
            
            b1 = self.boards[p1_idx]
            for x in range(7):
                for y in range(4):
                    c = b1.getChampion(x, y)
                    if c:
                        arena.placeChampion(x, y, c, 1, c.getStarLevel())
                        
            b2 = self.boards[p2_idx]
            for x in range(7):
                for y in range(4):
                    c = b2.getChampion(x, y)
                    if c:
                        arena.placeChampion(6 - x, 7 - y, c, 2, c.getStarLevel())

            stats = engine.simulate(arena, 40.0)
            
            survivors = 0
            for x in range(7):
                for y in range(8):
                    if arena.getChampion(x, y):
                        survivors += 1
                        
            total_damage = (self.current_stage * 2) + survivors

            if stats.winningTeam == 1:
                if is_p2_real:
                    self.players[p2_idx].takeDamage(total_damage)
            elif stats.winningTeam == 2:
                self.players[p1_idx].takeDamage(total_damage)
            else: 
                self.players[p1_idx].takeDamage(total_damage)
                if is_p2_real:
                    self.players[p2_idx].takeDamage(total_damage)

        self.current_round += 1
        if self.current_round > 7:
            self.current_round = 1
            self.current_stage += 1
            
        self.players_done_planning = [False] * self.num_players
        self.shops = [self._generate_random_shop() for _ in range(self.num_players)]

        for i, player in enumerate(self.players):
            if self.alive_status[i]:
                player.addNaturalXp()  
                player.applyInterest() 
                player.addGold(5)     
        
        self.alive_status = [p.getHp() > 0 for p in self.players]

    def _get_all_observations(self):
        obs = []
        for i, player in enumerate(self.players):
            if self.alive_status[i]:
                obs.append(self.state_encoder.encode(
                    player, self.boards[i], self.shops[i], self.global_pool
                ))
            else:
                obs.append(np.zeros(self.state_encoder.get_observation_shape()))
        return obs