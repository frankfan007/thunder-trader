#pragma once

#include "TwsSpi.h"
#include "TwsDataStructDef.h"
#ifdef WIN32

#ifdef _EXPORT 
#define EXPORT_INTERFACE __declspec(dllexport) 
#else
#define EXPORT_INTERFACE __declspec(dllimport) 
#endif

#else 
#define EXPORT_INTERFACE
#endif
class EXPORT_INTERFACE MTwsApi
{
public:
	static MTwsApi * CreateApi();
	virtual void RegisterSpi(CTwsSpi *pSpi) = 0;
	virtual int Connect(const char * pAddress,unsigned int port, unsigned int clientID) = 0;
	virtual int DisConnect() = 0;
	virtual int SubscribeMarketData(const CTwsContractField *) = 0;
	virtual int UnSubscribeMarketData(const CTwsContractField *) = 0;
	virtual int Release() = 0;
	virtual bool GetValue(const char * key, const char * currency, char * retval, size_t len) = 0;
	virtual TTwsOrderIdType ReqOrderInsert(CTwsContractField *, CTwsInputOrderField *) = 0;
	virtual void ReqOrderCancel(TTwsOrderIdType) = 0;
};