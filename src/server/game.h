#ifndef GAME_H
#define GAME_H

#include <world.h>
#include <vfs.h>

typedef struct game_s
{
    world_t* world;
    vfs_t*   vfs;
} game_t;

int Game_NewGame(game_t* game);
int Game_LoadGame(game_t* game, char path);

#endif // GAME_H
