#ifndef GAME_SHARED_H
#define GAME_SHARED_H
// Shared header to avoid cyclic includes

#include <stdint.h>

#include "player.h"
#include "../logic/arraylist/arraylist.h"


typedef enum TickType {
	TICK_PLAYERWISE
} tick_type_t;

typedef struct TickPlayerWiseData {
	uint16_t current_player_index;
	uint32_t current_ship_index;
} tick_playerwise_data_t;

typedef union TickData {
	tick_playerwise_data_t playerwise;
} tick_data_t;

typedef struct Game {
	arraylist_t* ship_lists;
	tick_type_t  tick_type;
	tick_data_t  tick_information;
	player_t*    players;
	uint16_t     player_count;
	world_t*     world;
} game_t;


#endif //GAME_SHARED_H