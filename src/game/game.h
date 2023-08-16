#ifndef GAME_H
#define GAME_H

#include "player.h"

typedef struct Game {
	arraylist_t* ship_lists;
	player_t*    players;
	uint16_t     tick_current_player;  // Never access this variable, only use it to pass it to the tick function, it may be inaccurate when accessed while ticking
	uint32_t     tick_current_ship;    // Never access this variable, only use it to pass it to the tick function, it may be inaccurate when accessed while ticking
	uint16_t     player_count;
	world_t*     world;
} game_t;

/// @brief Tick ship ship of player player in game game
/// @param game 
/// @param player 
/// @param ship 
/// @return EINVAL, ENOMEM
int Game_TickShip(game_t *game, player_t *player, gameobject_t *ship);

/// @brief Tick all ships left in the current round in game
/// @param game game to tick
/// @return EINVAL
int Game_TickRound(game_t *game);

/// @brief Tick All remaining ships from one player in game
/// @param game game to tick
/// @return EINVAL
int Game_TickStep(game_t *game);

/// @brief Tick exactly one ship in game
/// @param game game to tick
/// @return EINVAL
int Game_TickSingle(game_t *game);

#endif

