#ifndef _QUANTFUNDHFTPROJECT_COMMONFILES_ARBITRAGEBASE_H_
#define _QUANTFUNDHFTPROJECT_COMMONFILES_ARBITRAGEBASE_H_
#include "StrategyDefine.h"
#include "StrategyData.h"
#include "Tick.h"
#include "StockTick.h"
#include <cmath>
#include <list>
#include <string>
#include "Order.h"
#include <fstream>
#include <atomic>
#include "FutureTick.h"
using namespace std;


#define Minuend_Yesterday 0
#define Subtrahend_Yesterday 1
#define Minuend_Today 2
#define Subtrahend_Today 3


#define MyLIMITORDER LIMITORDER
#define Subtrahend_Index 0
#define Minuend_Index 1


#define NO_PROBE_ public:TProbeStructType GetProbeStruct(){return CArbitrageBase::GetProbeStruct();};
#define END_PROBE_BIND_ {{ NULL,"",TProbe_Color_Powderblue}}};\
						public:\
						TProbeStructType GetProbeStruct(){return CArbitrageBase::GetProbeStruct();};
#define END_PARAMETER_BIND_ { "",LB1_IntType, NULL}};\
						public:\
						CParNode * GetParamStruct(){return CArbitrageBase::GetParamStruct();};
#define NO_PARAMETER_ public:CParNode * GetParamStruct(){return CArbitrageBase::GetParamStruct();};



enum TTradeSignalType {
	TEnumBuyMinuend_SellSubtrahend,	//：价差太低，预测将来价差会升高->做多价差		增加仓位		发出此信号后系统将会做多被减数合约G，做空减数合约F LB1_BuyO
	TEnumSellMinuend_BuySubtrahend,	//：价差已经升到正常水平       ->做空价差  	减少仓位		发出此信号后系统将会做空被减数合约G，做多减数合约F LB1_SellC
};

enum TPositionState
{
	HasNoPosition,
	HasBetSpreadRisePosition,
	HasBetSpreadDropPosition,
	HasErrorPosition
};

class CBar
{
	
public:
#define CycleBeginTime (ptime(date(1966, 1, 1), time_duration(0, 0, 0, 0)))
#define CYCLE_NUM_OF_UTC(UTCtime,cyclelen) ((int)(((UTCtime-CycleBeginTime).total_seconds()) / cyclelen))
	void Init(unsigned int TypeSecond)
	{
		m_dbOpen = 0.0;
		m_dbHighest = 0.0;
		m_dbLowest = 0.0;
		m_dbClose = 0.0;
	}

	TPriceType m_dbOpen;
	TPriceType m_dbHighest;
	TPriceType m_dbLowest;
	TPriceType m_dbClose;
	BEGIN_SERIALIZATION
		SERIALIZATION(m_dbOpen)
		SERIALIZATION(m_dbHighest)
		SERIALIZATION(m_dbLowest)
		SERIALIZATION(m_dbClose)
	END_SERIALIZATION
};




class CArbitrageBase : public MStrategy
{
public:
	STRATEGY_TEMPLATE_DECLARE(CArbitrageBase)
	bool OnInquiry(MStrategyInquiryDataInterface*) { return true; };
	bool IsSupport(TStrategyTickType ticktype);
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

	TLastErrorIdType OnInit_FromArchive(ptime);

	void MakeOrder(TTradeSignalType Signal,unsigned int copies, TVolumeType MinuendVolume, TVolumeType SubtrahendVolume);

	bool ParamAndProbeInit(CParNode * ,TProbeStructType);

	map<date, int> & GetMinuendPosition();

	map<date, int> & GetSubtrahendPosition();

	CStockTick & GetMinuendTick();

	CStockTick & GetSubtrahendTick();

	TPriceType  GetMinuendMinPriceTick();

	TPriceType  GetSubtrahendMinPriceTick();

	bool IsTwoLegHasInited();

	void ShowPosition();

	int GetMinuendInitPosition();

	int GetSubtrahendInitPosition();

	BEGIN_PROBE_BIND
	END_PROBE_BIND

	BEGIN_SERIALIZATION

		SERIALIZATION(MinuendMinPriceTick)
		SERIALIZATION(SubtrahendMinPriceTick)

		SERIALIZATION(MinuendNormalPosition)
		SERIALIZATION(SubtrahendNormalPosition)
		SERIALIZATION(MinuendYesterdayPosition)
		SERIALIZATION(SubtrahendYesterdayPosition)
		SERIALIZATION(MinuendTodayPosition)
		SERIALIZATION(SubtrahendTodayPosition)



		SERIALIZATION(m_ptimeGlobalCurrentTime)
		SERIALIZATION(m_datePositionTradeDay)
		SERIALIZATION(m_mapPosition)
		SERIALIZATION(m_tickData)
		SERIALIZATION(m_boolHasInitialize)

	END_SERIALIZATION

	BEGIN_PARAMETER_BIND
		PARAMETER("MinuendMinPriceTick",			LB1_DoubleType, &MinuendMinPriceTick)
		PARAMETER("SubtrahendMinPriceTick",			LB1_DoubleType, &SubtrahendMinPriceTick)

		PARAMETER("MinuendNormalPosition",			LB1_IntType,	&MinuendNormalPosition)
		PARAMETER("SubtrahendNormalPosition",		LB1_IntType,	&SubtrahendNormalPosition)
		PARAMETER("MinuendYesterdayPosition",		LB1_IntType,	&MinuendYesterdayPosition)
		PARAMETER("SubtrahendYesterdayPosition",	LB1_IntType,	&SubtrahendYesterdayPosition)
		PARAMETER("MinuendTodayPosition",			LB1_IntType,	&MinuendTodayPosition)
		PARAMETER("SubtrahendTodayPosition",		LB1_IntType,	&SubtrahendTodayPosition)
	END_PARAMETER_BIND

public:
	tuple<int, int, int, int> GetPosition();

	TPositionState GetPositionState();

	double MinuendMinPriceTick = 0.001;
	double SubtrahendMinPriceTick = 0.001;

	int MinuendNormalPosition = 0;//处于场外时被减数的仓位
	int SubtrahendNormalPosition = 0;//处于场外是减数的仓位
	int MinuendYesterdayPosition = 0;
	int SubtrahendYesterdayPosition = 0;
	int MinuendTodayPosition = 0;
	int SubtrahendTodayPosition = 0;



	ptime m_ptimeGlobalCurrentTime;
	date m_datePositionTradeDay;
	map<date, int> m_mapPosition[2];
	CStockTick m_tickData[2];
	bool m_boolHasInitialize[2];
	



};

#endif
