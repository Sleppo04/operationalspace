#include "parser.h"

int Parser_Parse(struct UbcParserConfig config, void** bytecode_destination)
{
    if (bytecode_destination == NULL) {
        return EDESTADDRREQ;
    }
    if (config.file_count == 0) {
        return EINVAL;
    }

    char* filename = config.files[0].fileName;
    lexer_t lexer;
    lexer.file = config.files[0].source;
    lexer.line = 1;
    lexer.pos  = 0;

    return 0;
}
