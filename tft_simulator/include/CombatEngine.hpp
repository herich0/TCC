#ifndef COMBAT_ENGINE_HPP
#define COMBAT_ENGINE_HPP

#include "Board.hpp"

struct CombatStats {
    int winningTeam;
    double team1Damage;
    double team2Damage;
    double duration;
};

class CombatEngine {
public:
    CombatStats simulate(Board& board, double timeLimit);
};

#endif