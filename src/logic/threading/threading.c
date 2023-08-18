#include "threading.h"

#if defined(__unix__) || (defined(__APPLE__) && defined(__MACH__))
int Sys_CreateThread(systhread_t* thread, void* arg, void* (*function) (void*))
{
    return pthread_create(thread, NULL, function, arg);
}

int Sys_CreateSemaphore(syssemaphore_t* sem, unsigned int value)
{
    if (sem_init(sem, 0, value) == 0) {
        return EXIT_SUCCESS;
    }
    return EXIT_FAILURE;
}

int Sys_WaitForSemaphore(syssemaphore_t* sem)
{
    if (sem_wait(sem) == 0) {
        return EXIT_SUCCESS;
    }
    return EXIT_FAILURE;
}

int Sys_ReleaseSemaphore(syssemaphore_t* sem)
{
    if (sem_post(sem) == 0) {
        return EXIT_SUCCESS;
    }
    return EXIT_FAILURE;
}

int Sys_DestroySemaphore(syssemaphore_t* sem)
{
    if (sem_destroy(sem) == 0) {
        return EXIT_SUCCESS;
    }
    return EXIT_FAILURE;
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

int Sys_CreateSemaphore(syssemaphore_t* sem, unsigned int value)
{
    *sem = CreateSemaphore(NULL, value, value, NULL);
    if (*sem != NULL)
        return EXIT_SUCCESS;
    return EXIT_FAILURE;
}

int Sys_WaitForSemaphore(syssemaphore_t* sem)
{
    return WaitForSingleObject(*sem, INFINITE);
}

int Sys_ReleaseSemaphore(syssemaphore_t* sem)
{
    if (ReleaseSemaphore(*sem, 1, NULL))
        return EXIT_SUCCESS;
    return EXIT_FAILURE;
}

int Sys_DestroySemaphore(syssemaphore_t* sem)
{
    if (CloseHandle(*sem))
        return EXIT_SUCCESS;
    return EXIT_FAILURE;
}

#endif
