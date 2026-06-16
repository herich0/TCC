import random

class GeneticAgent:
    def __init__(self, py_dict, genes=None):
        self.py_dict = py_dict
        self.traits_list = set()
        
        for champ_data in self.py_dict.values():
            for t in champ_data.get("traits", []):
                self.traits_list.add(t)
        self.traits_list = list(self.traits_list)
        
        if genes:
            self.genes = genes
        else:
            self.genes = {
                "min_gold": random.uniform(0, 50),
                "hp_panic_threshold": random.uniform(0, 40), 
                "target_basl": random.uniform(1.0, 3.0),    
                "buy_threshold": random.uniform(0.5, 3.0),
                "traits": {t: random.uniform(0, 1) for t in self.traits_list}
            }
        self.last_round = 0
        self.sold_this_turn = False
        
    def _score_champ(self, champ_name):
        if champ_name == "Empty" or champ_name not in self.py_dict:
            return 0.0
        score = 0.0
        for t in self.py_dict[champ_name].get("traits", []):
            score += self.genes["traits"].get(t, 0.0)
        return score

    def get_action(self, player_id, env):
        if env.current_round != self.last_round:
            self.last_round = env.current_round
            self.sold_this_turn = False # Reseta a flag de venda no início do turno

        player = env.players[player_id]
        board = env.boards[player_id]
        shop = env.shops[player_id]
        gold = player.getGold()
        level = player.getLevel()
        hp = player.getHp()
        bench = player.getBench()

        # 1. Compras na Loja (Prioridade)
        best_shop_idx = -1
        best_shop_score = -1.0
        has_bench_space = any(c is None for c in bench)
        
        if has_bench_space:
            for i, champ_name in enumerate(shop):
                if champ_name != "Empty":
                    cost = self.py_dict.get(champ_name, {}).get("cost", 1)
                    if gold >= cost:
                        score = self._score_champ(champ_name)
                        if score >= self.genes["buy_threshold"] and score > best_shop_score:
                            best_shop_score = score
                            best_shop_idx = i

            if best_shop_idx != -1:
                return 3 + best_shop_idx # Ações 3 a 7 (Comprar)

        # 2. Heurística de Poda Gulosa (Vender peças fracas para bater juros)
        if not self.sold_this_turn:
            current_interest_tier = gold // 10
            if current_interest_tier < 5: # Se não está no teto de 50 gold
                worst_bench_idx = -1
                worst_bench_score = 999.0
                
                for i, c in enumerate(bench):
                    if c is not None:
                        score = self._score_champ(c.getName())
                        # Poda peças de 1 estrela que o AG não valoriza muito (score baixo)
                        if score < worst_bench_score and c.getStarLevel() == 1:
                            worst_bench_score = score
                            worst_bench_idx = i
                
                # Se achou uma peça ruim (< 0.5 de interesse do genoma)
                if worst_bench_idx != -1 and worst_bench_score < 0.5:
                    cost = bench[worst_bench_idx].getCost()
                    # Vende SÓ SE for fazer o ouro pular pra próxima dezena
                    if (gold + cost) // 10 > current_interest_tier:
                        self.sold_this_turn = True
                        return 8 + worst_bench_idx # Ações 8 a 16 (Vender)

        # 3. Avaliação do BASL e Pânico
        units_on_board = 0
        total_stars = 0
        for y in range(4):
            for x in range(7):
                champ = board.getChampion(x, y)
                if champ:
                    units_on_board += 1
                    total_stars += champ.getStarLevel()
                    
        current_basl = (total_stars / units_on_board) if units_on_board > 0 else 0.0
        effective_min_gold = 0 if hp <= self.genes["hp_panic_threshold"] else self.genes["min_gold"]

        # 4. Decisão de Gastar Ouro (Level vs Roll)
        if gold > effective_min_gold:
            if current_basl < self.genes["target_basl"]:
                if gold >= 2:
                    return 2 # Rolar a loja
            else:
                if gold >= 4:
                    return 1 # Comprar XP

        # 5. Fim do planejamento (O C++ vai fazer o AutoDeploy na arena depois disso)
        return 0