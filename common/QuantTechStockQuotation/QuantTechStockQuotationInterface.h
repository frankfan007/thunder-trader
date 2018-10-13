#ifndef _COMMONFILES_QUANTTECHSTOCKQUOTATION_QUANTTECHSTOCKQUOTATIONINTERFACE_H_
#define _COMMONFILES_QUANTTECHSTOCKQUOTATION_QUANTTECHSTOCKQUOTATIONINTERFACE_H_

#ifdef WIN32
#ifdef _EXPORT
#define MD_API_EXPORT __declspec(dllexport)
#else
#define MD_API_EXPORT __declspec(dllimport)
#endif
#endif

#ifdef LINUX
#ifdef _EXPORT
#define MD_API_EXPORT __attribute__((visibility("default")))
#else
#define MD_API_EXPORT
#endif
#endif
#include "StockTick.h"

class CStockTick;
class CQuantTechStockQuotationSpi
{
public:
	virtual void OnRspError(const char * errormsg) {};
	virtual void OnRspSubMarketData(const char * symbol) {};
	virtual void OnRspUnSubMarketData(const char * symbol) {};
	virtual void OnRtnDepthMarketData(const CStockTick *) {};
};

class MD_API_EXPORT MQuantTechStockQuotationInterface
{
public:
	static MQuantTechStockQuotationInterface * Create();
	virtual void Release() = 0;
	virtual void Init(string IP, string Port, CQuantTechStockQuotationSpi*) = 0;
	virtual void UnInit() = 0;
	virtual void SubscribeMarketData(const char * symbol) = 0;
	virtual void UnSubscribeMarketData(const char * symbol) = 0;
};

#endif