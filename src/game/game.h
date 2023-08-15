#ifndef GAME_H
#define GAME_H

#include "player.h"

typedef struct Game {
	arraylist_t* ship_lists;
	player_t*    players;
	uint16_t     tick_current_player;
	uint32_t     tick_current_ship;
	uint16_t     player_count;
	world_t*     world;
} game_t;

#endif
