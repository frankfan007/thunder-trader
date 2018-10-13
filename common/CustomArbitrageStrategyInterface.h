#ifndef _ARBITRAGE_LIB_INTERFACE
#define _ARBITRAGE_LIB_INTERFACE
#include "StrategyData.h"
#include "StrategyContext.h"
#include <tuple>
#include "Tick.h"
using namespace std;
using namespace StrategyData;
#ifdef _EXPORTING
#define API_DECLSPEC    __declspec(dllexport)
#else
#define API_DECLSPEC    __declspec(dllimport)
#endif



enum TTradeSignalType {
	TEnumBuyMinuend_SellSubtrahend_Increase,	//：价差太低，预测将来价差会升高->做多价差		增加仓位		发出此信号后系统将会做多被减数合约G，做空减数合约F LB1_BuyO
	TEnumSellMinuend_BuySubtrahend_Descrease,	//：价差已经升到正常水平       ->做空价差  	减少仓位		发出此信号后系统将会做空被减数合约G，做多减数合约F LB1_SellC
	
	TEnumSellMinuend_BuySubtrahend_Increase,	//：价差太高，预测将来价差会降低->做空价差		增加仓位		发出此信号后系统将会做空被减数合约G，做多减数合约F LB1_SellO
	TEnumBuyMinuend_SellSubtrahend_Descrease,	//：价差已经降低到正常水平     ->做多价差		减少仓位		发出此信号后系统将会做多被减数合约G，做空减数合约F LB1_BuyC
	
	TEnumDoNothing,								//：没有入场与入场信号						发出此信号后系统将不采取任何动作
};

enum TPositionState
{
	NoPosition,							//目前策略没有任何持仓,等待入场
	HasPosition							//策略目前有持仓
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
		/*SERIALIZATION(m_dbOpen)
		SERIALIZATION(m_dbHighest)
		SERIALIZATION(m_dbLowest)
		SERIALIZATION(m_dbClose)*/
	END_SERIALIZATION
};


struct CPosition
{
	void Init()
	{
		m_uLongPosition = 0;
		m_uShortPosition = 0;
		m_dbLongAvgPrice = 0.0;
		m_dbShortAvgPrice = 0.0;
	}
	BEGIN_SERIALIZATION
		/*SERIALIZATION(m_uLongPosition)
		SERIALIZATION(m_uShortPosition)
		SERIALIZATION(m_dbLongAvgPrice)
		SERIALIZATION(m_dbShortAvgPrice)*/
	END_SERIALIZATION
	unsigned int m_uLongPosition;
	unsigned int m_uShortPosition;
	double m_dbLongAvgPrice;
	double m_dbShortAvgPrice;
};

//#define BuyMinuend_SellSubtrahend_Increase(MinuendVol,SubtrahendVol) make_tuple(TEnumBuyMinuend_SellSubtrahend_Increase,MinuendVol,SubtrahendVol)
//#define SellMinuend_BuySubtrahend_Descrease(MinuendVol,SubtrahendVol) make_tuple(TEnumSellMinuend_BuySubtrahend_Descrease,MinuendVol,SubtrahendVol)
//
//#define SellMinuend_BuySubtrahend_Increase(MinuendVol,SubtrahendVol) make_tuple(TEnumSellMinuend_BuySubtrahend_Increase,MinuendVol,SubtrahendVol)
//#define BuyMinuend_SellSubtrahend_Descrease(MinuendVol,SubtrahendVol) make_tuple(TEnumBuyMinuend_SellSubtrahend_Descrease,MinuendVol,SubtrahendVol)

#define DoNothing make_tuple(TEnumDoNothing,0,0)

typedef std::tuple<TTradeSignalType, unsigned int, unsigned int> TSignal;

class API_DECLSPEC MCustomArbitrageStrategyInterface
{
public:
	
	static MCustomArbitrageStrategyInterface * Create();
	virtual TLastErrorIdType OnInit(TStrategyIdType Sid, double _legaPriceTick,double _legbPriceTick) = 0;
	virtual void OnEndUp() = 0;

	virtual TTradeSignalType GetTradeSignal(
		const ptime & CurrentUTCTime,
		const CBar & UpperSpreadBar,
		const CBar & LowerSpreadBar,
		const CBar & LastSpreadBar,
		unsigned int BarCount_Overall,
		unsigned int BarCount_Inday,
		unsigned int BarTypeSecond,
		TPositionState CurrentState,
		const CPosition & SubtrahendPosition,
		const CPosition & MinuendPosition,
		bool CanExcute,
		MStrategyContext*_StrategyContext
		) = 0;

	virtual TProbeStructType GetProbeStruct() = 0;
	virtual CParNode * GetParamStruct() = 0;
	virtual void Release() = 0;
	virtual string OnMeddle(const string &) = 0;
};


#endif
