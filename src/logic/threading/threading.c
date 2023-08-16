#include "threading.h"

#if defined(__unix__) || (defined(__APPLE__) && defined(__MACH__))
int Sys_CreateThread(systhread_t* thread, void* arg, void* (*function) (void*))
{
    return pthread_create(thread, NULL, function, arg);
}


#elif defined _WIN32
int Sys_CreateThread(systhread_t* thread, void* arg, void* (*function) (void*))
{
    *thread = CreateThread(NULL, 1024, function, arg, NULL, NULL);
    if (thread[0] != NULL) {
        return EXIT_SUCCESS;
    }

    return GetLastError();
}
#endif