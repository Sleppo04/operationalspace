#ifndef WRENCALLBACKS_H
#define WRENCALLBACKS_H

#include "wren.h"
#include "wrenuserdata.h"
#include "wrenlogs.h"

void WrenCallback_WriteFn(WrenVM* vm, const char* text);

void WrenCallback_ErrorFn(WrenVM* vm, WrenErrorType type, const char* module, int line, const char* message);

#endif
