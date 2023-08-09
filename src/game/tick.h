#ifndef TICK_H
#define TICK_H

#include <stdlib.h>

#include "shared.h"

#include "action.h"

// All tick functions suffixed with 'Round' will tick each ship that wasn't ticked already this round (via calls to _Single or _Step)

// All tick functions suffixed with 'Step' will perform a implementation-defined amount of ship ticks

// All tick functions suffixed with 'Tick' will tick exactly one ship


/// @brief Tick all ships that weren't already
/// @param data tick data required by the tick algorithm
/// @param game game that will be ticked
/// @return EINVAL, EXIT_SUCCESS
int Tick_Playerwise_Round(tick_playerwise_data_t* data, game_t* game);

/// @brief Tick all ships of the current player that were not ticked already
/// @param data tick data required by the tick algorithm
/// @param game game in which the ticks will be performed
/// @return EINVAL, EXIT_SUCCESS
int Tick_Playerwise_Step(tick_playerwise_data_t* data, game_t* game);

/// @brief Tick exactly one ship, or switch to the next player
/// @param data tick data required by the tick algorithm
/// @param game game where ships will get ticks
/// @return EINVAL, EXIT_SUCCESS
int Tick_Playerwise_Single(tick_playerwise_data_t* data, game_t* game);

/// @brief Set the tick data to reasonable start values
/// @param data data to initialize
/// @return EINVAL, EXIT_SUCCESS
int Tick_Playerwise_Init_Data(tick_playerwise_data_t *data);


#endif //TICK_H
