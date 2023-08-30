#ifndef TOKENREADER_H
#define TOKENREADER_H

#include <string.h>
#include "../lexer/lexer.h"

#define TT_UBC_WHILE    1000
#define TT_UBC_IF       1001
#define TT_UBC_ELIF     1002
#define TT_UBC_ELSE     1003
#define TT_UBC_BREAK    1004
#define TT_UBC_RETURN   1005
#define TT_UBC_FUNCTION 1006
#define TT_UBC_VAR      1007
#define TT_UBC_TYPE     1008
#define TT_UBC_PERSIST  1009
#define TT_UBC_INCLUDE  1010
#define TT_UBC_FALSE    1011
#define TT_UBC_TRUE     1012
#define TT_UBC_INT      1013
#define TT_UBC_FLOAT    1014
#define TT_UBC_STRING   1015
#define TT_UBC_BOOL     1016
#define TT_UBC_DISCARD  1017

void Ubc_ReadNextToken(lexer_t* lexer, token_t* token);

#endif