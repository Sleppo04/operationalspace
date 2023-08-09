#ifndef SCRIPT_H
#define SCRIPT_H

#include "world.h"

#include "wren.h"

typedef int (*CScriptFunction) (const world_t* world, void** data_pointer);

typedef enum ScriptType {
	SCRIPT_WREN,
	SCRIPT_C_FUNC
} script_type_t;

typedef struct CSCriptData {
	CScriptFunction c_function;
	void**          data_pointer;
} c_script_data_t;

typedef struct WrenScriptData {
	WrenHandle* function_handle;
	WrenVM*     vm;
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
	script_t ship_tick_script;
	WrenVM*  vm;
	void*    c_data;
} player_scripts_t;

#endif //SCRIPT_H
