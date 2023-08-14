#ifndef TILE_H
#define TILE_H

#include "gameobject.h"
#include "ship.h"

typedef enum Occupation {
    OCCUPIED_BY_NOTHING,
    OCCUPIED_BY_SHIP,
    OCCUPIED_BY_GAMEOBJECT
} occupation_t;

typedef struct Tile {
    enum Occupation occupation;
    union 
    {
        ship_t* ship;
        gameobject_t* gameobject;
    } object;
    
    char glyph; // Gets overriden by objects on it
    char color;
} tile_t;

#endif // TILE_H