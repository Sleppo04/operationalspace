#include <parser.h>

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

    ubcfile_t files[] = {"main.ubc", "type IntWrapper { int wrapped; }\n"
    								 "type IntWrapperWrapper { IntWrapper wrappedIntWrapper; }"};

    ParserConfig_Init(&config);
    config.file_count = 1;
    config.files = files;
    config.error_report = error_report;

    int create_code = Parser_Create(&parser, &config);
    assert(create_code == EXIT_SUCCESS);

    int parse_code = Parser_Parse(&parser, "main.ubc");
    assert(parse_code == EXIT_SUCCESS);

    assert(parser.types.count == 2);
    assert(parser.types.array[0].field_count == 1);
    assert(strcmp(parser.types.array[0].name, "IntWrapper") == 0);
    assert(strcmp(parser.types.array[0].field_names[0], "wrapped") == 0);
    assert(parser.types.array[0].type_size == UBC_INT_BYTE_SIZE);

    assert(parser.types.array[1].field_count == 1);
    assert(strcmp(parser.types.array[1].name, "IntWrapperWrapper") == 0);
    assert(strcmp(parser.types.array[1].field_names[0], "wrappedIntWrapper") == 0);
    assert(parser.types.array[1].type_size == UBC_INT_BYTE_SIZE);

    Parser_Destroy(&parser);
}
