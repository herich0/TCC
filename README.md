# TCC - Simulador TFT

Um simulador de combates táticos de alto desempenho desenvolvido para treinar agentes de Aprendizado por Reforço (Reinforcement Learning).

Este projeto replica as mecânicas complexas de um ambiente de *Auto Battler* (inspirado em Teamfight Tactics), utilizando uma arquitetura híbrida projetada para máxima eficiência: um back-end robusto em C++ para o motor lógico de tempo real, acoplado a um front-end em Python para orquestração de partidas e integração com bibliotecas de Inteligência Artificial.

## Arquitetura do Sistema

O simulador adota o padrão da indústria para ambientes de IA, separando a execução de alta performance das regras de negócio e controle de estado:

* **Core Engine (C++):** Responsável por todo o processamento computacional intensivo. Gerencia a matriz de combate (7x8), colisões, pathfinding via BFS (Breadth-First Search), redução de cooldowns, acúmulo de mana, instanciamento de feitiços complexos (AoE, Stuns, Execuções) e cálculos de mitigação de dano (Armadura/MR).
* **Orchestrator (Python):** Interage com o motor nativo através do `pybind11`. Funciona como o gerenciador de estado, controlando a transição de rounds (PvE/PvP), economia de ouro, rolagens de loja e operando como um wrapper (estilo OpenAI Gym) para os agentes de Aprendizado por Reforço.
* **Data Ingestion Dinâmica:** Utiliza a biblioteca `nlohmann/json` para carregar o banco de dados de campeões, itens e sinergias em tempo de execução. Isso permite ajustes de balanceamento sem a necessidade de recompilar o binário em C++.

## Funcionalidades do Motor de Combate

A engine foi construída do zero para replicar com precisão as mecânicas sistêmicas do jogo:

* **Sinergias de Pré-combate:** Distribuição de escudos de vida máxima, amplificação de dano e manipulação de status bruto logo na inicialização da matriz.
* **Sinergias In-combat:** Tratamento de eventos em tempo real, como congelamento global de tempo (Stun), execuções baseadas em limiares de HP (True Damage) e conjurações duplas de habilidades utilizando interceptação de chamadas.
* **Targeting e Pathfinding Dinâmico:** Os agentes recalculam rotas em malha hexagonal e redefinem alvos otimizados a cada tick da simulação (0.1s), reagindo instantaneamente a mortes e movimentações no tabuleiro.
* **Rastreamento Contábil para IA:** Diferenciação estrita entre dano bruto gerado e dano real mitigado. O sistema mantém uma auditoria interna (`totalDamageDealt`) por entidade, permitindo que o Python extraia métricas exatas para formular Funções de Recompensa (Reward Functions) densas e precisas para a rede neural.

## Como Compilar e Executar

O motor requer `CMake`, um compilador C++ compatível com C++17 e ambiente Python 3.

```bash
git clone [https://github.com/herich0/TCC.git](https://github.com/herich0/TCC.git)
cd TCC/tft_simulator

mkdir build
cd build
cmake ..
make

cd ..
python3 test_in_combat_traits.py