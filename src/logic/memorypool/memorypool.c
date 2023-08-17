#include "memorypool.h"


uintptr_t MemoryPool_PoolCapacityLeft(memory_pool_t* pool)
{
    if (pool->max_pool_capacity == 0) {
        return 0;
    }

    return pool->max_pool_capacity - pool->used_pool_capacity;
} 


int MemoryPool_Create(memory_pool_t *destination, uintptr_t object_size, uintptr_t arena_size, uintptr_t max_pool_capacity)
{
    if (destination == NULL) {
        return EDESTADDRREQ;
    }
    if (object_size == 0) {
        return EINVAL;
    }
    if (arena_size == 0) {
        arena_size = MemoryArena_DefaultSize(object_size, max_pool_capacity);
    }

    memory_arena_t* arena_array = malloc(sizeof(memory_arena_t));
    if (arena_array == NULL) {
        return ENOMEM;
    }

    if (MemoryArena_Create(arena_array, object_size, arena_size)) {
        free(arena_array);
        return ENOMEM;
    }

    destination->arenas             = arena_array;
    destination->arena_count        = 1;
    destination->object_size        = object_size;
    destination->max_pool_capacity  = max_pool_capacity;
    destination->used_pool_capacity = MemoryArena_GetMemoryByteCount(arena_array);

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

int MemoryPool_AddArena(memory_pool_t* pool, uintptr_t needed_object_capacity)
{
    if (pool == NULL) {
        return EINVAL;
    }
    uintptr_t pool_capacity_left   = MemoryPool_PoolCapacityLeft(pool);
    uintptr_t object_capacity_left = pool_capacity_left / pool->object_size;
    if (pool->max_pool_capacity != 0 && needed_object_capacity > object_capacity_left) {
        return ENOMEM;
    }

    uintptr_t arena_object_capacity = MemoryArena_DefaultSize(pool->object_size, pool_capacity_left);
    if (needed_object_capacity < (arena_object_capacity)) {
        MemoryArena_AlignSize(needed_object_capacity, pool->object_size, pool_capacity_left);
    }
    

    memory_arena_t new_arena;
    int arena_code;
    arena_code = MemoryArena_Create(&new_arena, pool->object_size, arena_object_capacity);
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
    pool->used_pool_capacity += MemoryArena_GetMemoryByteCount(&new_arena);

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
    int arena_code = MemoryPool_AddArena(pool, 1);
    if (arena_code) {
        return arena_code;
    }


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
