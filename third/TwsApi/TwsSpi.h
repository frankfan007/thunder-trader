#pragma once

#include "TwsDataStructDef.h"

class CTwsSpi
{
public:
	virtual void OnRspUserLogin(CTwsRspUserLoginField * loginField, bool IsSucceed) {};
	virtual void OnRtnDepthMarketData(CTwsDepthMarketDataField * pDepthMarketData) {};
	virtual void OnRspError(int ErrID,int ErrCode, const char * ErrMsg) {};
	virtual void OnDisconnected() {};
	virtual void OnRtnOrder(CTwsOrderField * ) {};
	virtual void OnRtnTrade(CTwsTradeField * ) {};
};