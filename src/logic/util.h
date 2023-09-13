#ifndef UTIL_H
#define UTIL_H

#include <stdlib.h>
#include <string.h>

char* Sys_Strdup(char* s)
{
    char*  dup;
    size_t len;

    len = strlen(s);
    if (len == 0) return NULL;
    dup = (char*) malloc(len + 1);
    if (dup == NULL) return NULL;
    memcpy(dup, s, len + 1);
    
    return dup;
}

char* strnchr(char* string, char character, size_t limit)
{
    for (size_t i = 0; i < limit; i++) {
        if (string[i] == character) {
            return string + i;
        }

        if (string[i] == '\0') {
            i = limit;
        }
    }

    return NULL;
}

#endif