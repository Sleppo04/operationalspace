#include "tokenreader.h"

void Ubc_ReadNextToken(lexer_t* lexer, token_t* token)
{
    Lexer_NextToken(lexer, token);

    if (token->type == TT_IDENTIFIER) {
        if (strncmp("while", token->ptr, token->value.length) == 0)    token->type = TT_UBC_WHILE;
        if (strncmp("if", token->ptr, token->value.length) == 0)       token->type = TT_UBC_IF;
        if (strncmp("elif", token->ptr, token->value.length) == 0)     token->type = TT_UBC_ELIF;
        if (strncmp("else", token->ptr, token->value.length) == 0)     token->type = TT_UBC_ELSE;
        if (strncmp("break", token->ptr, token->value.length) == 0)    token->type = TT_UBC_BREAK;
        if (strncmp("return", token->ptr, token->value.length) == 0)   token->type = TT_UBC_RETURN;
        if (strncmp("function", token->ptr, token->value.length) == 0) token->type = TT_UBC_FUNCTION;
        if (strncmp("var", token->ptr, token->value.length) == 0)      token->type = TT_UBC_VAR;
        if (strncmp("type", token->ptr, token->value.length) == 0)     token->type = TT_UBC_TYPE;
        if (strncmp("persist", token->ptr, token->value.length) == 0)  token->type = TT_UBC_PERSIST;
        if (strncmp("include", token->ptr, token->value.length) == 0)  token->type = TT_UBC_INCLUDE;
        if (strncmp("false", token->ptr, token->value.length) == 0)    token->type = TT_UBC_FALSE;
        if (strncmp("true", token->ptr, token->value.length) == 0)     token->type = TT_UBC_TRUE;
    }

    return;
}