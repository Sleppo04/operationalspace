#include "gameobject.h"

void Ship_RecalculateStats(gameobject_t* ship)
{
    ship->stats = (stats_t) { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    for (int i = 0; i < ship->numModules[MODULETYPE_STATS]; i++) {
        stat_module_t* module = ship->modules[i];
        Stats_ApplyModifier(&ship->stats, &module->base.statMods);
    }
        
    for (int i = 0; i < ship->numModules[MODULETYPE_UTILITY]; i++) {
        utility_module_t* module = ship->modules[i];
        Stats_ApplyModifier(&ship->stats, &module->base.statMods);
    }
        
    for (int i = 0; i < ship->numModules[MODULETYPE_CUSTOM]; i++) {
        custom_module_t* module = ship->modules[i];
        Stats_ApplyModifier(&ship->stats, &module->base.statMods);
    }
    
    for (int i = 0; i < ship->numModules[MODULETYPE_WEAPON]; i++) {
        weapon_module_t* module = ship->modules[i];
        Stats_ApplyModifier(&ship->stats, &module->base.statMods);
    }
    
    // TODO: Check current values for OOB after change of max!
    return;
}

int Ship_AddModule(gameobject_t* ship, module_t* module, moduleType_t type)
{
    void* resizedArray;
    void* moduleArray;
    void* newModuleLocation;
    size_t moduleSize;
    size_t skip;

    moduleSize = MODULESTRSIZE(type);
    
    if (ship->numModules[type] == 0) {
        // We currently don't have an equipped module in the slot
        ship->modules[type] = malloc(moduleSize);
        if (ship->modules[type] == NULL)
            return ENOMEM;

        ship->numModules[type] = 1;
    } else {
        // We have at least one equipped, so just resize array
        resizedArray = realloc(ship->modules[type], moduleSize * (ship->numModules[type] + 1));
        if (resizedArray == NULL)
            return ENOMEM;

        ship->modules[type] = resizedArray;
        ship->numModules[type]++;
    }
    
    // Finally, copy the module and recalculate the ships stats
    moduleArray       = ship->modules[type];
    skip              = ship->numModules[type] * moduleSize;
    newModuleLocation = (char*) (moduleArray) + skip;

    memcpy(newModuleLocation, module, moduleSize);
    Ship_RecalculateStats(ship);

    return EXIT_SUCCESS;
}
