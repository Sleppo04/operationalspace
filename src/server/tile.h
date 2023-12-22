#ifndef TILE_H
#define TILE_H

#include "gameobject.h"

typedef struct Tile {
    gameobject_t* object;    
    char          glyph; // TODO: Replace
} tile_t;

#endif // TILE_H
