#include "../../src/logic/ubc/parser.h"

#include <assert.h>
#include <stdio.h>

int error_report(void* data, const char* filename, int line, const char* message, enum UbcParserErrorType type)
{
    // There should be no errors reported
    printf("%s\nReported in file %s, line %d\n", message, filename, line);
    assert(false);

    return EXIT_SUCCESS;
}

int main()
{
    ubcparser_t parser;
    ubcparserconfig_t config;

    ubcfile_t files[] = {"main.ubc", "3 + 3 - 3 / 3 * 3"};

    ParserConfig_Init(&config);
    config.file_count = 1;
    config.files = files;
    config.error_report = error_report;


    int create_code = Parser_Create(&parser, &config);
    assert(create_code == EXIT_SUCCESS);

    int parse_code = Parser_Parse(&parser, "main.ubc");
    assert(parse_code == EXIT_SUCCESS);

    assert(((ubcscope_t*) (parser.scopes.memory))->temporary_bytes == 0);
    // Nothing may be left on the stack

    Parser_Destroy(&parser);
}
