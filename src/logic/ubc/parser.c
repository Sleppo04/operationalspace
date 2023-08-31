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

// Type registering and request functions
bool _Parser_IsBuiltInTypename(char* typename, int32_t name_length)
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

bool _Parser_IsTypenameRegistered(ubcparser_t* parser, char* typename, int32_t name_length)
{
    if (_Parser_IsBuiltInTypename(typename, name_length))
        return true;

    for (uint16_t i = 0; i != parser->type_count; i++) {
        char* defined_typename = parser->defined_types[i].name;
        int32_t length = strlen(defined_typename);
        if (length != name_length) continue;
        if (strncmp(defined_typename, typename, name_length) == 0) {
            return true;
        }
    }

    for (uint16_t i = 0; i != parser->config.type_count; i++) {
        char* foreign_typename = parser->config.foreign_types[i].name;
        int32_t length = strlen(foreign_typename);
        if (length != name_length) continue;
        if (strncmp(foreign_typename, typename, name_length)) {
            return true;
        }
    }

    return false;
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

size_t _Parser_BuiltInTypeSize(char* typename, int32_t name_length)
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
size_t _Parser_GetTypeSize(ubcparser_t* parser, char* typename, int32_t name_length)
{
    if (_Parser_IsBuiltInTypename(typename, name_length)) {
        return _Parser_BuiltInTypeSize(typename, name_length);
    }

    // User defined types
    for (uint16_t i = 0; i != parser->type_count; i++) {
        ubccustomtype_t* type = parser->defined_types + i;
        int32_t length        = strlen(type->name);
        if (length != name_length) continue;
        if (strncmp(type->name, typename, name_length) == 0) {
            return type->type_size;
        }
    }

    // foreign types
    for (uint16_t i = 0; i != parser->config.type_count; i++) {
        ubccustomtype_t* type = parser->config.foreign_types + i;
        int32_t length        = strlen(type->name);
        if (length != name_length) continue;
        if (strncmp(type->name, typename, name_length) == 0) {
            return type->type_size;
        }
    }

    return 0;
}

// This function copies the specified type and does not keep a reference
int _Parser_RegisterCustomType(ubcparser_t* parser, ubccustomtype_t new_type)
{
    if (parser->type_count == 0) {
        parser->defined_types = _Parser_Malloc(parser, sizeof(ubccustomtype_t) * 1);
        if (parser->defined_types == NULL) {
            return ENOMEM;
        }
    } else {
        size_t old_size = sizeof(ubccustomtype_t) * parser->type_count;
        size_t new_size = old_size + sizeof(ubccustomtype_t);
        ubccustomtype_t* new_array = _Parser_Realloc(parser, parser->defined_types, new_size, old_size);
        if (new_array == NULL) {
            return ENOMEM;
        }
        parser->defined_types = new_array;
    }

    parser->defined_types[parser->type_count] = new_type;
    parser->type_count++;

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

/// Parsing Functions


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

int _Parser_ParsePersist(ubcparser_t* parser)
{
    //TODO: Implement
    return EXIT_FAILURE;
}

int _Parser_ParseAssignmentExpression(ubcparser_t* parser)
{
    
}

int _Parser_ParseTopLevelStatement(ubcparser_t* parser)
{
    _Parser_AssumeLookaheadFill(parser);
    token_t lookahead_token;
    if (_Parser_LookAhead(parser, 0, &lookahead_token)) {
        return EXIT_FAILURE;
    }

    if (lookahead_token.type == TT_UBC_VAR) {
        return _Parser_ParseVariableDefinition(parser);
    }
    else if (lookahead_token.type == TT_UBC_PERSIST) {
        return _Parser_ParsePersist(parser);
    }
    else if (lookahead_token.type == TT_UBC_TYPE) {
        return _Parser_ParseTypeDefinition(parser);
    } 
    else if (lookahead_token.type == TT_UBC_FUNCTION) {
        return _Parser_ParseFunctionDefinition(parser);
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
    destination->bytecode_buffer.array    = NULL;
    destination->bytecode_buffer.used     = 0;
    
    destination->lookahead.available = 0;

    destination->defined_types = NULL;
    destination->type_count    = 0;

    return 0;
}

// This function does not use the error callback
int Parser_Destroy(ubcparser_t* parser)
{
    if (parser == NULL) {
        return EINVAL;
    }

    if (parser->bytecode_buffer.array != NULL)
    DynamicBuffer_Destroy(&(parser->bytecode_buffer));

    if (parser->lexer_stack.lexers != NULL)
    _Parser_Free(parser, parser->lexer_stack.lexers, parser->lexer_stack.stack_size * sizeof(lexer_t));

    if (parser->defined_types != NULL) {
        for (uint16_t i = 0; i != parser->type_count; i++) {
            _Parser_DestroyCustomType(parser, parser->defined_types + i);
        }
        _Parser_Free(parser, parser->defined_types, sizeof(ubccustomtype_t) * parser->type_count);
    }

    return EXIT_SUCCESS;
}
