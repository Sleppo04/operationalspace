#include <stdio.h>
#include "../src/common/lexer/lexer.h"

int main(int argc, char** argv)
{
    token_t token;
    lexer_t lexer;
    char* str = "123   123.4 ident (keyword) \"Hiya~!\"";

    lexer.file = str;
    lexer.pos  = str;
    lexer.line = 1;

    while(token.type != TT_EOF) {
        Lexer_NextToken(&lexer, &token);
        printf("Token Type: %i\n", token.type);
        if (token.type > TT_LEFT_BRACKET && token.type < TT_DOT) printf("Token Value: %c\n\n", *token.ptr);
        if (token.type == TT_INT_LITERAL) printf("Token value: %i\n\n", token.value.intValue);
        if (token.type == TT_FLOAT_LITERAL) printf("Token value: %f\n\n", token.value.floatValue);
        if (token.type == TT_STRING_LITERAL || token.type == TT_IDENTIFIER) printf("Token value: \"%.*s\"\n\n", token.value.length, token.ptr);
    }

    return 0;
}
