class HeuristicAgent:
    def __init__(self, py_dict):
        self.py_dict = py_dict
        self.target_units = [
            "Jhin", "Karma", "Kai'Sa", "Mordekaiser", "Cho'Gath", "Lissandra",
            "Pantheon", "Morgana", "Blitzcrank"
        ]
        self.rolls_this_turn = 0
        self.last_round = 0

    def get_action(self, player_id, env):
        if env.current_round != self.last_round:
            self.rolls_this_turn = 0
            self.last_round = env.current_round

        player = env.players[player_id]
        board = env.boards[player_id]
        shop = env.shops[player_id]
        gold = player.getGold()
        level = player.getLevel()

        bench = player.getBench()
        has_bench_space = any(c is None for c in bench)

        if has_bench_space:
            for i, champ_name in enumerate(shop):
                if champ_name in self.target_units and champ_name != "Empty":
                    cost = self.py_dict.get(champ_name, {}).get("cost", 1)
                    if gold >= cost:
                        return 3 + i

        if gold > 50:
            if level < 8:
                return 1
            elif level == 8:
                if self.rolls_this_turn < 3:
                    self.rolls_this_turn += 1
                    return 2
                else:
                    return 1

        units_on_board = 0
        for y in range(4):
            for x in range(7):
                if board.isOccupied(x, y):
                    units_on_board += 1

        if units_on_board < level:
            best_bench_idx = -1
            best_priority = 999

            for i, champ in enumerate(bench):
                if champ is not None:
                    name = champ.getName()
                    if name in self.target_units:
                        prio = self.target_units.index(name)
                        if prio < best_priority:
                            best_priority = prio
                            best_bench_idx = i

            if best_bench_idx != -1:
                empty_slot = -1
                for slot in range(28):
                    x = slot % 7
                    y = slot // 7
                    if not board.isOccupied(x, y):
                        empty_slot = slot
                        break

                if empty_slot != -1:
                    return 45 + (best_bench_idx * 28) + empty_slot

        return 0