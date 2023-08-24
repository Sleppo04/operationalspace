#include "parser.h"

// This function does report an error to the user
int _Parser_ReportError(ubcparser_t* parser, const char* filename, int line, const char* message)
{
    parser->config.report_return = parser->config.error_report(parser->config.userdata, filename, line, message);

    return parser->config.report_return;
}

// This function does not report errors to the user
void* _Parser_Malloc(ubcparser_t* parser, size_t size)
{
    return malloc(size);
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

// This function does not report errors to the user
int _Parser_LookAhead(ubcparser_t* parser, uint32_t range, token_t* destination)
{
    if (destination == NULL) {
        return EDESTADDRREQ;
    }
    if (parser == NULL) {
        return EINVAL;
    }
    if (range > parser->lookahead.available || range < 1) {
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

    Lexer_NextToken(parser->lexer_stack.lexers + parser->lexer_stack.stack_size - 1,
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
    }
}

// This function will call ReportError to report errors
int Parser_Parse(ubcparser_t* parser, char* filename, void** bytecode_destination)
{
    if (bytecode_destination == NULL) {
        return EDESTADDRREQ;
    }

    int init_code = _Parser_AddParseFile(parser, filename);
    if (init_code) {
        return init_code;
    }

    init_code = _Parser_FillLookahead(parser);
    if (init_code) {
        return init_code;
    }


    return 0;
}

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
    free(parser->lexer_stack.lexers);

    return EXIT_SUCCESS;
}
