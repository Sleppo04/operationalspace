#include "script.h"

int Script_ComputeShipActions(script_t* script, struct Game* game)
{
    if (script == NULL) {
        return EINVAL;
    }
    if (game == NULL) {
        return EINVAL;
    }

    return 0;
}