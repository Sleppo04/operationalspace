#ifndef MODULE_H
#define MODULE_H

#include <stdint.h>

#include "gameobject.h"
#include "stats.h"

// Probably the ugliest macro in the code
#define MODULESTRSIZE(x) (x==MODULETYPE_ARMOR)?sizeof(armor_module_t):(x==MODULETYPE_CUSTOM)?sizeof(custom_module_t):(x==MODULETYPE_ENGINE)?sizeof(engine_module_t):(x==MODULETYPE_UTILITY)?sizeof(utility_module_t):(x==MODULETYPE_WEAPON)?sizeof(weapon_module_t):0

#define NUM_MODULETYPES 5 // Number of existing module types, need to update manually :(
typedef enum moduleType_e
{
    MODULETYPE_UTILITY,
    MODULETYPE_CUSTOM,
    MODULETYPE_WEAPON,
    MODULETYPE_ENGINE,
    MODULETYPE_ARMOR
} moduleType_t;

typedef void module_t; // Required for naughty hacky thingies UwU

typedef struct baseModule_s {
    // Stat Modifiers
    stats_t statMods;
} baseModule_t;

typedef struct customModule_s {
    baseModule_t base;
    // custom module members
} custom_module_t;

typedef struct armorModule_s {
    baseModule_t base;
    // armor module members
} armor_module_t;

typedef struct weaponModule_s {
    baseModule_t base;
    // weapon module members
    int32_t armor_damage;
    int32_t shield_damage;
    int32_t area_armor_damage;
    int32_t area_shield_damage;
    int32_t armor_damage_area;
    int32_t shield_damage_area;
    int32_t cooldown;
    int32_t activation_energy;
    int32_t recharge_energy;
} weapon_module_t;

typedef struct engineModule_s {
    baseModule_t base;
    // engine module members
} engine_module_t;

typedef enum utilityModuleTypes_e {
    UMODULE_SOLAR,
    UMODULE_MINING_LASER,
    UMODULE_HANGAR_BAY,
    UMODULE_PORT
} utility_module_type_t;

typedef struct utilityModule_s {
    baseModule_t base;
    utility_module_type_t type;
    // members
} utility_module_t;


#endif
