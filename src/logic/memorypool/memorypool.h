#ifndef MEMORYPOOL
#define MEMORYPOOL

#include <errno.h>
#include <stdint.h>
#include <stdlib.h>

#include "memoryarena.h"

typedef struct MemoryPool {
    uint64_t        arena_count;
    uintptr_t       object_size;
    memory_arena_t* arenas;
} memory_pool_t;

/// @brief The Size the Memory Pool would pick by default for objects of the given size
/// @param object_size 
/// @return The arena size, at least 128 objects and aligned to 4096
uintptr_t MemoryPool_DefaultArenaSizeForObjectSize(uintptr_t object_size);

/// @brief Create a memory-pool at destination for objects of size object-size
/// @param destination where the memory pool will be placed in memory
/// @param object_size The size of the objects placed in the pool
/// @param arena_size Size of the memory arenas, impacts minimum memory usage and malloc frequency, leave zero for a default value
/// @return EDESTADDRREQ, EINVAL, ENOMEM
int MemoryPool_Create(memory_pool_t* destination, uintptr_t object_size, uintptr_t arena_size);

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
 *  | EBUSY  | The provided address is not in use and cannot be freed |
 *  | ENXIO  | The provided address is not contained in this memory pool |
 *  | 0      | Success |
 **/
int MemoryPool_Free(memory_pool_t* pool, void* address);

#endif //MEMORYPOOL