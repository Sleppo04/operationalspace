#include "parser.h"

// This function does report an error to the user
int _Parser_ReportError(ubcparser_t* parser, const char* filename, int line, const char* message, enum UbcParserErrorType type)
{
    parser->config.report_return = parser->config.error_report(parser->config.userdata, filename, line, message, type);

    return parser->config.report_return;
}

int _Parser_ReportLexerTraceback(ubcparser_t* parser)
{
    lexer_t top_lexer = parser->lexer_stack.lexers[parser->lexer_stack.stack_size - 1];
    _Parser_ReportError(parser, top_lexer.file, top_lexer.line, "Error occured in file", UBCPARSERERROR_TRACEBACK);

    // Skip first file
    for (uint16_t lexer_index = parser->lexer_stack.stack_size - 2; lexer_index + 1 != 0 ; lexer_index--) {
        lexer_t* lexer = parser->lexer_stack.lexers + lexer_index;
        _Parser_ReportError(parser, lexer->file, lexer->line, "File was included in", UBCPARSERERROR_TRACEBACK);
    }
}


// This function does not report errors to the user
void* _Parser_Malloc(ubcparser_t* parser, size_t size)
{
    return malloc(size);
}

void _Parser_Free(ubcparser_t* parser, void* address)
{
    free(address);
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
        return ENOMEDIUM;
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

int _Parser_ConsumeToken(ubcparser_t* parser)
{
    if (parser->lookahead.available < 1) {
        return EXIT_FAILURE;
    }

    parser->lookahead.tokens[0] = parser->lookahead.tokens[1];
    parser->lookahead.available--;

    _Parser_FillLookahead(parser);
    // TODO: Think about the Lookahead return here

    return EXIT_SUCCESS;
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

/// Parsing Functions


int _Parser_ParseInclude(ubcparser_t* parser)
{
    token_t include_token;
    int lookahead_code;
    _Parser_AssumeLookaheadFill(parser);
    lookahead_code = _Parser_LookAhead(parser, 0, &include_token);
    
    // No token available
    if (lookahead_code == ENOMEDIUM) {
        _Parser_ReportTopTracebackError(parser, "Expected token");
        return ECANCELED;
    }

    if (include_token.type != TT_UBC_INCLUDE) {
        _Parser_ReportTopTracebackError(parser, "Expected Include keyword at start of include statement");
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
        _Parser_ReportTopTracebackError(parser, "Unable to get token, expected STRING_LITERAL");
        return ECANCELED;
    }
    if (filename_token.type != TT_STRING_LITERAL) {
        _Parser_ReportTopTracebackError(parser, "Expected String literal token at position 2 of include statement");
        return ECANCELED;
    }
    _Parser_ConsumeToken(parser);

    token_t semicolon_token;
    lookahead_code = _Parser_LookAhead(parser, 0, &semicolon_token);
    _Parser_AssumeLookaheadFill(parser);

    if (lookahead_code) {
        _Parser_ReportTopTracebackError(parser, "Unable to get next token, expected TOKEN_SEMICOLON");
        return ECANCELED;
    }
    if (semicolon_token.type != TT_SEMICOLON) {
        _Parser_ReportTopTracebackError(parser, "Expected semicolon token at position 3 of include statement");
        return ECANCELED;
    }
    _Parser_ConsumeToken(parser);

    // All tokens were extracted successfully

    int add_code = _Parser_AddParseFile(parser, filename_token.ptr, filename_token.value.length);
    if (add_code) {
        _Parser_ReportTopTracebackError(parser, "Unable to find file referenced in include statement");
        return ECANCELED;
    }
    
    return EXIT_SUCCESS;
}

int _Parser_ParseTopLevelStatement(ubcparser_t* parser)
{
    token_t lookahead;
    int lookahead_code = _Parser_LookAhead(parser, 0, &lookahead);
    if (lookahead_code) {
        _Parser_ReportTopTracebackError(parser, "Expected token when parsing Top Level Statement");
        return EXIT_FAILURE;
    }

    switch (lookahead.type)
    {
    case TT_UBC_VAR:
        // TODO: Implement this
        break;
    
    case TT_UBC_PERSIST:
        // TODO: Implement this
        break;

    case TT_
    
    default:
        break;
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
            // We need not bother with reallocation here because why
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
            _Parser_Free(parser, states);
            return EXIT_FAILURE;
        }


        // File is in include state
        if (states[state_count - 1] == state_include) {
            parse_code = _Parser_ParseInclude(parser);
        }
        if (parse_code) {
            _Parser_Free(parser, states);
            return EXIT_FAILURE;
        } else {
            // Parsing was successful

            // Expand state array for new files
            uint8_t* new_states = _Parser_Malloc(parser, sizeof(uint8_t) * parser->lexer_stack.stack_size);
            if (new_states == NULL) {
                _Parser_Free(parser, states);
                return ENOMEM;
            }

            // Transfer old state data
            memcpy(new_states, states, state_count * sizeof(uint8_t));
            state_count = parser->lexer_stack.stack_size;
            new_states[state_count - 1] = state_include;
            
            // Free old state array
            _Parser_Free(parser, states);
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
    _Parser_Free(parser, parser->lexer_stack.lexers);

    return EXIT_SUCCESS;
}
