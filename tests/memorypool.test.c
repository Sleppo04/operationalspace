#include "../src/common/memorypool/memorypool.h"
#include "../src/common/memorypool/memoryarena.h"

#include <assert.h>
#include <string.h>

void check_basic(void)
{
    memory_pool_t pool;
    MemoryPool_Create(&pool, sizeof(int), 0, 0);

    int** addresses = malloc(sizeof(int*) * 8000);
    for (int i = 0; i < 8000; i++) {
        MemoryPool_Allocate(&pool, (void**) addresses + i);
        addresses[i][0] = i;
    }

    for (int i = 7999; i > -1; i--) {
        assert(addresses[i][0] == i);
        MemoryPool_Free(&pool, addresses[i]);
    }

    MemoryPool_Destroy(&pool);
    free(addresses);
}

void check_array(void)
{
    memory_pool_t pool;
    MemoryPool_Create(&pool, sizeof(int), 0, 0);
    int* array = NULL;
    MemoryPool_AllocateArray(&pool, 16000, (void**) &array);

    assert(array != NULL);
    memset(array, 'a', 16000);
    assert(MemoryPool_FreeArray(&pool, array, 16000) == EXIT_SUCCESS);
    MemoryPool_Destroy(&pool);
}

void check_both(void)
{
    memory_pool_t pool;
    MemoryPool_Create(&pool, sizeof(int), 0, 4096);
    int* first[250];
    int* second[250];
    int* third[250];

    for (int i = 0; i < 250; i++) {
        assert(MemoryPool_Allocate(&pool, (void**) first + i) == EXIT_SUCCESS);
    }
    for (int i = 0; i < 250; i++) {
        assert(MemoryPool_Allocate(&pool, (void**) second + i) == EXIT_SUCCESS);
    }
    for (int i = 0; i < 250; i++) {
        assert(MemoryPool_Allocate(&pool, (void**) third + i) == EXIT_SUCCESS);
    }

    int* array;
    int* _;
    assert(!MemoryPool_AllocateArray(&pool, 274, (void**) &array));
    assert(MemoryPool_Allocate(&pool, (void**) &_) == ENOMEM);
    assert(MemoryPool_AllocateArray(&pool, 1, (void**) &_) == ENOMEM);

    for (int i = 0; i < 50; i++) {
        assert(!MemoryPool_Free(&pool, first[i]));
        assert(!MemoryPool_Free(&pool, second[i]));
        assert(!MemoryPool_Free(&pool, third[i]));
    }
    int* arrays[3];

    assert(MemoryPool_AllocateArray(&pool, 75, (void**) &_) == ENOMEM);
    assert(MemoryPool_AllocateArray(&pool, 51, (void**) &_) == ENOMEM);

    int code;
    for (int i = 0; i < 3; i++) {
        code = MemoryPool_AllocateArray(&pool, 50, (void**) arrays + i);
        assert(code == 0);
    }

    MemoryPool_FreeArray(&pool, (void**) array, 274);
    MemoryPool_FreeArray(&pool, (void**) arrays[1], 50);
    MemoryPool_FreeArray(&pool, (void**) arrays[0], 50);
    MemoryPool_FreeArray(&pool, (void**) arrays[2], 50);

    for (int i = 50; i < 250; i++) {
        assert(MemoryPool_Free(&pool, (void**) third[i]) == EXIT_SUCCESS);
        assert(MemoryPool_Free(&pool, (void**) second[i]) == EXIT_SUCCESS);
        assert(MemoryPool_Free(&pool, (void**) first[i]) == EXIT_SUCCESS);
    }

    MemoryPool_Destroy(&pool);
}

int main()
{
    check_basic();
    check_array();
    check_both();
}
