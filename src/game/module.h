#ifndef MODULE_H
#define MODULE_H

#include "gameobject.h"

#include <stdint.h>

typedef struct Module {
    /** stat modifiers **/
    int32_t hull_mod;
    int32_t shield_mod;
    int32_t speed_mod;
    int32_t storage_mod;
    int32_t battery_mod;
} module_t;

typedef struct CustomModule {
    module_t base;
    // custom module members
} custom_module_t;

typedef struct ArmorModule {
    module_t base;
    // armor module members
} armor_module_t;

typedef struct WeaponModule {
    module_t base;
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

typedef struct EngineModule {
    module_t base;
    // engine module members
} engine_module_t;

typedef enum UtilityModuleTypes {
    UMODULE_SOLAR,
    UMODULE_MINING_LASER,
    UMODULE_HANGAR_BAY,
    UMODULE_PORT
} utility_module_type;

typedef struct UtilityModule {
    module_t base;
    utility_module_type type;
    // members
} utility_module_t;


#endif
