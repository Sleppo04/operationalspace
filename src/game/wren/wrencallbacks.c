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
	
	char* format_string;
	switch (type) {
		case WREN_ERROR_COMPILE:
			format_string = "\n[Compilation Error]\n%s\nLine %d in module %s\n";
			break;
		case WREN_ERROR_RUNTIME:
			format_string = "\n[Runtime Error]\n%s\nLine %d in module %s\n";
			break;
		case WREN_ERROR_STACK_TRACE:
			format_string = "Function\n%s\nLine %d in module %s\n";
			break;
	}

	size_t module_length  = strlen(module);
	size_t message_length = strlen(message);
	size_t int_length     = 15; // Just to be sure
	size_t format_length  = strlen(format_string);
	size_t total_length   = module_length + message_length + int_length + format_length + 1;

	char* string = malloc(total_length);
	if (string == NULL) {
		//TODO: Error handling ENOMEM
		return;
	}
	snprintf(string, total_length, format_string, message, line, module);

	WrenLogs_WriteMessage(data->errors, string);
	//TODO: Error checking here

	free(string);

	return;
}
