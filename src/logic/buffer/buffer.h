#ifndef BUFFER_H
#define BUFFER_H

#include <stdlib.h>
#include <stdio.h>

typedef struct
{
    char* data;
    char* ptr;
    size_t totalSize;
} buffer_t;

buffer_t* Buffer_Create(size_t size);
void      Buffer_Write(buffer_t* buffer, void* data, size_t len);
void      Buffer_Reset(buffer_t* buffer);

size_t    Buffer_GetSize(buffer_t* buffer);
void      Buffer_Delete(buffer_t* buffer);

#endif