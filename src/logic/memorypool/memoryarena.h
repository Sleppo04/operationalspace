#ifndef MEMORYARENA_H
#define MEMORYARENA_H

#include <errno.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

typedef struct MemoryArena {
    char* memory;
    uintptr_t arena_size;
    uintptr_t used_count;
    uintptr_t object_size;
    uint8_t* used;
} memory_arena_t;


int MemoryArena_Create(memory_arena_t *destination, uintptr_t object_size, uintptr_t arena_size);

int MemoryArena_Destroy(memory_arena_t *arena);

int MemoryArena_Allocate(memory_arena_t *arena, void **pointer_destination);

int MemoryArena_Free(memory_arena_t* arena, void* address);

int MemoryArena_AllocateArray(memory_arena_t* arena, uintptr_t array_length, void** pointer_destination);

int MemoryArena_FreeArray(memory_arena_t* arena, void* array_start, uintptr_t array_length);

uintptr_t MemoryArena_GetMemoryByteCount(memory_arena_t* arena);

uintptr_t MemoryArena_DefaultSize(uintptr_t object_size, uintptr_t max_bytes);

uintptr_t MemoryArena_AlignSize(uintptr_t min_objects, uintptr_t object_size, uintptr_t max_bytes);

#endif
