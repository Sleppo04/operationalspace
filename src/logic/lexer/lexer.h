#ifndef LEXER_H
#define LEXER_H

#include <stdlib.h>
#include <stdint.h>
#include <ctype.h>
#include <stdbool.h>

typedef struct lexer_s
{
    char* pos;
    char* file;
    int   line;
} lexer_t;

typedef enum tokenType_e
{
    TT_EOF               = 0,
    TT_LEFT_BRACKET      = 1,  // "["
    TT_RIGHT_BRACKET     = 2,  // "]"
    TT_LEFT_PARENTHESIS  = 3,  // "("
    TT_RIGHT_PARENTHESIS = 4,  // ")"
    TT_LEFT_BRACE        = 5,  // "{"
    TT_RIGHT_BRACE       = 6,  // "}"
    TT_LESS_THAN         = 7,  // "<"
    TT_GREATER_THAN      = 8,  // ">"
    TT_COMMA             = 9,  // ","
    TT_SEMICOLON         = 10, // ";"
    TT_COLON             = 11, // ":"
    TT_PLUS              = 12, // "+"
    TT_MINUS             = 13, // "-"
    TT_ASTERISK          = 14, // "*"
    TT_SLASH             = 15, // "/"    
    TT_IDENTIFIER        = 16,
    TT_INT_LITERAL       = 17,
    TT_FLOAT_LITERAL     = 18,
    TT_STRING_LITERAL    = 19
} tokenType_t;

typedef union tokenValue_u
{
    float   floatValue;
    int32_t intValue;
    int32_t length;
} tokenValue_t;

typedef struct token_s
{
    tokenType_t  type;
    tokenValue_t value;
    char* ptr;
    int   line;
} token_t;

void Lexer_NextToken(lexer_t* lexer, token_t* token);

#endif