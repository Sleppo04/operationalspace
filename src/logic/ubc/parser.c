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
    for (uint16_t lexer_index = parser->lexer_stack.stack_size - 2; lexer_index > -1; lexer_index++) {
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
int _Parser_AddParseFile(ubcparser_t* parser, char* filename)
{
    uint16_t file_index;
    for (file_index = 0; file_index < parser->config.file_count; file_index++) {
        if (strcmp(parser->config.files[file_index].fileName, filename)) {
            break;
        }
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
    if (range < 0) {
        return ERANGE;
    }

    destination[0] = parser->lookahead.tokens[range];

    return EXIT_SUCCESS;
}

// This function does not report errors to the user
int _Parser_NextLexToken(ubcparser_t* parser, token_t* destination)
{
    token_t token;

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
    while (parser->lookahead.available < 2) {
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



/// Parsing Functions


int _Parser_ParseInclude(ubcparser_t* parser)
{
    token_t include_token;
    int lookahead_code;
    lookahead_code = _Parser_LookAhead(parser, 0, &include_token);
    
    // No token available
    if (lookahead_code == ENOMEDIUM) {
        lexer_t* top_lexer = &(parser->lexer_stack.lexers[parser->lexer_stack.stack_size - 1]);
        _Parser_ReportError(parser, top_lexer->file, top_lexer->line, "Expected tokens to parse include statement", UBCPARSERERROR_ERRORMESSAGE);
        _Parser_ReportLexerTraceback(parser);
        return ECANCELED;
    }

    if (include_token.type != TT_UBC_INCLUDE) {
        lexer_t* top_lexer = &(parser->lexer_stack.lexers[parser->lexer_stack.stack_size - 1]);
        _Parser_ReportError(parser, top_lexer->file, top_lexer->line, "Expected Include keyword at start of include statement", UBCPARSERERROR_ERRORMESSAGE);
        _Parser_ReportLexerTraceback(parser);
        return ECANCELED;
    }

    // This is guaranteed to suceed because we were able to look ahead earlier
    _Parser_ConsumeToken(parser);
    token_t filename_literal;
    // TODO: Continue parsing this

}

int _Parser_ParseScript(ubcparser_t* parser)
{
    uint8_t state_include    = 0x01;
    uint8_t state_statements = 0x02;
    token_t token;
    int lookahead_code, parse_code;
    uint8_t* states = _Parser_Malloc(parser, sizeof(uint8_t));
    uint16_t state_count = parser->lexer_stack.stack_size;
    if (states == NULL) {
        return ENOMEM;
    }

    while (states != NULL) {

        if (states[state_count - 1] == state_include && token.type != TT_UBC_INCLUDE) {
            states[state_count - 1] = state_statements;
        }

        // File is in include state
        if (states[state_count - 1] == state_include) {
            parse_code = _Parser_ParseInclude(parser);
        }
        if (parse_code) {
            _Parser_Free(parser, states);
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

        if (states[state_count - 1] == state_statements) {
            parse_code = _Parser_ParseTopLevelStatement(parser);
        }
    }

    return EXIT_SUCCESS;
}

// This function will call ReportError to report errors
int Parser_Parse(ubcparser_t* parser, char* filename)
{
    int init_code = _Parser_AddParseFile(parser, filename);
    if (init_code) {
        return init_code;
    }

    init_code = _Parser_FillLookahead(parser);
    if (init_code) {
        return init_code;
    }

    int loop_code = _Parser_ParseScript(parser);
    if (loop_code) {
        return loop_code;
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
