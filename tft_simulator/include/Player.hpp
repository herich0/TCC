#ifndef PLAYER_HPP
#define PLAYER_HPP

#include <string>
#include <vector>
#include <map>
#include "Champion.hpp"
#include "Board.hpp"
#include "GlobalPool.hpp"

class Player {
private:
    int hp;
    int gold;
    int level;
    int xp;
    int streak;
    Champion* bench[9];

public:
    Player();
    ~Player();

    void addGold(int amount);
    void applyInterest();
    void buyXp();
    void addNaturalXp();
    void takeDamage(int amount);
    void updateStreak(int result);
    
    int getGold() const;
    int getHp() const;
    int getLevel() const;
    int getXp() const;
    int getStreak() const;

    bool buyChampion(std::string name, int cost, GlobalPool& pool, const std::map<std::string, Champion>& db);
    void sellChampionFromBench(int benchIndex, GlobalPool& pool);
    bool moveBenchToBoard(int benchIndex, int boardX, int boardY, Board& board);
    bool moveBoardToBench(int boardX, int boardY, Board& board);
    bool moveBoardToBoard(int fromX, int fromY, int toX, int toY, Board& board, int teamId = 1);    
    
    Champion* getBenchChampion(int index);
    void checkAutoCombine(Board& board);
    std::vector<Champion*> getBench() const;

    void autoDeploy(Board& board, int teamId);
};

#endif