import sys
import os

sys.path.append(os.path.abspath("build"))
import tft_engine as tft

def main():
    json_path = os.path.join(os.path.dirname(__file__), "data", "toy_set.json")
    champions_db = tft.loadChampionsFromJson(json_path)
    
    lobby = tft.Lobby(champions_db)
    motor = tft.CombatEngine()
    
    # Pegando o Jogador 0, seu tabuleiro e a Pool Global
    p0 = lobby.getPlayer(0)
    p0_board = lobby.getBoard(0)
    pool = lobby.getPool()
    
    print("\n================================================")
    print("TESTE DE IA: ECONOMIA, RESERVAS E AUTO-COMBINE")
    print("================================================\n")
    
    # Dando ouro para o Jogador 0 poder brincar
    p0.addGold(50)
    print(f"-> Jogador 0 recebeu ouro. Saldo: {p0.getGold()}")
    
    # Comprando 3 Aatrox (Custo 1) do nosso novo banco de dados
    print("-> Jogador 0 comprou 3x 'Aatrox' da loja...")
    p0.buyChampion("Aatrox", 1, pool, champions_db)
    p0.buyChampion("Aatrox", 1, pool, champions_db)
    p0.buyChampion("Aatrox", 1, pool, champions_db)
    
    print("\n[BANCADA ANTES DA FUSÃO]")
    for i in range(9):
        c = p0.getBenchChampion(i)
        if c:
            print(f"Slot {i}: {c.getName()} ({c.getStarLevel()} Estrelas)")
            
    # Chamando a verificação de fusão (Auto-Combine)
    print("\n-> Sistema de Auto-Combine acionado...")
    p0.checkAutoCombine(p0_board)
    
    print("\n[BANCADA APÓS A FUSÃO]")
    for i in range(9):
        c = p0.getBenchChampion(i)
        if c:
            print(f"Slot {i}: {c.getName()} ({c.getStarLevel()} Estrelas)")
            
    # Movendo o Aatrox 2 Estrelas (que está no Slot 0) para o tabuleiro
    print("\n-> Movendo o Aatrox Prata do Slot 0 para o Tabuleiro (X=3, Y=0)...")
    sucesso = p0.moveBenchToBoard(0, 3, 0, p0_board)
    if sucesso:
        print("Peça posicionada com sucesso!")
        
    # Distribuindo Cho'Gaths para os outros jogadores apanharem do Aatrox 2 estrelas
    for i in range(1, 8):
        # Alterado de Sett para Cho'Gath
        lobby.getBoard(i).placeChampion(3, 0, champions_db["Cho'Gath"], 1)
        
    print("\n================================================")
    print("INICIANDO A SIMULAÇÃO ATÉ O ESTÁGIO 2-7")
    print("================================================\n")
    
    match = lobby.getMatchTracker()
    
    # Loop que roda até o Estágio 3 começar
    while match.getStage() < 3:
        lobby.playRound(motor)
        
        # Quebra o loop no final do estágio 2
        if match.getStage() == 2 and match.getRound() == 7:
            print("\n--- STATUS DO LOBBY FIM DO ESTÁGIO 2 ---")
            for i in range(8):
                if lobby.getIsAlive(i):
                    print(f"Jogador {i} | HP: {lobby.getPlayer(i).getHp()} | Ouro: {lobby.getPlayer(i).getGold()}")
            print("=======================================\n")
            break

if __name__ == "__main__":
    main()