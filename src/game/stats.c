#include "stats.h"

void Stats_ApplyModifier(stats_t* stats, stats_t* mods)
{
    stats->battery             += mods->battery;
    stats->energy_consumption  += mods->energy_consumption;
    stats->energy_regeneration += mods->energy_regeneration;
    stats->hull                += mods->hull;
    stats->hull_regeneration   += mods->hull_regeneration;
    stats->mass                += mods->mass;
    stats->shield              += mods->shield;
    stats->shield_regeneration += mods->shield_regeneration;
    stats->speed               += mods->speed;
    stats->storage             += mods->storage;
}