#ifndef _COMMONFILES_HSSTOCK_HSSTOCKTRADEAPIINTERFACE_H_
#define _COMMONFILES_HSSTOCK_HSSTOCKTRADEAPIINTERFACE_H_

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
#include "HsStockTradeDataStructs.h"
class MHsStockTradeSpi
{
public:
	virtual void OnRspWarnning(const CHsStockTradeRspInfoField *pRspInfo) {};
	virtual void OnRspError(const CHsStockTradeRspInfoField *pRspInfo) {};
	virtual void OnFrontConnected() {};
	virtual void OnRspUserLogin(const CHsStockTradeRspUserLoginField * pRsp, const CHsStockTradeRspInfoField *pRspInfo) {};
	virtual void OnRspQueryMoney(const CHsStockTradeRspQueryMoneyField * pRsp, const CHsStockTradeRspInfoField *pRspInfo) {};
	virtual void OnRspQueryTrade(const CHsStockOutputOrderTradeField * pRsp, const CHsStockTradeRspInfoField *pRspInfo) {};
	virtual void OnRspQueryEntrust(const CHsStockOutputOrderEntrustField * pRsp, const CHsStockTradeRspInfoField *pRspInfo) {};
	virtual void OnRtnOrder(const CHsStockTradeOrderField *pOrder) {};
	virtual void OnRtnTrade(const CHsStockTradeTradeField *pTrade) {};
};


class MD_API_EXPORT MHsStockTradeApiInterface
{
public:
	static MHsStockTradeApiInterface * CreateApi();
	
	virtual void Init(
		long lBranchCode, 
		const char * strAddress, 
		const char * strAddressBackup, 
		unsigned short uPort, 
		MHsStockTradeSpi* spi) = 0;
	virtual void UnInit() = 0;
	virtual void Release() = 0;

	virtual int ReqUserLogin(const char *szAccount, const char *szPassWord) = 0;
	virtual int ReqQueryMoney() = 0;
	virtual int ReqOrderInsert(const CHsStockTradeInputOrderField *pInputOrder, long * entrust_no) = 0;
	virtual void ReqOrderCancel(const CHsStockInputOrderCancelField *pInputOrderAction) = 0;
	virtual void ReqOrderTrade(const CHsStockInputOrderTradeField *pInputOrderAction) = 0;
	virtual void ReqOrderEntrust(const CHsStockInputOrderEntrustField *pInputOrderAction) = 0;
};






#endif

