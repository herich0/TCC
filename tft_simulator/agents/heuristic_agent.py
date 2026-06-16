class HeuristicAgent:
    def __init__(self, py_dict):
        self.py_dict = py_dict
        self.last_round = 0
        self.sold_this_turn = False

    def get_action(self, player_id, env):
        if env.current_round != self.last_round:
            self.sold_this_turn = False
            self.last_round = env.current_round

        player = env.players[player_id]
        shop = env.shops[player_id]
        gold = player.getGold()
        bench = player.getBench()

        has_bench_space = any(c is None for c in bench)
        if has_bench_space:
            best_shop_idx = -1
            highest_cost = -1
            
            for i, champ_name in enumerate(shop):
                if champ_name != "Empty":
                    cost = self.py_dict.get(champ_name, {}).get("cost", 1)
                    if gold >= cost and cost > highest_cost:
                        highest_cost = cost
                        best_shop_idx = i
                        
            if best_shop_idx != -1:
                return 3 + best_shop_idx 

        if gold >= 4 and player.getLevel() < 9:
            return 1 
        if gold >= 2:
            return 2 
        return 0 