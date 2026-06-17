#include "Player.hpp"
#include <iostream>
#include <algorithm>

Player::Player() : hp(100), gold(0), level(1), xp(0), streak(0) {
    for (int i = 0; i < 9; ++i) {
        bench[i] = nullptr;
    }
}

Player::~Player() {
    for (int i = 0; i < 9; ++i) {
        if (bench[i] != nullptr) {
            delete bench[i];
        }
    }
}

void Player::addGold(int amount) { gold += amount; }
void Player::applyInterest() { gold += std::min(gold / 10, 5); }

void Player::buyXp() { 
    if (gold >= 4) { 
        gold -= 4; 
        xp += 4; 
        
        int xpTable[] = {0, 0, 2, 6, 10, 20, 36, 56, 80, 124, 9999};
        while (level < 10 && xp >= xpTable[level + 1]) {
            level++;
        }
    } 
}

void Player::addNaturalXp() { 
    xp += 2; 
    
    int xpTable[] = {0, 0, 2, 6, 10, 20, 36, 56, 80, 124, 9999};
    while (level < 10 && xp >= xpTable[level + 1]) {
        level++;
    }
}

void Player::takeDamage(int amount) { hp -= amount; }
void Player::updateStreak(int result) { }

int Player::getGold() const { return gold; }
int Player::getHp() const { return hp; }
int Player::getLevel() const { return level; }
int Player::getXp() const { return xp; }
int Player::getStreak() const { return streak; }

bool Player::buyChampion(std::string name, int cost, GlobalPool& pool, const std::map<std::string, Champion>& db) {
    if (gold < cost || name == "Empty") return false;
    
    if (!pool.takeChampion(name)) {
        return false; 
    }
    
    int emptySlot = -1;
    for (int i = 0; i < 9; ++i) {
        if (bench[i] == nullptr) {
            emptySlot = i;
            break;
        }
    }
    
    if (emptySlot != -1) {
        gold -= cost;
        auto it = db.find(name);
        if (it != db.end()) {
            bench[emptySlot] = new Champion(it->second);
            return true;
        }
    }
    
    pool.returnChampion(name, cost);
    return false;
}

void Player::sellChampionFromBench(int benchIndex, GlobalPool& pool) {
    if (benchIndex >= 0 && benchIndex < 9 && bench[benchIndex] != nullptr) {
        pool.returnChampion(bench[benchIndex]->getName(), bench[benchIndex]->getCost());
        gold += bench[benchIndex]->getCost(); 
        delete bench[benchIndex];
        bench[benchIndex] = nullptr;
    }
}

bool Player::moveBenchToBoard(int benchIndex, int boardX, int boardY, Board& board) {
    if (benchIndex < 0 || benchIndex >= 9 || bench[benchIndex] == nullptr) return false;
    if (board.isOccupied(boardX, boardY)) return false;
    
    board.placeChampion(boardX, boardY, *bench[benchIndex], 1);
    delete bench[benchIndex];
    bench[benchIndex] = nullptr;
    return true;
}

bool Player::moveBoardToBench(int boardX, int boardY, Board& board) {
    Champion* c = board.getChampion(boardX, boardY);
    if (!c) return false;

    for (int i = 0; i < 9; ++i) {
        if (bench[i] == nullptr) {
            bench[i] = new Champion(*c);
            board.removeChampion(boardX, boardY);
            return true;
        }
    }
    return false;
}

Champion* Player::getBenchChampion(int index) {
    if (index >= 0 && index < 9) return bench[index];
    return nullptr;
}

void Player::checkAutoCombine(Board& board) {
    bool combined = false;

    for (int starTarget = 1; starTarget <= 2; ++starTarget) {
        std::map<std::string, std::vector<std::pair<int, int>>> locations;

        for (int i = 0; i < 9; ++i) {
            if (bench[i] && bench[i]->getStarLevel() == starTarget) {
                locations[bench[i]->getName()].push_back({-1, i});
            }
        }

        for (auto pos : board.getAllPositions()) {
            Champion* c = board.getChampion(pos.first, pos.second);
            if (c && c->getStarLevel() == starTarget) {
                locations[c->getName()].push_back({pos.first, pos.second});
            }
        }

        for (auto const& [name, locs] : locations) {
            if (locs.size() >= 3) {
                auto target = locs[0];
                bool targetOnBoard = false;

                for (size_t i = 0; i < 3; ++i) {
                    if (locs[i].first != -1) {
                        target = locs[i];
                        targetOnBoard = true;
                        break;
                    }
                }

                if (targetOnBoard) {
                    board.getChampion(target.first, target.second)->promote();
                } else {
                    bench[target.second]->promote();
                }

                int removed = 0;
                for (auto loc : locs) {
                    if (loc == target) continue;
                    if (removed == 2) break;

                    if (loc.first == -1) {
                        delete bench[loc.second];
                        bench[loc.second] = nullptr;
                    } else {
                        board.removeChampion(loc.first, loc.second);
                    }
                    removed++;
                }
                combined = true;
                break; 
            }
        }
    }

    if (combined) {
        checkAutoCombine(board);
    }
}

