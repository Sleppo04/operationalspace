#ifndef GAME_H
#define GAME_H

#include "gameobject.h"
#include "player.h"
#include "world.h"
#include "tick.h"

typedef struct Game {
	tick_type_t tick_type;
	tick_data_t tick_information;
	player_t*   players;
	uint16_t    player_count;
	world_t*    world;
} game_t;

int Game_TickRound(game_t* game);
int Game_TickStep(game_t* game);   // For future implementation
int Game_TickSingle(game_t* game); // For future implementation

#endif
