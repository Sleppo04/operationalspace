#ifndef WRENUSERDATA_H
#define WRENUSERDATA_H

#include "wrenerror.h"
#include "wrenlogs.h"

typedef struct s_WrenUserData {

    // Array of error reports
    // if the first element is of type COMPILE_ERROR, then the array is of length 1
    // if the first element is of type RUNTIME_ERROR, the following elements will be stack-traces
    WrenError* errors;
    size_t error_count;

    // Script output (via print())
    WrenLogs*  logs;
} WrenUserData;

#endif
