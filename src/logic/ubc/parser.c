#include "parser.h"

void* _Parser_Malloc(ubcparser_t* parser, size_t size)
{
    return malloc(size);
}

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

    parser->lexer_stack.lexers     = _Parser_Malloc(parser, sizeof(lexer_t) * parser->lexer_stack.stack_size);
    if (parser->lexer_stack.lexers == NULL) {
        return ENOMEM;
    }

    parser->lexer_stack.lexers[lexer_index].file = parser->config.files[file_index].fileName;
    parser->lexer_stack.lexers[lexer_index].pos  = parser->config.files[file_index].source;
    parser->lexer_stack.lexers[lexer_index].line = 1;

    return EXIT_SUCCESS;
}

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

int Parser_Parse(ubcparser_t* parser, char* filename, void** bytecode_destination)
{
    if (bytecode_destination == NULL) {
        return EDESTADDRREQ;
    }

    int init_code = _Parser_AddParseFile(parser, filename);
    if (init_code) {
        return init_code;
    }



    return 0;
}

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
    
    destination->bytecode_buffer->capacity = 0;
    destination->bytecode_buffer->array    = NULL;
    destination->bytecode_buffer->used     = 0;
    
    destination->lookahead.available = 0;

    return 0;
}
