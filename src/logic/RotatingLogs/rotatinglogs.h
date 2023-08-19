#ifndef WRENLOGS_H
#define WRENLOGS_H

#include <errno.h>
#include <stdlib.h>
#include <string.h>

#include "../memorypool/memorypool.h"

typedef struct s_LogNode {
	char* message;
	struct s_LogNode* next;
} LogNode;

typedef struct s_RotatingLogs {
	memory_pool_t memory_pool;
	LogNode* head;
	LogNode* tail;
} RotatingLogs;

int RotatingLogs_Create(RotatingLogs* destination, uintptr_t max_memory);
int RotatingLogs_Destroy(RotatingLogs* logs);

// The message will be copied, the caller keeps ownership of the memory passed into the function
int RotatingLogs_WriteMessage(RotatingLogs* logs, const char* message);

#endif //WRENLOGS_H
