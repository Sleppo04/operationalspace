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

#define TT_EOF               0
#define TT_LEFT_BRACKET      1  // "["
#define TT_RIGHT_BRACKET     2  // "]"
#define TT_LEFT_PARENTHESIS  3  // "("
#define TT_RIGHT_PARENTHESIS 4  // ")"
#define TT_LEFT_BRACE        5  // "{"
#define TT_RIGHT_BRACE       6  // "}"
#define TT_LESS_THAN         7  // "<"
#define TT_GREATER_THAN      8  // ">"
#define TT_EQUALS            9  // "="
#define TT_COMMA            10  // ","
#define TT_SEMICOLON        11  // ";"
#define TT_COLON            12  // ":"
#define TT_PLUS             13  // "+"
#define TT_MINUS            14  // "-"
#define TT_ASTERISK         15  // "*"
#define TT_SLASH            16  // "/" 
#define TT_DOT              17  // "."   
#define TT_IDENTIFIER       18
#define TT_INT_LITERAL      19
#define TT_FLOAT_LITERAL    10
#define TT_STRING_LITERAL   21


typedef union tokenValue_u
{
    float   floatValue;
    int32_t intValue;
    int32_t length;
} tokenValue_t;

typedef struct token_s
{
    int          type;
    tokenValue_t value;
    char* ptr;
    int   line;
} token_t;

void Lexer_NextToken(lexer_t* lexer, token_t* token);

#endif