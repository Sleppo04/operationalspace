#ifndef TICK_H
#define TICK_H

#include "shared.h"


//TODO: Argument to allow access to the world
int Tick_Playerwise_Round(tick_playerwise_data_t* data, game_t* game);

int Tick_Playerwise_Step(tick_playerwise_data_t* data, game_t* game);

int Tick_Playerwise_Single(tick_playerwise_data_t* data, game_t* game);

#endif //TICK_H
