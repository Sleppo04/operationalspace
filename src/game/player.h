#ifndef PLAYER_H
#define PLAYER_H

#include "script.h"

typedef struct Player {
	player_scripts_t scripts;
	arraylist_t      ship_list;
	uint16_t         player_id;
	char             name[64];
} player_t;

#endif //PLAYER_H
