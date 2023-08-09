#ifndef GAMEACTION_H
#define GAMEACTION_H

#include "shared.h"

typedef enum ActionType {
    ACTION_MOVEMENT,
    ACTION_MODULE
} action_type_t;

typedef struct MovementActionData {
    direction_t direction;
} movement_action_data_t;

typedef struct ModuleActionData {
    bool activation_state;
    uint32_t module_id;
} module_action_data_t;

typedef union ActionData {
    movement_action_data_t movement;
    module_action_data_t module;
} action_data_t;

typedef struct Action {
    enum ActionType type;
    union ActionData data;
} action_t;

#endif //GAMEACTION_H