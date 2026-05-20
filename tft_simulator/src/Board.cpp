#include "Board.hpp"
#include <cmath>

Board::Board(int width, int height) : width(width), height(height) {}

bool Board::placeChampion(int x, int y, const Champion& baseChamp, int team, int starLevel) {
    if (isOccupied(x, y)) return false;
    
    Champion newChamp = baseChamp;
    newChamp.setOwnerId(team);
    newChamp.setPosition(x, y);
    
    while (newChamp.getStarLevel() < starLevel) {
        newChamp.promote();
    }
    
    grid.insert(std::make_pair(std::make_pair(x, y), std::make_pair(newChamp, team)));
    
    return true;
}

void Board::removeChampion(int x, int y) {
    grid.erase({x, y});
}

bool Board::moveChampion(int startX, int startY, int targetX, int targetY) {
    if (!isOccupied(startX, startY) || isOccupied(targetX, targetY)) return false;
    if (targetX < 0 || targetX >= width || targetY < 0 || targetY >= height) return false;
    
    auto it = grid.find({startX, startY});
    auto piece = it->second;
    grid.erase(it);
    grid.insert({{targetX, targetY}, piece});
    return true;
}

Champion* Board::getChampion(int x, int y) {
    auto it = grid.find({x, y});
    if (it != grid.end()) return &(it->second.first);
    return nullptr;
}

int Board::getTeam(int x, int y) {
    auto it = grid.find({x, y});
    if (it != grid.end()) return it->second.second;
    return 0;
}

bool Board::isOccupied(int x, int y) const {
    return grid.find({x, y}) != grid.end();
}

int Board::calculateDistance(int x1, int y1, int x2, int y2) const {
    return std::abs(x1 - x2) + std::abs(y1 - y2);
}

std::vector<std::pair<int, int>> Board::getAllPositions() const {
    std::vector<std::pair<int, int>> pos;
    for (const auto& pair : grid) pos.push_back(pair.first);
    return pos;
}

std::vector<Champion*> Board::getChampionsInRange(int x, int y, int range, int targetOwnerId) {
    std::vector<Champion*> result;
    
    for (auto pos : getAllPositions()) {
        int cx = pos.first;
        int cy = pos.second;
        
        int dist = std::abs(cx - x) + std::abs(cy - y);
        
        if (dist <= range) {
            Champion* c = getChampion(cx, cy);
            if (c != nullptr && c->getOwnerId() == targetOwnerId) {
                result.push_back(c);
            }
        }
    }
    
    return result;
}