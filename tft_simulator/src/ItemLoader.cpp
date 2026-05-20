#include "Item.hpp"
#include <nlohmann/json.hpp>
#include <fstream>
#include <iostream>

using json = nlohmann::json;

std::map<std::string, Item> loadItemsFromJson(const std::string& filepath) {
    std::map<std::string, Item> itemsDb;
    std::ifstream file(filepath);

    if (!file.is_open()) {
        std::cerr << "[ERRO] Nao foi possivel abrir o arquivo de itens: " << filepath << std::endl;
        return itemsDb;
    }

    json j;
    try {
        file >> j;
    } catch (const json::parse_error& e) {
        std::cerr << "[ERRO] Falha no parse do JSON de itens: " << e.what() << std::endl;
        return itemsDb;
    }

    for (auto it = j.begin(); it != j.end(); ++it) {
        Item item;
        item.name = it.key();

        auto& val = it.value();

        if (val.contains("adPercent")) item.adPercent = val["adPercent"];
        if (val.contains("ap")) item.ap = val["ap"];
        if (val.contains("armor")) item.armor = val["armor"];
        if (val.contains("mr")) item.mr = val["mr"];
        if (val.contains("hp")) item.hp = val["hp"];
        if (val.contains("hpPercent")) item.hpPercent = val["hpPercent"];
        if (val.contains("asPercent")) item.asPercent = val["asPercent"];
        if (val.contains("critChance")) item.critChance = val["critChance"];
        if (val.contains("manaRegenPercent")) item.manaRegenPercent = val["manaRegenPercent"];
        if (val.contains("omnivamp")) item.omnivamp = val["omnivamp"];
        if (val.contains("damageAmp")) item.damageAmp = val["damageAmp"];
        if (val.contains("durability")) item.durability = val["durability"];

        if (val.contains("passives") && val["passives"].is_array()) {
            for (const auto& p : val["passives"]) {
                item.passives.push_back(p.get<std::string>());
            }
        }

        itemsDb[item.name] = item;
    }

    //std::cout << "[INFO] " << itemsDb.size() << " itens carregados com sucesso de " << filepath << std::endl;
    return itemsDb;
}