#include "parser.h"


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
    // C99 Standard allows this
    size_t needed_length = 1 + vsnprintf(NULL, 0, format, args);

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

bool _UbcParserBuffer_EnsureFreeCapacity(ubcparser_t* parser, ubcparserbuffer_t* buffer, size_t needed)
{
	if (parser == NULL) return false;
	if (buffer == NULL) return false;

	if (buffer->memory == NULL) {
		buffer->memory = _Parser_Malloc(parser, needed);
		if (buffer->memory == NULL) {
			return false;
		}
	} else if (needed > (buffer->capacity - buffer->used)) {
		size_t new_capacity = buffer->capacity * 2;
		if (needed > (new_capacity - buffer->used)) {
			new_capacity = buffer->capacity + needed;
		}

		void* new_memory = _Parser_Realloc(parser, buffer->memory, new_capacity, buffer->capacity);
		if (new_memory == NULL) return false;
		buffer->memory = new_memory;
	}

	return true;
}

int _UbcParserBuffer_Write(ubcparser_t* parser, ubcparserbuffer_t* buffer, void* memory, size_t length)
{
	if (parser == NULL) return EINVAL;
	if (memory == NULL) return EINVAL;
	if (buffer == NULL) return EDESTADDRREQ;

	if (_UbcParserBuffer_EnsureFreeCapacity(parser, buffer, length)) return ENOMEM;

	void* destination = (void*) ((char*) (buffer->memory) + length);
	memcpy(destination, memory, length);

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
    lexer_t top_lexer = parser->lexer_stack.lexers[parser->lexer_stack.stack_size - 1];
    _Parser_ReportError(parser, top_lexer.file, top_lexer.line, "in file", UBCPARSERERROR_TRACEBACK);

    // Skip first file
    for (uint16_t lexer_index = parser->lexer_stack.stack_size - 2; lexer_index + 1 != 0 ; lexer_index--) {
        lexer_t* lexer = parser->lexer_stack.lexers + lexer_index;
        _Parser_ReportError(parser, lexer->file, lexer->line, "included by", UBCPARSERERROR_TRACEBACK);
    }
}

