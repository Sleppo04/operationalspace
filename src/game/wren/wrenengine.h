#ifndef WRENENGINE_H
#define WRENENGINE_H

#include "wren.h"
#include "wrenerror.h"
#include "wrenlogs.h"

typedef struct WrenEngine {
    WrenVM*    vm;

    // Array of error reports
    // if the first element is of type COMPILE_ERROR, then the array is of length 1
    // if the first element is of type RUNTIME_ERROR, the following elements will be stack-traces
    WrenError* errors;

    // Script output (via print())
    WrenLogs*  logs;
} wren_engine_t;

#endif //WRENENGINE_H
