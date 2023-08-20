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
    TT_EOF,
    TT_LEFT_BRACKET,      // "["
    TT_RIGHT_BRACKET,     // "]"
    TT_LEFT_PARENTHESIS,  // "("
    TT_RIGHT_PARENTHESIS, // ")"
    TT_LEFT_BRACE,        // "{"
    TT_RIGHT_BRACE,       // "}"
    TT_LESS_THAN,         // "<"
    TT_GREATER_THAN,      // ">"
    TT_COMMA,             // ","
    TT_SEMICOLON,         // ";"
    TT_COLON,             // ":"
    TT_PLUS,              // "+"
    TT_MINUS,             // "-"
    TT_ASTERISK,          // "*"
    TT_SLASH,             // "/"    
    TT_IDENTIFIER,
    TT_INT_LITERAL,
    TT_FLOAT_LITERAL,
    TT_STRING_LITERAL
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