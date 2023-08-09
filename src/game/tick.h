#ifndef TICK_H
#define TICK_H

#include <stdlib.h>

#include "shared.h"

#include "action.h"

// All tick functions suffixed with 'Round' will tick each ship exactly once

// All tick functions suffixed with 'Step' will perform a implementation-defined amount of ship ticks

// All tick functions suffixed with 'Tick' will tick exactly one ship


int Tick_Playerwise_Round(tick_playerwise_data_t* data, game_t* game);

int Tick_Playerwise_Step(tick_playerwise_data_t* data, game_t* game);

int Tick_Playerwise_Single(tick_playerwise_data_t* data, game_t* game);

int Tick_Playerwise_Init_Data(tick_playerwise_data_t *data);


#endif //TICK_H
