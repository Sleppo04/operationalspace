#include "memoryarena.h"

uintptr_t MemoryArena_AlignSize(uintptr_t min_objects, uintptr_t object_size, uintptr_t max_bytes)
{
    uintptr_t arena_capacity = min_objects;
    uintptr_t arena_memory   = min_objects * object_size;
    arena_capacity += (4096 - (arena_memory % 4096)) / object_size;

    arena_memory = arena_capacity * object_size;
    if (max_bytes != 0 && arena_memory > max_bytes) {
        arena_capacity = max_bytes / object_size;
    }

    return arena_capacity;
}

uintptr_t MemoryArena_GetMemoryByteCount(memory_arena_t* arena)
{
    return arena->arena_size * arena->object_size;
}

uintptr_t MemoryArena_DefaultSize(uintptr_t object_size, uintptr_t max_bytes)
{
    // Try to calculate an arena size for storing a lot of objects (256+) and close to multiples of 4096 (Page Size?)
    uintptr_t arena_memory = 128 * object_size;
    uintptr_t arena_size   = 128;
    arena_size += (4096 - (arena_memory % 4096)) / object_size;
    
    uintptr_t arena_bytes = arena_size * object_size;
    if (max_bytes != 0 && arena_bytes > max_bytes) {
        arena_size = max_bytes / object_size;
    }

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
        return EINVAL;
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
    destination->used_count  = 0;

    return EXIT_SUCCESS;
}

int MemoryArena_Destroy(memory_arena_t* arena)
{
    if (arena == NULL) return EINVAL;
    if (arena->used_count != 0) {
	    // There are still allocated memory areas in this arena
	    return EBUSY;
    }

    free(arena->memory);
    free(arena->used);
    arena->memory = NULL;
    arena->used   = NULL;
    arena->arena_size  = 0;
    arena->object_size = 0;
    arena->used_count  = 0;

    return EXIT_SUCCESS;
}

int MemoryArena_ForceDestroy(memory_arena_t* arena)
{
    if (arena == NULL) {
        return EINVAL;
    }

    free(arena->used);
    free(arena->memory);
    arena->arena_size  = 0;
    arena->memory      = NULL;
    arena->object_size = 0;
    arena->used        = NULL;
    arena->used_count  = 0;

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

int MemoryArena_NextFreeIndex(uint8_t* memory, uintptr_t bit_count, uintptr_t* index_destination)
{
    if (memory == NULL) {
        return EINVAL;
    }
    if (index_destination == NULL) {
        return EDESTADDRREQ;
    }

    int exit_code = EXIT_FAILURE;
    uintptr_t byte_size = (bit_count / 8) + (bit_count % 8 > 1 ? 1 : 0);
    uintptr_t free_index;
    for (uintptr_t byte_index = 0; byte_index < byte_size; byte_index++) {
        uint8_t byte = memory[byte_index];
        free_index = MemoryArena_FirstZeroBit(byte);

        uintptr_t bit_index = byte_index * 8  + free_index;
        if (free_index != 8 && bit_index < bit_count) {
            exit_code = EXIT_SUCCESS;
            index_destination[0] = bit_index;
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
    if (arena->used_count == arena->arena_size) {
	// This arena has no free addresses
	return ENOMEM;
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
    arena->used_count += 1;

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

    if ((~arena->used[byte_index]) & (0x80 >> bit_index)) {
        // Is not used, can't free
        return EBUSY;
    }
    
    arena->used[byte_index] ^= 0x80 >> bit_index;
    arena->used_count -= 1;

    return EXIT_SUCCESS;
}

int MemoryArena_SuccessiveZeroBits(memory_arena_t* arena, uintptr_t array_length, uintptr_t* index_destination)
{
    uintptr_t bit_count = arena->arena_size;
    uintptr_t bit_check_limit = bit_count - array_length;
    uint8_t byte;
    uintptr_t byte_bit_index;
    for (uintptr_t bit_index = 0; bit_index <= bit_check_limit; bit_index++) {
        byte = arena->used[bit_index / 8];
        byte_bit_index = bit_index % 8;
        if (byte & (0x80 >> byte_bit_index)) {
            continue;
        }

        uintptr_t advance_index;
        uintptr_t advance_bit;
        uint8_t advance_byte;
        bool success = true;
        for (advance_index = 0; advance_index < array_length; advance_index++) {
            advance_byte = arena->used[(bit_index + advance_index) / 8];
            advance_bit  = (byte_bit_index + advance_index) % 8;

            if (advance_byte & (0x80 >> advance_bit)) {
                success = false;
                break;
            }
        }

        if (success) {
            index_destination[0] = bit_index;
            return EXIT_SUCCESS;
        }
    }

    return EXIT_FAILURE;
}

int MemoryArena_AllocateArray(memory_arena_t *arena, uintptr_t array_length, void **pointer_destination)
{
    if (arena == NULL) {
        return EINVAL;
    }
    if (array_length == 0) {
        return EINVAL;
    }
    if (pointer_destination == NULL) {
        return EDESTADDRREQ;
    }

    uintptr_t arena_slots_left = arena->arena_size - arena->used_count;
    if (arena_slots_left < array_length) {
        return ENOMEM;
    }

    uintptr_t start_index;
    int find_code = MemoryArena_SuccessiveZeroBits(arena, array_length, &start_index);
    if (find_code == EXIT_FAILURE) {
        return ENOMEM;
    }

    // Mark all the bits as used
    uintptr_t bit_index;
    uintptr_t end_index = start_index + array_length;
    for (bit_index = start_index; bit_index < end_index; bit_index++) {
        arena->used[bit_index / 8] |= (0x80 >> bit_index % 8);
    }

    void* array_pointer    = arena->memory + (arena->object_size * start_index);
    pointer_destination[0] = array_pointer;

    arena->used_count += array_length;

    return EXIT_SUCCESS;
}

int MemoryArena_FreeArray(memory_arena_t* arena, void* array_start, uintptr_t array_length)
{
    if (arena == NULL) {
        return EINVAL;
    }
    if (array_start == NULL) {
        return EINVAL;
    }

    void* arena_start = arena->memory;
    char* arena_end   = arena->memory + (arena->object_size * arena->arena_size);
    if (array_start < arena_start) {
        return ENXIO;
    }
    if (array_start > (void*) arena_end) {
        return ENXIO;
    }

    char* array_zone_end = arena_end - (array_length * arena->object_size);
    if (array_start > (void*) array_zone_end) {
        return ENOENT;
    }
    if (((uintptr_t) (array_start) - (uintptr_t) (arena_end)) % arena->object_size != 0) {
        // Invalid offset, not the real array start
        return ENOENT;
    }

    uintptr_t object_index = ((uintptr_t) (array_start) - (uintptr_t) (arena_start)) / arena->object_size;
    uint8_t byte;
    uintptr_t byte_index;
    uintptr_t bit_index;
    // Check whether all bits are marked as reserved
    for (uintptr_t array_index = 0; array_index < array_length; array_index++) {
        byte_index = (object_index + array_index) / 8;
        bit_index  = (object_index + array_index) % 8;
        byte = arena->used[byte_index];

        if (~byte & (0x80 >> bit_index)) {
            // Bit is not in 'used' state
            return EBUSY;
        }
    }

    for (uintptr_t array_index = 0; array_index < array_length; array_index++) {
        byte_index = (object_index + array_index) / 8;
        bit_index  = (object_index + array_index) % 8;
        
        // Mark as used
        arena->used[byte_index] ^= 0x80 >> bit_index;
    }

    return EXIT_SUCCESS;
}
