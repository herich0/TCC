#include "ItemEngine.hpp"
#include <cmath>
#include <vector>

void triggerCombatStart(Champion* champ, Board& board) {
    for (const auto& item : champ->getInventory()) {
        for (const auto& passive : item.passives) {
            if (passive == "crownguard_shield") {
                champ->addShield(champ->getMaxHp() * 0.25);
            }
        }
    }
}

void triggerOnTick(Champion* champ, double tick, double currentTime, Board& board) {
    int currentDeci = std::round(currentTime * 10.0);

    for (const auto& item : champ->getInventory()) {
        for (const auto& passive : item.passives) {
            if (passive == "dragon_claw_regen") {
                if (currentDeci > 0 && currentDeci % 20 == 0) {
                    champ->heal(champ->getMaxHp() * 0.025);
                }
            }
            else if (passive == "archangel_scaling") {
                if (currentDeci > 0 && currentDeci % 50 == 0) {
                    champ->addBonusStats(0.0, 0.0, 20.0, 0.0, 0.0, 0.0);
                }
            }
        }
    }
}

void triggerOnAttack(Champion* attacker, Champion* target, Board& board) {
    for (const auto& item : attacker->getInventory()) {
        for (const auto& passive : item.passives) {
            if (passive == "shojin_mana_on_attack") {
                attacker->addMana(5.0);
            }
        }
    }
}

void triggerOnTakeDamage(Champion* victim, Champion* attacker, double damage, Board& board) {
    for (const auto& item : victim->getInventory()) {
        for (const auto& passive : item.passives) {
            if (passive == "bramble_thorns") {
                std::vector<Champion*> adjacentEnemies = board.getChampionsInRange(victim->getX(), victim->getY(), 1, victim->getOwnerId());
                for (Champion* enemy : adjacentEnemies) {
                    enemy->takeDamage(100.0, false, false);
                }
            }
        }
    }
}