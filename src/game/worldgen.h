#ifndef WORLDGEN_H
#define WORLDGEN_H

// TODO: Increase this in the future for more distinct feature placement

#include <stdlib.h>
#include <stdbool.h>

#include "sector.h"
#include "feature.h"
#include "world.h"

#include "../logic/rand/opensimplex.h"
#include "../logic/rand/xoshiro256.h"
#include "../logic/dynamicbuffer/dynamicbuffer.h"


typedef struct WorldGenData {
    feature_t* features;
    world_t*   destination;
    world_t    local_world;
    size_t     feature_count;
    long       noise_seeds[NOISE_COUNT]; // seed for each of the noises
} worldgendata_t;

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

int WorldGen_GenerateWorld(uint16_t sector_rows, uint16_t sector_cols, feature_t* features, world_t* destination, xoshiro256_state_t state);

#endif