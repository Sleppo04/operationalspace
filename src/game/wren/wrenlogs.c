#include "wrenlogs.h"

int WrenLogs_Create(WrenLogs* logs, uintptr_t max_memory)
{
	if (logs == NULL) {
		return EDESTADDRREQ;
	}
	
	int pool_code = MemoryPool_Create(&(logs->memory_pool), sizeof(char), 0, max_memory);
	if (pool_code) {
		return pool_code;
	}

	logs->head = NULL;
	logs->tail = NULL;

	return EXIT_SUCCESS;
}

int WrenLogs_Destroy(WrenLogs* logs)
{
	logs->head = NULL;
	logs->tail = NULL;	
	// We have rendered all existing references to the pool useless
	// => We can force-destroy it
	return MemoryPool_ForceDestroy(&(logs->memory_pool));
}

int WrenLogs_RemoveMessage(WrenLogs* logs)
{
	if (logs == NULL) {
		return EINVAL;
	}
	if (logs->head == NULL) {
		return ENXIO;
	}

	MessageNode* current_head = logs->head;
	MessageNode* next_head    = current_head->next;
	
	int free_code;
	// + 1 for \0 byte
	size_t length = sizeof(MessageNode*) + strlen(current_head->message) + 1;
	// They are continously allocated, and can be freed together
	free_code = MemoryPool_FreeArray(&(logs->memory_pool), current_head, sizeof(MessageNode));
	if (free_code) {
		// This should not happen by design
		return free_code;
	}
	
	logs->head = next_head;
	if (logs->head == NULL) {
		logs->tail = NULL;
	}

	return EXIT_SUCCESS;
}

int WrenLogs_WriteMessage(WrenLogs* logs, char* message)
{
	if (logs == NULL) {
		return EINVAL;
	}
	if (message == NULL) {
		return EINVAL;
	}

	// + 1 for \0 byte
	size_t message_length = strlen(message) + 1;
	size_t struct_length  = sizeof(MessageNode);
	size_t allocation_length = struct_length + message_length;

	char* owned_message;
	int allocation_code, remove_code;
	
	allocation_code = MemoryPool_AllocateArray(&(logs->memory_pool), allocation_length, (void**) &owned_message);
	while (allocation_code != EXIT_SUCCESS && remove_code == EXIT_SUCCESS) {
		remove_code = WrenLogs_RemoveMessage(logs);

		allocation_code = MemoryPool_AllocateArray(&(logs->memory_pool), allocation_length, (void**) &owned_message);
	}
	
	// We were unable to remove another element to free more memory
	if (remove_code != EXIT_SUCCESS) {
		return remove_code;
	}
	// else: allocation was successfull
	
	// Fun pointer stuff because of continous allocation
	MessageNode* node = (MessageNode*) owned_message;
	owned_message     = owned_message + struct_length;

	// Copy the message
	memcpy(owned_message, message, message_length);
	
	node->message = message;

	if (logs->head == NULL) {
		logs->head = node;
	}
	if (logs->tail == NULL) {
		logs->tail = node;
	} else {
		logs->tail->next = node;
	}

	return EXIT_SUCCESS;
}
