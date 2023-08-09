#include "gameobject.h"

void Obj_RecalculateStats(gameobject_t* obj)
{
    obj->stats = (stats_t) { 0, 0, 0, 0, 0, 0 };
    for (int i = 0; i < obj->numModules[MODULETYPE_ARMOR]; i++) {
        armor_module_t* module = obj->modules[i];
        Stats_ApplyModifier(&obj->stats, &module->base.statMods);
    }
        
    for (int i = 0; i < obj->numModules[MODULETYPE_UTILITY]; i++) {
        utility_module_t* module = obj->modules[i];
        Stats_ApplyModifier(&obj->stats, &module->base.statMods);
    }
        
    for (int i = 0; i < obj->numModules[MODULETYPE_CUSTOM]; i++) {
        custom_module_t* module = obj->modules[i];
        Stats_ApplyModifier(&obj->stats, &module->base.statMods);
    }
        
    for (int i = 0; i < obj->numModules[MODULETYPE_ENGINE]; i++) {
        engine_module_t* module = obj->modules[i];
        Stats_ApplyModifier(&obj->stats, &module->base.statMods);
    }
    
    for (int i = 0; i < obj->numModules[MODULETYPE_WEAPON]; i++) {
        weapon_module_t* module = obj->modules[i];
        Stats_ApplyModifier(&obj->stats, &module->base.statMods);
    }
    
    // TODO: Check current values for OOB after change of max!
    return;
}

int Obj_AddModule(gameobject_t* obj, module_t* module, moduleType_t type)
{
    void* resizedArray;
    void* moduleArray;
    void* newModuleLocation;
    size_t moduleSize;
    size_t skip;

    moduleSize = MODULESTRSIZE(type);
    
    if (obj->numModules[type] == 0) {
        // We currently don't have an equipped module in the slot
        obj->modules[type] = malloc(moduleSize);
        if (obj->modules[type] == NULL)
            return ENOMEM;

        obj->numModules[type] = 1;
    } else {
        // We have at least one equipped, so just resize array
        resizedArray = realloc(obj->modules[type], moduleSize * (obj->numModules[type] + 1));
        if (resizedArray == NULL)
            return ENOMEM;

        obj->modules[type] = resizedArray;
        obj->numModules[type]++;
    }
    
    // Finally, copy the module and recalculate the ships stats
    moduleArray       = obj->modules[type];
    skip              = obj->numModules[type] * moduleSize;
    newModuleLocation = (char*) (moduleArray) + skip;

    memcpy(newModuleLocation, module, moduleSize);
    Obj_RecalculateStats(obj);

    return EXIT_SUCCESS;
}
