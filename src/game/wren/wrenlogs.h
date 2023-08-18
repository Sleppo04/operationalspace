#ifndef WRENLOGS_H
#define WRENLOGS_H

#include <errno.h>
#include <stdlib.h>
#include <string.h>

#include "../../logic/memorypool/memorypool.h"

typedef struct s_MessageNode {
	char* message;
	struct s_MessageNode* next;
} MessageNode;

typedef struct s_WrenLogs {
	memory_pool_t memory_pool;
	MessageNode* head;
	MessageNode* tail;
} WrenLogs;

int WrenLogs_Create(WrenLogs* destination, uintptr_t max_memory);
int WrenLogs_Destroy(WrenLogs* logs);

// The message will be copied, the caller keeps ownership of the memory passed into the function
int WrenLogs_WriteMessage(WrenLogs* logs, char* message);

#endif //WRENLOGS_H
