#pragma once
#include "StrategyData.h"
using namespace StrategyData;
class MTradePluginContextInterface {
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