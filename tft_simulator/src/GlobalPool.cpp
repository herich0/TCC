#include "GlobalPool.hpp"
#include <random>
#include <algorithm>

GlobalPool::GlobalPool(const std::map<std::string, Champion>& db) {
    std::map<int, int> counts = {{1, 29}, {2, 22}, {3, 18}, {4, 12}, {5, 10}};

    for (const auto& pair : db) {
        int cost = pair.second.getCost();
        int copies = counts[cost];
        for (int i = 0; i < copies; i++) {
            pool[cost].push_back(pair.first);
        }
    }
}

std::string GlobalPool::drawChampion(int cost) {
    if (pool.find(cost) == pool.end() || pool[cost].empty()) {
        return "Empty";
    }

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, pool[cost].size() - 1);

    int index = dis(gen);
    std::string drawn = pool[cost][index];

    pool[cost].erase(pool[cost].begin() + index);

    return drawn;
}

void GlobalPool::returnChampion(const std::string& name, int cost) {
    pool[cost].push_back(name);
}

int GlobalPool::getAvailableCount(const std::string& name) const {
    int count = 0;
    for (const auto& pair : pool) {
        for (const auto& champ : pair.second) {
            if (champ == name) {
                count++;
            }
        }
    }
    return count;
}