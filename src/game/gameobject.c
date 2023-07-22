#include "gameobject.h"

Obj_RecalculateStats(gameobject_t* obj)
{
    obj->stats = (stats_t) { 0, 0, 0, 0, 0 };
    for (int i = 0; i < obj->num_armor_modules; i++)
        Stats_ApplyModifier(&obj->stats, &obj->armor_modules[i].base.statMods);
    for (int i = 0; i < obj->num_utility_modules; i++)
        Stats_ApplyModifier(&obj->stats, &obj->utility_modules[i].base.statMods);
    for (int i = 0; i < obj->num_custom_modules; i++)
        Stats_ApplyModifier(&obj->stats, &obj->custom_modules[i].base.statMods);
    for (int i = 0; i < obj->num_engine_modules; i++)
        Stats_ApplyModifier(&obj->stats, &obj->engine_modules[i].base.statMods);
    for (int i = 0; i < obj->num_weapon_modules; i++)
        Stats_ApplyModifier(&obj->stats, &obj->weapon_modules[i].base.statMods);
    // TODO: Check current values for OOB!
    return;
}