#ifndef SCRIPT_H
#define SCRIPT_H

#include "world.h"

typedef int (*CScriptFunction) (const world_t* world, void** data_pointer);

typedef enum ScriptType {
	SCRIPT_WREN,
	SCRIPT_C_FUNC
} script_type_t;

typedef struct CSCriptData {
	CScriptFunction c_function;
} c_script_data_t;

typedef struct WrenScriptData {
	char* source;
} wren_script_data_t;

typedef union ScriptData {
	struct c_script_data_t    c_data;
	struct WrenScriptData wren_data;
} script_data_t;

typedef struct Script {
	enum ScriptType type;
	union ScriptData data;
} script_t;

typedef struct PlayerScripts {
	script_t ship_tick_script;
} player_scripts_t;

#endif //SCRIPT_H
