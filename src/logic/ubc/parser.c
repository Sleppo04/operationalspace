#include "parser.h"


int ParserConfig_Init(ubcparserconfig_t* config)
{
    config->store_explanations = false;
    config->bytecode_callback  = NULL;
    config->foreign_functions  = NULL;
    config->realloc_function   = NULL;
    config->malloc_function    = NULL;
    config->function_count     = 0;
    config->foreign_types      = NULL;
    config->free_function      = NULL;
    config->report_return      = 0;
    config->error_report       = NULL;
    config->type_count         = 0;
    config->file_count         = 0;
    config->userdata           = NULL;
    config->files              = NULL;

    return EXIT_SUCCESS;
}

// Function group dealing with memory allocations

// This function does not report errors to the user
void* _Parser_Malloc(ubcparser_t* parser, size_t size)
{
    if (parser->config.malloc_function != NULL) {
        return parser->config.malloc_function(parser->config.userdata, size);
    } else {
        return malloc(size);
    }
}

void _Parser_Free(ubcparser_t* parser, void* address, size_t size)
{
    if (parser->config.free_function != NULL) {
        parser->config.free_function(parser->config.userdata, address, size);
    } else {
        free(address);
    }
}

void* _Parser_Realloc(ubcparser_t* parser, void* address, size_t new_size, size_t old_size)
{
    if (parser->config.realloc_function != NULL) {
        return parser->config.realloc_function(parser->config.userdata, address, new_size, old_size);
    }

    if (parser->config.malloc_function == NULL && parser->config.free_function == NULL) {
        // No other function is defined, we can use sysrealloc
        return realloc(address, new_size);
    }


    void* new_array = _Parser_Malloc(parser, new_size);
    if (new_array == NULL) {
        return NULL;
    }

    memcpy(new_array, address, old_size);
    _Parser_Free(parser, address, old_size);

    return new_array;
}

void* _Parser_Memdup(ubcparser_t* parser, void* memory, size_t size)
{
    void* dupmem = _Parser_Malloc(parser, size);
    if (dupmem == NULL) {
        return NULL;
    }

    memcpy(dupmem, memory, size);
    return dupmem;
}

char* _Parser_vsnprintf(ubcparser_t* parser, const char* format, va_list args)
{
    va_list copy;
    va_copy(copy, args);

    // C99 Standard allows this to calculate the needed string capacity
    size_t needed_length = 1 + vsnprintf(NULL, 0, format, copy);
    va_end(copy);

    char* string = _Parser_Malloc(parser, needed_length);
    if (string == NULL) {
        return NULL;
    }

    size_t truncated = vsnprintf(string, needed_length, format, args);
    if (truncated >= needed_length) {
        _Parser_Free(parser, string, needed_length);
        return NULL;
    }

    return string;
}

char* _Parser_snprintf(ubcparser_t* parser, const char* format, ...)
{
    va_list args;
    va_start(args, format);
    char* string = _Parser_vsnprintf(parser, format, args);
    va_end(args);

    return string;
}

// This function will allocate an extra byte for \0
char* _Parser_strndup(ubcparser_t* parser, char* source, size_t length)
{
    size_t strnlen = 0;
    while (source[strnlen] != '\0' && strnlen < length) {
        strnlen++;
    }

    char* dupstr = _Parser_Malloc(parser, strnlen + 1);
    if (dupstr == NULL) {
        return NULL;
    }

    memcpy(dupstr, source, strnlen);
    dupstr[strnlen] = '\0';

    return dupstr;
}


// UpcParserBuffer

int _UbcParserBuffer_Create(ubcparser_t* parser, ubcparserbuffer_t* buffer)
{
    if (parser == NULL) return EINVAL;
    if (buffer == NULL) return EDESTADDRREQ;
    buffer->memory   = NULL;
    buffer->capacity = 0;
    buffer->used     = 0;

    return EXIT_SUCCESS;
}

int _UbcParserBuffer_EnsureFreeCapacity(ubcparser_t* parser, ubcparserbuffer_t* buffer, size_t needed)
{
    if (parser == NULL) return EINVAL;
    if (buffer == NULL) return EINVAL;

    if (buffer->memory == NULL) {
    	buffer->memory = _Parser_Malloc(parser, needed);
    	if (buffer->memory == NULL) {
    		return ENOMEM;
    	}
    	buffer->capacity = needed;

    } else if (needed > (buffer->capacity - buffer->used)) {
    	size_t new_capacity = buffer->capacity * 2;
    	if (needed > (new_capacity - buffer->used)) {
    		new_capacity = buffer->capacity + needed;
    	}

    	void* new_memory = _Parser_Realloc(parser, buffer->memory, new_capacity, buffer->capacity);
    	if (new_memory == NULL) return ENOMEM;
    	buffer->memory   = new_memory;
    	buffer->capacity = new_capacity;
    }

    return EXIT_SUCCESS;
}

int _UbcParserBuffer_Write(ubcparser_t* parser, ubcparserbuffer_t* buffer, void* memory, size_t length)
{
    if (parser == NULL) return EINVAL;
    if (memory == NULL) return EINVAL;
    if (buffer == NULL) return EDESTADDRREQ;

    if (_UbcParserBuffer_EnsureFreeCapacity(parser, buffer, length)) return ENOMEM;

    void* destination = (void*) ((char*) (buffer->memory) + buffer->used);
    memcpy(destination, memory, length);
    buffer->used += length; // Don't forget to increment the counter if you write to the buffer, stupid

    return EXIT_SUCCESS;
}

void _UbcParserBuffer_Destroy(ubcparser_t* parser, ubcparserbuffer_t* buffer)
{
    if (parser == NULL) return;
    if (buffer == NULL) return;
    if (buffer->memory != NULL) {
    	_Parser_Free(parser, buffer->memory, buffer->capacity);
    }
    buffer->memory   = NULL;
    buffer->used     = 0;
    buffer->capacity = 0;

    return;
}

// Group of error reporting functions

// This function does report an error to the user
int _Parser_ReportError(ubcparser_t* parser, const char* filename, int line, const char* message, enum UbcParserErrorType type)
{
    parser->config.report_return = parser->config.error_report(parser->config.userdata, filename, line, message, type);

    return parser->config.report_return;
}

int _Parser_ReportLexerTraceback(ubcparser_t* parser)
{
    int report_code;
    lexer_t top_lexer = parser->lexer_stack.lexers[parser->lexer_stack.stack_size - 1];
    report_code = _Parser_ReportError(parser, top_lexer.file, top_lexer.line, "in file", UBCPARSERERROR_PARSERTRACEBACK);
    if (report_code) {
        return EXIT_FAILURE;
    }

    // Skip first file
    for (uint16_t lexer_index = parser->lexer_stack.stack_size - 2; lexer_index + 1 != 0 ; lexer_index--) {
        lexer_t* lexer = parser->lexer_stack.lexers + lexer_index;
        report_code = _Parser_ReportError(parser, lexer->file, lexer->line, "included by", UBCPARSERERROR_PARSERTRACEBACK);
        if (report_code) {
            return EXIT_FAILURE;
        }
    }

    return EXIT_SUCCESS;
}

void _Parser_ReportTopTracebackError(ubcparser_t* parser, const char* message)
{
    if (parser->lexer_stack.stack_size == 0) {
        _Parser_ReportError(parser, "No file", -1, message, UBCPARSERERROR_PARSERERROR);
        return;
    }

    lexer_t* top_lexer = &(parser->lexer_stack.lexers[parser->lexer_stack.stack_size - 1]);
    _Parser_ReportError(parser, top_lexer->file, top_lexer->line, message, UBCPARSERERROR_PARSERERROR);
    _Parser_ReportLexerTraceback(parser);

}

int _Parser_ReportFormattedTracebackError(ubcparser_t* parser, const char* format, ...)
{
    va_list args;
    va_start(args, format);
    char* formatted = _Parser_vsnprintf(parser, format, args);
    va_end(args);

    if (formatted == NULL) {
        return ENOMEM;
    }

    _Parser_ReportTopTracebackError(parser, formatted);

    _Parser_Free(parser, formatted, strlen(formatted) + 1);

    return EXIT_SUCCESS;
}

int _Parser_ReportFormattedTracebackErrorFallback(ubcparser_t* parser, const char* fallback, const char* format, ...)
{
    va_list args;
    va_start(args, format);
    char* formatted = _Parser_vsnprintf(parser, format, args);
    va_end(args);

    if (formatted == NULL) {
        _Parser_ReportTopTracebackError(parser, fallback);
    } else {
        _Parser_ReportTopTracebackError(parser, formatted);
        _Parser_Free(parser, formatted, strlen(formatted) + 1);
    }

    return EXIT_SUCCESS;
}

int _Parser_ReportUnexpectedToken(ubcparser_t* parser, const char* message, const char* expected, token_t unexpected)
{
    size_t unexpected_length = unexpected.value.length > 64 ? 64 : unexpected.value.length;
    size_t expected_length   = strlen(expected);
    size_t message_length    = strlen(message);
    char* format = "%s\nExpected \"%s\" but found \"%.64s%s\".";
    size_t result_length = strlen(format) + unexpected_length + expected_length + message_length + 4;

    char* result_string = _Parser_Malloc(parser, result_length);
    if (result_string != NULL) {
    	snprintf(result_string, result_length, format, message, expected, unexpected.ptr, unexpected.value.length > 64 ? "..." : "");

        	lexer_t top_lexer = parser->lexer_stack.lexers[parser->lexer_stack.stack_size - 1];
    	_Parser_ReportError(parser, top_lexer.file, unexpected.line, result_string, UBCPARSERERROR_PARSERERROR);
    	_Parser_ReportLexerTraceback(parser);
    	_Parser_Free(parser, result_string, result_length);
    } else {
    	_Parser_ReportTopTracebackError(parser, "Unable to allocate memory for a detailed error message: parser encountered an unexpected token.");
    }

    return EXIT_SUCCESS;
}


/// Type functions

// Type registering and request functions
bool _Types_IsBuiltInTypename(char* typename, size_t name_length)
{
    if (strncmp(typename, UBC_BOOL_TYPENAME, name_length) == 0) 
        return true;
    if (strncmp(typename, UBC_INT_TYPENAME, name_length) == 0) 
        return true;
    if (strncmp(typename, UBC_FLOAT_TYPENAME, name_length) == 0) 
        return true;
    if (strncmp(typename, TT_UBC_STRING_TYPENAME, name_length) == 0) 
        return true;
    
    return false;
}

bool _Parser_IsTypenameRegistered(ubcparser_t* parser, char* typename, size_t name_length)
{
    if (_Types_IsBuiltInTypename(typename, name_length))
        return true;

    for (uint16_t i = 0; i != parser->types.count; i++) {
        char* defined_typename = parser->types.array[i].name;
        size_t length = strlen(defined_typename);
        if (length != name_length) continue;
        if (strncmp(defined_typename, typename, name_length) == 0) {
            return true;
        }
    }

    for (uint16_t i = 0; i != parser->config.type_count; i++) {
        char* foreign_typename = parser->config.foreign_types[i].name;
        size_t length = strlen(foreign_typename);
        if (length != name_length) continue;
        if (strncmp(foreign_typename, typename, name_length)) {
            return true;
        }
    }

    return false;
}

ubccustomtype_t* _Parser_GetTypeByName(ubcparser_t* parser, char* typename, size_t name_length)
{
    ubccustomtype_t* current_type;
    for (uint16_t i = 0; i != parser->types.count; i++) {
        current_type = parser->types.array + i;

        if (name_length != strlen(current_type->name)) continue;
        if (strncmp(typename, current_type->name, name_length) == 0) {
            return current_type;
        }
    }

    
    for (uint16_t i = 0; i != parser->config.type_count; i++) {
        current_type = parser->config.foreign_types + i;
    
        if (name_length != strlen(current_type->name)) continue;
        if (strncmp(typename, current_type->name, name_length) == 0) {
            return current_type;
        }
    }

    return NULL;
}

bool _IsTypenameIdentifierToken(token_t* token)
{
    if (token->type == TT_UBC_INT) return true;
    if (token->type == TT_UBC_BOOL) return true;
    if (token->type == TT_UBC_FLOAT) return true;
    if (token->type == TT_UBC_STRING) return true;

    if (token->type == TT_IDENTIFIER) return true;

    return false;
}

