#include "CombatEngine.hpp"
#include "Spells.hpp"
#include "ItemEngine.hpp"
#include "TraitEngine.hpp"
#include <iostream>
#include <limits>
#include <vector>
#include <queue>
#include <map>
#include <algorithm>

std::pair<int, int> getNextMoveBFS(Board& board, int startX, int startY, int targetX, int targetY, int range) {
    std::queue<std::pair<int, int>> q;
    std::map<std::pair<int, int>, std::pair<int, int>> cameFrom;
    
    q.push({startX, startY});
    cameFrom[{startX, startY}] = {-1, -1};

    std::pair<int, int> goal = {-1, -1};
    
    int dx[] = {1, -1, 0, 0};
    int dy[] = {0, 0, 1, -1};

    while (!q.empty()) {
        auto current = q.front();
        q.pop();

        if (board.calculateDistance(current.first, current.second, targetX, targetY) <= range) {
            goal = current;
            break;
        }

        for (int i = 0; i < 4; i++) {
            int nx = current.first + dx[i];
            int ny = current.second + dy[i];
            std::pair<int, int> next = {nx, ny};

            if (nx >= 0 && nx < 7 && ny >= 0 && ny < 8) {
                if (cameFrom.find(next) == cameFrom.end() && !board.isOccupied(nx, ny)) {
                    cameFrom[next] = current;
                    q.push(next);
                }
            }
        }
    }

    if (goal.first == -1 || goal == std::pair<int, int>{startX, startY}) {
        return {startX, startY};
    }

    std::pair<int, int> step = goal;
    while (cameFrom[step] != std::pair<int, int>{startX, startY}) {
        step = cameFrom[step];
    }
    return step;
}

void printBoardState(Board& board, double currentTime) {
    std::cout << "\n[MATRIZ DE COMBATE @ " << currentTime << "s]\n";
    for (int y = 7; y >= 0; y--) {
        for (int x = 0; x < 7; x++) {
            Champion* c = board.getChampion(x, y);
            if (c) {
                std::string pName = c->getName().substr(0, 3);
                int team = board.getTeam(x, y);
                std::cout << "[" << pName << team << "] ";
            } else {
                std::cout << "[ -- ] ";
            }
        }
        std::cout << "\n";
    }
    std::cout << "\n";
}

