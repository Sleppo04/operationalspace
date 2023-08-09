#ifndef GAME_H
#define GAME_H

#include "shared.h"

#include "tick.h"

int Game_TickRound(game_t* game);
int Game_TickStep(game_t* game); 
int Game_TickSingle(game_t* game);

#endif
