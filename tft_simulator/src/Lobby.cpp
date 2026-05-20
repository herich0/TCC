#include "Lobby.hpp"
#include <random>
#include <algorithm>
#include <iostream>
#include "TraitEngine.hpp"

Lobby::Lobby(const std::map<std::string, Champion>& db) : pool(db) {
    for (int i = 0; i < 8; i++) {
        players.push_back(Player());
        boards.push_back(new Board(7, 4));
        isAlive.push_back(true);
    }
}

Lobby::~Lobby() {
    for (auto b : boards) {
        delete b;
    }
}

int Lobby::calculatePlayerDamage(int stage, int survivingUnits) {
    int baseDamage[] = {0, 0, 0, 3, 4, 6, 9, 15};
    int dmg = 0;
    if (stage < 8) {
        dmg = baseDamage[stage];
    } else {
        dmg = 15;
    }
    return dmg + (survivingUnits * 2);
}

void Lobby::playRound(CombatEngine& engine) {
    matchTracker.nextRound();
    
    int currentStage = matchTracker.getStage();
    //std::cout << "\n=== INICIANDO RODADA " << currentStage << "-" << matchTracker.getRound() << " ===" << std::endl;

    for (int i = 0; i < 8; i++) {
        if (!isAlive[i]) continue;
        players[i].addNaturalXp();
        players[i].applyInterest();
        players[i].addGold(5);
    }

    if (matchTracker.isCarousel() || matchTracker.isPvE()) {
        //std::cout << "[LOBBY] Rodada de Carrossel ou PvE. Ignorando combates PvP." << std::endl;
        return;
    }

    std::vector<int> aliveIds;
    for (int i = 0; i < 8; i++) {
        if (isAlive[i]) aliveIds.push_back(i);
    }

    std::random_device rd;
    std::mt19937 g(rd());
    std::shuffle(aliveIds.begin(), aliveIds.end(), g);

    if (aliveIds.size() % 2 != 0) {
        aliveIds.push_back(aliveIds[0]);
    }

    for (size_t i = 0; i < aliveIds.size(); i += 2) {
        int p1 = aliveIds[i];
        int p2 = aliveIds[i+1];
        
        Board arena(7, 8);
        Board* b1 = boards[p1];
        Board* b2 = boards[p2];

        for (auto pos : b1->getAllPositions()) {
            arena.placeChampion(pos.first, pos.second, *b1->getChampion(pos.first, pos.second), 1);
        }

        for (auto pos : b2->getAllPositions()) {
            arena.placeChampion(6 - pos.first, 7 - pos.second, *b2->getChampion(pos.first, pos.second), 2);
        }

        //std::cout << "\n--- Combate: Jogador " << p1 << " VS Jogador " << p2 << " ---" << std::endl;
        TraitEngine traitEngine;
        traitEngine.applyPreCombatTraits(*boards[p1], p1);
        traitEngine.applyPreCombatTraits(*boards[p2], p2);

        CombatStats stats = engine.simulate(*boards[p1], 30.0);

        bool p1Won = (stats.winningTeam == 1);
        bool p2Won = (stats.winningTeam == 2);

        traitEngine.processPostCombatTraits(*boards[p1], players[p1], p1Won);
        traitEngine.processPostCombatTraits(*boards[p2], players[p2], p2Won);

        int p1Survivors = 0;
        int p2Survivors = 0;
        for (auto pos : arena.getAllPositions()) {
            if (arena.getTeam(pos.first, pos.second) == 1) p1Survivors++;
            else p2Survivors++;
        }

        if (stats.winningTeam == 1) {
            int dmg = calculatePlayerDamage(currentStage, p1Survivors);
            //std::cout << ">> Jogador " << p1 << " VENCEU! Jogador " << p2 << " sofre " << dmg << " de dano." << std::endl;
            if (p1 != p2) players[p2].takeDamage(dmg);
        } else if (stats.winningTeam == 2) {
            int dmg = calculatePlayerDamage(currentStage, p2Survivors);
            //std::cout << ">> Jogador " << p2 << " VENCEU! Jogador " << p1 << " sofre " << dmg << " de dano." << std::endl;
            players[p1].takeDamage(dmg);
        } else {
            int dmg1 = calculatePlayerDamage(currentStage, 0);
            int dmg2 = calculatePlayerDamage(currentStage, 0);
            //std::cout << ">> EMPATE! Ambos sofrem dano base de " << dmg1 << "." << std::endl;
            players[p1].takeDamage(dmg1);
            if (p1 != p2) players[p2].takeDamage(dmg2);
        }
        
        if (players[p1].getHp() <= 0 && isAlive[p1]) {
            isAlive[p1] = false;
            //std::cout << "!!! JOGADOR " << p1 << " FOI ELIMINADO !!!" << std::endl;
        }
        if (players[p2].getHp() <= 0 && isAlive[p2]) {
            isAlive[p2] = false;
            //std::cout << "!!! JOGADOR " << p2 << " FOI ELIMINADO !!!" << std::endl;
        }
    }
}

Player* Lobby::getPlayer(int id) {
    if (id < 0 || id >= 8) return nullptr;
    return &players[id];
}

Board* Lobby::getBoard(int id) {
    if (id < 0 || id >= 8) return nullptr;
    return boards[id];
}

bool Lobby::getIsAlive(int id) const {
    if (id < 0 || id >= 8) return false;
    return isAlive[id];
}

int Lobby::getAliveCount() const {
    int count = 0;
    for (bool alive : isAlive) {
        if (alive) count++;
    }
    return count;
}

Match* Lobby::getMatchTracker() {
    return &matchTracker;
}

GlobalPool& Lobby::getPool() { return pool; }
Shop& Lobby::getShop() { return shop; }