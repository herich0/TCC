#ifndef SHOP_HPP
#define SHOP_HPP

#include <vector>
#include <string>
#include "GlobalPool.hpp"

class Shop {
private:
    std::vector<std::vector<double>> dropRates;
    int rollCostCategory(int playerLevel);

public:
    Shop();
    std::vector<std::string> rollShop(int playerLevel, GlobalPool& pool);
};

#endif