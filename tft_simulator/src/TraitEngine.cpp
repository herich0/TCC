#include "TraitEngine.hpp"
#include <algorithm>

std::map<std::string, int> TraitEngine::countTraits(Board& board, int ownerId) {
    std::map<std::string, int> counts;
    std::set<std::string> uniqueChampions;

    for (auto pos : board.getAllPositions()) {
        Champion* c = board.getChampion(pos.first, pos.second);
        if (c && c->getOwnerId() == ownerId) {
            if (uniqueChampions.find(c->getName()) == uniqueChampions.end()) {
                uniqueChampions.insert(c->getName());
                for (const std::string& trait : c->getTraits()) {
                    counts[trait]++;
                }
            }
        }
    }
    return counts;
}

void TraitEngine::applyPreCombatTraits(Board& board, int ownerId) {
    std::map<std::string, int> traits = countTraits(board, ownerId);

    for (auto pos : board.getAllPositions()) {
        Champion* c = board.getChampion(pos.first, pos.second);
        if (!c) continue;

        if (c->getOwnerId() == ownerId) {
            if (traits["Bulwark"] >= 1) {
                c->addBonusStats(0, 0, 0, 0, 0, 0.05);
                c->addShield(c->getMaxHp() * 0.05);
            }
            if (traits["Dark Lady"] >= 1) {
                c->addDurability(0.07);
            }
            if (traits["Timebreaker"] >= 2) {
                c->addBonusStats(0, 0, 0, 0, 0, 0.15);
            }
            if (traits["Challenger"] >= 2) {
                c->addBonusStats(0, 0, 0, 0, 0, 0.10);
            }
            if (traits["Marauder"] >= 2) {
                c->addOmnivamp(0.05);
            }
            if (traits["Voyager"] >= 2) {
                double shieldBase = 0, ampBase = 0;
                if (traits["Voyager"] >= 6) { shieldBase = 700; ampBase = 0.27; }
                else if (traits["Voyager"] >= 5) { shieldBase = 500; ampBase = 0.22; }
                else if (traits["Voyager"] >= 4) { shieldBase = 350; ampBase = 0.18; }
                else if (traits["Voyager"] >= 3) { shieldBase = 250; ampBase = 0.15; }
                else if (traits["Voyager"] >= 2) { shieldBase = 175; ampBase = 0.09; }

                std::vector<std::string> cTraits = c->getTraits();
                bool isVoyager = (std::find(cTraits.begin(), cTraits.end(), "Voyager") != cTraits.end());
                double multiplier = isVoyager ? 2.0 : 1.0;

                if (c->getRole() == Role::Tank) {
                    c->addShield(shieldBase * multiplier);
                } else {
                    c->addDamageAmp(ampBase * multiplier);
                }
            }
        } else {
            if (traits["Eradicator"] >= 1) {
                c->multiplyArmorAndMr(0.90);
            }
        }
    }

    for (auto pos : board.getAllPositions()) {
        Champion* c = board.getChampion(pos.first, pos.second);
        if (!c || c->getOwnerId() != ownerId) continue;

        std::vector<std::string> cTraits = c->getTraits();

        if (std::find(cTraits.begin(), cTraits.end(), "Brawler") != cTraits.end()) {
            if (traits["Brawler"] >= 6) c->addBonusStats(c->getMaxHp() * 0.60, 0, 0, 0, 0, 0);
            else if (traits["Brawler"] >= 4) c->addBonusStats(c->getMaxHp() * 0.40, 0, 0, 0, 0, 0);
            else if (traits["Brawler"] >= 2) c->addBonusStats(c->getMaxHp() * 0.20, 0, 0, 0, 0, 0);
        }
        if (std::find(cTraits.begin(), cTraits.end(), "Bastion") != cTraits.end()) {
            if (traits["Bastion"] >= 6) c->addBonusStats(0, 0, 0, 60, 60, 0);
            else if (traits["Bastion"] >= 4) c->addBonusStats(0, 0, 0, 40, 40, 0);
            else if (traits["Bastion"] >= 2) c->addBonusStats(0, 0, 0, 16, 16, 0);
        }
        if (std::find(cTraits.begin(), cTraits.end(), "Vanguard") != cTraits.end()) {
            if (traits["Vanguard"] >= 6) c->addShield(c->getMaxHp() * 0.40);
            else if (traits["Vanguard"] >= 4) c->addShield(c->getMaxHp() * 0.30);
            else if (traits["Vanguard"] >= 2) c->addShield(c->getMaxHp() * 0.16);
        }
        if (std::find(cTraits.begin(), cTraits.end(), "Space Groove") != cTraits.end()) {
            if (traits["Space Groove"] >= 5) c->addBonusStats(0, 0, 0, 0, 0, 0.50);
            else if (traits["Space Groove"] >= 3) c->addBonusStats(0, 0, 0, 0, 0, 0.25);
            else if (traits["Space Groove"] >= 1) c->addBonusStats(0, 0, 0, 0, 0, 0.10);
        }
        if (std::find(cTraits.begin(), cTraits.end(), "Dark Star") != cTraits.end()) {
            if (traits["Dark Star"] >= 6) {
                c->addBonusStats(0, c->getAd() * 0.45, 45, 0, 0, 0);
                if (c->getName() == "Cho'Gath" || c->getName() == "Mordekaiser") {
                    c->addBonusStats(750, 0, 0, 0, 0, 0);
                }
            } else if (traits["Dark Star"] >= 4) {
                c->addBonusStats(0, c->getAd() * 0.45, 45, 0, 0, 0);
            }
        }
        if (std::find(cTraits.begin(), cTraits.end(), "Doomer") != cTraits.end()) {
            c->addBonusStats(0, 30, 30, 0, 0, 0);
        }
        if (std::find(cTraits.begin(), cTraits.end(), "Timebreaker") != cTraits.end()) {
            if (traits["Timebreaker"] >= 4) c->addBonusStats(0, 0, 0, 0, 0, 0.50);
        }
        if (std::find(cTraits.begin(), cTraits.end(), "Challenger") != cTraits.end()) {
            if (traits["Challenger"] >= 5) c->addBonusStats(0, 0, 0, 0, 0, 0.55);
            else if (traits["Challenger"] >= 4) c->addBonusStats(0, 0, 0, 0, 0, 0.40);
            else if (traits["Challenger"] >= 3) c->addBonusStats(0, 0, 0, 0, 0, 0.22);
            else if (traits["Challenger"] >= 2) c->addBonusStats(0, 0, 0, 0, 0, 0.15);
        }
        if (std::find(cTraits.begin(), cTraits.end(), "Fateweaver") != cTraits.end()) {
            if (traits["Fateweaver"] >= 4) c->addCritChance(0.20);
        }
        if (std::find(cTraits.begin(), cTraits.end(), "Marauder") != cTraits.end()) {
            if (traits["Marauder"] >= 6) { c->addOmnivamp(0.10); c->addBonusStats(0, c->getAd() * 0.55, 0, 0, 0, 0); }
            else if (traits["Marauder"] >= 4) { c->addOmnivamp(0.07); c->addBonusStats(0, c->getAd() * 0.35, 0, 0, 0, 0); }
            else if (traits["Marauder"] >= 2) { c->addOmnivamp(0.05); c->addBonusStats(0, c->getAd() * 0.18, 0, 0, 0, 0); }
        }
        if (std::find(cTraits.begin(), cTraits.end(), "Rogue") != cTraits.end()) {
            if (traits["Rogue"] >= 5) c->addBonusStats(0, c->getAd() * 0.55, 55, 0, 0, 0);
            else if (traits["Rogue"] >= 4) c->addBonusStats(0, c->getAd() * 0.40, 40, 0, 0, 0);
            else if (traits["Rogue"] >= 3) c->addBonusStats(0, c->getAd() * 0.25, 25, 0, 0, 0);
            else if (traits["Rogue"] >= 2) c->addBonusStats(0, c->getAd() * 0.12, 12, 0, 0, 0);
        }
        if (std::find(cTraits.begin(), cTraits.end(), "Sniper") != cTraits.end()) {
            if (traits["Sniper"] >= 4) c->addDamageAmp(0.28);
            else if (traits["Sniper"] >= 3) c->addDamageAmp(0.24);
            else if (traits["Sniper"] >= 2) c->addDamageAmp(0.18);
        }
    }
}
void TraitEngine::processPostCombatTraits(Board& board, Player& player, bool wonCombat) {
    std::map<std::string, int> traits = countTraits(board, 0); 
    
    if (traits["Anima"] >= 3) {
        int currentStreak = player.getStreak();
        
        if (wonCombat && currentStreak < 0) {
            int losses = std::abs(currentStreak);
            int goldReward = losses * 5; 
            player.addGold(goldReward);
        }
    }
}