#include "../../src/logic/ubc/parser.h"

#include <assert.h>
#include <stdio.h>

int error_report(void* data, const char* filename, int line, const char* message, enum UbcParserErrorType type)
{
    // This is expected to be called with an error about unkown variables
    assert(strcmp(filename, "main.ubc") == 0);
    assert(line == 1);
    assert(type == UBCPARSERERROR_PARSERERROR);
    assert(strcmp(message, "Reference to unknown variable \"name\"") == 0);
    exit(0);
}

int main()
{
    ubcparser_t parser;
    ubcparserconfig_t config;

    ubcfile_t files[] = {"main.ubc", "name"};

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
