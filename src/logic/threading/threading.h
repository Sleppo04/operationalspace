#ifndef THREADING_H
#define THREADING_H

#if defined _WIN32
#include <windows.h>
typedef HANDLE systhread_t;

#elif defined(__unix__) || (defined(__APPLE__) && defined(__MACH__))
#include <pthread.h>
typedef pthread_t systhread_t;

#else
#error "The threading library doesn't support this OS!"
#endif

int Sys_CreateThread(systhread_t* thread, void* arg, void* (*function) (void*));
void Sys_YieldThread();

#endif //THREADING_H