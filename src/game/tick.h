#ifndef TICK_H
#define TICK_H

#include <stdlib.h>

#include "action.h"
#include "game.h"

/// @brief Tick all ships that weren't already
/// @param data tick data required by the tick algorithm
/// @param game game that will be ticked
/// @return EINVAL, EXIT_SUCCESS
int Tick_Round(uint16_t* current_player, uint32_t* current_ship, game_t* game);

/// @brief Tick all ships of the current player that were not ticked already
/// @param data tick data required by the tick algorithm
/// @param game game in which the ticks will be performed
/// @return EINVAL, EXIT_SUCCESS
int Tick_Step(uint16_t* current_player, uint32_t* current_ship, game_t* game);

/// @brief Tick exactly one ship, or switch to the next player
/// @param data tick data required by the tick algorithm
/// @param game game where ships will get ticks
/// @return EINVAL, EXIT_SUCCESS
int Tick_Single(uint16_t* current_player, uint32_t* current_ship, game_t* game);


#endif //TICK_H
