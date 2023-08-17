#ifndef MEMORYPOOL_H
#define MEMORYPOOL_H

#include <errno.h>
#include <stdint.h>
#include <stdlib.h>

#include "memoryarena.h"

typedef struct MemoryPool {
    uint64_t        arena_count;
    uintptr_t       object_size;
    uintptr_t       max_pool_capacity;
    uintptr_t       used_pool_capacity;
    memory_arena_t* arenas;
} memory_pool_t;

/// @brief Create a memory-pool at destination for objects of size object-size
/// @param destination where the memory pool will be placed in memory
/// @param object_size The size of the objects placed in the pool
/// @param arena_size Size of the memory arenas, impacts minimum memory usage and malloc frequency, leave zero for a default value
/// @param max_pool_capacity Maximum byte count the user will be able to receive from the pool, zero for unlimited
/// @return EDESTADDRREQ, EINVAL, ENOMEM
int MemoryPool_Create(memory_pool_t* destination, uintptr_t object_size, uintptr_t arena_size, uintptr_t max_pool_capacity);

/// @brief Destroy the passed memory pool and free all resourced associated with it
/// @param pool pool
/// @return EINVAL, EXIT_SUCCESS
int MemoryPool_Destroy(memory_pool_t* pool);

/// @brief Get a pointer to an allocated memory address from the fixed-size pool
/// @param pool pool to allocate on
/// @param pointer_destination where the pool writes the pointer you can use
/// @return EINVAL, EDESTADDRREQ, ENOMEM
int MemoryPool_Allocate(memory_pool_t* pool, void** pointer_destination);

/// @brief Mark the address of memory pool as free
/// @param pool pool
/// @param address address that the pool regains control over
/** @return
 *  | code   | description |
 *  | ------ | ----------- |
 *  | EINVAL | pool is NULL |
 *  | EINVAL | address is NULL |
 *  | ENXIO  | The provided address is not contained in this memory pool |
 *  | EBUSY  | The provided address is not in use and cannot be freed |
 *  | 0      | Success |
 **/
int MemoryPool_Free(memory_pool_t* pool, void* address);

/// @brief Allocate a continous array from the memory pool
/// @param pool Memory provider
/// @param array_length how many object-sizes the array needs successive in memory
/// @param pointer_destination where the pool will write the address of the array
/// @return EINVAL, EDESTADDRREQ, ENOMEM, EXIT_SUCCESS
int MemoryPool_AllocateArray(memory_pool_t* pool, uintptr_t array_length, void** pointer_destination);

/// @brief Free a continous array from the memory pool
/// @param pool Memory provider
/// @param address address the array starts at
/// @param array_length how many objects the array spans
/** @return
 * | code   | description |
 * | ------ | ----------- |
 * | EINVAL | Pool is NULL |
 * | EINVAL | Address is NULL |
 * | ENXIO  | The Address is not contained in this memory-pool |
 * | ENOENT | The Address was invalid for an array of this size |
 * | ENOENT | The Address passed was not the start of the array |
 * | EBUSY  | One of the array members was not in use, can't free the array |
 * | 0      | Success |
 **/
int MemoryPool_FreeArray(memory_pool_t* pool, void* address, uintptr_t array_length);

#endif //MEMORYPOOL