cmake_minimum_required(VERSION 3.12)
project(tft_simulator)

set(PYBIND11_FINDPYTHON ON)
find_package(pybind11 REQUIRED)
find_package(nlohmann_json REQUIRED)

include_directories(include)

pybind11_add_module(tft_engine 
    src/engine.cpp 
    src/Player.cpp
    src/Match.cpp
    src/Champion.cpp
    src/SetLoader.cpp
    src/Shop.cpp
    src/Board.cpp
    src/CombatEngine.cpp
    src/GlobalPool.cpp
    src/Lobby.cpp
    src/TraitEngine.cpp
    src/Spells.cpp
    src/ItemLoader.cpp
    src/ItemEngine.cpp
    )

target_link_libraries(tft_engine PRIVATE nlohmann_json::nlohmann_json)