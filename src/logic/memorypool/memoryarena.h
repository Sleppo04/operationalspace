#ifndef MEMORYARENA_H
#define MEMORYARENA_H

#include <errno.h>
#include <stdint.h>
#include <stdlib.h>

typedef struct MemoryArena {
    char* memory;
    uintptr_t arena_size;
    uintptr_t object_size;
    uint8_t* used;
} memory_arena_t;


int MemoryArena_Create(memory_arena_t *destination, uintptr_t object_size, uintptr_t arena_size);

int MemoryArena_Destroy(memory_arena_t *arena);

int MemoryArena_Allocate(memory_arena_t *arena, void **pointer_destination);

int MemoryArena_Free(memory_arena_t* arena, void* address);

uintptr_t MemoryArena_DefaultSizeForObjectSize(uintptr_t object_size);

#endif
