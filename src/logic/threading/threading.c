#include "threading.h"

#if defined(__unix__) || (defined(__APPLE__) && defined(__MACH__))
void Sys_CreateThread(systhread_t* thread, void* arg, void* func)
{
    if (pthread_create(thread, NULL, func, arg) != NULL)
        *thread = NULL;
    return;
}
#else if defined _WIN32
void Sys_CreateThread(systhread_t* thread, void* arg, void* func)
{
    *thread = CreateThread(NULL, 1024, func, arg, NULL, NULL);
    return;
}
#endif