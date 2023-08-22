#ifndef UBCPARSER_H
#define UBCPARSER_H

#include <stdint.h>
#include <stdlib.h>
#include <errno.h>

#include "lexer.h"

typedef int (*UBCErrorReportFunction) (void* userdata, const char* filename, int line, const char* message);

typedef struct UbcParserConfig {
    void* userdata;
    UBCErrorReportFunction error_report;
    ubcfile_t* files;
    uint16_t file_count;
} ubcparserconfig_t;

typedef struct UbcParser {
    ubcparserconfig_t config;
} ubcparser_t;

typedef struct UbcFile {
    char* fileName;
    char* source;
} ubcfile_t;

int Parser_Parse(struct UbcParserConfig config, void** bytecode_destination);

#endif //UBCPARSER_H