std::vector<Champion*> Player::getBench() const {
    return std::vector<Champion*>(std::begin(bench), std::end(bench));
}

bool Player::moveBoardToBoard(int fromX, int fromY, int toX, int toY, Board& board, int teamId) {
    if (fromX < 0 || fromX >= 7 || fromY < 0 || fromY >= 4 ||
        toX < 0 || toX >= 7 || toY < 0 || toY >= 4) {
        return false;
    }

    Champion* c = board.getChampion(fromX, fromY);
    if (!c || board.getTeam(fromX, fromY) != teamId) {
        return false;
    }

    if (board.isOccupied(toX, toY)) {
        Champion* target = board.getChampion(toX, toY);
        if (target && board.getTeam(toX, toY) == teamId) {
            Champion tempC(*c);
            Champion tempTarget(*target);
            int star1 = tempC.getStarLevel();
            int star2 = tempTarget.getStarLevel();
            
            board.removeChampion(fromX, fromY);
            board.removeChampion(toX, toY);
            
            board.placeChampion(toX, toY, tempC, teamId, star1);
            board.placeChampion(fromX, fromY, tempTarget, teamId, star2);
            return true;
        }
        return false;
    }

    Champion tempC(*c);
    int star = tempC.getStarLevel();
    board.removeChampion(fromX, fromY);
    board.placeChampion(toX, toY, tempC, teamId, star);
    return true;
}

void Player::autoDeploy(Board& board, int teamId) {
    std::vector<Champion> allUnits;

    for (int i = 0; i < 9; ++i) {
        if (bench[i] != nullptr) {
            allUnits.push_back(*bench[i]);
            delete bench[i];
            bench[i] = nullptr;
        }
    }

    auto positions = board.getAllPositions();
    for (auto pos : positions) {
        if (board.getTeam(pos.first, pos.second) == teamId) {
            Champion* c = board.getChampion(pos.first, pos.second);
            if (c) {
                allUnits.push_back(*c);
                board.removeChampion(pos.first, pos.second);
            }
        }
    }

    if (allUnits.empty()) return;

    std::sort(allUnits.begin(), allUnits.end(), [](const Champion& a, const Champion& b) {
        int powerA = a.getCost() + ((a.getStarLevel() - 1) * 3);
        int powerB = b.getCost() + ((b.getStarLevel() - 1) * 3);

        if (powerA != powerB) return powerA > powerB;

        if (a.getStarLevel() != b.getStarLevel()) return a.getStarLevel() > b.getStarLevel();
        
        if (a.getCost() != b.getCost()) return a.getCost() > b.getCost();
        
        return a.getName() > b.getName();
    });

    int placedCount = 0;
    int currentLevel = this->getLevel();
    std::vector<Champion> leftovers;

    for (const auto& champ : allUnits) {
        if (placedCount < currentLevel) {
            bool placed = false;
            
            int startY = (champ.getRange() == 1) ? 0 : 2; 
            int endY = (champ.getRange() == 1) ? 2 : 4;

            for (int y = startY; y < endY && !placed; ++y) {
                for (int x = 0; x < 7 && !placed; ++x) {
                    if (!board.isOccupied(x, y)) {
                        board.placeChampion(x, y, champ, teamId, champ.getStarLevel());
                        placed = true;
                        placedCount++;
                    }
                }
            }
            
            if (!placed) {
                for (int y = 0; y < 4 && !placed; ++y) {
                    for (int x = 0; x < 7 && !placed; ++x) {
                        if (!board.isOccupied(x, y)) {
                            board.placeChampion(x, y, champ, teamId, champ.getStarLevel());
                            placed = true;
                            placedCount++;
                        }
                    }
                }
            }
        } else {
            leftovers.push_back(champ);
        }
    }

    for (size_t i = 0; i < leftovers.size() && i < 9; ++i) {
        bench[i] = new Champion(leftovers[i]);
    }
}