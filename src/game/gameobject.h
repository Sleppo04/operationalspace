#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H

#include "module.h"
#include "stats.h"

typedef struct GameObject {
    // Modules
    utility_module_t* utility_modules; int num_utility_modules;
    custom_module_t*  custom_modules;  int num_custom_modules;
    weapon_module_t*  weapon_modules;  int num_weapon_modules;
    engine_module_t*  engine_modules;  int num_engine_modules;
    armor_module_t*   armor_modules;   int num_armor_modules;
    
    // Precalculated stats
    stats_t stats;
    
    // Current state of the ship
    int32_t current_hull;
    int32_t current_shield;
    int32_t current_load;
    int32_t current_energy;

    // Rendering stuff
    char glyph;
    char color;
} gameobject_t;

void Obj_RecalculateStats(gameobject_t* obj);

#endif

