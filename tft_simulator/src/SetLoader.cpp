#include "SetLoader.hpp"
#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>
#include <vector>

using json = nlohmann::json;

std::map<std::string, Champion> loadChampionsFromJson(const std::string& filepath) {
    std::map<std::string, Champion> champions;
    std::ifstream file(filepath);
    
    if (!file.is_open()) {
        std::cerr << "Falha ao abrir o arquivo: " << filepath << std::endl;
        return champions;
    }

    json j;
    file >> j;

    for (auto& el : j.items()) {
        auto& item = el.value();
        
        std::string name = item["name"];
        int cost = item["cost"];
        
        std::vector<std::string> traits;
        Role championRole = Role::Specialist; 

        for (const auto& trait : item["traits"]) {
            std::string t = trait.get<std::string>();
            traits.push_back(t);
            
            if (t == "Marksman" || t == "Sniper") championRole = Role::Marksman;
            else if (t == "Caster") championRole = Role::Caster;
            else if (t == "Vanguard" || t == "Bastion" || t == "Brawler") championRole = Role::Tank;
            else if (t == "Fighter") championRole = Role::Fighter;
            else if (t == "Rogue" || t == "Assassin") championRole = Role::Assassin;
        }

        double maxHp = item["maxHp"];
        double manaMax = item["manaMax"];
        double manaInitial = item["manaInitial"];
        double ad = item["ad"];
        double ap = item["ap"];
        double armor = item["armor"];
        double mr = item["mr"];
        double as = item["as"];
        int range = item["range"];

        Champion champ(
            name, cost, traits, maxHp, manaMax, manaInitial, 
            ad, ap, armor, mr, as, range, championRole
        );
        
        champions.insert({name, champ});
    }

    return champions;
}