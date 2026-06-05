class RewardCalculator:
    def __init__(self):
        self.invalid_penalty = -0.05
        self.win_round_reward = 0.2
        self.hp_loss_multiplier = -0.1

    def calculate_step_reward(self, invalid_action):
        if invalid_action:
            return self.invalid_penalty
        return 0.0

    def calculate_combat_reward(self, hp_lost):
        if hp_lost == 0:
            return self.win_round_reward
        
        return hp_lost * self.hp_loss_multiplier

    def calculate_episode_reward(self, final_placement):
        if final_placement == 1:
            return 10.0
        elif final_placement == 2:
            return 7.0
        elif final_placement <= 4:
            return 4.0
        elif final_placement <= 6:
            return -3.0
        else:
            return -8.0