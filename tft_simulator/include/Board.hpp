#ifndef BOARD_HPP
#define BOARD_HPP

#include <map>
#include <vector>
#include <utility>
#include <cmath>
#include "Champion.hpp"

class Board {
private:
    int width;
    int height;
    std::map<std::pair<int, int>, std::pair<Champion, int>> grid;

public:
    Board(int width, int height);
    
    bool placeChampion(int x, int y, const Champion& champ, int team, int starLevel = 1);
    void removeChampion(int x, int y);
    bool moveChampion(int startX, int startY, int targetX, int targetY);
    
    Champion* getChampion(int x, int y);
    int getTeam(int x, int y);
    bool isOccupied(int x, int y) const;
    
    int calculateDistance(int x1, int y1, int x2, int y2) const;
    std::vector<std::pair<int, int>> getAllPositions() const;

    std::vector<Champion*> getChampionsInRange(int x, int y, int range, int targetOwnerId);
};

#endif