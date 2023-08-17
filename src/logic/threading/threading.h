#ifndef THREADING_H
#define THREADING_H

#if defined _WIN32

#include <windows.h>
typedef HANDLE systhread_t;
typedef HANDLE syssemaphore_t;

#elif defined(__unix__) || (defined(__APPLE__) && defined(__MACH__))

#include <pthread.h>
#include <sys/semaphore.h>
typedef pthread_t systhread_t;
typedef sem_t syssemaphore_t;

#else

#error "The threading library doesn't support this OS!"

#endif

int Sys_CreateThread(systhread_t* thread, void* arg, void* (*function) (void*));
int Sys_CreateSemaphore(syssemaphore_t* sem, unsigned int value);
int Sys_WaitForSemaphore(syssemaphore_t* sem);
int Sys_ReleaseSemaphore(syssemaphore_t* sem);
int Sys_DestroySemaphore(syssemaphore_t* sem);

#endif //THREADING_H