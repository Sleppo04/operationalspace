#include "buffer.h"

buffer_t* Buffer_Create(size_t size)
{
    buffer_t* buffer;
    
    buffer = (buffer_t*)malloc(sizeof(buffer_t));
    buffer->data = (char*)malloc(size);
    buffer->totalSize = size;
    buffer->ptr = buffer->data;
    return buffer;
}

void Buffer_Write(buffer_t* buffer, void* data, size_t len)
{
    char* bytes = (char*) data;
    if(len + Buffer_GetSize(buffer) > buffer->totalSize) {
        printf("ERROR: Buffer Overflow! Please resize! (If you can find out how)\n");
        return;
    }
    
    for(size_t i = 0; i < len; i++) {
        buffer->ptr[0] = bytes[i];
        buffer->ptr++;
    }
    return;
}

void Buffer_Reset(buffer_t* buffer)
{
    buffer->ptr = buffer->data;
    return;
}

size_t Buffer_GetSize(buffer_t* buffer)
{
    return buffer->ptr - buffer->data;
}

void Buffer_Delete(buffer_t* buffer)
{
    free(buffer->data);
    free(buffer);
    return;
}