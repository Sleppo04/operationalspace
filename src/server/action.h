#ifndef GAMEACTION_H
#define GAMEACTION_H

#include <stdbool.h>
#include <stdint.h>

typedef enum ActionType {
    ACTION_NONE,
    ACTION_MOVEMENT,
    ACTION_MODULE,
    ACTION_MODULE_DIRECTION,
} action_type_t;

typedef enum Direction {
	DIRECTION_UP,
	DIRECTION_DOWN,
	DIRECTION_LEFT,
	DIRECTION_RIGHT
} direction_t;

typedef struct NoActionData {

} no_action_data_t;

typedef struct MovementActionData {
    direction_t direction;
} movement_action_data_t;

typedef struct ModuleActionData {
    bool     activation_state;
    uint32_t module_id;
} module_action_data_t;

typedef struct ModuleDirectionActionData {
    uint32_t module_id;
    int32_t  x_direction;
    int32_t  y_direction;
} module_direction_action_data_t;

typedef union ActionData {
    no_action_data_t none;
    movement_action_data_t movement;
    module_action_data_t module;
    module_direction_action_data_t module_direction;
} action_data_t;

typedef struct Action {
    enum ActionType type;
    union ActionData data;
} action_t;

#endif //GAMEACTION_H