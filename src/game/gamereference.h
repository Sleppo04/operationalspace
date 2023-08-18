#ifndef GAMEREFERENCE_H
#define GAMEREFERENCE_H

#include "world.h"

// This is a struct with references to all the members of game_t
// This is used to not reference game_t directly to avoid circular dependencies
typedef struct GameReference {
	world_t**  world;
	uint16_t*  player_count;
	uint16_t*  tick_current_player;
	uint32_t*  tick_current_ship;
	player_t** players;
} game_reference_t;

#endif //GAMEREFERENCE_H
