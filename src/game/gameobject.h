#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H

#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include "module.h"
#include "stats.h"

typedef struct GameObject {
    // Modules
    module_t* modules[NUM_MODULETYPES];
    int       numModules[NUM_MODULETYPES];
    
    // Combined module stats (update on module change using Obj_RecalculateStats)
    stats_t stats;
    
    // Current state of the ship
    int32_t current_hull;
    int32_t current_shield;
    int32_t current_storage;
    int32_t current_energy;

    // Rendering stuff
    char glyph;
    char color;
} gameobject_t;

void Obj_RecalculateStats(gameobject_t* obj);
int Obj_AddModule(gameobject_t* obj, module_t* module, moduleType_t type);

#endif //GAMEOBJECT_H
