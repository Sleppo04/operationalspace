#ifndef SHIP_H
#define SHIP_H

#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include "module.h"
#include "stats.h"

typedef struct Ship {
    // Modules
    module_t* modules[NUM_MODULETYPES];
    int       numModules[NUM_MODULETYPES];
    
    // Precalculated stats (update on module change using Obj_RecalculateStats)
    stats_t stats;
    
    // Current state of the ship
    int32_t current_hull;
    int32_t current_shield;
    int32_t current_storage;
    int32_t current_energy;

    // Rendering stuff
    char glyph;
    char color;
} ship_t;

void Ship_RecalculateStats(ship_t* obj);
int Ship_AddModule(ship_t* obj, module_t* module, moduleType_t type);

#endif //SHIP_H