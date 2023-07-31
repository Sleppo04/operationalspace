#ifndef WORLDGEN_H
#define WORLDGEN_H

#define NEARBY_ARRAY_LENGTH 100

#include <stdlib.h>

#include "sector.h"
#include "feature.h"
#include "world.h"

#include "../logic/rand/xoshiro256.h"

typedef struct WorldGenData {
    feature_t* features;
    size_t feature_count;
    world_t* destination;
}

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
int generate_world(size_t sector_rows, size_t sector_cols, feature_t* features, world_t* destination, xoshiro256_state_t state);

/// @brief Populate a sector with generated tiles
/// @param sector sector to populate
/// @param row what row is the sector in
/// @param col what col is the sector in
/// @param features what features are available for placement
/// @param feature_count how many features are there
/// @param placed_features where have which features been placed already (array of arraylist_t, so multiple ones)
/// @return int, error codes (EINVAL, EDESTADDRREQ)
int populate_sector(sector_t* sector, size_t row, size_t col, feature_t* features, size_t feature_count, arraylist_t* placed_features, unsigned int* seedp);

/// @brief Places a feature on the given tile by setting the provided gameobject and adding it to the feature_placement_buffer
/// @param feature Feature to place
/// @param tile tile the feature shall be placed on
/// @param coordinate coordinate of the feature, to write it to the list
/// @param feature_placement_list list where all the coordinates of this feature are stored
/// @return EINVAL, EDESTADDRREQ, EINVAL, ENOMEM, returns from feature-provider(), arraylist_append() and EXIT_SUCCESS
int place_feature(feature_t* feature, tile_t* tile, coordinate_t coordinate, arraylist_t* feature_placement_list);

#endif