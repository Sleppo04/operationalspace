#ifndef MODULE_H
#define MODULE_H

#include "gameobject.h"

#include <stdint.h>

typedef struct Module {
    // module members
    /** stat modifiers **/
    int32_t hull_mod;
    int32_t shield_mod;
    //int32_t weight_mod;
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
} weapon_module_t;

typedef struct EngineModule {
    module_t base;
    // engine module members
} engine_module_t;

typedef struct UtilityModule {
    module_t base;
    // members
} utility_module_t;


#endif