#ifndef STATS_H
#define STATS_H

#include <stdint.h>

typedef struct stats_s
{
    int32_t hull;
    int32_t shield;
    int32_t speed;
    int32_t storage;
    int32_t battery;
} stats_t;

typedef struct statMods_s
{
    int32_t hullMod;
    int32_t shieldMod;
    int32_t speedMod;
    int32_t storageMod;
    int32_t batteryMod;
} statMods_t;

#endif