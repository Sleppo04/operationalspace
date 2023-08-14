#ifndef STATS_H
#define STATS_H

#include <stdint.h>
#include <stdbool.h>

typedef struct ExtendedStats {
    int32_t hull;
    int32_t shield;
    int32_t speed;
    int32_t mass;
    int32_t storage;
    int32_t battery;
    int32_t energy_consumption;
    int32_t energy_regeneration;
    int32_t shield_regeneration;
    int32_t hull_regeneration;
} extended_stats_t;

typedef struct ModuleStats {
    extended_stats_t base_stats;
    bool activatable;
    extended_stats_t activatable_stats;
} module_stats_t;

typedef struct stats_s
{
    int32_t hull;
    int32_t shield;
    int32_t speed;
    int32_t mass;
    int32_t storage;
    int32_t battery;
} stats_t;

void Stats_ApplyModifier(stats_t* stats, stats_t* mods);

#endif