#ifndef TRAITENGINE_HPP
#define TRAITENGINE_HPP

#include "Board.hpp"
#include "Player.hpp"
#include <map>
#include <string>
#include <vector>
#include <set>

class TraitEngine {
public:
    std::map<std::string, int> countTraits(Board& board, int ownerId);
    void applyPreCombatTraits(Board& board, int ownerId);
    void processPostCombatTraits(Board& board, Player& player, bool wonCombat);
};

#endif