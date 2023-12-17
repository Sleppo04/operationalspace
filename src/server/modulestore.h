#ifndef MODULESTORE_H
#define MODULESTORE_H

#include "module.h"

typedef struct ModuleStore {
    stat_module_t*    stats_modules;
    weapon_module_t*  weapon_modules;
    custom_module_t*  custom_modules;
    utility_module_t* utility_modules;
} modulestore_t;

#endif