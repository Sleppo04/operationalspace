#include "memorypool.h"

uintptr_t MemoryPool_DefaultArenaSizeForObjectSize(uintptr_t object_size)
{
    // Try to calculate an arena size for storing a lot of objects (128+) and close to multiples of 4096 (Page Size?)
    uintptr_t arena_memory = 128 * object_size;
    uintptr_t arena_size   = 128;
    arena_size += (4096 - (arena_memory % 4096)) / object_size;


    return arena_size;
}

int MemoryPool_Create(memory_pool_t *destination, uintptr_t object_size, uintptr_t arena_size)
{
    if (destination == NULL) {
        return EDESTADDRREQ;
    }
    if (object_size == 0) {
        return EINVAL;
    }
    if (arena_size == 0) {
        arena_size = MemoryPool_DefaultArenaSizeForObjectSize(object_size);
    }

    memory_arena_t* arena_array = malloc(sizeof(memory_arena_t));
    if (arena_array == NULL) {
        return ENOMEM;
    }

    if (MemoryArena_Create(arena_array, object_size, arena_size)) {
        free(arena_array);
        return ENOMEM;
    }

    destination->arenas      = arena_array;
    destination->arena_count = 1;
    destination->object_size = object_size;

    return EXIT_SUCCESS;
}

int MemoryPool_Destroy(memory_pool_t* pool)
{
    if (pool == NULL) {
        return EINVAL;
    }

    pool->object_size = 0;
    for (uint64_t arena_index = 0; arena_index < pool->arena_count; arena_index++) {
        MemoryArena_Destroy(pool->arenas + arena_index);
    }

    free(pool->arenas);

    return EXIT_SUCCESS;
}

int MemoryPool_Allocate(memory_pool_t* pool, void** pointer_destination)
{
    if (pool == NULL) {
        return EINVAL;
    }
    if (pointer_destination == NULL) {
        return EDESTADDRREQ;
    }

    int allocate_code    = ENOMEM;
    uint64_t arena_index = 0;
    while (allocate_code != EXIT_SUCCESS && arena_index < pool->arena_count) {
        allocate_code = MemoryArena_Allocate(pool->arenas + arena_index, pointer_destination);
        arena_index++;
    }

    // allocation was successful, our work is done here
    if (allocate_code == EXIT_SUCCESS) {
        return EXIT_SUCCESS;
    }

    // Allocation wasn't successful, we need more arenas
    memory_arena_t new_arena;
    int arena_code;
    arena_code = MemoryArena_Create(&new_arena, pool->object_size, MemoryPool_DefaultArenaSizeForObjectSize(pool->object_size));
    if (arena_code) {
        return arena_code;
    }

    // Resize the arena array
    size_t new_array_size           = sizeof(memory_arena_t) * (pool->arena_count + 1);
    memory_arena_t* new_arena_array = realloc(pool->arenas, new_array_size);
    if (new_arena_array == NULL) {
        MemoryArena_Destroy(&new_arena);
        return ENOMEM;
    }
    // Place arena in arena array
    pool->arenas = new_arena_array;
    pool->arenas[pool->arena_count] = new_arena;
    pool->arena_count++;


    memory_arena_t* new_arena_address = pool->arenas + pool->arena_count - 1;
    allocate_code = MemoryArena_Allocate(new_arena_address, pointer_destination);
    if (allocate_code) {
        // ENOMEM, but shouldn't happen at this point
        return allocate_code;
    }

    return EXIT_SUCCESS;
}

int MemoryPool_Free(memory_pool_t *pool, void *address)
{
    if (pool == NULL) {
        return EINVAL;
    }
    if (address == NULL) {
        return EINVAL;
    }

    int free_code;
    memory_arena_t* arena;
    for (uint64_t arena_index = 0; arena_index < pool->arena_count; arena_index++) {
        arena = pool->arenas + arena_index;
        free_code = MemoryArena_Free(arena, address);
        if (free_code == EXIT_SUCCESS) {
            return EXIT_SUCCESS;
        } else if (free_code == EBUSY) {
            return EBUSY;
        }   
    }

    return ENXIO;
}
