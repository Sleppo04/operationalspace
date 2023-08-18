#ifndef WRENUSERDATA_H
#define WRENUSERDATA_H

#include "wrenerror.h"
#include "wrenlogs.h"

typedef struct s_WrenUserData {
    WrenLogs* errors;

    // Script output (via print())
    WrenLogs*  logs;
} WrenUserData;

#endif
