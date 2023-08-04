#ifndef WORLD_H
#define WORLD_H

#include <errno.h>
#include <float.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "sector.h"
#include "feature.h"
#include "../logic/arraylist/arraylist.h"
#include "../logic/coordinate/coordinate.h"
#include "../logic/rand/xoshiro256.h"


typedef struct World {
    xoshiro256_state_t seed;
    unsigned int       sector_rows;
    unsigned int       sector_cols;
    sector_t*          sectors;
    arraylist_t        objects;
} world_t;

void World_Create(world_t* world, unsigned int width, unsigned int height);

/// @brief get a sector at row and col from the world
/// @param world world to get the sector from
/// @param row in which row is the sector
/// @param col what is the column of the sector
/// @param destination where the result pointer will be written
/// @return int, errorcodes (EINVAL, EDESTADDRREQ)
int World_GetSector(world_t* world, size_t row, size_t col, sector_t** destination);

/// @brief Get a tile from its absolute address 
/// @param x The tiles absolute x-position
/// @param y The tiles absolute y-position
/// @return Pointer to the requested tile struct
tile_t* World_GetTile(world_t* world, int x, int y);

#endif
