#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include "Player.hpp"
#include "Match.hpp"
#include "Champion.hpp"
#include "SetLoader.hpp"
#include "Shop.hpp"
#include "Board.hpp"
#include "CombatEngine.hpp"
#include "GlobalPool.hpp"
#include "Lobby.hpp"
#include "Item.hpp"

namespace py = pybind11;

PYBIND11_MODULE(tft_engine, m)
{
    py::class_<Player>(m, "Player")
        .def(py::init<>())
        .def("addGold", &Player::addGold)
        .def("applyInterest", &Player::applyInterest)
        .def("buyXp", &Player::buyXp)
        .def("addNaturalXp", &Player::addNaturalXp)
        .def("takeDamage", &Player::takeDamage)
        .def("updateStreak", &Player::updateStreak)
        .def("getGold", &Player::getGold)
        .def("getHp", &Player::getHp)
        .def("getLevel", &Player::getLevel)
        .def("getXp", &Player::getXp)
        .def("getStreak", &Player::getStreak)
        .def("buyChampion", &Player::buyChampion)
        .def("sellChampionFromBench", &Player::sellChampionFromBench)
        .def("moveBenchToBoard", &Player::moveBenchToBoard)
        .def("moveBoardToBench", &Player::moveBoardToBench)
        .def("getBenchChampion", &Player::getBenchChampion, py::return_value_policy::reference)
        .def("checkAutoCombine", &Player::checkAutoCombine)
        .def("getBench", &Player::getBench, py::return_value_policy::reference)
        .def("moveBoardToBoard", &Player::moveBoardToBoard);

    py::class_<Match>(m, "Match")
        .def(py::init<>())
        .def("nextRound", &Match::nextRound)
        .def("getStage", &Match::getStage)
        .def("getRound", &Match::getRound)
        .def("isPvE", &Match::isPvE)
        .def("isCarousel", &Match::isCarousel);

    py::enum_<Role>(m, "Role")
        .value("Tank", Role::Tank)
        .value("Fighter", Role::Fighter)
        .value("Assassin", Role::Assassin)
        .value("Marksman", Role::Marksman)
        .value("Caster", Role::Caster)
        .value("Specialist", Role::Specialist)
        .export_values();

    py::class_<Champion>(m, "Champion")
        .def(py::init<std::string, int, std::vector<std::string>, double, double, double, double, double, double, double, double, int, Role>())

        .def("takeDamage", &Champion::takeDamage, py::arg("rawDamage"), py::arg("isTrueDamage") = false, py::arg("isTargeted") = false)

        .def("heal", &Champion::heal)
        .def("performBasicAttack", &Champion::performBasicAttack)
        .def("castSpell", &Champion::castSpell)
        .def("getName", &Champion::getName)
        .def("getRole", &Champion::getRole)
        .def("getCost", &Champion::getCost)
        .def("getRange", &Champion::getRange)
        .def("getStarLevel", &Champion::getStarLevel)
        .def("getHp", &Champion::getHp)
        .def("getMaxHp", &Champion::getMaxHp)
        .def("getMana", &Champion::getMana)
        .def("getShield", &Champion::getShield)
        .def("getAd", &Champion::getAd)
        .def("getArmor", &Champion::getArmor)
        .def("getRealAttackSpeed", &Champion::getRealAttackSpeed)
        .def("getTraits", &Champion::getTraits)
        .def("getX", &Champion::getX)
        .def("getY", &Champion::getY)
        .def("getOwnerId", &Champion::getOwnerId)
        .def("getTotalDamageDealt", &Champion::getTotalDamageDealt)
        .def("triggerOvertime", &Champion::triggerOvertime)
        .def("equipItem", &Champion::equipItem);

    m.def("loadChampionsFromJson", &loadChampionsFromJson);

    py::class_<Board>(m, "Board")
        .def(py::init<int, int>())
        .def("placeChampion", &Board::placeChampion, py::arg("x"), py::arg("y"), py::arg("baseChamp"), py::arg("team"), py::arg("starLevel") = 1)
        .def("getChampion", &Board::getChampion, py::return_value_policy::reference)
        .def("getTeam", &Board::getTeam)
        .def("isOccupied", &Board::isOccupied)
        .def("calculateDistance", &Board::calculateDistance);

    py::class_<CombatStats>(m, "CombatStats")
        .def_readwrite("winningTeam", &CombatStats::winningTeam)
        .def_readwrite("team1Damage", &CombatStats::team1Damage)
        .def_readwrite("team2Damage", &CombatStats::team2Damage)
        .def_readwrite("duration", &CombatStats::duration);

    py::class_<CombatEngine>(m, "CombatEngine")
        .def(py::init<>())
        .def("simulate", &CombatEngine::simulate);

    py::class_<GlobalPool>(m, "GlobalPool")
        .def(py::init<const std::map<std::string, Champion> &>())
        .def("drawChampion", &GlobalPool::drawChampion)
        .def("returnChampion", &GlobalPool::returnChampion)
        .def("getAvailableCount", &GlobalPool::getAvailableCount)
        .def("takeChampion", &GlobalPool::takeChampion);

    py::class_<Shop>(m, "Shop")
        .def(py::init<>())
        .def("rollShop", &Shop::rollShop);

    py::class_<Lobby>(m, "Lobby")
        .def(py::init<const std::map<std::string, Champion> &>())
        .def("playRound", &Lobby::playRound)
        .def("getPlayer", &Lobby::getPlayer, py::return_value_policy::reference)
        .def("getBoard", &Lobby::getBoard, py::return_value_policy::reference)
        .def("getIsAlive", &Lobby::getIsAlive)
        .def("getAliveCount", &Lobby::getAliveCount)
        .def("getMatchTracker", &Lobby::getMatchTracker, py::return_value_policy::reference)
        .def("getPool", &Lobby::getPool, py::return_value_policy::reference)
        .def("getShop", &Lobby::getShop, py::return_value_policy::reference);

    py::class_<Item>(m, "Item")
        .def(py::init<>()) 
        .def_readwrite("name", &Item::name)
        .def_readwrite("passives", &Item::passives);

    m.def("loadItemsFromJson", &loadItemsFromJson, py::arg("filepath"));

}