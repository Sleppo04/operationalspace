#ifndef GAMECONFIG_H
#define GAMECONFIG_H

#include <stdint.h>
#include <stdlib.h>

typedef struct ScriptConfig {
    size_t   MaxScriptMemory;
    uint64_t MaxScriptOpcodes;
    time_t   ScriptTimeoutSeconds;
} script_config_t;

typedef struct WorldConfig {
    uint16_t sector_rows;
    uint16_t sector_cols;
} world_config_t;

typedef struct GameConfig {
    script_config_t script;
    world_config_t  world;
} config_t;

#endif