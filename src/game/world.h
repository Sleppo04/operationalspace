#ifndef WORLD_H
#define WORLD_H

#include <errno.h>
#include <float.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "sector.h"
#include "../logic/arraylist/arraylist.h"
#include "../logic/coordinate/coordinate.h"


typedef struct World {
    size_t sector_rows;
    size_t sector_cols;
    sector_t* sectors;
} world_t;

typedef int (*FeatureProviderFunction) (gameobject_t** destination, void* user_data);

typedef struct Feature {
    int (*provider) (gameobject_t** destination, void* user_data);
    void* user_data;
    double max_probability;    // The modified probability shall never exceed this, if the distances are kept
    double min_probability;    // The modified probability shall never subceed this, if the distances are kept
    double base_probability;   // Probability to place this feature on one tile
    double probability_mod;    // this will be added to the base probability for every tile distance higher then minimum_distance
    double probablity_growth;  // the probability will be multiplied by this for each tile of distance
    size_t foreign_distance;   // minimum distance of this feature to other features
    size_t minimum_distance;   // minimum distance between two of these features
} feature_t;

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
int generate_world(size_t sector_rows, size_t sector_cols, feature_t* features, world_t* destination, unsigned short seed[3]);

/// @brief get a sector at row and col from the world
/// @param world world to get the sector from
/// @param row in which row is the sector
/// @param col what is the column of the sector
/// @param destiantion where the result pointer will be written
/// @return int, errorcodes (EINVAL, EDESTADDRREQ)
int world_get_sector(world_t* world, size_t row, size_t col, sector_t** destination);

/// @brief Populate a sector with generated tiles
/// @param sector sector to populate
/// @param row what row is the sector in
/// @param col what col is the sector in
/// @param features what features are available for placement
/// @param feature_count how many features are there
/// @param placed_features where have which features been placed already (array of arraylist_t, so multiple ones)
/// @return int, error codes (EINVAL, EDESTADDRREQ)
int populate_sector(sector_t* sector, size_t row, size_t col, feature_t* features, size_t feature_count, arraylist_t* placed_features, struct drand48_data* rng_buffer);


#endif
