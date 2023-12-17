#ifndef WORLD_H
#define WORLD_H

#include "sector.h"
#include "feature.h"

typedef struct
{
    xoshiro256_state_t seed;
    unsigned int       sector_rows;
    unsigned int       sector_cols;
    sector_t*          sectors;
    arraylist_t        objects;
} world_t;

#endif
