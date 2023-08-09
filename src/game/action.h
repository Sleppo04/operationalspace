#ifndef GAMEACTION_H
#define GAMEACTION_H

#include <stdint.h>

#include "shared.h"

typedef enum ActionType {
    ACTION_MOVEMENT,
    ACTION_MODULE,
    ACTION_DIRECTION_MODULE
} action_type_t;

typedef struct MovementActionData {
    direction_t direction;
} movement_action_data_t;

typedef struct ModuleActionData {
    bool activation_state;
    uint32_t module_id;
} module_action_data_t;

typedef struct DirectionModuleActionData {
    bool activation_state;
    uint32_t module_id;
    int32_t  x_direction;
    int32_t  y_direction;
} direction_module_action_data_t;

typedef union ActionData {
    movement_action_data_t movement;
    module_action_data_t module;
} action_data_t;

typedef struct Action {
    enum ActionType type;
    union ActionData data;
} action_t;

#endif //GAMEACTION_H