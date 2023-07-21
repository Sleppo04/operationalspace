#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H

#include "module.h"

typedef struct GameObject {
    // Modules
    utility_module_t* utility_modules;
    custom_module_t*  custom_modules;
    weapon_module_t*  weapon_modules;
    engine_module_t*  engine_modules;
    armor_module_t*   armor_modules;
    
    // Precalculated stats
    int32_t hull;
    int32_t shield;
    int32_t speed;
    int32_t storage;
    int32_t battery;
    
    // Current state of the ship
    int32_t current_hull;
    int32_t current_shield;
    int32_t current_load;
    int32_t current_energy;

    // Rendering stuff
    char glyph;
    char color;
} gameobject_t;

#endif

