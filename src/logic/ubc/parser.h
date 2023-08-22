#ifndef UBCPARSER_H
#define UBCPARSER_H

typedef int (*UBCFileRequestFunction) (void* userdata, char* filename, char** destination);
typedef int (*UBCErrorReportFunction) (void* userdata, const char* filename, int line, const char* message);

typedef struct UbcParserConfig {
    void* userdata;
    UBCFileRequestFunction file_request;
    UBCErrorReportFunction error_report;
} ubcparserconfig_t;

typedef struct UbcParser {
    ubcparserconfig_t config;
} ubcparser_t;

typedef struct UbcFile {
    char* fileName;
    char* source;
} ubcfile_t;

#endif //UBCPARSER_H