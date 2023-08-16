#ifndef WRENENGINE_H
#define WRENENGINE_H

#include "wren.h"

typedef struct WrenEngine {
    WrenVM* vm;
} wren_engine_t;

#endif //WRENENGINE_H