#ifndef WORLD_H
#define WORLD_H

#include <stdlib.h>

#include "sector.h"

typedef struct World {
    size_t sector_rows;
    size_t sector_cols;
    sector_t** sectors;
} world_t;

#endif