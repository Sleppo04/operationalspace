#ifndef SECTOR_H
#define SECTOR_H

#include <stdlib.h>

#include "tile.h"

typedef struct Sector {
    size_t tile_rows;
    size_t tile_cols;
    tile_t** tiles;
} sector_t;

#endif