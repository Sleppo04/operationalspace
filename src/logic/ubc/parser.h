#ifndef UBCPARSER_H
#define UBCPARSER_H

#include <stdint.h>
#include <stdlib.h>
#include <errno.h>

#include "lexer.h"

typedef int (*UBCErrorReportFunction) (void* userdata, const char* filename, int line, const char* message);

typedef struct UbcFile {
    char* fileName;
    char* source;
} ubcfile_t;

typedef struct UbcForeignFunction {
	char*    identifier;
	uint16_t num_args;
	char**   argument_typenames;
	char*    return_typename;
	int (*callable) (void* vm, void* user_data);
}

typedef struct UbcParserConfig {
    void* userdata;
    UBCErrorReportFunction error_report;
    
    ubcfile_t* files;
    uint16_t   file_count;
    
    ubccustomtype_t* foreign_types;
    uint16_t         type_count;

    ubcforeignfunction_t* foreign_functions;
    uint16_t              function_count;
} ubcparserconfig_t;

typedef struct ParserStack {
    ubcfile_t* files;
    lexer_t*   lexers;
    uint16_t   stack_size;
} parserstack_t;

typedef struct UbcParser {
    struct ParserStack parser_stack;
    ubcparserconfig_t config;
} ubcparser_t;

int Parser_Parse(struct UbcParserConfig config, void** bytecode_destination);

#endif //UBCPARSER_H
