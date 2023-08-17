#ifndef WRENENGINE_H
#define WRENENGINE_H

#include "wren.h"
#include "wrenerror.h"

typedef struct WrenEngine {
    WrenVM* vm;
    WrenError* errors;
} wren_engine_t;

#endif //WRENENGINE_H
