#include "../../src/logic/ubc/parser.h"

#include <assert.h>
#include <stdio.h>

int error_report(void* data, const char* filename, int line, const char* message, enum UbcParserErrorType type)
{
    // There should be no errors reported
    printf("%s\nReported in file %s, line %d\n", message, filename, line);
    assert(false);
}

int main()
{
    ubcparser_t parser;
    ubcparserconfig_t config;

    ubcfile_t files[] = {"main.ubc", "type inner {int count;int length;} type outer {inner stuff;}"};

    config.foreign_functions = NULL;
    config.function_count = 0;
    config.file_count = 1;
    config.files = files;
    config.type_count = 0;
    config.foreign_types = NULL;
    config.userdata = NULL;
    config.error_report = error_report;
    config.free_function    = NULL;
    config.malloc_function  = NULL;
    config.realloc_function = NULL;

    int create_code = Parser_Create(&parser, &config);
    assert(create_code == EXIT_SUCCESS);

    int parse_code = Parser_Parse(&parser, "main.ubc");
    assert(parse_code == EXIT_SUCCESS);

    Parser_Destroy(&parser);
}