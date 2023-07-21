#ifndef WORLD_H
#define WORLD_H

#include <stdlib.h>

#include "sector.h"

typedef struct World {
    size_t sector_rows;
    size_t sector_cols;
    sector_t** sectors;
} world_t;

typedef struct Feature {
    int (*feature_provider) (gameobject_t*);
    double base_probability;   // Probability to place this feature
    double minimum_distance;   // minimum distance between two of these features
    double probability_growth; // this will be added to the base probability for every tile distance higher then minimum_distance
} feature_t;

/**
 * @brief Generate a new world
 * 
 * @param sector_rows How many sectors should there be vertically
 * @param sector_cols How many sectors should there be horizontally
 * @param features What features should be generated (everything is a game object)
 * @param destination world_t struct to place everything in
 * @return int error codes ()
 */
int generate_world(size_t sector_rows, size_t sector_cols, feature_t* features, world_t* destination);

#endif