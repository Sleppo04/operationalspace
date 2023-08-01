#ifndef DYNAMICBUFFER_H
#define DYNAMICBUFFER_H

#include <errno.h>
#include <stdlib.h>
#include <string.h>

typedef struct DynamicBuffer {
    void*  array;
    size_t capacity;
    size_t used;
} dynamic_buffer_t;

/// @brief Create a new Dynamic Buffer at destination with initialCapacity initialCapacity
/// @param destination where the buffer will be stored
/// @param initialCapacity what it's initialCapacity will be
/// @return EINVAL, EINVAL, ENOMEM, EXIT_SUCCESS
int DynamicBuffer_Create(dynamic_buffer_t* destination, size_t initialCapacity);

/// @brief This function checks that there are at least needed_unused free bytes in the allocated area
/// @param buffer buffer to check
/// @param needed_unused required free array size
/// @return EINVAL, ENOMEM, EXIT_SUCCESS
int DynamicBuffer_EnsureUnusedCapacity(dynamic_buffer_t *buffer, size_t needed_unused);

/// @brief This function resizes the buffer to minimal_capacity, if necessary
/// @param buffer buffer to check
/// @param minimal_capacity minimal capacity of the buffer array
/// @return EINVAL, ENOMEM, EXIT_SUCCESS
int DynamicBuffer_EnsureCapacity(dynamic_buffer_t *buffer, size_t minimal_capacity);

/// @brief This function resizes the buffers array via realloc to new_capacity
/// @param buffer buffer to resize
/// @param new_capacity capacity of the new buffer array
/// @return EINVAL, ENOMEM, EXIT_SUCCESS
int DynamicBuffer_Resize(dynamic_buffer_t *buffer, size_t new_capacity);

/// @brief Resets the count of used bytes to "clear" the buffer
/// @param buffer buffer to reset
/// @return EINVAL, EXIT_SUCCESS
int DynamicBuffer_Reset(dynamic_buffer_t *buffer);

/// @brief Stores data[.data_size] at the end of the buffer array and resizes if necessary
/// @param buffer buffer in which the data will be stored
/// @param data data to store
/// @param data_size how many bytes will be copied from data
/// @return EINVAL, EINVAL, ENOMEM, EXIT_SUCCESS
int DynamicBuffer_Store(dynamic_buffer_t *buffer, void *data, size_t data_size);

/// @brief Destroys the dynamic buffer and releases all resources held, the struct will not hold anything it did before
/// @param buffer buffer that shall be destroyed
/// @return EINVAL, EXIT_SUCCESS
int DynamicBuffer_Destroy(dynamic_buffer_t *buffer);

#endif