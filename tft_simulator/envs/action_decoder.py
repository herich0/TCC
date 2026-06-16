class ActionDecoder:
    def __init__(self):
        # O espaço de ações caiu de 1081 para apenas 17!
        self.total_actions = 17 
        self.num_bench_slots = 9

    def decode_and_execute(self, player, board, shop_array, global_pool, cpp_db, py_dict, action_id):
        if action_id < 0 or action_id >= self.total_actions:
            return False

        # 0: Passar o turno (Não faz nada)
        if action_id == 0:
            return True

        # 1: Comprar XP
        if action_id == 1:
            player.buyXp()
            return True

        # 2: Rolar a loja
        if action_id == 2:
            return "ROLL_SHOP"

        # 3 a 7: Comprar peça do Slot 1, 2, 3, 4 ou 5 da loja
        if 3 <= action_id <= 7:
            shop_idx = action_id - 3
            champ_name = shop_array[shop_idx]
            
            if champ_name != "Empty":
                cost = py_dict.get(champ_name, {}).get("cost", 1)
                success = player.buyChampion(champ_name, cost, global_pool, cpp_db)
                
                shop_array[shop_idx] = "Empty"
                
                if success:
                    player.checkAutoCombine(board)
                    return True
                else:
                    return False
            return False

        # 8 a 16: Vender a peça que está no banco de reservas (Slots 0 a 8)
        if 8 <= action_id <= 16:
            bench_index = action_id - 8
            player.sellChampionFromBench(bench_index, global_pool)
            return True

        return False