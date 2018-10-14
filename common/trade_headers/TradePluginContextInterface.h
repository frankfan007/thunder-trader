#ifndef _COMMONFILES_COMPRETRADESYSTEMHEADERS_TRADEPLUGINCONTEXTINTERFACE_H_
#define _COMMONFILES_COMPRETRADESYSTEMHEADERS_TRADEPLUGINCONTEXTINTERFACE_H_
#include "StrategyData.h"
using namespace StrategyData;
class MTradePluginContextInterface
{
public:
	virtual void OnTrade(
		TOrderRefIdType,
		TOrderSysIdType,
		TPriceType,
		TVolumeType) = 0;
	virtual void OnOrder(
		TOrderRefIdType,
		TOrderSysIdType,
		TOrderStatusType,
		TPriceType,
		TTradedVolumeType,
		TRemainVolumeType
		) = 0;
};
#endif