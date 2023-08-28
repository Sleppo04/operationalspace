#include "../../src/logic/ubc/parser.c"
#include "../../src/logic/lexer/lexer.c"
#include "../../src/logic/ubc/tokenreader.c"
#include "../../src/logic/dynamicbuffer/dynamicbuffer.c"

#include <assert.h>

int error_report()
{
    // There should be no errors reported
    assert(false);
}

int main()
{
    ubcparser_t parser;
    ubcparserconfig_t config;

    ubcfile_t files[] = {"main.ubc", ""};

    config.foreign_functions = NULL;
    config.function_count = 0;
    config.file_count = 1;
    config.files = files;
    config.type_count = 0;
    config.foreign_types = NULL;
    config.userdata = NULL;
    config.error_report = error_report;

    int create_code = Parser_Create(&parser, &config);
    assert(create_code == EXIT_SUCCESS);

    int parse_code = Parser_Parse(&parser, "main.ubc");
    assert(parse_code == EXIT_SUCCESS);

    Parser_Destroy(&parser);

    return 0;
}