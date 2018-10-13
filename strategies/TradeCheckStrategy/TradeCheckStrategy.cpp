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
using namespace std;



class MyStrategy : public MStrategy
{
public:
	TOrderRefIdType m_uOrderRef;
	string m_strSysid;
	STRATEGY_TEMPLATE_DECLARE(MyStrategy)


	friend class boost::serialization::access;
	template<class Archive> void serialize(Archive& ar, const unsigned int version) {
		int a = 0;
		ar & m_enumCurrentState;
	}



	enum TCurrentStateType {TestStrategy_Pedding, TestStrategy_Free};
	TCurrentStateType m_enumCurrentState= TestStrategy_Free;


	double m_dbPriceTick=0.2;
	int m_dbNeedSysid = 1;
	int m_intLevel = 2;
	int m_uVolume = 1;
	BEGIN_PARAMETER_BIND
		PARAMETER("m_dbPriceTick", LB1_DoubleType, &m_dbPriceTick)
		PARAMETER("m_dbNeedSysid", LB1_IntType, &m_dbNeedSysid)
		PARAMETER("m_intLevel", LB1_IntType, &m_intLevel)
		PARAMETER("m_uVolume", LB1_IntType, &m_uVolume)
	END_PARAMETER_BIND



	bool IsSupport(TStrategyTickType ticktype)
	{
		return true;
	}
	
	BEGIN_PROBE_BIND
		BEGIN_GRAPH
			PROBE(&out_AskPrice1, "out_AskPrice1", TProbe_Color_Green)
			PROBE(&out_BidPrice1, "out_BidPrice1", TProbe_Color_Green)
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
	m_enumCurrentState = TestStrategy_Free;
	return LB1_NO_ERROR;
}

void MyStrategy::OnTick(TMarketDataIdType dataid, const CTick *pDepthMarketData)
{
	out_AskPrice1.store(pDepthMarketData->m_dbAskPrice[0]);
	out_BidPrice1.store(pDepthMarketData->m_dbBidPrice[0]);
	switch (m_enumCurrentState)
	{
	case TestStrategy_Free:
	{
		char buf[1024];
		if (false == MEDDLE(buf, 1024))
			return;
		string cmd = buf;
		if ("test" == cmd)
		{
			m_uOrderRef = LIMITORDER(
				LB1_Buy,
				LB1_Increase,
				m_uVolume,
				pDepthMarketData->m_dbBidPrice[0] - m_dbPriceTick*m_intLevel,
				0);
			m_enumCurrentState = TestStrategy_Pedding;
			m_strSysid = "";
			LOG("Strategy%d: Limit order command is sent,waiting....",g_StrategyId);
		}
	}
		break;
	case TestStrategy_Pedding:
	{
		if (0!=m_dbNeedSysid&&m_strSysid.empty())
			return;
		CANCEL(m_uOrderRef, (char *)m_strSysid.c_str(), 0);
		m_enumCurrentState = TestStrategy_Free;
		LOG("Strategy%d: Cancel Order command is sent,waiting....", g_StrategyId);
	}
		break;
	}
	
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
	if (LB1_StatusCanceled == Status)
		LOG("Cancel order test succeed!");
	if(sysId)
		m_strSysid = sysId;
}
