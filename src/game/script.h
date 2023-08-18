#ifndef SCRIPT_H
#define SCRIPT_H

#include "world.h"

#include "wren.h"
#include "wren/wrenengine.h"

struct Game;

typedef int (*CScriptFunction) (struct Game* game, void** data_pointer);

typedef enum ScriptType {
	SCRIPT_WREN,
	SCRIPT_C_FUNC
} script_type_t;

typedef struct CSCriptData {
	CScriptFunction c_function;
} c_script_data_t;

typedef struct WrenScriptData {
	WrenHandle* function_handle;
} wren_script_data_t;

typedef union ScriptData {
	struct CSCriptData c_data;
	struct WrenScriptData wren_data;
} script_data_t;

typedef struct Script {
	enum ScriptType type;
	union ScriptData data;
} script_t;

typedef struct PlayerScripts {
	script_t      ship_tick_script;
	script_t      round_tick_script;
	wren_engine_t wren;
	void*         c_data;
} player_scripts_t;

#endif //SCRIPT_H
