import sys
import os

sys.path.append(os.path.abspath("build"))
import tft_engine as tft

def test_balance():
    champions_db = tft.loadChampionsFromJson("data/toy_set.json")
    items_db = tft.loadItemsFromJson("data/items.json")

    arena = tft.Board(7, 8)
    motor = tft.CombatEngine()

    arena.placeChampion(2, 3, champions_db["Maokai"], 1, 2)
    mao_item = arena.getChampion(2, 3)
    mao_item.equipItem(items_db["Warmog's Armor"])
    mao_item.equipItem(items_db["Bramble Vest"])
    mao_item.equipItem(items_db["Dragon's Claw"])

    arena.placeChampion(4, 3, champions_db["Maokai"], 1, 2)

    arena.placeChampion(2, 0, champions_db["Caitlyn"], 1, 2)
    cait_item = arena.getChampion(2, 0)
    cait_item.equipItem(items_db["Spear of Shojin"])
    cait_item.equipItem(items_db["Infinity Edge"])
    cait_item.equipItem(items_db["Last Whisper"])

    arena.placeChampion(4, 0, champions_db["Caitlyn"], 1, 2)

    arena.placeChampion(2, 4, champions_db["Mordekaiser"], 2, 2)
    arena.placeChampion(4, 4, champions_db["Mordekaiser"], 2, 2)

    arena.placeChampion(2, 7, champions_db["Lissandra"], 2, 2)
    liss_item1 = arena.getChampion(2, 7)
    liss_item1.equipItem(items_db["Blue Buff"])
    liss_item1.equipItem(items_db["Rabadon's Deathcap"])
    liss_item1.equipItem(items_db["Jeweled Gauntlet"])

    arena.placeChampion(4, 7, champions_db["Lissandra"], 2, 2)
    liss_item2 = arena.getChampion(4, 7)
    liss_item2.equipItem(items_db["Archangel's Staff"])
    liss_item2.equipItem(items_db["Spear of Shojin"])
    liss_item2.equipItem(items_db["Morellonomicon"])

    print("\n--- INICIANDO TESTE DE BALANCEAMENTO (4v4) ---")
    stats = motor.simulate(arena, 40.0)

    print("\n--- ESTATÍSTICAS FINAIS ---")
    print(f"Time Vencedor: {stats.winningTeam}")
    print(f"Duração do Combate: {stats.duration:.1f}s")
    print(f"Dano Time 1 (Cait/Mao): {stats.team1Damage:.1f}")
    print(f"Dano Time 2 (Liss/Morde): {stats.team2Damage:.1f}")

if __name__ == "__main__":
    test_balance()