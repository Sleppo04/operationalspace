#include "wrencallbacks.h"

void WrenCallback_WriteFn(WrenVM* vm, const char* message)
{
	WrenUserData* data = (WrenUserData*) wrenGetUserData(vm);
	int log_code       = WrenLogs_WriteMessage(data->logs, message);

	if (log_code) {
		// TODO: Find some error handling
		fprintf(stderr, "WrenLogs_WriteMessage failed with error code %d\n", log_code);
	}
}

void WrenCallback_ErrorFn(WrenVM* vm, WrenErrorType type, const char* module, int line, const char* message)
{
	WrenUserData* data = (WrenUserData*) wrenGetUserData(vm);
	
	WrenError* errors = realloc(data->errors, (error_count + 1) * sizeof(WrenError));
	if (errors == NULL) {
		return; //ENOMEM
	}

	size_t module_length  = strlen(module);
	size_t message_length = strlen(message);

	data->errors = errors;
	
	WrenError* new_error = data->errors + data->error_count;
	new_error->type = type;
	new_error->module = malloc(module_length + 1);
	if (new_error->module != NULL) {
		strcpy(new_error->module, module);
	}
	new_error->line = line;
	new_error->message = malloc(message_length + 1);
	if (new_error->message != NULL) {
		strcpy(new_error->message, message);
	}

	data->error_count++;

	//TODO: Memory limitation?
}
