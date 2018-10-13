// StrategyDemo.cpp : 定义 DLL 应用程序的导出函数。
//

#include "stdafx.h"
#include "Order.h"
#include "StrategyDefine.h"
#include "StrategyData.h"
#include "Tick.h"
#include "FutureTick.h"
#include "StockTick.h"
#include "FutureTick.h"
#include "OptionTick.h"
#include <cmath>
#include <list>
#include <string>
#include <fstream>
#include "ta_func.h"
using namespace std;



class MyStrategy : public MStrategy
{
public:
	
	STRATEGY_TEMPLATE_DECLARE(MyStrategy)


	BEGIN_SERIALIZATION
		SERIALIZATION(m_uPar) 
	END_SERIALIZATION




	int m_uPar = 300;
	BEGIN_PARAMETER_BIND
		PARAMETER_INT(m_uPar)
	END_PARAMETER_BIND



	bool IsSupport(TStrategyTickType ticktype)
	{
		return true;
	}
	
	BEGIN_PROBE_BIND
		BEGIN_GRAPH
			PROBE(&out_AskPrice1, "out_AskPrice1", TProbe_Color_Green)
		END_GRAPH("Line")
		BEGIN_GRAPH
			PROBE(&out_BidPrice1, "out_BidPrice1", TProbe_Color_Blue)
		END_GRAPH("Line")
	END_PROBE_BIND
	bool OnInquiry(MStrategyInquiryDataInterface*);
	TLastErrorIdType OnInit(ptime);
	void OnTick(TMarketDataIdType, const CTick *);
	void OnTrade(
		TOrderRefIdType,
		TOrderSysIdType,
		TVolumeType,
		TPriceType,
		TOrderDirectionType,
		TOrderOffsetType);
	void OnOrder(
		TOrderRefIdType,
		TOrderSysIdType,
		TOrderDirectionType,
		TOrderStatusType,
		TPriceType,
		TTradedVolumeType,
		TRemainVolumeType
		);
	void OnEndup();
	
	TLastErrorIdType OnInit_FromArchive(ptime)
	{
		return TLastErrorIdType::LB1_NO_ERROR;
	};
	
	
private:
	atomic<double> out_AskPrice1;
	atomic<double> out_BidPrice1;
};

STRATEGY_TEMPLATE_DEFINITION(MyStrategy)


bool MyStrategy::OnInquiry(MStrategyInquiryDataInterface * data)
{
	return true;
}

TLastErrorIdType MyStrategy::OnInit(ptime)
{
	return LB1_NO_ERROR;
}

void MyStrategy::OnTick(TMarketDataIdType dataid, const CTick *pDepthMarketData)
{
	out_AskPrice1.store(pDepthMarketData->m_dbAskPrice[0]);
	out_BidPrice1.store(pDepthMarketData->m_dbBidPrice[0]);
}


void MyStrategy::OnEndup()
{
}


void MyStrategy::OnTrade(
	TOrderRefIdType ref,
	TOrderSysIdType sys,
	TVolumeType volume,
	TPriceType price,
	TOrderDirectionType dir,
	TOrderOffsetType offset)
{


}

void MyStrategy::OnOrder(
	TOrderRefIdType ref,
	TOrderSysIdType sysId,
	TOrderDirectionType direction,
	TOrderStatusType Status,
	TPriceType LimitPrice,
	TTradedVolumeType VolumeTraded,
	TRemainVolumeType VolumeRemain)
{
}
