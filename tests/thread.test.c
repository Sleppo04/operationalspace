#include <stdio.h>
#include "../src/logic/threading/threading.h"

#ifdef _WIN32
void foo(void* a)
{
    for (int i = 0; i < 10; i++) {
        printf("Func %i says: %i\n", *((int*)a), i);
    }
    return;
}
#endif

#if defined(__unix__) || (defined(__APPLE__) && defined(__MACH__))
void* foo(void* argument)
{
    for (int i = 0; i < 10; i++) {
        printf("Func %i says %i\n", *((int*)argument), i);
    }

    return NULL;
}
#endif

int main(int argc, char** argv)
{
    systhread_t threads[3];
    int arguments[] = {1, 2, 3};

    Sys_CreateThread(&threads[0], &arguments[0], &foo);
    Sys_CreateThread(&threads[1], &arguments[1], &foo);
    Sys_CreateThread(&threads[2], &arguments[2], &foo);

    return 0;
}