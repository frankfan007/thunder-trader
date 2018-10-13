#ifndef _COMMONFILES_COMPRETRADESYSTEMHEADERS_PUBLIC_H_
#define _COMMONFILES_COMPRETRADESYSTEMHEADERS_PUBLIC_H_

#define ARCHIVE_FILE_SUFFIX ".sarchive"
#ifdef WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#define _WIN32_WINNT 0x0501
#define FILE_PATH_SEPARATOR "\\"
#define STRATEGY_SUFFIX ".dll"
#define snprintf _snprintf
typedef HINSTANCE StrategyHandleType;
#define LoadStrategyBin(filename) LoadLibraryA(filename)
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
#define FILE_PATH_SEPARATOR "/"
#define STRATEGY_SUFFIX ".so"
typedef void * StrategyHandleType;
#define LoadStrategyBin(filename) dlopen(filename,RTLD_NOW)
#define UnLoadStrategyBin(handle) dlclose(handle)
#define GetProcessAddressByName(handle,name) dlsym(handle,name)
#endif


#endif