CombatStats CombatEngine::simulate(Board& board, double timeLimit) {
    CombatStats stats = {0, 0.0, 0.0, 0.0};
    double tick = 0.1;
    double currentTime = 0.0;
    double lastBoardPrint = -1.0;
    bool overtimeTriggered = false;
    bool novaTriggered = false;
    
    std::map<std::pair<int, int>, double> cooldowns;

    TraitEngine traitEngine;
    traitEngine.applyPreCombatTraits(board, 1);
    traitEngine.applyPreCombatTraits(board, 2);

    for (auto pos : board.getAllPositions()) {
        Champion* c = board.getChampion(pos.first, pos.second);
        if (c && c->getHp() > 0) {
            triggerCombatStart(c, board);
        }
    }

    while (currentTime < timeLimit) {
        
        // if (currentTime - lastBoardPrint >= 2.0) {
        //     printBoardState(board, currentTime);
        //     lastBoardPrint = currentTime;
        // }

        if (currentTime >= 6.0 && !novaTriggered) {
            novaTriggered = true;
            int novaTeam = 0;
            for (auto pos : board.getAllPositions()) {
                Champion* c = board.getChampion(pos.first, pos.second);
                if (c) {
                    std::vector<std::string> cTraits = c->getTraits();
                    if (std::find(cTraits.begin(), cTraits.end(), "NOVA") != cTraits.end()) {
                        novaTeam = board.getTeam(pos.first, pos.second);
                        break;
                    }
                }
            }
            if (novaTeam != 0) {
                for (auto pos : board.getAllPositions()) {
                    Champion* c = board.getChampion(pos.first, pos.second);
                    if (c && board.getTeam(pos.first, pos.second) != novaTeam) {
                        c->applyStun(2.0);
                    }
                }
            }
        }

        if (currentTime >= 30.0 && !overtimeTriggered) {
            overtimeTriggered = true;
            //std::cout << ">>> [" << currentTime << "s] TEMPO ESGOTADO! URF OVERTIME ATIVADO! <<<\n";
            
            for (auto pos : board.getAllPositions()) {
                Champion* c = board.getChampion(pos.first, pos.second);
                if (c && c->getHp() > 0) {
                    c->triggerOvertime();
                }
            }
        }

        auto positions = board.getAllPositions();
        
        int team1Count = 0;
        int team2Count = 0;
        
        for (auto pos : positions) {
            Champion* champ = board.getChampion(pos.first, pos.second);
            if (!champ || champ->getHp() <= 0) continue;
            
            int myTeam = board.getTeam(pos.first, pos.second);
            if (myTeam == 1) team1Count++;
            if (myTeam == 2) team2Count++;
            
            champ->updateTimers(tick);
            champ->applyPassiveMana();
            
            triggerOnTick(champ, tick, currentTime, board);
            
            if (!champ->canAct()) continue;
                        
            if (cooldowns.find(pos) == cooldowns.end()) {
                cooldowns[pos] = 0.0;
            }
            
            cooldowns[pos] -= tick;

            // BLOCO DE CONJURAÇÃO DE HABILIDADE
            if (champ->canCast()) {
                std::pair<int, int> targetPos = {-1, -1};
                int minDistance = std::numeric_limits<int>::max();
                
                for (auto ePos : board.getAllPositions()) {
                    Champion* enemy = board.getChampion(ePos.first, ePos.second);
                    if (enemy && enemy->getHp() > 0 && board.getTeam(ePos.first, ePos.second) != myTeam) {
                        int dist = board.calculateDistance(pos.first, pos.second, ePos.first, ePos.second);
                        if (dist < minDistance) { minDistance = dist; targetPos = ePos; }
                    }
                }

                if (targetPos.first != -1) {
                    Champion* target = board.getChampion(targetPos.first, targetPos.second);

                    int casts = 1;
                    std::vector<std::string> champTraits = champ->getTraits();
                    
                    if (std::find(champTraits.begin(), champTraits.end(), "Replicator") != champTraits.end()) {
                        casts = 2;
                    }

                    for (int i = 0; i < casts; i++) {
                        if (target->getHp() <= 0) break;

                        double preSpellDamage = champ->getTotalDamageDealt();

                        if (i == 0) {
                            champ->castSpell();
                            //std::cout << "[" << currentTime << "s] " << champ->getName() << " CONJUROU sua habilidade em " << target->getName() << "!\n";
                        } else {
                            //std::cout << "[" << currentTime << "s] " << champ->getName() << " RECONJUROU (Replicator) em " << target->getName() << "!\n";
                        }
                        
                        std::string name = champ->getName();

                        if (name == "Blitzcrank") castBlitzcrankSpell(champ, target, board);
                        else if (name == "Fiora") castFioraSpell(champ, target, board);
                        else if (name == "Jhin") castJhinSpell(champ, target, board);
                        else if (name == "Morgana") castMorganaSpell(champ, target, board);
                        else if (name == "Shen") castShenSpell(champ, target, board);
                        else if (name == "Vex") castVexSpell(champ, target, board);
                        else if (name == "Karma") castKarmaSpell(champ, target, board);
                        else if (name == "Kindred") castKindredSpell(champ, target, board);
                        else if (name == "Nami") castNamiSpell(champ, target, board);
                        else if (name == "Riven") castRivenSpell(champ, target, board);
                        else if (name == "Aurora") castAuroraSpell(champ, target, board);
                        else if (name == "Illaoi") castIllaoiSpell(champ, target, board);
                        else if (name == "Kai'Sa") castKaisaSpell(champ, target, board);
                        else if (name == "Maokai") castMaokaiSpell(champ, target, board);
                        else if (name == "Ornn") castOrnnSpell(champ, target, board);
                        else if (name == "Samira") castSamiraSpell(champ, target, board);
                        else if (name == "Akali") castAkaliSpell(champ, target, board);
                        else if (name == "Gwen") castGwenSpell(champ, target, board);
                        else if (name == "Jinx") castJinxSpell(champ, target, board);
                        else if (name == "Milio") castMilioSpell(champ, target, board);
                        else if (name == "Mordekaiser") castMordekaiserSpell(champ, target, board);
                        else if (name == "Pantheon") castPantheonSpell(champ, target, board);
                        else if (name == "Aatrox") castAatroxSpell(champ, target, board);
                        else if (name == "Briar") castBriarSpell(champ, target, board);
                        else if (name == "Caitlyn") castCaitlynSpell(champ, target, board);
                        else if (name == "Cho'Gath") castChogathSpell(champ, target, board);
                        else if (name == "Ezreal") castEzrealSpell(champ, target, board);
                        else if (name == "Lissandra") castLissandraSpell(champ, target, board);
                        else if (name == "Nasus") castNasusSpell(champ, target, board);

                        double spellDamage = champ->getTotalDamageDealt() - preSpellDamage;

                        if (std::find(champTraits.begin(), champTraits.end(), "Dark Star") != champTraits.end()) {
                            if (target->getHp() > 0 && target->getHp() <= target->getMaxHp() * 0.08) {
                                double execDmg = target->takeDamage(9999.0, true, true);
                                champ->addDamageDealt(execDmg);
                                spellDamage += execDmg;
                                //std::cout << ">>> [" << currentTime << "s] " << target->getName() << " foi EXECUTADO por " << champ->getName() << " (Dark Star)!\n";
                            }
                        }

                        if (myTeam == 1) stats.team1Damage += spellDamage;
                        else stats.team2Damage += spellDamage;
                    }
                }
                
                cooldowns[pos] = std::max(cooldowns[pos], 0.7);
                continue; 
            }
            
            // BLOCO DE ATAQUE BÁSICO
            if (cooldowns[pos] <= 0) {
                std::pair<int, int> targetPos = {-1, -1};
                int minDistance = std::numeric_limits<int>::max();
                
                for (auto ePos : board.getAllPositions()) {
                    Champion* enemy = board.getChampion(ePos.first, ePos.second);
                    if (!enemy || enemy->getHp() <= 0 || board.getTeam(ePos.first, ePos.second) == myTeam) continue;
                    
                    int dist = board.calculateDistance(pos.first, pos.second, ePos.first, ePos.second);
                    if (dist < minDistance) {
                        minDistance = dist;
                        targetPos = ePos;
                    }
                }
                
                if (targetPos.first != -1) {
                    if (minDistance <= champ->getRange()) {
                        Champion* enemy = board.getChampion(targetPos.first, targetPos.second);
                        double rawDamage = champ->performBasicAttack();
                        
                        double actualDamage = enemy->takeDamage(rawDamage, false, true);
                        
                        triggerOnAttack(champ, enemy, board);
                        triggerOnTakeDamage(enemy, champ, actualDamage, board);
                        
                        std::vector<std::string> champTraits = champ->getTraits();
                        if (std::find(champTraits.begin(), champTraits.end(), "Dark Star") != champTraits.end()) {
                            if (enemy->getHp() > 0 && enemy->getHp() <= enemy->getMaxHp() * 0.08) {
                                double execDmg = enemy->takeDamage(9999.0, true, true);
                                actualDamage += execDmg;
                                //std::cout << ">>> [" << currentTime << "s] " << enemy->getName() << " foi EXECUTADO por " << champ->getName() << " (Dark Star)!\n";
                            }
                        }

                        champ->addDamageDealt(actualDamage);                        
                        //std::cout << "[" << currentTime << "s] [T" << myTeam << "] " << champ->getName() 
                                  //<< " atacou [T" << board.getTeam(targetPos.first, targetPos.second) << "] " 
                                  //<< enemy->getName() << " (" << actualDamage << " dano. Inimigo com " << enemy->getHp() << " HP).\n";
                        if (myTeam == 1) stats.team1Damage += actualDamage;
                        else stats.team2Damage += actualDamage;

                        cooldowns[pos] = 1.0 / champ->getRealAttackSpeed();
                    } else {
                        std::pair<int, int> nextStep = getNextMoveBFS(board, pos.first, pos.second, targetPos.first, targetPos.second, champ->getRange());
                        if (nextStep.first != pos.first || nextStep.second != pos.second) {
                            if (board.moveChampion(pos.first, pos.second, nextStep.first, nextStep.second)) {
                                //std::cout << "[" << currentTime << "s] " << champ->getName() << " andou para (" << nextStep.first << "," << nextStep.second << ").\n";
                                cooldowns[nextStep] = 0.2;
                                cooldowns.erase(pos);
                            }
                        } else {
                            cooldowns[pos] = 0.2;
                        }
                    }
                }
            }
        }
        
        for (auto pos : board.getAllPositions()) {
            Champion* c = board.getChampion(pos.first, pos.second);
            if (c && c->getHp() <= 0) {
                //std::cout << ">>> [" << currentTime << "s] " << c->getName() << " MORREU!\n";
                board.removeChampion(pos.first, pos.second);
                cooldowns.erase(pos);
            }
        }
        
        if (team1Count == 0 || team2Count == 0) {
            if (team1Count > 0 && team2Count == 0) stats.winningTeam = 1;
            else if (team2Count > 0 && team1Count == 0) stats.winningTeam = 2;
            //std::cout << "--- FIM DE COMBATE EM " << currentTime << " SEGUNDOS ---\n";
            break;
        }
        
        currentTime += tick;
        stats.duration = currentTime;
    }
    
    return stats;
}