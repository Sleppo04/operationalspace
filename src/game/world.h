#ifndef WORLD_H
#define WORLD_H

#include <errno.h>
#include <stdlib.h>

#include "sector.h"

typedef struct World {
    size_t sector_rows;
    size_t sector_cols;
    sector_t** sectors;
} world_t;

typedef struct Feature {
    int (*feature_provider) (gameobject_t*);
    double max_probability;    // The modified probability shall never exceed this
    double min_probability;    // The modified probability shall never subceed this
    double base_probability;   // Probability to place this feature on one tile
    double minimum_distance;   // minimum distance between two of these features
    double probability_mod;    // this will be added to the base probability for every tile distance higher then minimum_distance
    double probablity_growth;  // the probability will be multiplied by this for each tile of distance
    double foreign_distance;   // minimum distance of this feature to other features
} feature_t;

/**
 * @brief Generate a new world
 * 
 * @param sector_rows How many sectors should there be vertically
 * @param sector_cols How many sectors should there be horizontally
 * @param features What features should be generated (everything is a game object)
 * @param destination world_t struct to place everything in
 * @return int error codes (EINVAL, EDESTADDRREQ)
 */
int generate_world(size_t sector_rows, size_t sector_cols, feature_t* features, world_t* destination);

#endif