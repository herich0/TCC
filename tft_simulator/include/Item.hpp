#ifndef ITEM_HPP
#define ITEM_HPP

#include <string>
#include <vector>
#include <map>

struct Item {
    std::string name;
    
    double adPercent = 0.0;
    double ap = 0.0;
    double armor = 0.0;
    double mr = 0.0;
    double hp = 0.0;
    double hpPercent = 0.0;
    double asPercent = 0.0;
    double critChance = 0.0;
    double manaRegenPercent = 0.0;
    double omnivamp = 0.0;
    double damageAmp = 0.0;
    double durability = 0.0;

    std::vector<std::string> passives;
};

std::map<std::string, Item> loadItemsFromJson(const std::string& filepath);

#endif