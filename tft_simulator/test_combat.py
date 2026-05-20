import sys
import os

sys.path.append(os.path.abspath("build"))
import tft_engine as tft

def test_in_combat_traits():
    json_path = os.path.join(os.path.dirname(__file__), "data", "toy_set.json")
    champions_db = tft.loadChampionsFromJson(json_path)
    
    arena = tft.Board(7, 8)
    motor = tft.CombatEngine()

    arena.placeChampion(3, 2, champions_db["Aatrox"], 1, 2)
    arena.placeChampion(4, 2, champions_db["Maokai"], 1, 2)
    arena.placeChampion(3, 0, champions_db["Caitlyn"], 1, 2)

    arena.placeChampion(3, 5, champions_db["Gwen"], 2, 2)
    arena.placeChampion(2, 7, champions_db["Lissandra"], 2, 2)
    arena.placeChampion(4, 7, champions_db["Vex"], 2, 2)

    print("--- INICIANDO TESTE DE SINERGIAS EM COMBATE ---")

    stats = motor.simulate(arena, 20.0)

    print("\n--- RESULTADO DO COMBATE ---")
    print(f"Time Vencedor: {stats.winningTeam}")
    print(f"Duração: {stats.duration:.1f}s")
    print(f"Dano Time 1: {stats.team1Damage:.1f}")
    print(f"Dano Time 2: {stats.team2Damage:.1f}")

if __name__ == "__main__":
    test_in_combat_traits()