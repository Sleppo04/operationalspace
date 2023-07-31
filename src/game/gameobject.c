#include "gameobject.h"

void Obj_RecalculateStats(gameobject_t* obj)
{
    obj->stats = (stats_t) { 0, 0, 0, 0, 0 };
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
    if (obj->numModules[type] == 0) {
        // We currently don't have an equipped module in the slot
        obj->modules[type] = malloc(MODULESTRSIZE(type));
        if (obj->modules[type] == NULL) {
            return ENOMEM;
        }

        obj->numModules[type] = 1;
    } else {
        // We have at least one equipped, so just resize array
        void* resized_array = realloc(obj->modules[type], MODULESTRSIZE(type) * (obj->numModules[type] + 1));
        if (resized_array == NULL) {
            return ENOMEM;
        }

        obj->modules[type] = resized_array;
        obj->numModules[type]++;
    }
    
    // Finally, copy the module and recalculate the ships stats
    char* module_array        = obj->modules[type];
    size_t module_type_size   = MODULESTRSIZE(type);
    size_t skip_bytes         = obj->numModules[type] * module_type_size;
    void* new_module_location = module_array + skip_bytes;

    memcpy(new_module_location, module, MODULESTRSIZE(type));
    Obj_RecalculateStats(obj);

    return EXIT_SUCCESS;
}