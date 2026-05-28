import numpy as np

class StateEncoder:
    def __init__(self, champions_data):
        self.champ_to_id = {"EMPTY": 0}
        self.max_pool_sizes = {}
        
        current_id = 1
        for name, data in champions_data.items():
            self.champ_to_id[name] = current_id
            
            cost = data.get("cost", 1)
            if cost == 1: pool = 29
            elif cost == 2: pool = 22
            elif cost == 3: pool = 18
            elif cost == 4: pool = 12
            else: pool = 10
            
            self.max_pool_sizes[current_id] = pool
            current_id += 1
            
        self.num_champions = len(self.champ_to_id)

    def encode(self, player, global_pool):
        status = np.array([
            max(0.0, player.getHp()) / 100.0,
            min(player.getGold(), 100.0) / 100.0,
            player.getLevel() / 10.0,
            player.getXp() / 100.0,
            (player.getStreak() + 5.0) / 10.0 
        ], dtype=np.float32)

        shop_state = np.zeros(5, dtype=np.float32)
        for i, champ_name in enumerate(player.getShop()):
            shop_state[i] = self.champ_to_id.get(champ_name, 0) / float(self.num_champions)

        bench_state = np.zeros(9 * 2, dtype=np.float32)
        for i, champ in enumerate(player.getBench()):
            idx = i * 2
            if champ:
                bench_state[idx] = self.champ_to_id.get(champ.getName(), 0) / float(self.num_champions)
                bench_state[idx + 1] = champ.getStarLevel() / 3.0

        board_state = np.zeros(28 * 2, dtype=np.float32)
        
        for y in range(4):
            for x in range(7):
                champ = player.getBoard().getChampion(x, y)
                idx = (y * 7 + x) * 2
                if champ:
                    board_state[idx] = self.champ_to_id.get(champ.getName(), 0) / float(self.num_champions)
                    board_state[idx + 1] = champ.getStarLevel() / 3.0

        pool_state = np.zeros(self.num_champions, dtype=np.float32)
        for name, champ_id in self.champ_to_id.items():
            if name == "EMPTY":
                continue
            
            remaining = global_pool.getRemainingCount(name)
            maximum = self.max_pool_sizes[champ_id]
            pool_state[champ_id] = remaining / float(maximum)

        final_observation = np.concatenate([
            status, 
            shop_state, 
            bench_state, 
            board_state, 
            pool_state
        ])

        return final_observation

    def get_observation_shape(self):
        shape_size = 5 + 5 + (9 * 2) + (28 * 2) + self.num_champions
        return (shape_size,)