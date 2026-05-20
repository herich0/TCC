#ifndef SET_LOADER_HPP
#define SET_LOADER_HPP

#include <string>
#include <map>
#include "Champion.hpp"

std::map<std::string, Champion> loadChampionsFromJson(const std::string& filepath);

#endif