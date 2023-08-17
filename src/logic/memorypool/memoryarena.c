#include "memoryarena.h"

uintptr_t MemoryArena_DefaultSizeForObjectSize(uintptr_t object_size)
{
    // Try to calculate an arena size for storing a lot of objects (128+) and close to multiples of 4096 (Page Size?)
    uintptr_t arena_memory = 128 * object_size;
    uintptr_t arena_size   = 128;
    arena_size += (4096 - (arena_memory % 4096)) / object_size;

    return arena_size;
}

int MemoryArena_Create(memory_arena_t* destination, uintptr_t object_size, uintptr_t arena_size)
{
    if (destination == NULL) {
        return EDESTADDRREQ;
    }
    if (object_size == 0) {
        return EINVAL;
    }
    if (arena_size == 0) {
        arena_size = MemoryArena_DefaultSizeForObjectSize(object_size);
    }

    char* arena_memory = malloc(arena_size * object_size);
    if (arena_memory == NULL) {
        return ENOMEM;
    }
    uintptr_t bytes = (arena_size / 8) + (arena_size % 8 > 1 ? 1 : 0);
    uint8_t* indicator_bits = calloc(bytes, 1); // All unused at the start
    if (indicator_bits == NULL) {
        free(arena_memory);
        return ENOMEM;
    }

    destination->memory      = arena_memory;
    destination->used        = indicator_bits;
    destination->arena_size  = arena_size;
    destination->object_size = object_size;

    return EXIT_SUCCESS;
}

int MemoryArena_Destroy(memory_arena_t* arena)
{
    if (arena == NULL) return EINVAL;

    free(arena->memory);
    free(arena->used);
    arena->memory = NULL;
    arena->used   = NULL;
    arena->arena_size  = 0;
    arena->object_size = 0;

    return EXIT_SUCCESS;
}

uintptr_t MemoryArena_FirstZeroBit(uint8_t byte)
{
    for (uintptr_t bit_index = 0; bit_index < 8; bit_index++) {
        if ((~byte) & (0x80 >> bit_index)) {
            return bit_index;
        }
    }

    return 8;
}

int MemoryArena_NextFreeIndex(uint8_t* memory, uintptr_t bit_size, uintptr_t* index_destination)
{
    if (memory == NULL) {
        return EINVAL;
    }
    if (index_destination == NULL) {
        return EDESTADDRREQ;
    }

    int exit_code = EXIT_FAILURE;
    uintptr_t byte_size = (bit_size / 8) + (bit_size % 8 > 1 ? 1 : 0);
    uintptr_t free_index;
    for (uintptr_t byte_index = 0; byte_index < byte_size; byte_index++) {
        uint8_t byte = memory[byte_index];
        free_index = MemoryArena_FirstZeroBit(byte);

        if (free_index != 8) {
            exit_code = EXIT_SUCCESS;
            index_destination[0] = byte_index * 8 + free_index;
            break;
        }
    }

    return exit_code;
}

int MemoryArena_Allocate(memory_arena_t* arena, void** pointer_destination)
{
    if (arena == NULL) {
        return EINVAL;
    }
    if (pointer_destination == NULL) {
        return EDESTADDRREQ;
    }

    uintptr_t free_index;
    if (MemoryArena_NextFreeIndex(arena->used, arena->arena_size, &free_index)) {
        return ENOMEM;
    }

    // Set "return" value
    pointer_destination[0] = arena->memory + (free_index * arena->object_size);
    
    // Mark place as used
    uintptr_t byte_index =  free_index / 8;
    uintptr_t bit_index  = free_index % 8;
    arena->used[byte_index] |= 0x80 >> bit_index;

    return EXIT_SUCCESS;
}

int MemoryArena_Free(memory_arena_t *arena, void *address)
{
    if (arena == NULL) {
        return EINVAL;
    }
    if (address == NULL) {
        return EINVAL;
    }

    char* min_address = arena->memory;
    char* max_address = arena->memory + (arena->arena_size * arena->object_size);
    if ((char*) address < min_address || (char*) address > max_address) {
        return ENXIO;
    }

    // Address exists
    
    // Where is it in the memory space
    uintptr_t address_offset = (uintptr_t) (address) - (uintptr_t) (min_address);
    uintptr_t address_index  =  address_offset / arena->object_size;

    // where is the bit
    uintptr_t byte_index = address_index / 8;
    uintptr_t bit_index  = address_index % 8;

    if (arena->used[byte_index] & (0x80 >> bit_index)) {
        // Was used, mark as free
        arena->used[byte_index] ^= 0x80 >> bit_index;
    } else {
        // Is not used, can't free
        return EBUSY;
    }

    return EXIT_SUCCESS;
}
