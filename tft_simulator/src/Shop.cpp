#include "Shop.hpp"
#include <random>

Shop::Shop() {
    dropRates = {
        {1.00, 0.00, 0.00, 0.00, 0.00},
        {1.00, 0.00, 0.00, 0.00, 0.00},
        {1.00, 0.00, 0.00, 0.00, 0.00},
        {0.75, 0.25, 0.00, 0.00, 0.00},
        {0.55, 0.30, 0.15, 0.00, 0.00},
        {0.45, 0.33, 0.20, 0.02, 0.00},
        {0.30, 0.40, 0.25, 0.05, 0.00},
        {0.19, 0.30, 0.35, 0.15, 0.01},
        {0.18, 0.25, 0.32, 0.22, 0.03},
        {0.15, 0.20, 0.25, 0.30, 0.10},
        {0.05, 0.10, 0.20, 0.40, 0.25}
    };
}

int Shop::rollCostCategory(int playerLevel) {
    if (playerLevel < 1) playerLevel = 1;
    if (playerLevel > 10) playerLevel = 10;

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(0.0, 1.0);
    double roll = dis(gen);

    const auto& rates = dropRates[playerLevel];
    double cumulative = 0.0;

    for (int i = 0; i < 5; ++i) {
        cumulative += rates[i];
        if (roll <= cumulative) {
            return i + 1;
        }
    }
    return 1;
}

std::vector<std::string> Shop::rollShop(int playerLevel, GlobalPool& pool) {
    std::vector<std::string> currentShop;
    for (int i = 0; i < 5; ++i) {
        int cost = rollCostCategory(playerLevel);
        std::string champ = pool.drawChampion(cost);
        currentShop.push_back(champ);
    }
    return currentShop;
}