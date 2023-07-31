#include "dynamicbuffer.h"

int DynamicBuffer_Create(dynamic_buffer_t *destination, size_t initial_capacity)
{
    if (destination == NULL) {
        return EDESTADDRREQ;
    }
    // Some malloc() implementations return NULL and other return valid pointers on zero-byte-allocation, I'm not handling this shit
    if (initial_capacity == 0) {
        return EINVAL;
    }

    dynamic_buffer_t local;
    local.array = malloc(initial_capacity);
    if (local.array == NULL) {
        return ENOMEM;
    }

    local.capacity = initial_capacity;
    local.used     = 0;

    return EXIT_SUCCESS;
}

int DynamicBuffer_EnsureUnusedCapacity(dynamic_buffer_t* buffer, size_t needed_unused)
{
    if (buffer == NULL) {
        return EINVAL;
    }

    size_t unused_space = buffer->capacity - buffer->used;

    if (needed_unused > unused_space) {
        return EXIT_SUCCESS;
    }

    size_t needed_capacity = buffer->capacity + needed_unused;
    int resize_code = DynamicBuffer_Resize(buffer, needed_capacity);

    if (resize_code) {
        // ENOMEM
        return resize_code;
    }

    return EXIT_SUCCESS;
}

int DynamicBuffer_EnsureCapacity(dynamic_buffer_t* buffer, size_t minimal_capacity)
{
    if (buffer == NULL) {
        return EINVAL;
    }

    if (minimal_capacity < buffer->capacity) {
        return EXIT_SUCCESS;
    }

    int resize_code = DynamicBuffer_Resize(buffer, minimal_capacity);
    if (resize_code) {
        // ENOMEM
        return resize_code;
    }

    return EXIT_SUCCESS;
}

int DynamicBuffer_Resize(dynamic_buffer_t* buffer, size_t new_capacity)
{
    if (buffer == NULL) {
        return EINVAL;
    }
    if (new_capacity == 0) {
        return EINVAL;
    }

    char* new_array = realloc(buffer, new_capacity);
    if (new_array == NULL) {
        return ENOMEM;
    }

    buffer->array = new_array;

    return EXIT_SUCCESS;
}

int DynamicBuffer_Reset(dynamic_buffer_t* buffer)
{
    if (buffer == NULL) {
        return EINVAL;
    }

    buffer->used = 0;

    return EXIT_SUCCESS;
}

int DynamicBuffer_Store(dynamic_buffer_t* buffer, void* data, size_t data_size)
{
    if (buffer == NULL) {
        return EINVAL;
    }
    if (data == NULL) {
        return EINVAL;
    }
    if (data_size == 0) {
        return EXIT_SUCCESS;
    }

    int ensure_code = DynamicBuffer_EnsureUnusedCapacity(buffer, data_size);
    if (ensure_code) {
        // ENOMEM
        return ensure_code;
    }

    memcpy(buffer->array, data, data_size);

    return EXIT_SUCCESS;
}