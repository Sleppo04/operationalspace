#include "tokenreader.h"

#define MAPTOKEN(x,y) if (strncmp(x, token->ptr, token->value.length) == 0) token->type = y

void Ubc_ReadNextToken(lexer_t* lexer, token_t* token)
{
    Lexer_NextToken(lexer, token);

    if (token->type == TT_IDENTIFIER) {
        MAPTOKEN("while", TT_UBC_WHILE);
        MAPTOKEN("if", TT_UBC_IF);
        MAPTOKEN("elif", TT_UBC_ELIF);
        MAPTOKEN("else", TT_UBC_ELSE);
        MAPTOKEN("break", TT_UBC_BREAK);
        MAPTOKEN("return", TT_UBC_RETURN);
        MAPTOKEN("function", TT_UBC_FUNCTION);
        MAPTOKEN("var", TT_UBC_VAR);
        MAPTOKEN("type", TT_UBC_TYPE);
        MAPTOKEN("persist", TT_UBC_PERSIST);
        MAPTOKEN("include", TT_UBC_INCLUDE);
        MAPTOKEN("false", TT_UBC_FALSE);
        MAPTOKEN("true", TT_UBC_TRUE);
        MAPTOKEN(TT_UBC_INT_TYPENAME, TT_UBC_INT);
        MAPTOKEN(TT_UBC_BOOL_TYPENAME, TT_UBC_BOOL);
        MAPTOKEN(TT_UBC_FLOAT_TYPENAME, TT_UBC_FLOAT);
        MAPTOKEN(TT_UBC_STRING_TYPENAME, TT_UBC_STRING);
        MAPTOKEN("discard", TT_UBC_DISCARD);
    }

    return;
}