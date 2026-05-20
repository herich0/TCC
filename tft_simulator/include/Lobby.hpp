#ifndef LOBBY_HPP
#define LOBBY_HPP

#include <vector>
#include <map>
#include <string>
#include "Player.hpp"
#include "Board.hpp"
#include "Match.hpp"
#include "GlobalPool.hpp"
#include "Shop.hpp"
#include "CombatEngine.hpp"

class Lobby {
private:
    std::vector<Player> players;
    std::vector<Board*> boards;
    std::vector<bool> isAlive;
    Match matchTracker;
    GlobalPool pool;
    Shop shop;
    
    int calculatePlayerDamage(int stage, int survivingUnits);

public:
    Lobby(const std::map<std::string, Champion>& db);
    ~Lobby();

    void playRound(CombatEngine& engine);
    
    Player* getPlayer(int id);
    Board* getBoard(int id);
    bool getIsAlive(int id) const;
    int getAliveCount() const;
    Match* getMatchTracker();
    GlobalPool& getPool();
    Shop& getShop();
};

#endif