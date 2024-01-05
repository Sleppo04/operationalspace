#include <parser.h>

#include <assert.h>
#include <stdio.h>

int error_report(void* data, const char* filename, int line, const char* message, enum UbcParserErrorType type)
{
    // There should be no errors reported
    printf("%s in file %s:%d\n", message, filename, line);
    assert(false);
}

int main()
{
    ubcparser_t parser;
    ubcparserconfig_t config;

    ubcfile_t files[] = {{"main.ubc", "include \"second.ubc\";"}, {"second.ubc", ""}};

    ParserConfig_Init(&config);
    config.file_count = 2;
    config.files = files;
    config.error_report = error_report;

    int create_code = Parser_Create(&parser, &config);
    assert(create_code == EXIT_SUCCESS);

    int parse_code = Parser_Parse(&parser, "main.ubc");
    assert(parse_code == EXIT_SUCCESS);

    Parser_Destroy(&parser);

    return 0;
}