// Returns the size of a built-in type in bytes
size_t _Parser_BuiltInTypeSize(char* typename, size_t name_length)
{
    if (strncmp(typename, UBC_BOOL_TYPENAME, name_length) == 0) 
        return 1;
    if (strncmp(typename, UBC_INT_TYPENAME, name_length) == 0) 
        return 4;
    if (strncmp(typename, UBC_FLOAT_TYPENAME, name_length) == 0) 
        return 4;
    if (strncmp(typename, TT_UBC_STRING_TYPENAME, name_length) == 0) 
        return SIZE_MAX; /// TODO: Change this when strings are implemented
    
    return 0;
}

// This function returns 0 if the type does not exist
size_t _Parser_GetTypeSize(ubcparser_t* parser, char* typename, size_t name_length)
{
    if (_Types_IsBuiltInTypename(typename, name_length)) {
        return _Parser_BuiltInTypeSize(typename, name_length);
    }

    // User defined types
    for (uint16_t i = 0; i != parser->types.count; i++) {
        ubccustomtype_t* type = parser->types.array + i;
        size_t length         = strlen(type->name);
        if (length != name_length) continue;
        if (strncmp(type->name, typename, name_length) == 0) {
            return type->type_size;
        }
    }

    // foreign types
    for (uint16_t i = 0; i != parser->config.type_count; i++) {
        ubccustomtype_t* type = parser->config.foreign_types + i;
        size_t length         = strlen(type->name);
        if (length != name_length) continue;
        if (strncmp(type->name, typename, name_length) == 0) {
            return type->type_size;
        }
    }

    return 0;
}

char* _Types_GetMemberTypename(ubccustomtype_t* type, char* member, size_t name_length)
{
    for (uint16_t i = 0; i < type->field_count; i++) {
        if (strncmp(type->field_names[i], member, name_length) == 0) {
            return type->field_typenames[i];
        }
    }

    return NULL;
}

bool _Types_MemberExists(ubccustomtype_t* type, char* name, size_t name_length)
{
    for (uint16_t i = 0; i < type->field_count; i++) {
        if (strncmp(type->field_names[i], name, name_length) == 0) {
            return true;
        }
    }

    return false;
}

char* _Parser_TypeMemberPathResultTypename(ubcparser_t* parser, ubccustomtype_t* type, ubclvalue_t* member_path)
{
    ubccustomtype_t* current      = type;
    ubclvalue_t      current_path = member_path[0];

    size_t name_length;
    char* next, *member_typename;

    while (current_path.path_length != 0) {
        next = strnchr(current_path.variable_path, '.', current_path.path_length);
        if (next == NULL) {
            name_length = current_path.path_length;
        } else {
            name_length = (uintptr_t) next - (uintptr_t) current_path.variable_path;
        }

        if (_Types_IsBuiltInTypename(member_typename, strlen(member_typename))) {
            // Can't dive further into builtin types
            char* format_format = "Cannot access member path %%.%ds of built-in type %%.%ds";
        char* format = _Parser_snprintf(parser, format_format, current_path.path_length, name_length);
        if (format == NULL) {
    	    // printing failed for memory reasons
    	    _Parser_ReportTopTracebackError(parser, "Could not allocate detailed error message. Cannot access member of built-in type.");
    	    return NULL;
        }

        if (_Parser_ReportFormattedTracebackError(parser, format, current_path.variable_path, member_typename)) {
    	    // Need not free here, it is done below
    	    _Parser_ReportTopTracebackError(parser, "Could not allocated detailed error message. Cannot access member of built-in type.");
        }

        // If you remove this free, add one to the if above
            _Parser_Free(parser, format, strlen(format) + 1);
        return NULL;
        }
        if (! _Types_MemberExists(current, current_path.variable_path, name_length)) {
            return NULL;
        }

        // We can assume this to succeed because we tested that the member exists
        member_typename = _Types_GetMemberTypename(current, current_path.variable_path, name_length);
        current         = _Parser_GetTypeByName(parser, member_typename, strlen(member_typename));

        current_path.path_length   -= name_length;
        current_path.variable_path += name_length;
    }

    return current->name;
}

bool _Parser_TypeMemberPathExists(ubcparser_t* parser, ubccustomtype_t* type, ubclvalue_t* member_path)
{
   return _Parser_TypeMemberPathResultTypename(parser, type, member_path) != NULL;
}

// This function copies the specified type and does not keep a reference
int _Parser_RegisterCustomType(ubcparser_t* parser, ubccustomtype_t new_type)
{
    if (parser->types.count == 0) {
        parser->types.array = _Parser_Malloc(parser, sizeof(ubccustomtype_t) * 1);
        if (parser->types.array == NULL) {
            return ENOMEM;
        }
    } else {
        size_t old_size = sizeof(ubccustomtype_t) * parser->types.count;
        size_t new_size = old_size + sizeof(ubccustomtype_t);
        ubccustomtype_t* new_array = _Parser_Realloc(parser, parser->types.array, new_size, old_size);
        if (new_array == NULL) {
            return ENOMEM;
        }
        parser->types.array = new_array;
    }

    parser->types.array[parser->types.count] = new_type;
    parser->types.count++;

    return EXIT_SUCCESS;
}

// This function does not report errors to the user
int _Parser_AddParseFile(ubcparser_t* parser, char* filename, size_t length)
{
    uint16_t file_index;
    char* registered_name;
    for (file_index = 0; file_index < parser->config.file_count; file_index++) {
        registered_name = parser->config.files[file_index].fileName;
        if (strncmp(registered_name, filename, length) == 0) {
            // No difference, file is located
            break;
        }
    }
    if (file_index == parser->config.file_count) {
        // No file found
        return EXIT_FAILURE;
    }

    parser->lexer_stack.stack_size += 1;
    uint16_t lexer_index = parser->lexer_stack.stack_size - 1;

    lexer_t* new_lexer_array = _Parser_Malloc(parser, sizeof(lexer_t) * parser->lexer_stack.stack_size);
    if (new_lexer_array == NULL) {
        return ENOMEM;
    }
    free(parser->lexer_stack.lexers);
    parser->lexer_stack.lexers = new_lexer_array;

    parser->lexer_stack.lexers[lexer_index].file = parser->config.files[file_index].fileName;
    parser->lexer_stack.lexers[lexer_index].pos  = parser->config.files[file_index].source;
    parser->lexer_stack.lexers[lexer_index].line = 1;

    return EXIT_SUCCESS;
}


/// Lookahead functions

// This function does not report errors to the user
int _Parser_LookAhead(ubcparser_t* parser, uint32_t range, token_t* destination)
{
    if (destination == NULL) {
        return EDESTADDRREQ;
    }
    if (parser == NULL) {
        return EINVAL;
    }
    if (range >= parser->lookahead.available) {
        return ERANGE;
    }

    destination[0] = parser->lookahead.tokens[range];

    return EXIT_SUCCESS;
}

// This function does not report errors to the user
int _Parser_NextLexToken(ubcparser_t* parser, token_t* destination)
{
    if (parser->lexer_stack.stack_size == 0) {
        return EBADFD;
    }

    Ubc_ReadNextToken(parser->lexer_stack.lexers + parser->lexer_stack.stack_size - 1,
                    destination);

    return EXIT_SUCCESS;
}

// This function does not report errors to the user
int _Parser_FillLookahead(ubcparser_t* parser)
{
    token_t* destination;
    int lex_code;
    uint32_t lookahead_capacity = sizeof(parser->lookahead.tokens) / sizeof(token_t);
    while (parser->lookahead.available < lookahead_capacity) {
        destination = parser->lookahead.tokens + parser->lookahead.available;
        lex_code = _Parser_NextLexToken(parser, destination);
        if (lex_code) {
            return lex_code;
        }
        parser->lookahead.available++;
    }

    return EXIT_SUCCESS;
}

