#ifndef GLOBAL_POOL_HPP
#define GLOBAL_POOL_HPP

#include <string>
#include <map>
#include <vector>
#include "Champion.hpp"

class GlobalPool {
private:
    std::map<int, std::vector<std::string>> pool;

public:
    GlobalPool(const std::map<std::string, Champion>& db);
    std::string drawChampion(int cost);
    void returnChampion(const std::string& name, int cost);
    int getAvailableCount(const std::string& name) const;
    bool takeChampion(std::string name);
};

#endif