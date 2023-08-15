#ifndef THREADING_H
#define THREADING_H

#if defined _WIN32
#include <windows.h>
typedef HANDLE systhread_t;
#elif defined(__unix__) || (defined(__APPLE__) && defined(__MACH__))
#include <pthreads.h>
typedef pthread_t systhread_t;
#else
#error "The threading library doesn't support this OS!"
#endif

void Sys_CreateThread(systhread_t* thread, void* arg, void* func);

#endif //THREADING_H