void _Parser_ReportTopTracebackError(ubcparser_t* parser, const char* message)
{
    if (parser->lexer_stack.stack_size == 0) {
        _Parser_ReportError(parser, "No file", -1, message, UBCPARSERERROR_ERRORMESSAGE);
        return;
    }

    lexer_t* top_lexer = &(parser->lexer_stack.lexers[parser->lexer_stack.stack_size - 1]);
    _Parser_ReportError(parser, top_lexer->file, top_lexer->line, message, UBCPARSERERROR_ERRORMESSAGE);
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
		_Parser_ReportError(parser, top_lexer.file, unexpected.line, result_string, UBCPARSERERROR_ERRORMESSAGE);
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
    if (strncmp(typename, TT_UBC_BOOL_TYPENAME, name_length) == 0) 
        return true;
    if (strncmp(typename, TT_UBC_INT_TYPENAME, name_length) == 0) 
        return true;
    if (strncmp(typename, TT_UBC_FLOAT_TYPENAME, name_length) == 0) 
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

size_t _Parser_BuiltInTypeSize(char* typename, size_t name_length)
{
    if (strncmp(typename, TT_UBC_BOOL_TYPENAME, name_length) == 0) 
        return 8;
    if (strncmp(typename, TT_UBC_INT_TYPENAME, name_length) == 0) 
        return 32;
    if (strncmp(typename, TT_UBC_FLOAT_TYPENAME, name_length) == 0) 
        return 32;
    if (strncmp(typename, TT_UBC_STRING_TYPENAME, name_length) == 0) 
        return 32;
    
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

int _Parser_DestroyExpression(ubcparser_t* parser, ubccompareexpression_t* expression)
{
    // TODO: Implement
    return EXIT_FAILURE;
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

    negation->value.base.parent.type = UBCEXPRESSIONTYPE_NEGATE;
    negation->value.base.parent.as.negation = negation;

    // Leave the negation truth value empty for valgrind to detect if we depend on it
}

void _Expressions_InitializeDivisionExpression(ubcdivisionexpression_t* division)
{
    _Expressions_InitExpressionBase(&(division->base));
    _Expressions_InitializeNegationExpression(&(division->former.expression));
    _Expressions_InitializeNegationExpression(&(division->current.expression));

    // Set parents
    division->former.expression.base.parent.type  = UBCEXPRESSIONTYPE_DIVISION;
    division->current.expression.base.parent.type = UBCEXPRESSIONTYPE_DIVISION;
    division->current.expression.base.parent.as.division = division;
    division->former.expression.base.parent.as.division  = division;

    // Leave the former->operator and current->operator unspecified for valgrind to detect
}

void _Expressions_InitializeAdditionExpression(ubcadditionexpression_t* addition)
{
    _Expressions_InitExpressionBase(&(addition->base));
    _Expressions_InitializeDivisionExpression(&(addition->former.expression));
    _Expressions_InitializeDivisionExpression(&(addition->current.expression));

    // Set parents
    addition->former.expression.base.parent.type  = UBCEXPRESSIONTYPE_ADDITION;
    addition->current.expression.base.parent.type = UBCEXPRESSIONTYPE_ADDITION;
    addition->current.expression.base.parent.as.addition = addition;
    addition->former.expression.base.parent.as.addition  = addition;

    // Leave the former->operator and current->operator unspecified for valgrind to detect
}

void _Expressions_InitializeCompareExpression(ubccompareexpression_t* comparison)
{
    _Expressions_InitExpressionBase(&(comparison->base));
    _Expressions_InitializeAdditionExpression(&(comparison->left_hand_side));
    _Expressions_InitializeAdditionExpression(&(comparison->right_hand_side));

    // Set parents
    comparison->left_hand_side.base.parent.type  = UBCEXPRESSIONTYPE_COMPARISON;
    comparison->right_hand_side.base.parent.type = UBCEXPRESSIONTYPE_COMPARISON;
    comparison->left_hand_side.base.parent.as.comparison  = comparison;
    comparison->right_hand_side.base.parent.as.comparison = comparison;

    // Leave the comparator type empty for valgrind to detect if a jump depends on it
}

/// Bytecode functions

int _Parser_GenerateExpressionBytecode(ubcparser_t* parser, ubccompareexpression_t* expression)
{
    if (parser == NULL || expression == NULL) return EXIT_FAILURE;

    return EXIT_FAILURE;
}

int _Parser_BytecodePopUnusedBytes(ubcparser_t* parser, size_t bytes)
{
    return EXIT_FAILURE;    
}


/// Scope and LValue functions

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
    } else {
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
    new_type.name = _Parser_strndup(parser, typename_token.ptr, sizeof(char) * (typename_token.value.length + 1));
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
    case UBCEXPRESSIONTYPE_COMPARISON:
        if (expression->as.comparison->left_hand_side.base.needs_parsing) {
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
        if (expression->as.addition->former.expression.base.needs_parsing) {
            destination[0] = true;
            return EXIT_SUCCESS;
        }
        
        _Parser_AssumeLookaheadFill(parser);
        if (_Parser_LookAhead(parser, 0, &lookahead_1)) return EXIT_FAILURE;
        destination[0] = lookahead_1.type == TT_PLUS || lookahead_1.type == TT_MINUS;
        break;
    

    case UBCEXPRESSIONTYPE_DIVISION:
        if (expression->as.division->former.expression.base.needs_parsing) {
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

int _Parser_ExpandComparisonExpression(ubcparser_t* parser, ubcexpression_t* expression)
{
    ubccompareexpression_t* comparison = expression->as.comparison;

    if (comparison->left_hand_side.base.needs_parsing) {
    	ubcadditionexpression_t* lhs = &(comparison->left_hand_side);
    	expression->as.addition      = lhs;
    	expression->type             = UBCEXPRESSIONTYPE_ADDITION;
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
    ubcadditionexpression_t* rhs     = &(comparison->right_hand_side);

    // forward the addition to being parsed
    expression->type = UBCEXPRESSIONTYPE_ADDITION;
    expression->as.addition = rhs;

    return EXIT_SUCCESS;
}

int _Parser_ExpandAdditionExpression(ubcparser_t* parser, ubcexpression_t* expression)
{
    ubcadditionexpression_t* addition = expression->as.addition;
    ubcadditionelement_t*    next_target;

    if (!addition->former.expression.base.needs_parsing && !addition->current.expression.base.needs_parsing) {
        // There has been a former and a current expression parsed
        // TODO: Generate Bytecode for addition
        addition->former = addition->current;
    }

    // There has not been a single expression parsed by this addition yet
    if (addition->former.expression.base.needs_parsing) {
        next_target = &(addition->former);
    } else {
        next_target = &(addition->current);
    }

    // Token indicating the operation
    token_t operator_token;
    _Parser_AssumeLookaheadFill(parser);
    if (_Parser_LookAhead(parser, 0, &operator_token)) return EXIT_FAILURE;

    // Determine operator type
    if (operator_token.type == TT_PLUS) {
        next_target->operator = UBCADDITIONOPERATOR_PLUS;
    } else if (operator_token.type == TT_MINUS) {
        next_target->operator = UBCADDITIONOPERATOR_MINUS;
    } else if (next_target == &(addition->former)) {
        // Plus can only be omitted if this is the beginning of the addition
        next_target->operator = UBCADDITIONOPERATOR_PLUS;
    } else {
        _Parser_ReportUnexpectedToken(parser, "Unexpected token while parsing addition expression", "+-", operator_token);
        return EXIT_FAILURE;
    }


    // set next parsing target
    next_target->expression.base.needs_parsing = true;
    expression->type = UBCEXPRESSIONTYPE_DIVISION;
    expression->as.division = &(addition->former.expression);

    return EXIT_SUCCESS;
}

int _Parser_ExpandDivisionExpression(ubcparser_t* parser, ubcexpression_t* expression)
{
    ubcdivisionexpression_t* division = expression->as.division;
    ubcdivisionoperand_t*    next_target;

    if (!division->former.expression.base.needs_parsing && !division->current.expression.base.needs_parsing) {
        // There are two parsed expressions for which this needs to generate bytecode now
        // TODO: Generate Bytecode
        division->former = division->current;
    }

    // Determine what is going to be processed
    if (division->former.expression.base.needs_parsing) {
        next_target = &(division->former);
    } else {
        next_target = &(division->current);
    }
    bool is_first_expression = next_target == &(division->former);

    // Is there a token to be processed?
    token_t lookahead;
    _Parser_AssumeLookaheadFill(parser);
    if (_Parser_LookAhead(parser, 0, &lookahead)) return EXIT_FAILURE;

    if (lookahead.type == TT_ASTERISK && !is_first_expression) {
        next_target->operator = UBCDIVISIONOPERATOR_MULTIPLY;
        _Parser_ConsumeToken(parser);
    } else if (lookahead.type == TT_SLASH && !is_first_expression) {
        next_target->operator = UBCDIVISIONOPERATOR_DIVIDE;
        _Parser_ConsumeToken(parser);
    } else if (is_first_expression) {
        next_target->operator = UBCDIVISIONOPERATOR_MULTIPLY; // This operator is going to be ignored
    } else {
        // No valid operator
        _Parser_ReportUnexpectedToken(parser, "Unexpected token when parsing division operand", "Operator */", lookahead);
        return EXIT_FAILURE;
    }

    // Update current expression to parse
    next_target->expression.base.needs_parsing = true;
    expression->type         = UBCEXPRESSIONTYPE_NEGATE;
    expression->as.negation = &(next_target->expression);

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
    }


    // Determine type of the next expression
    negation->value.base.needs_parsing = true;
    expression->type = UBCEXPRESSIONTYPE_VALUE;
    expression->as.value = &(negation->value);

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

    /// TODO: Push literals
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
            value->base.result_typename = TT_UBC_BOOL_TYPENAME;
            break;
        case UBCLITERALTYPE_INT:
            value->base.result_typename = TT_UBC_INT_TYPENAME;
            break;
        case UBCLITERALTYPE_FLOAT:
            value->base.result_typename = TT_UBC_FLOAT_TYPENAME;
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

int _Parser_ExpandExpression(ubcparser_t* parser, ubcexpression_t* expression)
{
    switch (expression->type)
    {
    case UBCEXPRESSIONTYPE_COMPARISON:
        return _Parser_ExpandComparisonExpression(parser, expression);
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
    
    // TODO: More functions for more types
    
    default:
        return ENOMEDIUM;
        break;
    }
}

int _Parser_FinalizeParsedValueExpression(ubcparser_t* parser, ubcexpression_t* expression)
{
	return EXIT_FAILURE;
}

int _Parser_FinalizeParsedExpression(ubcparser_t* parser, ubcexpression_t* expression)
{
	switch (expression->type) {
		case UBCEXPRESSIONTYPE_VALUE:
			return _Parser_FinalizeParsedValueExpression(parser, expression);
			break;

		default:
			_Parser_ReportTopTracebackError(parser, "Encountered unexpected expression type \"none\" when finalizing parsed value expression.");
			return EXIT_FAILURE;
	}
}

int _Parser_ParseExpression(ubcparser_t* parser, ubccompareexpression_t* supplied_root)
{
    ubccompareexpression_t root;

    if (supplied_root == NULL) {
        _Expressions_InitializeCompareExpression(&root);
    } else {
        root = supplied_root[0];
    }

    ubcexpression_t current;
    current.type = UBCEXPRESSIONTYPE_COMPARISON;
    current.as.comparison = &root;

    bool parsing_needed = false;
    while (current.type != UBCEXPRESSIONTYPE_NONE) {
        // While there is a expression to continue parsing

        // Check if the current expression can be parsed further
        if (_Parser_ExpressionNeedsParsing(parser, &current, &parsing_needed)) {
            _Parser_DestroyExpression(parser, &root);
            return EXIT_FAILURE;
        }

        // Expand the expression if needed
        if (parsing_needed) {
            if (_Parser_ExpandExpression(parser, &current)) {
                _Parser_DestroyExpression(parser, &root);
                return EXIT_FAILURE;
            }
        } else {
            current = current.as.addition->base.parent;
        }
    }

    return EXIT_FAILURE;
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
    ubccompareexpression_t expression;
    if (_Parser_ParseExpression(parser, NULL)) {
        return EXIT_FAILURE;
    }

    if (_Parser_GenerateExpressionBytecode(parser, &expression)) {
        _Parser_DestroyExpression(parser, &expression);
        return EXIT_FAILURE;
    }

    size_t typesize = _Parser_GetTypeSize(parser, expression.base.result_typename, strlen(expression.base.result_typename));
    _Parser_DestroyExpression(parser, &expression);

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

    destination->config                    = config[0];
    
    destination->lexer_stack.lexers        = NULL;
    destination->lexer_stack.stack_size    = 0;
    
    destination->bytecode_buffer.capacity = 0;
    destination->bytecode_buffer.memory   = NULL;
    destination->bytecode_buffer.used     = 0;
    
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

    if (parser->bytecode_buffer.memory != NULL)
    _UbcParserBuffer_Destroy(parser, &(parser->bytecode_buffer));

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
