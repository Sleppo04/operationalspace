#include <parser.h>

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

    ParserConfig_Init(&config);
    config.file_count = 1;
    config.files = files;
    config.error_report = error_report;

    int create_code = Parser_Create(&parser, &config);
    assert(create_code == EXIT_SUCCESS);

    int parse_code = Parser_Parse(&parser, "main.ubc");
    assert(parse_code == EXIT_SUCCESS);

    Parser_Destroy(&parser);

    return 0;
}
