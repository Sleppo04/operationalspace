#ifndef WRENENGINE_H
#define WRENENGINE_H

#include "wren.h"
#include "wrenuserdata.h"

typedef struct WrenEngine {
    WrenVM*    vm;

    WrenUserData data;

} wren_engine_t;

#endif //WRENENGINE_H
