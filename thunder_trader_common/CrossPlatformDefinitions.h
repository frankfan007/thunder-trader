#ifndef CrossPlatformDefinitions_H
#define CrossPlatformDefinitions_H
#include <stdio.h>

#ifdef WIN32
#define _WIN32_WINNT 0x0501
#include <windows.h>
#define FILE_PATH_SEPARATOR '\\'
#define snprintf _snprintf
#define LockTypeName CRITICAL_SECTION
#define InitLock(name) InitializeCriticalSection(&name)
#define Lock(name) EnterCriticalSection(&name)
#define UnLock(name) LeaveCriticalSection(&name)
#define DelLock(name) DeleteCriticalSection(&name)

#define StrategyHandleType HINSTANCE
#define LoadStrategyBin(filename) LoadLibrary(filename)
#define UnLoadStrategyBin(handle) FreeLibrary(handle)
#define GetProcessAddressByName(handle,name) GetProcAddress(handle, name)
#else
#include <dlfcn.h>
#include <pthread.h>
#include <linux/unistd.h>
#include <sys/syscall.h>
#include <iconv.h>
#include<fcntl.h>
#include<sys/types.h>
#include<unistd.h>
#define FILE_PATH_SEPARATOR '/'
#define LockTypeName LockTypeName
#define InitLock(name) pthread_spin_init(&name,PTHREAD_PROCESS_PRIVATE)
#define Lock(name) pthread_spin_lock(&name)
#define UnLock(name) UnLock(name)
#define DelLock(name) pthread_spin_destroy(&name);

#define StrategyHandleType void *
#define LoadStrategyBin(filename) dlopen(filename,RTLD_NOW)
#define UnLoadStrategyBin(handle) dlclose(handle)
#define dlsym(handle,name) GetProcAddress(handle, name)
#endif
#endif