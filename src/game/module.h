#ifndef MODULE_H
#define MODULE_H

#include <stdint.h>

#include "stats.h"

// Probably the ugliest macro in the code
// Not anymore since we included wren
#define MODULESTRSIZE(x) (((x)==MODULETYPE_CUSTOM)?sizeof(custom_module_t):((x)==MODULETYPE_STATS)?sizeof(stat_module_t):((x)==MODULETYPE_UTILITY)?sizeof(utility_module_t):((x)==MODULETYPE_WEAPON)?sizeof(weapon_module_t):0)

#define NUM_MODULETYPES 4 // Number of existing module types, need to update manually :(
typedef enum moduleType_e
{
    MODULETYPE_UTILITY,
    MODULETYPE_CUSTOM,
    MODULETYPE_WEAPON,
    MODULETYPE_STATS
} moduleType_t;

typedef void module_t; // Required for naughty hacky thingies UwU

typedef struct baseModule_s {
    // Stat Modifiers
    stats_t statMods;
} base_module_t;

typedef struct customModule_s {
    base_module_t base;
    // custom module members
} custom_module_t;

// Module type used to provide stats for the ship
typedef struct StatModule {
    base_module_t base;
    bool activatable;
    stats_t activatable_stats;
} stat_module_t;

typedef struct weaponModule_s {
    base_module_t base;
    // weapon module members
    int32_t armor_damage;
    int32_t shield_damage;
    int32_t area_armor_damage;
    int32_t area_shield_damage;
    int32_t armor_damage_area;
    int32_t shield_damage_area;
    int32_t charge_cooldown;
    int32_t charges;
    int32_t activation_energy;
    int32_t recharge_energy;
    int32_t range;
} weapon_module_t;

typedef enum utilityModuleTypes_e {
    UMODULE_SOLAR_PANEL,
    UMODULE_MINING_LASER,
    UMODULE_GAS_COLLECTOR,
    UMODULE_HANGAR_BAY,
    UMODULE_PORT
} utility_module_type_t;

typedef struct utilityModule_s {
    base_module_t base;
    utility_module_type_t type;
    // members
} utility_module_t;


#endif
