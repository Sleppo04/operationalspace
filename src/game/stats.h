#ifndef STATS_H
#define STATS_H

#include <stdint.h>

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