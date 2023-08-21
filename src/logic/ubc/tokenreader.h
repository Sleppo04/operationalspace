#ifndef TOKENREADER_H
#define TOKENREADER_H

#include <string.h>
#include "../lexer/lexer.h"

enum
{
    TT_UBC_WHILE    = 1000,
    TT_UBC_IF       = 1001,
    TT_UBC_ELIF     = 1002,
    TT_UBC_ELSE     = 1003,
    TT_UBC_BREAK    = 1004,
    TT_UBC_RETURN   = 1005,
    TT_UBC_FUNCTION = 1006,
    TT_UBC_VAR      = 1007,
    TT_UBC_TYPE     = 1008,
    TT_UBC_PERSIST  = 1009,
    TT_UBC_INCLUDE  = 1010,
    TT_UBC_FALSE    = 1011,
    TT_UBC_TRUE     = 1012
};

void Ubc_ReadNextToken(lexer_t* lexer, token_t* token);

#endif