// This function will inform the user
int _Parser_AssumeLookaheadFill(ubcparser_t* parser)
{
    int lookahead_code = _Parser_FillLookahead(parser);
    if (lookahead_code) {
        _Parser_ReportTopTracebackError(parser, "Unable to generate the next token.");
        _Parser_ReportLexerTraceback(parser);
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

int _Parser_ConsumeToken(ubcparser_t* parser)
{
    if (parser->lookahead.available < 1) {
        return EXIT_FAILURE;
    }

    uint32_t lookahead_capacity = sizeof(parser->lookahead.tokens) / sizeof(token_t);
    for (uint32_t i = 0; i < lookahead_capacity - 1; i++) {
        parser->lookahead.tokens[i] = parser->lookahead.tokens[i + 1];
    }
    parser->lookahead.available--;

    _Parser_FillLookahead(parser);

    return EXIT_SUCCESS;
}

int _Parser_DestroyCustomType(ubcparser_t* parser, ubccustomtype_t* type)
{
    if (type == NULL) {
        return EDESTADDRREQ;
    }

    if (type->name != NULL) {
        size_t name_length = strlen(type->name) + 1;
        _Parser_Free(parser, type->name, sizeof(char) * name_length);
        type->name = NULL;
    }

    if (type->field_names != NULL) {
        for (int i = 0; i < type->field_count; i++) {
            char*  field_name         = type->field_names[i];
            size_t name_buffer_length = strlen(field_name) + 1;
            _Parser_Free(parser, field_name, name_buffer_length);
            // Don't set it to NULL because we free it right after the loop finishes
        }

        size_t array_bytes = sizeof(char*) * type->field_count;
        _Parser_Free(parser, type->field_names, array_bytes);
        type->field_names = NULL;
    }

    if (type->field_typenames != NULL) {
        for (int i = 0; i < type->field_count; i++) {
            char*  field_typename    = type->field_typenames[i];
            size_t namebuffer_length = strlen(field_typename) + 1;
            _Parser_Free(parser, field_typename, namebuffer_length);
        }

        size_t array_bytes = sizeof(char*) * type->field_count;
        _Parser_Free(parser, type->field_typenames, array_bytes);
        type->field_typenames = NULL;
    }

    return EXIT_SUCCESS;
}

// This function assumes that the member type exists
int _Parser_AddCustomTypeMember(ubcparser_t* parser, ubccustomtype_t* type, token_t* member_type, token_t* member_name)
{

    if (type->field_count == 0) {
        type->field_names = _Parser_Malloc(parser, sizeof(char*) * 1);
        if (type->field_names == NULL) {
            return ENOMEM;
        }

        type->field_typenames = _Parser_Malloc(parser, sizeof(char*) * 1);
        if (type->field_typenames == NULL) {
            _Parser_Free(parser, type->field_names, sizeof(char*) * 1);
            type->field_names = NULL;

            return ENOMEM;
        }
    } else {
        size_t old_names_size  = sizeof(char*) * type->field_count;
        size_t new_names_size  = old_names_size + sizeof(char*);
        char** new_names_array = _Parser_Realloc(parser, type->field_names, new_names_size, old_names_size);
        if (new_names_array == NULL) {
            return ENOMEM;
        }
        type->field_names = new_names_array;

        size_t old_types_size = sizeof(char*) * type->field_count;
        size_t new_types_size = sizeof(char*) + old_types_size;
        char** new_types_array = _Parser_Realloc(parser, type->field_typenames, new_types_size, old_types_size);
        if (new_types_array == NULL) {
            _Parser_Free(parser, type->field_names, new_names_size);
            type->field_names = NULL;
            return ENOMEM;
        }
        type->field_typenames = new_types_array;
    }

    int new_member_index = type->field_count;
    type->field_count++;
    type->field_names[new_member_index] = _Parser_strndup(parser, member_name->ptr, member_name->value.length);
    if (type->field_names[new_member_index] == NULL) {
        return ENOMEM;
    }
    
    type->field_typenames[new_member_index] = _Parser_strndup(parser, member_type->ptr, member_type->value.length);
    if (type->field_typenames[new_member_index] == NULL) {
        return ENOMEM;
    }

    type->type_size += _Parser_GetTypeSize(parser, member_type->ptr, member_type->value.length);

    return EXIT_SUCCESS;
}

/// Expressions

void _Expressions_InitExpressionBase(ubcexpressionbase_t* base)
{
    base->result_typename = NULL;
    base->parent.type    = UBCEXPRESSIONTYPE_NONE;
    base->parent.as.comparison = NULL;
    base->needs_parsing   = true;
}

void _Expressions_InitializeParenExpression(ubcparenthesesexpression_t* paren)
{
    _Expressions_InitExpressionBase(&(paren->base));
    paren->parenthesized = NULL;
}

void _Expressions_InitializeLiteralExpression(ubcliteral_t* literal)
{
    literal->type = UBCLITERALTYPE_NONE;

    memset(&(literal->as), 0, sizeof(union UbcLiteralValue));
}

void _Expressions_InitializeValueExpression(ubcvalueexpression_t* value)
{
    _Expressions_InitExpressionBase(&(value->base));

    value->type = UBCVALUETYPE_NONE;
}


void _Expressions_InitializeNegationExpression(ubcnegateexpression_t* negation)
{
    _Expressions_InitExpressionBase(&(negation->base));
    _Expressions_InitializeValueExpression(&(negation->value));

    // leave negation->negation, there is no NONE value for it

    negation->value.base.parent.type = UBCEXPRESSIONTYPE_NEGATE;
    negation->value.base.parent.as.negation = negation;

    // Leave the negation truth value empty for valgrind to detect if we depend on it
}

void _Expressions_InitializeDivisionExpression(ubcdivisionexpression_t* division)
{
    _Expressions_InitExpressionBase(&(division->base));
    _Expressions_InitializeNegationExpression(&(division->child_expression));

    division->former_operand_typename = NULL;
    division->operator                = UBCDIVISIONOPERATOR_NONE;

    // Set parent information
    division->child_expression.base.parent.type        = UBCEXPRESSIONTYPE_DIVISION;
    division->child_expression.base.parent.as.division = division;

    // Leave the former->operator and current->operator unspecified for valgrind to detect
}

void _Expressions_InitializeAdditionExpression(ubcadditionexpression_t* addition)
{
    _Expressions_InitExpressionBase(&(addition->base));
    _Expressions_InitializeDivisionExpression(&(addition->child_expression));

    addition->former_operand_typename = NULL;
    addition->operator                = UBCADDITIONOPERATOR_NONE;

    // Set parents
    addition->child_expression.base.parent.type        = UBCEXPRESSIONTYPE_ADDITION;
    addition->child_expression.base.parent.as.addition = addition;

    // Leave the former->operator and current->operator unspecified for valgrind to detect
}

void _Expressions_InitializeCompareExpression(ubccompareexpression_t* comparison)
{
    _Expressions_InitExpressionBase(&(comparison->base));
    _Expressions_InitializeAdditionExpression(&(comparison->child_expression));

    comparison->comparator_type    = UBCCOMPARATORTYPE_NONE;
    comparison->left_side_typename = NULL;

    // Set parents
    comparison->child_expression.base.parent.type           = UBCEXPRESSIONTYPE_COMPARISON;
    comparison->child_expression.base.parent.as.comparison  = comparison;

    // Leave the comparator type empty for valgrind to detect if a jump depends on it
}

void _Expressions_InitializeLogicExpression(ubclogicexpression_t* expression)
{
    _Expressions_InitExpressionBase(&(expression->base));
    _Expressions_InitializeCompareExpression(&(expression->child_expression));

    expression->former_operand_type = NULL;
    expression->operator = UBCLOGICOPERATOR_NONE;

    expression->child_expression.base.parent.type     = UBCEXPRESSIONTYPE_LOGICAL;
    expression->child_expression.base.parent.as.logic = expression;
}


/// Scope and LValue functions

int _Scopes_AddScope(ubcparser_t* parser, enum UbcScopeType type)
{
    ubcscope_t new_scope;
    new_scope.temporary_bytes = 0;
    new_scope.type            = type;
    _UbcParserBuffer_Create(parser, &(new_scope.variables));

    if (_UbcParserBuffer_Write(parser, &(parser->scopes), &new_scope, sizeof(ubcscope_t))) {
    	return ENOMEM;
    }

    return EXIT_SUCCESS;
}

size_t _Parser_GetScopeCount(ubcparser_t* parser)
{
    return parser->scopes.used / sizeof(ubcscope_t);
}

// Index zero is the global scope
ubcscope_t* _Parser_GetScope(ubcparser_t* parser, size_t index)
{
    if (index >= _Parser_GetScopeCount(parser)) return NULL;
    ubcscope_t* scopes = parser->scopes.memory;

    return scopes + index;
}

size_t _Scope_GetVariableCount(ubcscope_t* scope)
{
    return scope->variables.used / sizeof(ubcvariable_t);
}

void _Scopes_DecreaseTemporaryBytes(ubcparser_t* parser, uint32_t count)
{
    ubcscope_t* scope = _Parser_GetScope(parser, _Parser_GetScopeCount(parser) - 1);
    scope->temporary_bytes -= count;
}

void _Scopes_IncreaseTemporaryBytes(ubcparser_t* parser, uint32_t count)
{
    ubcscope_t* scope = _Parser_GetScope(parser, _Parser_GetScopeCount(parser) - 1);
    scope->temporary_bytes += count;
}

// Returns the length of the variable, not any members accessed
size_t _LValue_GetVariableNameLength(ubclvalue_t* path)
{
    uintptr_t name_length = (uintptr_t) strnchr(path->variable_path, '.', path->path_length);
    if (name_length == 0) {
        name_length = path->path_length;
    } else {
        name_length = name_length - (uintptr_t) (path->variable_path);
    }

    return name_length;
}

// Returns NULL if the variable could not be found
ubcvariable_t* _Scope_GetVariable(ubcscope_t* scope, ubclvalue_t* variable)
{
    uintptr_t name_length = _LValue_GetVariableNameLength(variable);
    size_t variable_count = _Scope_GetVariableCount(scope);
    
    ubcvariable_t* current;
    for (size_t variable_index = 0; variable_index < variable_count; variable_index++) {
        current = (ubcvariable_t*) (scope->variables.memory) + variable_index;

        // name lengths are not equal? Names can't be equal, skip it
        if (current->name_length != name_length) {
            continue;
        }

        // Names are equal? return true
        if (strncmp(current->name, variable->variable_path, name_length) == 0) {
            return current;
        }
    }

    return NULL;
}

// Only the first name in the lvalue will be looked up
bool _Scope_VariableExists(ubcscope_t* scope, ubclvalue_t* variable_path)
{
    uintptr_t name_length = _LValue_GetVariableNameLength(variable_path);

    size_t variable_count = _Scope_GetVariableCount(scope);
    ubcvariable_t* current;
    for (size_t variable_index = 0; variable_index < variable_count; variable_index++) {
        current = (ubcvariable_t*) (scope->variables.memory) + variable_index;

        // name lengths are not equal? Names can't be equal, skip it
        if (current->name_length != name_length) {
            continue;
        }

        // Names are equal? return true
        if (strncmp(current->name, variable_path->variable_path, name_length) == 0) {
            return true;
        }
    }

    return false;
}

char* _Parser_ScopeLValueTypename(ubcparser_t* parser, ubcscope_t* scope, ubclvalue_t* lvalue)
{
    size_t variable_name_length = _LValue_GetVariableNameLength(lvalue);

    ubcvariable_t* variable = _Scope_GetVariable(scope, lvalue);
    if (variable == NULL) {
        char* format_format = "Reference to unknown variable \"%%.%ds\"";
    char  format_buffer[strlen(format_format) + 10];
    snprintf(format_buffer, strlen(format_format) + 10, format_format, variable_name_length);
    int report_code = _Parser_ReportFormattedTracebackError(parser, format_buffer, lvalue->variable_path);
    if (report_code) {
        // Reporting formatted failed for memory reasons
            _Parser_ReportTopTracebackError(parser, "Unable to allocate a detailed error message. Could not find variable in scope.");
    }
        return NULL;
    }

    
    ubccustomtype_t* variable_type = _Parser_GetTypeByName(parser, variable->typename, strlen(variable->typename));
    ubclvalue_t member_path;
    member_path.path_length        = lvalue->path_length   - variable_name_length;
    member_path.variable_path      = lvalue->variable_path + variable_name_length;

    return _Parser_TypeMemberPathResultTypename(parser, variable_type, &member_path);
}

char* _Parser_LValueTypename(ubcparser_t* parser, ubclvalue_t* lvalue)
{
    size_t scope_count = _Parser_GetScopeCount(parser);

    ubcscope_t* current_scope;
    for (size_t scope_index = 0; scope_index < scope_count; scope_index++) {
        current_scope = (ubcscope_t*) (parser->scopes.memory) + scope_index;
        if (_Scope_VariableExists(current_scope, lvalue)) {
            return _Parser_ScopeLValueTypename(parser, current_scope, lvalue);
        }
    }

    return NULL;
}

bool _Parser_LValueExists(ubcparser_t* parser, ubclvalue_t* lvalue)
{
    return _Parser_LValueTypename(parser, lvalue) != NULL;
}

/// Closure functions

uint32_t _Parser_ClosureGetCurrentBytecodeIndex(ubcparser_t* parser)
{
    return (uint32_t) parser->closure.bytecode.used; // Is this cast safe?
}

uintptr_t _Parser_ClosureStoreExplanationString(ubcparser_t* parser, char* string)
{
    ubcclosure_t* closure     = &(parser->closure);
    uintptr_t string_position = closure->explanation_strings.used;
    size_t string_length      = strlen(string);

    if (! parser->config.optimize_explanations) {

    	int write_code = _UbcParserBuffer_Write(parser, &(closure->explanation_strings), string, string_length);
    	if (write_code) return UINTPTR_MAX;

    	return string_position;
    }

    // Possibly a reverse search could speed it up, but I didn't want to implement that
    char* search_position = closure->explanation_strings.memory;
    while ((uintptr_t) search_position - (uintptr_t) closure->explanation_strings.memory < (uintptr_t) closure->explanation_strings.used) {
    	if (strncmp(search_position, string, string_length) == 0) {
    		return (uintptr_t) search_position - (uintptr_t) closure->explanation_strings.memory;
    	}

    	search_position += strlen(search_position);
    }

    int write_code = _UbcParserBuffer_Write(parser, &(closure->explanation_strings), string, string_length);
    if (write_code) return UINTPTR_MAX;

    return string_position;
}

int _Parser_ClosureAppendBytecode(ubcparser_t* parser, void* bytes, size_t count)
{
    ubcclosure_t* closure = &(parser->closure);

    int write_code;
    write_code = _UbcParserBuffer_Write(parser, &(closure->bytecode), bytes, count);
    if (write_code) return write_code;

    return EXIT_SUCCESS;
}

int _Parser_ClosureStoreExplanation(ubcparser_t* parser, uintptr_t index, uintptr_t range, char* string_explanation, ubcdebugsymbol symbolic_explanation)
{
    ubcclosure_t* closure = &(parser->closure);

    if (! parser->config.store_explanations) {
    	return EXIT_SUCCESS;
    }

    int write_code;
    ubcbytecodeexplanation_t explanation;

    if (parser->config.store_strings) {
    	uintptr_t stored_string_position = _Parser_ClosureStoreExplanationString(parser, string_explanation);
    	explanation.string_position = stored_string_position;

    	if (explanation.string_position == UINTPTR_MAX) {
    		_Parser_ReportError(parser, "No file", -1, "Failed to store explanation string.", UBCPARSERERROR_INTERNAL);
    		return ENOMEM;
    	}

    } else {
    	explanation.string_position = UINTPTR_MAX;
    }

    explanation.symbolic = symbolic_explanation;
    explanation.byte     = index;
    explanation.range    = range;

    write_code = _UbcParserBuffer_Write(parser, &(closure->code_explanation), &explanation, sizeof(ubcbytecodeexplanation_t));
    return write_code;
}

int _Parser_ClosureFixBytecodeIndex(ubcparser_t* parser, uint32_t offset, void* bytes, size_t count)
{
    if (offset + count >= parser->closure.bytecode.used) {
        _Parser_ReportError(parser, "No file", -1, "Failed to fix old bytecode jump target because it is out of range.", UBCPARSERERROR_INTERNAL);
        return ERANGE;
    }


    void* start = (char*) (parser->closure.bytecode.memory) + offset;
    memcpy(start, bytes, count);

    return EXIT_SUCCESS;
}

int _Parser_EmitBytecodeBytes(ubcparser_t* parser, void* bytes, size_t count, char* string_explanation, ubcdebugsymbol symbolic_explanation)
{
    if (parser->config.bytecode_callback != NULL)
    if (parser->config.bytecode_callback(parser->config.userdata, bytes, count, string_explanation, symbolic_explanation)) {
    	_Parser_ReportError(parser, "No file", -1, "Bytecode callback returned nonzero error indicator", UBCPARSERERROR_INTERNAL);
    	return EXIT_FAILURE;
    }

    if (_Parser_ClosureStoreExplanation(parser, parser->closure.bytecode.used, count, string_explanation, symbolic_explanation)) {
    	_Parser_ReportError(parser, "No file", -1, "Failed to store bytecode explanation in closure", UBCPARSERERROR_INTERNAL);
    	return ENOMEM;
    }

    if (_Parser_ClosureAppendBytecode(parser, bytes, count)) {
    	_Parser_ReportError(parser, "No file", -1, "Failed to append bytecode to closure.", UBCPARSERERROR_INTERNAL);
    	return ENOMEM;
    }

    return EXIT_SUCCESS;
}

int _Parser_BytecodePopUnusedBytes(ubcparser_t* parser, uint32_t bytes)
{
    uint8_t bytecode[5];

    bytecode[0] = UBC_OP_PUSHSP;

    if (_Parser_EmitBytecodeBytes(parser, (void*) bytecode, 1, "Push stack top address.", UBCDEBUGSYMBOL_PUSH_STACK_TOP)) {
    	return EXIT_FAILURE;
    }


    bytecode[0] = UBC_OP_PUSH32i;
    memcpy((void*) bytecode + 1, &bytes, 4);

    if (_Parser_EmitBytecodeBytes(parser, (void*) bytecode, 5, "Push unused byte count integer literal", UBCDEBUGSYMBOL_PUSH_LITERAL_INT)) {
    	return EXIT_FAILURE;
    }

    bytecode[0] = UBC_OP_SUBU;
    if (_Parser_EmitBytecodeBytes(parser, (void*) bytecode, 1, "Subtract unused byte count from stack top", UBCDEBUGSYMBOL_UNSIGNED_SUBTRACT)) {
    	return EXIT_FAILURE;
    }

    bytecode[0] = UBC_OP_POPSP;
    if (_Parser_EmitBytecodeBytes(parser, (void*) bytecode, 1, "Set changed stack top pointer, subtracted byte count will be 'forgotten'.", UBCDEBUGSYMBOL_SET_STACK_TOP)) {
    	return EXIT_FAILURE;
    }

    _Scopes_DecreaseTemporaryBytes(parser, bytes);

    return EXIT_SUCCESS;
}

int _Parser_GenerateAdditionBytecode(ubcparser_t* parser, ubcadditionexpression_t* addition)
{
    if (strcmp(addition->former_operand_typename, addition->child_expression.base.result_typename) != 0) {
        _Parser_ReportFormattedTracebackErrorFallback(parser, "Operand types of addition do not match", "Operand types \"%s\" and \"%s\" of addition do not match.", addition->former_operand_typename, addition->child_expression.base.result_typename);
        return EXIT_FAILURE;
    }

    if (addition->operator == UBCADDITIONOPERATOR_NONE) {
        _Parser_ReportTopTracebackError(parser, "Cannot apply operator \"None\" to addition operands");
        return EXIT_FAILURE;
    }

    uint8_t bytecode;
    int emit_code;
    ubcdebugsymbol symbol;
    char* explanation;

    if (strcmp(addition->former_operand_typename, UBC_FLOAT_TYPENAME) == 0) {
        switch (addition->operator)
        {
        case UBCADDITIONOPERATOR_PLUS:
            bytecode    = UBC_OP_ADDF;
            symbol      = UBCDEBUGSYMBOL_FLOAT_ADD;
            explanation = "Add the two most recently pushed floats";
            break;

        case UBCADDITIONOPERATOR_MINUS:
            bytecode    = UBC_OP_SUBF;
            symbol      = UBCDEBUGSYMBOL_FLOAT_SUBTRACT;
            explanation = "Subtract the two most recently pushed floats";
            break;
        }

        emit_code = _Parser_EmitBytecodeBytes(parser, &bytecode, 1, explanation, symbol);
        if (emit_code) return emit_code;
        _Scopes_DecreaseTemporaryBytes(parser, _Parser_BuiltInTypeSize(UBC_FLOAT_TYPENAME, strlen(UBC_FLOAT_TYPENAME)));

    // Set result type
    addition->base.result_typename = UBC_FLOAT_TYPENAME;

    } else if (strcmp(addition->former_operand_typename, UBC_INT_TYPENAME) == 0) {
        switch (addition->operator)
        {
        case UBCADDITIONOPERATOR_PLUS:
            bytecode    = UBC_OP_ADDI;
            symbol      = UBCDEBUGSYMBOL_INT_ADD;
            explanation = "Add the two most recently pushed ints";
            break;

        case UBCADDITIONOPERATOR_MINUS:
            bytecode    = UBC_OP_SUBI;
            symbol      = UBCDEBUGSYMBOL_INT_SUBTRACT;
            explanation = "Subtract the two most recently pushed ints";
            break;
        }

        emit_code = _Parser_EmitBytecodeBytes(parser, &bytecode, 1, explanation, symbol);
        if (emit_code) return emit_code;
        _Scopes_DecreaseTemporaryBytes(parser, _Parser_BuiltInTypeSize(UBC_INT_TYPENAME, strlen(UBC_INT_TYPENAME)));

    // Set result type
    addition->base.result_typename = UBC_INT_TYPENAME;

    } else {
        _Parser_ReportFormattedTracebackErrorFallback(parser, "Invalid operand type to addition", "Cannot add type \"%s\"", addition->former_operand_typename);
    }

    return EXIT_SUCCESS;
}

int _Parser_GenerateDivisionBytecode(ubcparser_t* parser, ubcdivisionexpression_t* division)
{

    if (strcmp(division->former_operand_typename, division->child_expression.base.result_typename) != 0) {
        _Parser_ReportFormattedTracebackErrorFallback(parser, "Operand types of division do not match", "Operand types of division do not match: Left-Hand-Side: %s, Right-Hand-Side: %s", division->former_operand_typename, division->child_expression.base.result_typename);
        return EXIT_FAILURE;
    }


    uint8_t bytecode;
    int emit_code;
    ubcdebugsymbol symbol;
    char* explanation;

    if (strcmp(division->former_operand_typename, UBC_FLOAT_TYPENAME) == 0) {
        switch (division->operator)
        {
        case UBCDIVISIONOPERATOR_DIVIDE:
            bytecode    = UBC_OP_DIVF;
            symbol      = UBCDEBUGSYMBOL_FLOAT_DIVIDE;
            explanation = "Divide the two most recently pushed floats";
            break;
        
        case UBCDIVISIONOPERATOR_MULTIPLY:
            bytecode = UBC_OP_MULF;
            symbol   = UBCDEBUGSYMBOL_FLOAT_MULTIPLY;
            explanation = "Multiply the two most recently pushed floats";
            break;
        
        default:
            // this catches the NONE operator
            _Parser_ReportTopTracebackError(parser, "Cannot apply Division Operator \"None\" on float division operands");
            return EXIT_FAILURE;
        }

        emit_code = _Parser_EmitBytecodeBytes(parser, &bytecode, 1, explanation, symbol);
        if (emit_code) return emit_code;
        _Scopes_DecreaseTemporaryBytes(parser, _Parser_BuiltInTypeSize(UBC_FLOAT_TYPENAME, strlen(UBC_FLOAT_TYPENAME)));
        division->base.result_typename = UBC_FLOAT_TYPENAME;

    } else if (strcmp(division->former_operand_typename, UBC_INT_TYPENAME) == 0) {

        switch (division->operator)
        {
        case UBCDIVISIONOPERATOR_DIVIDE:
            bytecode    = UBC_OP_DIVI;
            symbol      = UBCDEBUGSYMBOL_INT_DIVIDE;
            explanation = "Divide the two most recently pushed ints";
            break;
        
        case UBCDIVISIONOPERATOR_MULTIPLY:
            bytecode    = UBC_OP_MULI;
            symbol      = UBCDEBUGSYMBOL_INT_MULTIPLY;
            explanation = "Multiply the two most recently pushed ints";
            break;

        default:
            // None
            _Parser_ReportTopTracebackError(parser, "Cannot apply Division Operator \"None\" on integer division operands");
            break;
        }

        emit_code = _Parser_EmitBytecodeBytes(parser, &bytecode, 1, explanation, symbol);
        if (emit_code) return emit_code;
        _Scopes_DecreaseTemporaryBytes(parser, _Parser_BuiltInTypeSize(UBC_INT_TYPENAME, strlen(UBC_INT_TYPENAME)));
        division->base.result_typename = UBC_INT_TYPENAME;

    } else {
        _Parser_ReportFormattedTracebackErrorFallback(parser, "Invalid operand type to division", "Invalid operand type to division: %s", division->former_operand_typename);
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}


/// Parsing Functions

int _Parser_ParseLValue(ubcparser_t* parser, ubclvalue_t* lvlalue)
{
    token_t lookahead;
    token_t last;
    token_t start;

    _Parser_AssumeLookaheadFill(parser);
    if (_Parser_LookAhead(parser, 0, &start)) return EXIT_FAILURE;
    if (start.type != TT_IDENTIFIER) {
        _Parser_ReportUnexpectedToken(parser, "Unexpected token type when parsing lvalue", "identifier", start);
        return EXIT_FAILURE;
    }
    last = start;

    _Parser_AssumeLookaheadFill(parser);
    if (_Parser_LookAhead(parser, 0, &lookahead)) return EXIT_FAILURE;
    
    while (lookahead.type == TT_DOT) {

        // Consume dot
        _Parser_ConsumeToken(parser);

        _Parser_AssumeLookaheadFill(parser);
        if (_Parser_LookAhead(parser, 0, &last)) return EXIT_FAILURE;

        if (last.type != TT_IDENTIFIER) {
            _Parser_ReportUnexpectedToken(parser, "Unexpected token after TOKEN_DOT in lvalue.", "identifier", last);
            return EXIT_FAILURE;
        }
        _Parser_ConsumeToken(parser);

        // refresh dot lookahead token
        _Parser_AssumeLookaheadFill(parser);
        if (_Parser_LookAhead(parser, 0, &lookahead)) return EXIT_FAILURE;
    }

    lvlalue->path_length   = (uintptr_t) (last.ptr) - (uintptr_t)(start.ptr) + (uintptr_t)(start.value.length);
    lvlalue->variable_path = start.ptr;

    return EXIT_SUCCESS;
}

int _Parser_ParseInclude(ubcparser_t* parser)
{
    token_t include_token;
    int lookahead_code;
    _Parser_AssumeLookaheadFill(parser);
    lookahead_code = _Parser_LookAhead(parser, 0, &include_token);
    
    // No token available
    if (lookahead_code == ERANGE) {
        return ECANCELED;
    }

    if (include_token.type != TT_UBC_INCLUDE) {
        _Parser_ReportUnexpectedToken(parser, "Unexpected token at position 0 of include statement", "include", include_token);
        return ECANCELED;
    }

    // This is guaranteed to suceed because we were able to look ahead earlier
    // Consume include token
    _Parser_ConsumeToken(parser);
    _Parser_AssumeLookaheadFill(parser);

    // Check for string literal
    token_t filename_token;
    lookahead_code = _Parser_LookAhead(parser, 0, &filename_token);

    if (lookahead_code) {
        return ECANCELED;
    }
    if (filename_token.type != TT_STRING_LITERAL) {
        _Parser_ReportUnexpectedToken(parser, "Unexpected token at position 1 of include statement", "string literal", filename_token);
        return ECANCELED;
    }
    _Parser_ConsumeToken(parser);

    token_t semicolon_token;
    lookahead_code = _Parser_LookAhead(parser, 0, &semicolon_token);
    _Parser_AssumeLookaheadFill(parser);

    if (lookahead_code) {
        return ECANCELED;
    }
    if (semicolon_token.type != TT_SEMICOLON) {
        _Parser_ReportUnexpectedToken(parser, "Unexpected token at position 2 of include statement", ";", semicolon_token);
        return ECANCELED;
    }
    _Parser_ConsumeToken(parser);

    // All tokens were extracted successfully

    int add_code = _Parser_AddParseFile(parser, filename_token.ptr, filename_token.value.length);
    if (add_code == EXIT_FAILURE) {
        _Parser_ReportTopTracebackError(parser, "Unable to find file referenced in include statement");
        return ECANCELED;
    } else if (add_code != EXIT_SUCCESS) {
        // ENOMEM
        _Parser_ReportTopTracebackError(parser, "Unable to add included file to lexer queue because of insufficient memory");
    }
    
    return EXIT_SUCCESS;
}

int _Parser_ParseTypeDefinition(ubcparser_t* parser)
{
    token_t type_token;
    _Parser_AssumeLookaheadFill(parser);
    int lookahead_code = _Parser_LookAhead(parser, 0, &type_token);
    if (lookahead_code) {
        return EXIT_FAILURE;
    }

    if (type_token.type != TT_UBC_TYPE) {
        _Parser_ReportUnexpectedToken(parser, "Unexpected token at position 0 of type definition", "type", type_token);
        return EXIT_FAILURE;
    }
    _Parser_ConsumeToken(parser);

    token_t typename_token;
    _Parser_AssumeLookaheadFill(parser);
    if (_Parser_LookAhead(parser, 0, &typename_token)) return EXIT_FAILURE;
    if (typename_token.type != TT_IDENTIFIER) {
        _Parser_ReportUnexpectedToken(parser, "Unexpected token at position 1 of type definition", "identifier", typename_token);
        return EXIT_FAILURE;
    }
    _Parser_ConsumeToken(parser);

    if (_Parser_IsTypenameRegistered(parser, typename_token.ptr, typename_token.value.length)) {
        _Parser_ReportTopTracebackError(parser, "Bad typename, a type with this name already exists.");
        return EXIT_FAILURE;
    }


    token_t left_brace_token, right_brace_token;
    _Parser_AssumeLookaheadFill(parser);
    if (_Parser_LookAhead(parser, 0, &left_brace_token)) return EXIT_FAILURE;
    if (left_brace_token.type != TT_LEFT_BRACE) {
        _Parser_ReportUnexpectedToken(parser, "Unexpected token at position 2 in type definition", "{", left_brace_token);
        return EXIT_FAILURE;
    }
    _Parser_ConsumeToken(parser);

    token_t lookahead_token;
    _Parser_AssumeLookaheadFill(parser);
    lookahead_code = _Parser_LookAhead(parser, 0, &lookahead_token);
    if (lookahead_code) return EXIT_FAILURE;

    ubccustomtype_t new_type;
    new_type.name = _Parser_strndup(parser, typename_token.ptr, sizeof(char) * (typename_token.value.length));
    if (new_type.name == NULL) {
        return EXIT_FAILURE;
    }
    new_type.field_count     = 0;
    new_type.type_size       = 0;
    new_type.field_names     = NULL;
    new_type.field_typenames = NULL;

    token_t member_type, member_name, semicolon;
    while (_IsTypenameIdentifierToken(&lookahead_token))
    {
        // Collect tokens
        _Parser_AssumeLookaheadFill(parser);
        if (_Parser_LookAhead(parser, 0, &member_type)) {
            _Parser_DestroyCustomType(parser, &new_type);
            return EXIT_FAILURE;
        }
        if (!_IsTypenameIdentifierToken(&member_type)) {
            _Parser_ReportUnexpectedToken(parser, "Unexpected token at position 0 of member declaration in type definition", "typename", typename_token);
            _Parser_DestroyCustomType(parser, &new_type);
            return EXIT_FAILURE;
        }
        _Parser_ConsumeToken(parser);

        // Check whether the type exists
        bool type_exists = _Parser_IsTypenameRegistered(parser, member_type.ptr, member_type.value.length);
        if (!type_exists) {
            _Parser_ReportTopTracebackError(parser, "Unknown member type, cannot add member with unkown size to custom type.");
            _Parser_DestroyCustomType(parser, &new_type);
            return EXIT_FAILURE;
        }

        _Parser_AssumeLookaheadFill(parser);
        if (_Parser_LookAhead(parser, 0, &member_name)) {
            _Parser_DestroyCustomType(parser, &new_type);
            return EXIT_FAILURE;
        }
        if (member_name.type != TT_IDENTIFIER) {
            _Parser_ReportUnexpectedToken(parser, "Unexpected token at position 1 of member declaration in type definition", "identifier", member_name);
            _Parser_DestroyCustomType(parser, &new_type);
            return EXIT_FAILURE;
        }
        _Parser_ConsumeToken(parser);

        _Parser_AssumeLookaheadFill(parser);
        if (_Parser_LookAhead(parser, 0, &semicolon)) {
            _Parser_DestroyCustomType(parser, &new_type);
            return EXIT_FAILURE;
        }
        if (semicolon.type != TT_SEMICOLON) {
            _Parser_ReportUnexpectedToken(parser, "Unexpected token at position 2 of member declaration in type definition", ";", semicolon);
            _Parser_DestroyCustomType(parser, &new_type);
            return EXIT_FAILURE;
        }
        _Parser_ConsumeToken(parser);

        // All tokens gathered now
        int add_code = _Parser_AddCustomTypeMember(parser, &new_type, &member_type, &member_name);
        if (add_code) {
            _Parser_DestroyCustomType(parser, &new_type);
            return add_code;
        }

        // Refresh lookahead token
        _Parser_AssumeLookaheadFill(parser);
        if (_Parser_LookAhead(parser, 0, &lookahead_token)) {
            _Parser_DestroyCustomType(parser, &new_type);
            return EXIT_FAILURE;
        }
    }

    if (new_type.field_count == 0) {
        _Parser_ReportTopTracebackError(parser, "Empty custom types (without any members) are not allowed.");
        _Parser_DestroyCustomType(parser, &new_type);
        return EXIT_FAILURE;
    }

    // Successfully parsed members

    _Parser_AssumeLookaheadFill(parser);
    if (_Parser_LookAhead(parser, 0, &right_brace_token)) {
        _Parser_DestroyCustomType(parser, &new_type);
        return EXIT_FAILURE;
    }
    if (right_brace_token.type != TT_RIGHT_BRACE) {
        _Parser_ReportUnexpectedToken(parser, "Unexpected token after member declarations in type definition", "}", right_brace_token);
        return EXIT_FAILURE;
    }
    _Parser_ConsumeToken(parser);


    // Successfully parsed type definition statement
    int register_code = _Parser_RegisterCustomType(parser, new_type);
    if (register_code) {
        _Parser_DestroyCustomType(parser, &new_type);
        return register_code;
    }

    return EXIT_SUCCESS;
}

int _Parser_ExpressionNeedsParsing(ubcparser_t* parser, ubcexpression_t* expression, bool* destination)
{
    token_t lookahead_1, lookahead_2;
    switch (expression->type)
    {
    case UBCEXPRESSIONTYPE_LOGICAL:
        if (expression->as.logic->base.needs_parsing) {
        	destination[0] = true;
        	return EXIT_SUCCESS;
        }

        _Parser_AssumeLookaheadFill(parser);
        if (_Parser_LookAhead(parser, 0, &lookahead_1)) return EXIT_FAILURE;
        if (_Parser_LookAhead(parser, 1, &lookahead_2)) return EXIT_FAILURE;
        destination[0] = (lookahead_1.type == TT_AMPERSAND && lookahead_2.type == TT_AMPERSAND)
                        || (lookahead_1.type == TT_PIPE && lookahead_2.type == TT_PIPE)
    					|| (lookahead_1.type == TT_HAT);
        return EXIT_SUCCESS;
        break;

    case UBCEXPRESSIONTYPE_COMPARISON:
        if (expression->as.comparison->base.needs_parsing) {
            destination[0] = true;
            return EXIT_SUCCESS;
        }

        _Parser_AssumeLookaheadFill(parser);
        if (_Parser_LookAhead(parser, 0, &lookahead_1)) return EXIT_FAILURE;
        if (_Parser_LookAhead(parser, 1, &lookahead_2)) return EXIT_FAILURE;
        destination[0] = lookahead_1.type == TT_EQUALS
                        || lookahead_1.type == TT_LESS_THAN
                        || lookahead_1.type == TT_GREATER_THAN
                        || (lookahead_1.type == TT_BANG && lookahead_2.type == TT_EQUALS);
        break;
    

    case UBCEXPRESSIONTYPE_ADDITION:
        if (expression->as.addition->base.needs_parsing) {
            destination[0] = true;
            return EXIT_SUCCESS;
        }
        
        _Parser_AssumeLookaheadFill(parser);
        if (_Parser_LookAhead(parser, 0, &lookahead_1)) return EXIT_FAILURE;
        destination[0] = lookahead_1.type == TT_PLUS || lookahead_1.type == TT_MINUS;
        break;
    

    case UBCEXPRESSIONTYPE_DIVISION:
        if (expression->as.division->base.needs_parsing) {
            destination[0] = true;
            return EXIT_SUCCESS;
        }

        _Parser_AssumeLookaheadFill(parser);
        if (_Parser_LookAhead(parser, 0, &lookahead_1)) return EXIT_FAILURE;
        destination[0] = (lookahead_1.type == TT_ASTERISK || lookahead_1.type == TT_SLASH);
        break;
    
    case UBCEXPRESSIONTYPE_NEGATE:
    	destination[0] = expression->as.negation->base.needs_parsing;
    	return EXIT_SUCCESS;
        break;
    
    case UBCEXPRESSIONTYPE_PARENTHESES:
        if (expression->as.parenthesized->parenthesized == NULL) {
            destination[0] = true;
            return EXIT_SUCCESS;
        }
        break;
    
    case UBCEXPRESSIONTYPE_VALUE:
        ubcvalueexpression_t* value = expression->as.value;
    	destination[0] = value->base.needs_parsing;
    	return EXIT_SUCCESS;
        break;

    default:
        destination[0] = false;
        break;
    }

    return EXIT_SUCCESS;
}

int _Parser_ExpandCompareExpression(ubcparser_t* parser, ubcexpression_t* expression)
{
    ubccompareexpression_t* comparison = expression->as.comparison;

    if (comparison->base.needs_parsing) {
        // this is the left hand side expression
        ubcadditionexpression_t* child = &comparison->child_expression;
        _Expressions_InitializeAdditionExpression(child);
    child->base.parent.type = UBCEXPRESSIONTYPE_COMPARISON;
    child->base.parent.as.comparison = comparison;
    
    expression->as.addition        = child;
        expression->type               = UBCEXPRESSIONTYPE_ADDITION;
        
    // No explicit parsing needed, only if there are appropriate tokens
    comparison->base.needs_parsing = false;
        return EXIT_SUCCESS;
    }

    // Left hand side is present, parse operator and right hand side
    token_t lookahead1, lookahead2;
    _Parser_AssumeLookaheadFill(parser);
    if (_Parser_LookAhead(parser, 0, &lookahead1)) return EXIT_FAILURE;
    if (_Parser_LookAhead(parser, 1, &lookahead2)) return EXIT_FAILURE;

    if (lookahead1.type == TT_EQUALS && lookahead2.type == TT_EQUALS) {
        comparison->comparator_type = UBCCOMPARATORTYPE_EQUALITY;
        _Parser_ConsumeToken(parser);
        _Parser_ConsumeToken(parser);
    } else if (lookahead1.type == TT_LESS_THAN && lookahead2.type == TT_EQUALS) {
        comparison->comparator_type = UBCCOMPARATORTYPE_LESSEQUALS;
        _Parser_ConsumeToken(parser);
        _Parser_ConsumeToken(parser);
    } else if (lookahead1.type == TT_GREATER_THAN && lookahead2.type == TT_EQUALS) {
        comparison->comparator_type = UBCCOMPARATORTYPE_GREATEREQUALS;
        _Parser_ConsumeToken(parser);
        _Parser_ConsumeToken(parser);
    } else if (lookahead1.type == TT_BANG && lookahead2.type == TT_EQUALS) {
        comparison->comparator_type = UBCCOMPARATORTYPE_INEQUALITY;
        _Parser_ConsumeToken(parser);
        _Parser_ConsumeToken(parser);
    } else if (lookahead1.type == TT_LESS_THAN) {
        comparison->comparator_type = UBCCOMPARATORTYPE_LESSTHAN;
        _Parser_ConsumeToken(parser);
    } else if (lookahead1.type == TT_GREATER_THAN) {
        comparison->comparator_type = UBCCOMPARATORTYPE_GREATERTHAN;
        _Parser_ConsumeToken(parser);
    } else {
        // This function should only be called if there is something like this to parse, so this is an error
        _Parser_ReportUnexpectedToken(parser, "Unexpected token when parsing comparison expression", "Comparator !><=", lookahead1);
        return EXIT_FAILURE;
    }
    // The consuming cannot fail because the lookahead did not
    ubcadditionexpression_t* child = &(comparison->child_expression);

    // forward the addition to being parsed
    expression->type = UBCEXPRESSIONTYPE_ADDITION;
    expression->as.addition = child;

    _Expressions_InitializeAdditionExpression(expression->as.addition);

    return EXIT_SUCCESS;
}

int _Parser_ExpandAdditionExpression(ubcparser_t* parser, ubcexpression_t* expression)
{
    ubcadditionexpression_t* addition = expression->as.addition;

    if (addition->former_operand_typename != NULL) {
        int generate_code = _Parser_GenerateAdditionBytecode(parser, addition);

        if (generate_code) {
    	    return generate_code;
        }
    }
    if (!addition->base.needs_parsing) {
        addition->former_operand_typename = addition->child_expression.base.result_typename;
    }

    // Token indicating the operation
    token_t operator_token;
    _Parser_AssumeLookaheadFill(parser);
    if (_Parser_LookAhead(parser, 0, &operator_token)) return EXIT_FAILURE;

    // Determine operator type
    if (operator_token.type == TT_PLUS) {
        addition->operator = UBCADDITIONOPERATOR_PLUS;
    _Parser_ConsumeToken(parser);
    } else if (operator_token.type == TT_MINUS) {
        addition->operator = UBCADDITIONOPERATOR_MINUS;
    _Parser_ConsumeToken(parser);
    } else if (addition->former_operand_typename == NULL) {
        // Plus can only be omitted if this is the beginning of the addition
        addition->operator = UBCADDITIONOPERATOR_PLUS;
    } else {
        // This should not occur because this function is invoked only when there is such a token
        _Parser_ReportUnexpectedToken(parser, "Unexpected token while parsing addition expression", "+-", operator_token);
        return EXIT_FAILURE;
    }


    // set next parsing target
    expression->type = UBCEXPRESSIONTYPE_DIVISION;
    expression->as.division = &(addition->child_expression);

    _Expressions_InitializeDivisionExpression(expression->as.division);
    addition->child_expression.base.parent.as.addition = addition;
    addition->child_expression.base.parent.type        = UBCEXPRESSIONTYPE_ADDITION;

    addition->base.needs_parsing = false; // No explicit parsing needed, only on demand if there are more operators

    return EXIT_SUCCESS;
}

int _Parser_ExpandDivisionExpression(ubcparser_t* parser, ubcexpression_t* expression)
{
    ubcdivisionexpression_t* division = expression->as.division;

    if (division->base.needs_parsing) {
         // First expression to parse, not a single one has been parsed here yet
     
     _Expressions_InitializeNegationExpression(&(division->child_expression));
     division->child_expression.base.parent.as.division = division;
     division->child_expression.base.parent.type = UBCEXPRESSIONTYPE_DIVISION;

     division->base.needs_parsing = false;

     expression->as.negation = &(division->child_expression);
     expression->type        = UBCEXPRESSIONTYPE_NEGATE;

         return EXIT_SUCCESS;
    }

    if (!division->base.needs_parsing && division->former_operand_typename != NULL) {
        // There are two recent divisions ready to be processed
        int generate_code = _Parser_GenerateDivisionBytecode(parser, division);
        if (generate_code) return generate_code;

    division->former_operand_typename = division->child_expression.base.result_typename;
    }

    if (!division->base.needs_parsing && division->former_operand_typename == NULL) {
        // exactly one expression was parsed here until now, we move the type result to be able to process the next one
        division->former_operand_typename = division->child_expression.base.result_typename;
    }

    // Is there a token to be processed?
    token_t lookahead;
    _Parser_AssumeLookaheadFill(parser);
    if (_Parser_LookAhead(parser, 0, &lookahead)) return EXIT_FAILURE;
    if (lookahead.type == TT_ASTERISK) {
        division->operator = UBCDIVISIONOPERATOR_MULTIPLY;
        _Parser_ConsumeToken(parser);
    } else if (lookahead.type == TT_SLASH) {
        division->operator = UBCDIVISIONOPERATOR_DIVIDE;
        _Parser_ConsumeToken(parser);
    } else {
        // No valid operator
        _Parser_ReportUnexpectedToken(parser, "Unexpected token when parsing division operand", "Operator */", lookahead);
       return EXIT_FAILURE;
    }

    // Update current expression to parse
    expression->type         = UBCEXPRESSIONTYPE_NEGATE;
    expression->as.negation = &(division->child_expression);

    // Initialize the negation expression to a start state
    _Expressions_InitializeNegationExpression(expression->as.negation);
    division->child_expression.base.parent.as.division = division;
    division->child_expression.base.parent.type        = UBCEXPRESSIONTYPE_DIVISION;

    return EXIT_SUCCESS;
}

int _Parser_ExpandNegateExpression(ubcparser_t* parser, ubcexpression_t* expression)
{
    // argument conversion based on function
    ubcnegateexpression_t* negation = expression->as.negation;
    
    // Lookahead to determine whether the value expression is negated
    token_t lookahead;
    _Parser_AssumeLookaheadFill(parser);
    if (_Parser_LookAhead(parser, 0, &lookahead)) return EXIT_FAILURE;

    
    // Is there a negation?
    if (lookahead.type == TT_BANG) {
        negation->negation = true;
        _Parser_ConsumeToken(parser);
    } else {
        negation->negation = false;
    }


    // Determine type of the next expression
    negation->value.base.needs_parsing = true;
    expression->type = UBCEXPRESSIONTYPE_VALUE;
    expression->as.value = &(negation->value);

    negation->base.needs_parsing = false;

    return EXIT_SUCCESS;
}

int _Parser_ExpandParenExpression(ubcparser_t* parser, ubcexpression_t* expression)
{
    return EXIT_FAILURE;
}

int _Parser_ParseLiteral(ubcparser_t* parser, ubcliteral_t* literal)
{
    token_t lookahead;
    _Parser_AssumeLookaheadFill(parser);
    if (_Parser_LookAhead(parser, 0, &lookahead)) return EXIT_FAILURE;

    if (lookahead.type == TT_INT_LITERAL) {
        literal->type        = UBCLITERALTYPE_INT;
        literal->as.integer  = lookahead.value.intValue;
    } else if (lookahead.type == TT_FLOAT_LITERAL) {
        literal->as.floating = lookahead.value.floatValue;
        literal->type        = UBCLITERALTYPE_FLOAT;
    } else if (lookahead.type == TT_UBC_TRUE) {
        literal->as.boolean  = true;
        literal->type        = UBCLITERALTYPE_BOOL;
    } else if (lookahead.type == TT_UBC_FALSE) {
        literal->as.boolean  = false;
        literal->type        = UBCLITERALTYPE_BOOL;
    } else if (lookahead.type == TT_STRING_LITERAL) {
        literal->as.string.length  = lookahead.value.length;
        literal->as.string.pointer = lookahead.ptr;
        literal->type              = UBCLITERALTYPE_STRING;
    } else {
        _Parser_ReportUnexpectedToken(parser, "Unexpected token while parsing literal", "int literal, float literal, bool literal, string literal", lookahead);
        return EXIT_FAILURE;
    }

    _Parser_ConsumeToken(parser);

    return EXIT_SUCCESS;
}

int _Parser_ExpandValueExpression(ubcparser_t* parser, ubcexpression_t* expression)
{
    ubcvalueexpression_t* value = expression->as.value;
    token_t lookahead1, lookahead2;
    
    _Parser_AssumeLookaheadFill(parser);
    if (_Parser_LookAhead(parser, 0, &lookahead1)) return EXIT_FAILURE;

    _Parser_AssumeLookaheadFill(parser);
    if (_Parser_LookAhead(parser, 1, &lookahead2)) return EXIT_FAILURE;

    // TODO: Actions to do when a value type is detected

    if (lookahead1.type == TT_IDENTIFIER && lookahead2.type == TT_LEFT_PARENTHESIS) {
        value->type = UBCVALUETYPE_CALL;

    } else if (lookahead1.type == TT_IDENTIFIER && lookahead2.type != TT_LEFT_PARENTHESIS) {
        value->type = UBCVALUETYPE_LVALUE;
        if (_Parser_ParseLValue(parser, &(value->as.lvalue))) return EXIT_FAILURE;
        if (! _Parser_LValueExists(parser, &(value->as.lvalue))) {
            return EXIT_FAILURE;
        }
        value->base.needs_parsing   = false;
        value->base.result_typename = _Parser_LValueTypename(parser, &(value->as.lvalue));

    } else if (lookahead1.type == TT_LEFT_PARENTHESIS) {
        value->type = UBCVALUETYPE_PAREN;

    } else if (lookahead1.type == TT_INT_LITERAL || lookahead1.type == TT_STRING_LITERAL || lookahead1.type == TT_FLOAT_LITERAL || lookahead1.type == TT_UBC_FALSE || lookahead1.type == TT_UBC_TRUE) {
        value->type = UBCVALUETYPE_LITERAL;
        if (_Parser_ParseLiteral(parser, &(value->as.literal))) {
            return EXIT_FAILURE;
        }
        switch (value->as.literal.type)
        {
        case UBCLITERALTYPE_BOOL:
            value->base.result_typename = UBC_BOOL_TYPENAME;
            break;
        case UBCLITERALTYPE_INT:
            value->base.result_typename = UBC_INT_TYPENAME;
            break;
        case UBCLITERALTYPE_FLOAT:
            value->base.result_typename = UBC_FLOAT_TYPENAME;
            break;
        case UBCLITERALTYPE_STRING:
            value->base.result_typename = TT_UBC_STRING_TYPENAME;
            break;
        
        default:
            _Parser_ReportTopTracebackError(parser, "Got none type when finishing parsing literal. Please tell me how you managed to get this error");
            return EXIT_FAILURE;
            break;
        }

    } else {
        _Parser_ReportUnexpectedToken(parser, "Unexpected token while parsing value expression", "CALL(), IDENTIFIER, LITERAL, PARENTHESIS", lookahead1);
        value->type = UBCVALUETYPE_NONE;
        return EXIT_FAILURE;
    }
    value->base.needs_parsing = false;

    return EXIT_SUCCESS;
}

int _Parser_ExpandLogicExpression(ubcparser_t* parser, ubcexpression_t* expression)
{
    ubclogicexpression_t* logic = expression->as.logic;

    if (logic->base.needs_parsing) {
    	_Expressions_InitializeCompareExpression(&(logic->child_expression));
    	logic->child_expression.base.parent.type = UBCEXPRESSIONTYPE_LOGICAL;
    	logic->child_expression.base.parent.as.logic = logic;

    	expression->as.comparison = &(logic->child_expression);
    	expression->type = UBCEXPRESSIONTYPE_COMPARISON;

    	logic->base.needs_parsing = false;
    	return EXIT_SUCCESS;
    }

    return EXIT_FAILURE;

    return EXIT_SUCCESS;
}

int _Parser_ExpandExpression(ubcparser_t* parser, ubcexpression_t* expression)
{
    switch (expression->type)
    {
    case UBCEXPRESSIONTYPE_COMPARISON:
        return _Parser_ExpandCompareExpression(parser, expression);
        break;
    
    case UBCEXPRESSIONTYPE_ADDITION:
        return _Parser_ExpandAdditionExpression(parser, expression);
        break;
    
    case UBCEXPRESSIONTYPE_DIVISION:
        return _Parser_ExpandDivisionExpression(parser, expression);
        break;
    
    case UBCEXPRESSIONTYPE_NEGATE:
        return _Parser_ExpandNegateExpression(parser, expression);
        break;
    
    case UBCEXPRESSIONTYPE_VALUE:
        return _Parser_ExpandValueExpression(parser, expression);
        break;
    
    case UBCEXPRESSIONTYPE_PARENTHESES:
        return _Parser_ExpandParenExpression(parser, expression);
        break;
    
    case UBCEXPRESSIONTYPE_LOGICAL:
        return _Parser_ExpandLogicExpression(parser, expression);
        break;
 
    default:
        _Parser_ReportTopTracebackError(parser, "Cannot expand expression with unknown type");
        return ENOMEDIUM;
        break;
    }
}

int _Parser_FinalizeLiteralExpression(ubcparser_t* parser, ubcvalueexpression_t* value)
{
    ubcliteral_t* literal = &(value->as.literal);
    uint8_t bytecode[5];
    int emit_code = EXIT_SUCCESS;
    
    switch (literal->type) {
    	case UBCLITERALTYPE_FLOAT:
            bytecode[0]  = UBC_OP_PUSH32i;
            memcpy(bytecode + 1, &(literal->as.floating), sizeof(float));
            emit_code = _Parser_EmitBytecodeBytes(parser, bytecode, 5, "Push float literal from value expression", UBCDEBUGSYMBOL_PUSH_LITERAL_FLOAT);
            _Scopes_IncreaseTemporaryBytes(parser, _Parser_BuiltInTypeSize(UBC_FLOAT_TYPENAME, strlen(UBC_BOOL_TYPENAME)));
            value->base.result_typename = UBC_FLOAT_TYPENAME;
    		break;

    	case UBCLITERALTYPE_BOOL:
            bytecode[0] = UBC_OP_PUSH8i;
            memcpy(bytecode + 1, &literal->as.boolean, sizeof(bool));
            emit_code = _Parser_EmitBytecodeBytes(parser, bytecode, 2, "Push bool literal from value expression", UBCDEBUGSYMBOL_PUSH_LITERAL_BOOL);
    		_Scopes_IncreaseTemporaryBytes(parser, _Parser_BuiltInTypeSize(UBC_BOOL_TYPENAME, strlen(UBC_BOOL_TYPENAME)));
            value->base.result_typename = UBC_BOOL_TYPENAME;
            break;

    	case UBCLITERALTYPE_INT:
            bytecode[0] = UBC_OP_PUSH32i;
            memcpy(bytecode + 1, &literal->as.integer, sizeof(int32_t));
            emit_code = _Parser_EmitBytecodeBytes(parser, bytecode, 5, "Push integer literal from value expression", UBCDEBUGSYMBOL_PUSH_LITERAL_INT);
    		_Scopes_IncreaseTemporaryBytes(parser, _Parser_BuiltInTypeSize(UBC_INT_TYPENAME, strlen(UBC_INT_TYPENAME)));
            value->base.result_typename = UBC_INT_TYPENAME;
            break;

    	case UBCLITERALTYPE_STRING:
    		_Parser_ReportTopTracebackError(parser, "Strings are not supported yet.");
    		return EXIT_FAILURE;

    	default:
    		_Parser_ReportTopTracebackError(parser, "Cannot finalize literal with literal type \"None\"");
    		return EXIT_FAILURE;
    }

    return emit_code;
}

int _Parser_FinalizeParsedValueExpression(ubcparser_t* parser, ubcexpression_t* expression)
{
    ubcvalueexpression_t* value = expression->as.value;

    switch (value->type) {
    	case UBCVALUETYPE_LITERAL:
    		return _Parser_FinalizeLiteralExpression(parser, value);
    		break;

    	default:
    		_Parser_ReportTopTracebackError(parser, "Cannot finalize parsed value expression with value type \"None\"");
    		return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

int _Parser_FinalizeParsedNegateExpression(ubcparser_t* parser, ubcexpression_t* expression)
{
    ubcnegateexpression_t* negate = expression->as.negation;
    size_t result_length = strlen(negate->value.base.result_typename);
    uint8_t bytecode[5];
    int emit_code;
    uint32_t bytecode_position;

    if (negate->negation == true) {
    negate->base.result_typename = UBC_BOOL_TYPENAME;

        // Generate bytecode for negating something
        if (strncmp(UBC_BOOL_TYPENAME, negate->value.base.result_typename, result_length) == 0) {
            
            // Push comparison value
            bytecode[0] = UBC_OP_PUSH8i;
            bytecode[1] = 0x01;
            emit_code = _Parser_EmitBytecodeBytes(parser, bytecode, 2, "Push comparison boolean", UBCDEBUGSYMBOL_PUSH_LITERAL_BOOL);
            if (emit_code) return emit_code;

            // Compare boolean with comparison value
            bytecode[0] = UBC_OP_CMPB;
            emit_code = _Parser_EmitBytecodeBytes(parser, bytecode, 1, "Compare test value and true", UBCDEBUGSYMBOL_COMPARE_BOOLEANS);
            if (emit_code) return emit_code;

            // Skip conditional instructions
            bytecode_position  = _Parser_ClosureGetCurrentBytecodeIndex(parser);
            bytecode[0]        = UBC_OP_PUSH32i;
            bytecode_position += 5 + 1 + 2 + 5 + 1; // Advance bytecode target to skip true instructions
            memcpy(bytecode + 1, &bytecode_position, sizeof(uint32_t));
            _Parser_EmitBytecodeBytes(parser, bytecode, 5, "Push address of conditional jump target", UBCDEBUGSYMBOL_PUSH_JUMP_TARGET);
            if (emit_code) return emit_code;

            bytecode[0] = UBC_OP_JZ;
            emit_code = _Parser_EmitBytecodeBytes(parser, bytecode, 1, "Jump if test value was true", UBCDEBUGSYMBOL_SKIP_IF_JUMP);
            if (emit_code) return emit_code;

            // Push true if value was false
            bytecode[0] = UBC_OP_PUSH8i;
            bytecode[1] = 0x01;
            emit_code = _Parser_EmitBytecodeBytes(parser, bytecode, 2, "Push true if test value was false (negation, jump guard)", UBCDEBUGSYMBOL_PUSH_LITERAL_BOOL);
            if (emit_code) return emit_code;

            // Skip else instructions
            bytecode_position  = _Parser_ClosureGetCurrentBytecodeIndex(parser);
            bytecode[0]        = UBC_OP_JMPi;
            bytecode_position += 5 + 2 + 1; // Advance bytecode to skip following false instructions
            memcpy(bytecode + 1, &bytecode_position, sizeof(uint32_t));
            emit_code = _Parser_EmitBytecodeBytes(parser, bytecode, 5, "Jump to skip following branch instructions", UBCDEBUGSYMBOL_SKIP_ELSE_JUMP);
            if (emit_code) return emit_code;

            // Push false if value was true
            bytecode[0] = UBC_OP_PUSH8i;
            bytecode[1] = 0x00; // Push false if value was true
            emit_code = _Parser_EmitBytecodeBytes(parser, bytecode, 2, "Push false if test value was true (negation, jump guarded)", UBCDEBUGSYMBOL_PUSH_LITERAL_BOOL);
            if (emit_code) return emit_code;

        } else {
            _Parser_ReportFormattedTracebackError(parser, "Invalid operand type \"%s\" to negation.", negate->value.base.result_typename);
            // No need to specify the string length here because result_typenames are guaranteed to have \0 at the end
            return EXIT_FAILURE;
        }
    } else {
    // operand will not be negated, we just keep the type
    negate->base.result_typename = negate->value.base.result_typename;
    }

    return EXIT_SUCCESS;
}

int _Parser_FinalizeParsedDivisionExpression(ubcparser_t* parser, ubcexpression_t* expression)
{
    ubcdivisionexpression_t* division = expression->as.division;

    if (division->operator == UBCDIVISIONOPERATOR_NONE) {
        division->base.result_typename = division->child_expression.base.result_typename;
        
        return EXIT_SUCCESS;
    }

    if (division->former_operand_typename != NULL) {
        return _Parser_GenerateDivisionBytecode(parser, division);
    }

    return EXIT_SUCCESS;
}

int _Parser_FinalizeParsedAdditionExpression(ubcparser_t* parser, ubcexpression_t* expression)
{
    ubcadditionexpression_t* addition = expression->as.addition;

    if (addition->operator == UBCADDITIONOPERATOR_NONE) {
        // No real addition, just a single operand
        addition->base.result_typename = addition->child_expression.base.result_typename;

        return EXIT_SUCCESS;
    }

    if (addition->former_operand_typename != NULL) {
        return _Parser_GenerateAdditionBytecode(parser, addition);
    }

    return EXIT_SUCCESS;
}

int _Parser_FinalizeParsedComparisonExpression(ubcparser_t* parser, ubcexpression_t* expression)
{
    ubccompareexpression_t* comparison = expression->as.comparison;

    if (comparison->comparator_type == UBCCOMPARATORTYPE_NONE) {
    comparison->base.result_typename = comparison->child_expression.base.result_typename;
        return EXIT_SUCCESS;
    }

    if (strcmp(comparison->left_side_typename, comparison->child_expression.base.result_typename) != 0) {
        _Parser_ReportFormattedTracebackErrorFallback(parser, "Operand types for comparison operation do not match", "Cannot compare type \"%s\" to type \"%s\"", comparison->left_side_typename, comparison->child_expression.base.result_typename);
        
        return EXIT_FAILURE;
    }

    size_t float_typesize = _Parser_BuiltInTypeSize(UBC_FLOAT_TYPENAME, strlen(UBC_FLOAT_TYPENAME));
    size_t int_typesize   = _Parser_BuiltInTypeSize(UBC_INT_TYPENAME, strlen(UBC_INT_TYPENAME));
    size_t bool_typesize  = _Parser_BuiltInTypeSize(UBC_BOOL_TYPENAME, strlen(UBC_BOOL_TYPENAME));

    int emit_code, fixup_code;

    // Compare the arguments
    if (strcmp(comparison->left_side_typename, UBC_FLOAT_TYPENAME) == 0) {
        uint8_t bytecode;

        bytecode = UBC_OP_CMPF;
        emit_code = _Parser_EmitBytecodeBytes(parser, &bytecode, 1, "Compare the two most recently pushed floats", UBCDEBUGSYMBOL_COMPARE_FLOATS);
        if (emit_code) return emit_code;
        _Scopes_DecreaseTemporaryBytes(parser, float_typesize * 2); // Compare consumes the floats

    } else if (strcmp(comparison->left_side_typename, UBC_INT_TYPENAME) == 0) {
        uint8_t bytecode;

        bytecode = UBC_OP_CMPI;
        emit_code = _Parser_EmitBytecodeBytes(parser, &bytecode, 1, "Compare the two most recently pushed ints", UBCDEBUGSYMBOL_COMPARE_INTS);
        if (emit_code) return emit_code;
        _Scopes_DecreaseTemporaryBytes(parser, float_typesize * 2); // Compare consumes the ints

    } else {
        _Parser_ReportFormattedTracebackErrorFallback(parser, "Invalid operand type for comparison", "Cannot compare operands of type \"%s\"", comparison->left_side_typename);

        return EXIT_FAILURE;
    }

    uint8_t bytecode[5];
    uint32_t current_position;
    uint32_t fixup_position;

    // Push boolean depending on operator type and comparison result
    switch (comparison->comparator_type)
    {
    case UBCCOMPARATORTYPE_EQUALITY:

        bytecode[0]        = UBC_OP_PUSH32i;
        fixup_position     = 1 + _Parser_ClosureGetCurrentBytecodeIndex(parser); // Address of immediate value
        memset(bytecode + 1, 0xFF, 4); // Clear the target for now
        _Parser_EmitBytecodeBytes(parser, &bytecode, 5, "Push address of conditional jump target", UBCDEBUGSYMBOL_PUSH_JUMP_TARGET);
        if (emit_code) return emit_code;

        bytecode[0] = UBC_OP_JZ;
        emit_code = _Parser_EmitBytecodeBytes(parser, &bytecode, 1, "Jump on equality, continue for inequality", UBCDEBUGSYMBOL_SKIP_IF_JUMP);
        if (emit_code) return emit_code;

        emit_code = _Parser_BytecodePopUnusedBytes(parser, ADDRESS_BYTE_SIZE);
        if (emit_code) return emit_code;

        bytecode[0] = UBC_OP_PUSH8i;
        bytecode[1] = false;
        _Parser_EmitBytecodeBytes(parser, &bytecode, 2, "Push comparison result \"false\"", UBCDEBUG_PUSH_COMPARISON_RESULT);
        if (emit_code) return emit_code;

        bytecode[0]       = UBC_OP_JMPi;
        current_position  = _Parser_ClosureGetCurrentBytecodeIndex(parser);
        current_position += 5 + 2; // Skip this instruction and the next one
        memcpy(bytecode + 1, &current_position, 4);
        _Parser_EmitBytecodeBytes(parser, &bytecode, 5, "Jump to skip following branch instructions", UBCDEBUGSYMBOL_SKIP_ELSE_JUMP);
        if (emit_code) return emit_code;

        // Fix the old jump to jump here, we don't know how many bytes of bytecode were generated by PopUnusedBytes in-between
        current_position = _Parser_ClosureGetCurrentBytecodeIndex(parser);
        fixup_code = _Parser_ClosureFixBytecodeIndex(parser, current_position, &current_position, 4); // Write jump target
        if (fixup_code) return fixup_code;

        bytecode[0] = UBC_OP_PUSH8i;
        bytecode[1] = true;
        _Parser_EmitBytecodeBytes(parser, &bytecode, 2, "Push comparison result \"true\"", UBCDEBUG_PUSH_COMPARISON_RESULT);
        if (emit_code) return emit_code;

        /// TODO: Finish this

        break;

    /// TODO: Add more cases
    
    default:
        break;
    }

    return EXIT_FAILURE;
}

int _Parser_FinalizeParsedLogicExpression(ubcparser_t* parser, ubcexpression_t* expression)
{
    ubclogicexpression_t* logic = expression->as.logic;
    if (logic->former_operand_type == NULL && logic->operator == UBCLOGICOPERATOR_NONE) {
    	// There was no logic operation, just a single child expression
    	logic->base.result_typename = logic->child_expression.base.result_typename;
    	return EXIT_SUCCESS;
    }

    /// TODO: More operators

    if (logic->operator != UBCLOGICOPERATOR_NONE) {
    	_Parser_ReportTopTracebackError(parser, "Generating bytecode for logic operations is not implemented yet.");
    	logic->base.result_typename = NULL;
    	return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}


int _Parser_FinalizeParsedExpression(ubcparser_t* parser, ubcexpression_t* expression)
{

    switch (expression->type) {
        case UBCEXPRESSIONTYPE_VALUE:
            return _Parser_FinalizeParsedValueExpression(parser, expression);
            break;
        
        case UBCEXPRESSIONTYPE_NEGATE:
            return _Parser_FinalizeParsedNegateExpression(parser, expression);
            break;

        case UBCEXPRESSIONTYPE_DIVISION:
            return _Parser_FinalizeParsedDivisionExpression(parser, expression);
            break;
        
        case UBCEXPRESSIONTYPE_ADDITION:
            return _Parser_FinalizeParsedAdditionExpression(parser, expression);
        
        case UBCEXPRESSIONTYPE_COMPARISON:
            return _Parser_FinalizeParsedComparisonExpression(parser, expression);

        case UBCEXPRESSIONTYPE_LOGICAL:
            return _Parser_FinalizeParsedLogicExpression(parser, expression);

    	default:
    		_Parser_ReportTopTracebackError(parser, "Encountered unexpected expression type \"none\" when finalizing parsed expression.");
    		return EXIT_FAILURE;
    }
}

int _Parser_ParseExpression(ubcparser_t* parser, ubcexpression_t* starting_point)
{
    ubcexpression_t current;
    current = starting_point[0];

    bool parsing_needed = false;
    while (current.type != UBCEXPRESSIONTYPE_NONE) {
        // While there is a expression to continue parsing

        // Check if the current expression can be parsed further
        if (_Parser_ExpressionNeedsParsing(parser, &current, &parsing_needed)) {
            return EXIT_FAILURE;
        }

        // Expand the expression if needed
        if (parsing_needed) {
            if (_Parser_ExpandExpression(parser, &current)) {
                return EXIT_FAILURE;
            }
        } else {
            _Parser_FinalizeParsedExpression(parser, &current);
            current = current.as.addition->base.parent; // just cast to any type to access the parent
        }
    }

    return EXIT_SUCCESS;
}

int _Parser_ParseFunctionDefinition(ubcparser_t* parser)
{
    // TODO: Implement
    return EXIT_FAILURE;
}

int _Parser_ParseVariableDefinition(ubcparser_t* parser)
{
    // TODO: Implement
    return EXIT_FAILURE;
}

int _Parser_ParseAssignmentExpression(ubcparser_t* parser)
{
    ubclvalue_t lvalue;
    if (_Parser_ParseLValue(parser, &lvalue)) {
        return EXIT_FAILURE;
    }
    ubcscope_t scope;
    scope.variables.used = 0;
    _Parser_ScopeLValueTypename(parser, &scope, &lvalue);

    // Decide whether it is going to be an assignment or an expression

    return EXIT_SUCCESS;
}

int _Parser_ParseTopLevelExpression(ubcparser_t* parser, void* data)
{
    ubclogicexpression_t expression;
    _Expressions_InitializeLogicExpression(&expression);
    ubcexpression_t start;
    start.as.logic = &expression;
    start.type     = UBCEXPRESSIONTYPE_LOGICAL; 

    if (_Parser_ParseExpression(parser, &start)) {
        return EXIT_FAILURE;
    }

    size_t typesize = _Parser_GetTypeSize(parser, expression.base.result_typename, strlen(expression.base.result_typename));

    if (_Parser_BytecodePopUnusedBytes(parser, typesize)) {
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

int _Parser_ParseTopLevelStatement(ubcparser_t* parser)
{
    _Parser_AssumeLookaheadFill(parser);
    token_t lookahead_token;
    if (_Parser_LookAhead(parser, 0, &lookahead_token)) {
        return EXIT_FAILURE;
    }

    if (lookahead_token.type == TT_UBC_VAR || lookahead_token.type == TT_UBC_PERSIST) {
        return _Parser_ParseVariableDefinition(parser);
    }
    else if (lookahead_token.type == TT_UBC_TYPE) {
        return _Parser_ParseTypeDefinition(parser);
    } 
    else if (lookahead_token.type == TT_UBC_FUNCTION) {
        return _Parser_ParseFunctionDefinition(parser);
    } else if (lookahead_token.type != TT_IDENTIFIER) {
        return _Parser_ParseTopLevelExpression(parser, NULL);
    } else {
        return _Parser_ParseAssignmentExpression(parser);
    }

    return EXIT_SUCCESS;
}

int _Parser_ParseScript(ubcparser_t* parser)
{
    uint8_t state_include    = 0x01;
    uint8_t state_statements = 0x02;
    token_t token;
    int lookahead_code = 0, parse_code = 0;
    uint8_t* states = _Parser_Malloc(parser, sizeof(uint8_t));
    uint16_t state_count = parser->lexer_stack.stack_size;
    if (states == NULL) {
        return ENOMEM;
    }
    memset(states, state_include, sizeof(uint8_t) * state_count);

    while (states != NULL) {
        if (state_count == 0) {
            free(states);
            states = NULL;
            continue;
        }

        _Parser_AssumeLookaheadFill(parser);
        lookahead_code = _Parser_LookAhead(parser, 0, &token);
        if (lookahead_code) {
            return EXIT_FAILURE;
        }

        // switch from include to statement stage of scripts
        if (states[state_count - 1] == state_include && token.type != TT_UBC_INCLUDE) {
            states[state_count - 1] = state_statements;
        }

        // switch from statement state to removing the file
        if (states[state_count - 1] == state_statements && token.type == TT_EOF) {
            size_t old_size = sizeof(uint8_t) * (state_count);
            size_t new_size = old_size - sizeof(uint8_t);

            uint8_t* new_states = _Parser_Malloc(parser, new_size);
            if (new_states == NULL) {
                _Parser_Free(parser, states, old_size);
                return EXIT_FAILURE;
            }

            memcpy(new_states, states, new_size);
            _Parser_Free(parser, states, old_size);

            states = new_states;
            
            state_count -= 1;

            // Clear the lookahead because it's possibly filled with EOF tokens from the ending file
            parser->lookahead.available = 0;
            _Parser_AssumeLookaheadFill(parser);
            
            continue;
        }

        // File is in statement state
        if (states[state_count - 1] == state_statements) {
            //parse_code = _Parser_ParseTopLevelStatement(parser);
            _Parser_AssumeLookaheadFill(parser);
            parse_code = _Parser_ParseTopLevelStatement(parser);
        }
        if (parse_code) {
            _Parser_Free(parser, states, sizeof(uint8_t) * state_count);
            return EXIT_FAILURE;
        }


        // File is in include state
        if (states[state_count - 1] == state_include) {
            parse_code = _Parser_ParseInclude(parser);
        }
        if (states[state_count - 1] == state_include  && parse_code) {
            _Parser_Free(parser, states, sizeof(uint8_t) * state_count);
            return EXIT_FAILURE;
        } else if (states[state_count - 1] == state_include && !parse_code) {
            // Parsing was successful

            // Expand state array for new files
            uint8_t* new_states = _Parser_Malloc(parser, sizeof(uint8_t) * parser->lexer_stack.stack_size);
            if (new_states == NULL) {
                _Parser_Free(parser, states, state_count * sizeof(uint8_t));
                return ENOMEM;
            }

            // Transfer old state data
            memcpy(new_states, states, state_count * sizeof(uint8_t));
            state_count = parser->lexer_stack.stack_size;
            new_states[state_count - 1] = state_include;
            
            // Free old state array
            _Parser_Free(parser, states, state_count * sizeof(uint8_t));
            states = new_states;
        }
    }

    return EXIT_SUCCESS;
}

// This function will call ReportError to report errors
int Parser_Parse(ubcparser_t* parser, char* filename)
{
    int init_code = _Parser_AddParseFile(parser, filename, strlen(filename));
    if (init_code) {
        return init_code;
    }

    init_code = _Parser_FillLookahead(parser);
    if (init_code) {
        return init_code;
    }

    int script_code = _Parser_ParseScript(parser);
    if (script_code) {
        return script_code;
    }


    return 0;
}


/// Constructor, Deconstructors


// This function does not use the error callback
int Parser_Create(ubcparser_t* destination, ubcparserconfig_t* config)
{
    if (destination == NULL) {
        return EDESTADDRREQ;
    }
    if (config == NULL) {
        return EINVAL;
    }

    destination->config = config[0];

    destination->lexer_stack.lexers     = NULL;
    destination->lexer_stack.stack_size = 0;
    
    _UbcParserBuffer_Create(destination, &destination->scopes);
    _Scopes_AddScope(destination, UBCSCOPE_GLOBAL);

    _UbcParserBuffer_Create(destination, &destination->closure.bytecode);
    _UbcParserBuffer_Create(destination, &destination->closure.code_explanation);
    _UbcParserBuffer_Create(destination, &destination->closure.explanation_strings);
    
    destination->lookahead.available = 0;

    destination->types.array = NULL;
    destination->types.count = 0;

    return 0;
}

// This function does not use the error callback
int Parser_Destroy(ubcparser_t* parser)
{
    if (parser == NULL) {
        return EINVAL;
    }

    _UbcParserBuffer_Destroy(parser, &parser->scopes);

    _UbcParserBuffer_Destroy(parser, &parser->closure.bytecode);
    _UbcParserBuffer_Destroy(parser, &parser->closure.code_explanation);
    _UbcParserBuffer_Destroy(parser, &parser->closure.explanation_strings);

    if (parser->lexer_stack.lexers != NULL)
    _Parser_Free(parser, parser->lexer_stack.lexers, parser->lexer_stack.stack_size * sizeof(lexer_t));

    if (parser->types.array != NULL) {
        for (uint16_t i = 0; i != parser->types.count; i++) {
            _Parser_DestroyCustomType(parser, parser->types.array + i);
        }
        _Parser_Free(parser, parser->types.array, sizeof(ubccustomtype_t) * parser->types.count);
    }

    return EXIT_SUCCESS;
}
