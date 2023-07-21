#ifndef SECTOR_H
#define SECTOR_H

#include <stdlib.h>

#include "tile.h"

#define SECTOR_SIZE 64 // Sector edge length in tiles

typedef struct Sector {
    tile_t tiles[SECTOR_SIZE][SECTOR_SIZE];
} sector_t;

#endif