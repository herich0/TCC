class ActionDecoder:
    def __init__(self):
        self.total_actions = 1081
        self.num_board_slots = 28
        self.num_bench_slots = 9

    def decode_and_execute(self, player, board, shop_array, global_pool, cpp_db, py_dict, action_id):
        if action_id < 0 or action_id >= self.total_actions:
            return False

        if action_id == 0:
            return True

        if action_id == 1:
            player.buyXp()
            return True

        if action_id == 2:
            return "ROLL_SHOP"

        if 3 <= action_id <= 7:
            shop_idx = action_id - 3
            champ_name = shop_array[shop_idx]
            
            if champ_name != "Empty":
                # Removido o 'self.' do py_dict
                cost = py_dict.get(champ_name, {}).get("cost", 1)
                
                success = player.buyChampion(champ_name, cost, global_pool, cpp_db)
                
                shop_array[shop_idx] = "Empty"
                
                if success:
                    player.checkAutoCombine(board)
                    return True
                else:
                    return False
            return False

        if 8 <= action_id <= 16:
            bench_index = action_id - 8
            player.sellChampionFromBench(bench_index, global_pool)
            return True

        if 17 <= action_id <= 44:
            board_slot = action_id - 17
            x, y = self._get_xy_from_slot(board_slot)
            return player.moveBoardToBench(x, y, board)

        if 45 <= action_id <= 296:
            units_on_board = 0
            for y in range(4):
                for x in range(7):
                    if board.isOccupied(x, y):
                        units_on_board += 1
                        
            if units_on_board >= player.getLevel():
                return False

            offset = action_id - 45
            bench_index = offset // self.num_board_slots
            board_slot = offset % self.num_board_slots
            x, y = self._get_xy_from_slot(board_slot)
            return player.moveBenchToBoard(bench_index, x, y, board)

        if 297 <= action_id <= 1080:
            offset = action_id - 297
            from_slot = offset // self.num_board_slots
            to_slot = offset % self.num_board_slots
            
            if from_slot == to_slot:
                return False
                
            from_x, from_y = self._get_xy_from_slot(from_slot)
            to_x, to_y = self._get_xy_from_slot(to_slot)
            
            return player.moveBoardToBoard(from_x, from_y, to_x, to_y, board, 1)

        return False

    def _get_xy_from_slot(self, slot_index):
        y = slot_index // 7
        x = slot_index % 7
        return x, y