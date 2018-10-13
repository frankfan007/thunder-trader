#ifndef _COMMUNI_INTERFACE_H_
#define _COMMUNI_INTERFACE_H_
#include <iostream>
#include <sstream>
using namespace std;
#ifdef WIN32

#ifdef _EXPORT
#define _INTERFACE __declspec(dllexport) 
#else
#define _INTERFACE __declspec(dllimport) 
#endif

#else 

#define STRATEGY_INTERFACE

#endif
class MCommuniContext
{
public:
	virtual stringstream OnRequest(const stringstream &) = 0;//保证这个函数不会同时被多个线程调用
};

class _INTERFACE MCommuniInterface
{
public:
	static MCommuniInterface * Create();
	virtual bool Release() = 0;
	virtual bool Start(unsigned int port, MCommuniContext* ) = 0;
	virtual bool Stop() = 0;
};
#endif