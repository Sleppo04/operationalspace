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


typedef struct World {
    size_t sector_rows;
    size_t sector_cols;
    sector_t* sectors;
    unsigned int seed;
} world_t;

/**
 * @brief Generate a new world
 * 
 * @param sector_rows How many sectors should there be vertically
 * @param sector_cols How many sectors should there be horizontally
 * @param features What features should be generated (everything is a game object)
 * @param destination world_t struct to place everything in
 * @param seed seed for the world generation
 * @return int error codes (EINVAL, EDESTADDRREQ, ENOMEM)
 */
int generate_world(size_t sector_rows, size_t sector_cols, feature_t* features, world_t* destination, unsigned int seed);

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

/// @brief Populate a sector with generated tiles
/// @param sector sector to populate
/// @param row what row is the sector in
/// @param col what col is the sector in
/// @param features what features are available for placement
/// @param feature_count how many features are there
/// @param placed_features where have which features been placed already (array of arraylist_t, so multiple ones)
/// @return int, error codes (EINVAL, EDESTADDRREQ)
int populate_sector(sector_t* sector, size_t row, size_t col, feature_t* features, size_t feature_count, arraylist_t* placed_features, unsigned int* seedp);


#endif
