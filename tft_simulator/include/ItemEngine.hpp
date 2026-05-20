#ifndef ITEMENGINE_HPP
#define ITEMENGINE_HPP

#include "Champion.hpp"
#include "Board.hpp"

void triggerCombatStart(Champion* champ, Board& board);
void triggerOnTick(Champion* champ, double tick, double currentTime, Board& board);
void triggerOnAttack(Champion* attacker, Champion* target, Board& board);
void triggerOnTakeDamage(Champion* victim, Champion* attacker, double damage, Board& board);

#endif