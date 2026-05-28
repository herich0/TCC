class ActionDecoder:
    def __init__(self):
        # O tabuleiro possui 28 hexágonos (7 colunas x 4 linhas).
        self.total_actions = 1081
        self.num_board_slots = 28
        self.num_bench_slots = 9

    def decode_and_execute(self, player, shop, action_id):
        # Ação inválida ou fora dos limites, ignoramos.
        if action_id < 0 or action_id >= self.total_actions:
            return False

        # 0: Passar o turno (Tratado pelo orquestrador, não executa nada no C++)
        if action_id == 0:
            return True

        # 1: Comprar XP
        if action_id == 1:
            player.buyXp()
            return True

        # 2: Rolar a loja
        if action_id == 2:
            shop.rollShop(player) 
            return True

        # 3 a 7: Comprar da Loja
        if 3 <= action_id <= 7:
            shop_index = action_id - 3
            player.buyChampion(shop_index)
            return True

        # 8 a 16: Vender do Banco
        if 8 <= action_id <= 16:
            bench_index = action_id - 8
            player.sellChampionFromBench(bench_index)
            return True

        # 17 a 44: Mover do Tabuleiro para o Banco
        if 17 <= action_id <= 44:
            board_slot = action_id - 17
            x, y = self._get_xy_from_slot(board_slot)
            player.moveBoardToBench(x, y)
            return True

        # 45 a 296: Mover do Banco para o Tabuleiro
        if 45 <= action_id <= 296:
            offset = action_id - 45
            bench_index = offset // self.num_board_slots
            board_slot = offset % self.num_board_slots
            x, y = self._get_xy_from_slot(board_slot)
            player.moveBenchToBoard(bench_index, x, y)
            return True

        # 297 a 1080: Mover do Tabuleiro para o Tabuleiro (Reposicionamento)
        if 297 <= action_id <= 1080:
            offset = action_id - 297
            from_slot = offset // self.num_board_slots
            to_slot = offset % self.num_board_slots
            
            if from_slot == to_slot:
                return False # Mover para a própria casa é uma ação inválida
                
            from_x, from_y = self._get_xy_from_slot(from_slot)
            to_x, to_y = self._get_xy_from_slot(to_slot)
            
            # O C++ precisará de uma função moveBoardToBoard(x1, y1, x2, y2)
            # player.moveBoardToBoard(from_x, from_y, to_x, to_y)
            return True

        return False

    def _get_xy_from_slot(self, slot_index):
        # Converte o índice linear (0 a 27) em coordenadas X (0 a 6) e Y (0 a 3)
        y = slot_index // 7
        x = slot_index % 7
        return x, y