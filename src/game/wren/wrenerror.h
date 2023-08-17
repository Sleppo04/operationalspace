#ifndef WRENERROR_H
#define WRENERROR_H

#include "wren.h"

typedef struct s_WrenError {
	WrenErrorType type;
	char* module;
	int line;
	char* message;
} WrenError;

#endif
