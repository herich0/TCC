cmake_minimum_required(VERSION 3.12)
project(tft_simulator)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(PYBIND11_FINDPYTHON ON)
find_package(pybind11 REQUIRED)
include(FetchContent)
FetchContent_Declare(
    nlohmann_json
    GIT_REPOSITORY https://github.com/nlohmann/json.git
    GIT_TAG v3.11.3
)
FetchContent_MakeAvailable(nlohmann_json)

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