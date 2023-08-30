#ifndef UTIL_H
#define UTIL_H

#include <stdlib.h>
#include <string.h>

char* Sys_Strdup(char* s)
{
    char*  dup;
    size_t len;

    len = strlen(s);
    if (len == NULL) return NULL;
    dup = (char*) malloc(len + 1);
    if (dup == NULL) return NULL;
    memcpy(dup, s, len + 1);
    
    return dup;
}

#endif