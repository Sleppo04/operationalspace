#ifndef UBCPARSER_H
#define UBCPARSER_H

#include <stdint.h>
#include <stdlib.h>

#include "ubc.h"
#include "types.h"
#include "tokenreader.h"

#include "../errorcodes.h"
#include "../lexer/lexer.h"
#include "../dynamicbuffer/dynamicbuffer.h"



enum UbcParserErrorType {
    UBCPARSERERROR_ERRORMESSAGE,
    UBCPARSERERROR_TRACEBACK,
};

typedef int   (*UBCErrorReportFunction)  (void* userdata, const char* filename, int line, const char* message, enum UbcParserErrorType type);
typedef void  (*UBCCustomFreeFunction)   (void* userdata, void* address, size_t size);
typedef void* (*UBCCustomMallocFunction) (void* userdata, size_t size);

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
} ubcforeignfunction_t;

typedef struct UbcParserConfig {
    void* userdata;
    UBCErrorReportFunction error_report;
    int report_return;
    
    ubcfile_t* files;
    uint16_t   file_count;
    
    ubccustomtype_t* foreign_types;
    uint16_t         type_count;

    ubcforeignfunction_t* foreign_functions;
    uint16_t              function_count;

    UBCCustomMallocFunction malloc_function;
    UBCCustomFreeFunction   free_function;
} ubcparserconfig_t;

typedef struct UbcLexerStack {
    uint16_t stack_size;
    lexer_t* lexers;
} ubclexerstack_t;

typedef struct UbcParserLookahead {
    uint32_t available;
    token_t  tokens[3];
} ubcparserlookahead_t;

typedef struct UbcParser {
    struct UbcLexerStack      lexer_stack;
    dynamic_buffer_t          bytecode_buffer;
    ubcparserconfig_t         config;
    struct UbcParserLookahead lookahead;
} ubcparser_t;

int Parser_Create(ubcparser_t* destination, ubcparserconfig_t* config);

int Parser_Parse(ubcparser_t* parser, char* filename);

#endif //UBCPARSER_H
