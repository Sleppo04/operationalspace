#include "stats.h"

Stats_ApplyModifier(stats_t* stats, stats_t* mods)
{
    stats->battery += mods->battery;
    stats->storage += mods->storage;
    stats->shield  += mods->shield;
    stats->speed   += mods->speed;
    stats->hull    += mods->hull;
    return;
}