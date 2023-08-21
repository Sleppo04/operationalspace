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
    TT_EQUALS            = 9,  // "="
    TT_COMMA             = 10, // ","
    TT_SEMICOLON         = 11, // ";"
    TT_COLON             = 12, // ":"
    TT_PLUS              = 13, // "+"
    TT_MINUS             = 14, // "-"
    TT_ASTERISK          = 15, // "*"
    TT_SLASH             = 16, // "/" 
    TT_DOT               = 17, // "."   
    TT_IDENTIFIER        = 18,
    TT_INT_LITERAL       = 19,
    TT_FLOAT_LITERAL     = 10,
    TT_STRING_LITERAL    = 21
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