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

        player = env.players[player_id]
        board = env.boards[player_id]
        shop = env.shops[player_id]
        gold = player.getGold()
        level = player.getLevel()
        hp = player.getHp()
        bench = player.getBench()

        # 1. Compras na Loja
        best_shop_idx = -1
        best_shop_score = -1.0
        
        for i, champ_name in enumerate(shop):
            if champ_name != "Empty":
                cost = self.py_dict.get(champ_name, {}).get("cost", 1)
                if gold >= cost:
                    score = self._score_champ(champ_name)
                    if score >= self.genes["buy_threshold"] and score > best_shop_score:
                        best_shop_score = score
                        best_shop_idx = i

        if best_shop_idx != -1:
            has_bench_space = any(c is None for c in bench)
            if not has_bench_space:
                worst_bench_idx = -1
                worst_bench_score = 999.0
                for i, c in enumerate(bench):
                    if c is not None:
                        c_score = self._score_champ(c.getName())
                        if c_score < worst_bench_score:
                            worst_bench_score = c_score
                            worst_bench_idx = i
                if worst_bench_idx != -1 and worst_bench_score < best_shop_score:
                    return 8 + worst_bench_idx 
            else:
                return 3 + best_shop_idx

        # 2. Avaliação do BASL e Pânico
        units_on_board = 0
        total_stars = 0
        for y in range(4):
            for x in range(7):
                champ = board.getChampion(x, y)
                if champ:
                    units_on_board += 1
                    total_stars += champ.getStarLevel()
                    
        current_basl = (total_stars / units_on_board) if units_on_board > 0 else 0.0
        
        # Ativa o botão de pânico se a vida estiver baixa
        effective_min_gold = 0 if hp <= self.genes["hp_panic_threshold"] else self.genes["min_gold"]

        # 3. Decisão de Gastar Ouro (Level vs Roll)
        if gold > effective_min_gold:
            # Se o tabuleiro está mais fraco que a meta de estrelas, ele rola a loja para buscar upgrades
            if current_basl < self.genes["target_basl"]:
                if gold >= 2:
                    return 2
            # Se já bateu a meta de estrelas, ele tenta subir de nível
            else:
                if gold >= 4:
                    return 1

        # 4. Posicionamento Automático
        if units_on_board < level:
            best_bench_idx = -1
            best_bench_score = -1.0
            for i, c in enumerate(bench):
                if c is not None:
                    score = self._score_champ(c.getName())
                    if score > best_bench_score:
                        best_bench_score = score
                        best_bench_idx = i
            
            if best_bench_idx != -1:
                champ_name = bench[best_bench_idx].getName()
                champ_range = self.py_dict.get(champ_name, {}).get("range", 1)
                target_rows = [2, 3] if champ_range == 1 else [0, 1]
                
                empty_slot = -1
                for y in target_rows:
                    for x in range(7):
                        if not board.isOccupied(x, y):
                            empty_slot = y * 7 + x
                            break
                    if empty_slot != -1:
                        break
                        
                if empty_slot == -1:
                    for slot in range(28):
                        x = slot % 7
                        y = slot // 7
                        if not board.isOccupied(x, y):
                            empty_slot = slot
                            break

                if empty_slot != -1:
                    return 45 + (best_bench_idx * 28) + empty_slot

        return 0