#include "lexer.h"

void _TokenizeNumberLiteral(lexer_t* lexer, token_t* token)
{
    char* startPos = lexer->pos;
    bool isFloat = false;

    while (isdigit(lexer->pos[1]) || (lexer->pos[1] == '.' && isdigit(lexer->pos[2]))) {
        lexer->pos++;
        if (lexer->pos[1] == '.') isFloat = true;
    }

    if (isFloat) {
        token->value.floatValue = strtof(startPos, NULL);
        token->type = TT_FLOAT_LITERAL;
    }
    else {
        token->value.intValue = strtol(startPos, NULL, 10);
        token->type = TT_INT_LITERAL;
    }

    token->line = lexer->line;
    token->ptr = startPos;

    return;
}

void _TokenizeStringLiteral(lexer_t* lexer, token_t* token)
{
    char* startPos = lexer->pos;

    while (lexer->pos[1] != '"')
    {
        if (!isprint(lexer->pos[1])) {
            // TODO: Error Handling
            token->type = TT_EOF;
            token->line = lexer->line;
            token->ptr  = lexer->pos + 1;
            return;
        }
        lexer->pos++;
    }
    
    token->value.length = lexer->pos - startPos;
    token->type = TT_STRING_LITERAL;
    token->ptr = startPos + 1;
    token->line = lexer->line;
    
    lexer->pos++; // Set pos to closing quote
    return;
}

void _TokenizeIdentifier(lexer_t* lexer, token_t* token)
{
    char* startPos = lexer->pos;
    
    while (isalnum(lexer->pos[1]) || lexer->pos[1] == '_') {
        lexer->pos++;
    }

    token->type = TT_IDENTIFIER;
    token->value.length = lexer->pos - startPos + 1;
    token->ptr = startPos;
    token->line = lexer->line;

    return;
}

void Lexer_NextToken(lexer_t* lexer, token_t* token)
{
    // Single-character-tokens
         if (*lexer->pos == '[') *token = (token_t) { TT_LEFT_BRACKET, 0, lexer->pos, lexer->line };
    else if (*lexer->pos == ']') *token = (token_t) { TT_RIGHT_BRACKET, 0, lexer->pos, lexer->line };
    else if (*lexer->pos == '(') *token = (token_t) { TT_LEFT_PARENTHESIS, 0, lexer->pos, lexer->line };
    else if (*lexer->pos == ')') *token = (token_t) { TT_RIGHT_PARENTHESIS, 0, lexer->pos, lexer->line };
    else if (*lexer->pos == '{') *token = (token_t) { TT_LEFT_BRACE, 0, lexer->pos, lexer->line };
    else if (*lexer->pos == '}') *token = (token_t) { TT_RIGHT_BRACE, 0, lexer->pos, lexer->line };
    else if (*lexer->pos == '<') *token = (token_t) { TT_LESS_THAN, 0, lexer->pos, lexer->line };
    else if (*lexer->pos == '>') *token = (token_t) { TT_GREATER_THAN, 0, lexer->pos, lexer->line };
    else if (*lexer->pos == '=') *token = (token_t) { TT_EQUALS, 0, lexer->pos, lexer->line };
    else if (*lexer->pos == ',') *token = (token_t) { TT_COMMA, 0, lexer->pos, lexer->line };
    else if (*lexer->pos == ';') *token = (token_t) { TT_SEMICOLON, 0, lexer->pos, lexer->line };
    else if (*lexer->pos == ':') *token = (token_t) { TT_COLON, 0, lexer->pos, lexer->line };
    else if (*lexer->pos == '+') *token = (token_t) { TT_PLUS, 0, lexer->pos, lexer->line };
    else if (*lexer->pos == '-') *token = (token_t) { TT_MINUS, 0, lexer->pos, lexer->line };
    else if (*lexer->pos == '*') *token = (token_t) { TT_ASTERISK, 0, lexer->pos, lexer->line };
    else if (*lexer->pos == '/') *token = (token_t) { TT_SLASH, 0, lexer->pos, lexer->line };
    else if (*lexer->pos == '.') *token = (token_t) { TT_DOT, 0, lexer->pos, lexer->line };

    // Multi-character-tokens
    else if (isdigit(*lexer->pos)) _TokenizeNumberLiteral(lexer, token);
    else if (*lexer->pos == '"') _TokenizeStringLiteral(lexer, token);
    else if (isalpha(*lexer->pos) || *lexer->pos == '_') _TokenizeIdentifier(lexer, token);

    // Special tokens
    else if (!isprint(*lexer->pos)) {
        if (*lexer->pos == '\0') {
            token->type = TT_EOF;
            token->ptr = lexer->pos;
            token->line = lexer->line;
            return; // return so that we don't imcrement position
        }
        if (*lexer->pos == '\n') {
            lexer->line++;
        }
        // Unknown unprintable symbol or \n, skipping...
        lexer->pos++;
        Lexer_NextToken(lexer, token);
    }

    else if (isspace(*lexer->pos)) {
        lexer->pos++;
        Lexer_NextToken(lexer, token);
        return;
    }

    else {
        // TODO: Error Handling
        // Unknown printable symbol
        token->type = TT_EOF;
        token->line = lexer->line;
        token->ptr = lexer->pos;
        return; // return so that we don't increment position
    }
    
    lexer->pos++;
    return;
}