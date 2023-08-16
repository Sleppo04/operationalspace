#ifndef TILE_H
#define TILE_H

#include "gameobject.h"

typedef struct Tile {
    gameobject_t* object;    
    char glyph; // Gets overriden by objects on it
    char color;
} tile_t;

#endif // TILE_H