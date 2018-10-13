// StrategyDemo.cpp : 定义 DLL 应用程序的导出函数。
//

//#include "stdafx.h"
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
#include <numeric>
#include <map>
#include <deque>
using namespace std;



class COptionArbitrageStrategy : public MStrategy
{
public:
	
	STRATEGY_TEMPLATE_DECLARE(COptionArbitrageStrategy)


		BEGIN_SERIALIZATION
			SERIALIZATION(m_ticks)
			SERIALIZATION(m_boolIsValid)
			SERIALIZATION(m_OrderRefs)
			SERIALIZATION(m_mapTradedInfo)

			SERIALIZATION(m_enumOrderState)
			SERIALIZATION(m_deqBetRiseList)
			SERIALIZATION(m_deqBetDropList)
			SERIALIZATION(m_dbTargetSpread)
			SERIALIZATION(m_intStrategyOnOff)
			//参数
			SERIALIZATION(m_dbFixedUpperLine)
			SERIALIZATION(m_dbFixedLowerLine)
			SERIALIZATION(m_intValidBetRise)
			SERIALIZATION(m_intValidBetDrop)
			SERIALIZATION(m_intInitBetRisePosition)
			SERIALIZATION(m_intInitBetDropPosition)
			SERIALIZATION(m_intVolumePerCopy)
		END_SERIALIZATION

#pragma region 自定义数据
#define MinuendCall 0
#define MinuendPut 1
#define SubtrahendCall 2
#define SubtrahendPut 3
	enum TCurrentOrderState {
		Stable,
		BetRiseEntering,
		BetDropEntering
	};
	const double m_dbPriceTick = 0.0001;
	COptionTick m_ticks[4];
	bool m_boolIsValid[4] = { false,false,false,false };
	TOrderRefIdType m_OrderRefs[4] = {-1,-1,-1,-1};
	map<TOrderRefIdType, pair<unsigned int, double> > m_mapTradedInfo;
	
	TCurrentOrderState m_enumOrderState = Stable;
	deque<double> m_deqBetRiseList;
	deque<double> m_deqBetDropList;
	double m_dbTargetSpread = 0.0;
	int m_intStrategyOnOff = 1;
	//参数
	double m_dbFixedUpperLine = 1510;
	double m_dbFixedLowerLine = 1490;
	int m_intValidBetRise = 5;
	int m_intValidBetDrop = 5;
	int m_intInitBetRisePosition = 0;
	int m_intInitBetDropPosition = 0;
	int m_intVolumePerCopy = 1;

	BEGIN_SHOW(Show)
		SHOW_DOUBLE(m_dbPriceTick)
		SHOW_INT(m_enumOrderState)
		SHOW_INT(m_deqBetRiseList.size())
		SHOW_INT(m_deqBetDropList.size())
		SHOW_DOUBLE(m_dbTargetSpread)
		SHOW_INT(m_intStrategyOnOff)
		//参数
		SHOW_DOUBLE(m_dbFixedUpperLine)
		SHOW_DOUBLE(m_dbFixedLowerLine)
		SHOW_INT(m_intValidBetRise)
		SHOW_INT(m_intValidBetDrop)
		SHOW_INT(m_intInitBetRisePosition)
		SHOW_INT(m_intInitBetDropPosition)
		SHOW_INT(m_intVolumePerCopy)
	END_SHOW
#pragma endregion

	BEGIN_PARAMETER_BIND
		PARAMETER("m_dbFixedUpperLine", LB1_DoubleType, &m_dbFixedUpperLine)
		PARAMETER("m_dbFixedLowerLine", LB1_DoubleType, &m_dbFixedLowerLine)
		PARAMETER("m_intValidBetRise", LB1_IntType, &m_intValidBetRise)
		PARAMETER("m_intValidBetDrop", LB1_IntType, &m_intValidBetDrop)
		PARAMETER("m_intInitBetRisePosition", LB1_IntType, &m_intInitBetRisePosition)
		PARAMETER("m_intInitBetDropPosition", LB1_IntType, &m_intInitBetDropPosition)
		PARAMETER("m_intVolumePerCopy", LB1_IntType, &m_intVolumePerCopy)
	END_PARAMETER_BIND



	bool IsSupport(TStrategyTickType ticktype)
	{
		return true;
	}
	
	BEGIN_PROBE_BIND
		BEGIN_GRAPH
			PROBE(&m_adbFixedUpperLine, "m_adbFixedUpperLine", TProbe_Color_Green)
			PROBE(&m_adbFixedLowerLine, "m_adbFixedLowerLine", TProbe_Color_Red)
			PROBE(&m_adbUpperSpread, "m_adbUpperSpread", TProbe_Color_Red)
			PROBE(&m_adbLowerSpread, "m_adbLowerSpread", TProbe_Color_Green)
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
	atomic<double>  m_adbFixedUpperLine;
	atomic<double>  m_adbFixedLowerLine;
	atomic<double>  m_adbUpperSpread;
	atomic<double>  m_adbLowerSpread;


};

STRATEGY_TEMPLATE_DEFINITION(COptionArbitrageStrategy)


bool COptionArbitrageStrategy::OnInquiry(MStrategyInquiryDataInterface * data)
{
	return true;
}

TLastErrorIdType COptionArbitrageStrategy::OnInit(ptime)
{
	m_adbFixedUpperLine.store(PROBE_NULL_VALUE);
	m_adbFixedLowerLine.store(PROBE_NULL_VALUE);
	m_adbUpperSpread.store(PROBE_NULL_VALUE);
	m_adbLowerSpread.store(PROBE_NULL_VALUE);

	
	
	m_boolIsValid[MinuendCall] = 
		m_boolIsValid[MinuendPut] =
		m_boolIsValid[SubtrahendCall] =
		m_boolIsValid[SubtrahendPut] = false;
	m_OrderRefs[MinuendCall] =
		m_OrderRefs[MinuendPut] =
		m_OrderRefs[SubtrahendCall] =
		m_OrderRefs[SubtrahendPut] = -1;
	

	m_enumOrderState = Stable;

	m_mapTradedInfo.clear();

	m_deqBetRiseList.clear();
	m_deqBetDropList.clear();

	for (int i = 0;i < m_intInitBetRisePosition;i++)
		m_deqBetRiseList.push_back(0.0);
	for (int i = 0;i < m_intInitBetDropPosition;i++)
		m_deqBetDropList.push_back(0.0);

	Show();
	LOG("with MAX");
	return LB1_NO_ERROR;
}

void COptionArbitrageStrategy::OnTick(TMarketDataIdType dataid, const CTick *pDepthMarketData)
{
	if (pDepthMarketData->m_dbAskPrice[0] < 10e-8
		||
		pDepthMarketData->m_dbBidPrice[0] < 10e-8
		)
		return;
	m_ticks[dataid] = *static_cast<const COptionTick*>(pDepthMarketData);
	m_boolIsValid[dataid] = true;
	if (
		false == m_boolIsValid[MinuendCall] 
		||
		false == m_boolIsValid[MinuendPut] 
		||
		false == m_boolIsValid[SubtrahendCall] 
		||
		false == m_boolIsValid[SubtrahendPut]
		)
		return;
	if (pDepthMarketData->m_datetimeUTCDateTime.time_of_day() < time_duration(1, 34, 0, 0)
		||
		pDepthMarketData->m_datetimeUTCDateTime.time_of_day() > time_duration(6, 55, 0, 0)
		)
		return;
	double UpperSpread =
		(m_ticks[MinuendCall].m_dbAskPrice[0] - m_ticks[MinuendPut].m_dbBidPrice[0]) * 10000
		-
		(m_ticks[SubtrahendCall].m_dbBidPrice[0] - m_ticks[SubtrahendPut].m_dbAskPrice[0]) * 10000;
	double LowerSpread =
		(m_ticks[MinuendCall].m_dbBidPrice[0] - m_ticks[MinuendPut].m_dbAskPrice[0]) * 10000
		-
		(m_ticks[SubtrahendCall].m_dbAskPrice[0] - m_ticks[SubtrahendPut].m_dbBidPrice[0]) * 10000;

	
	if ((m_intStrategyOnOff!=0)
		&&
		(Stable == m_enumOrderState))
	{
		TOrderDirectionType Dir[4] = { LB1_UnknownDirection ,LB1_UnknownDirection ,LB1_UnknownDirection ,LB1_UnknownDirection };
		if (
			m_intValidBetRise>0
			&&
			UpperSpread < m_dbFixedLowerLine
			)
		{
			Dir[MinuendCall] = LB1_Buy;
			Dir[MinuendPut] = LB1_Sell;
			Dir[SubtrahendCall] = LB1_Sell;
			Dir[SubtrahendPut] = LB1_Buy;
			m_enumOrderState = BetRiseEntering;
			m_dbTargetSpread = UpperSpread;
		}
		else if (
			m_intValidBetDrop>0
			&&
			LowerSpread > m_dbFixedUpperLine
			)
		{
			Dir[MinuendCall] = LB1_Sell;
			Dir[MinuendPut] = LB1_Buy;
			Dir[SubtrahendCall] = LB1_Buy;
			Dir[SubtrahendPut] = LB1_Sell;
			m_enumOrderState = BetDropEntering;
			m_dbTargetSpread = LowerSpread;
		};

		if (Dir[0] != LB1_UnknownDirection)
		{
			for (unsigned int Tar = 0;Tar < 4;Tar++)
			{
				m_OrderRefs[Tar] = LIMITORDER(
					Dir[Tar],
					LB1_Increase,
					m_intVolumePerCopy,
					Dir[Tar] == LB1_Buy
					?
					m_ticks[Tar].m_dbAskPrice[0] + m_dbPriceTick * 10
					:
					max(m_ticks[Tar].m_dbBidPrice[0] - m_dbPriceTick * 10, m_dbPriceTick),
					Tar);
			}
			m_mapTradedInfo.clear();
			m_mapTradedInfo[m_OrderRefs[MinuendCall]] =
				m_mapTradedInfo[m_OrderRefs[MinuendPut]] =
				m_mapTradedInfo[m_OrderRefs[SubtrahendCall]] =
				m_mapTradedInfo[m_OrderRefs[SubtrahendPut]] = make_pair((unsigned int)0, (double)0.0);
		}

	}
#pragma region 干预策略
	char buf[1024];
	if (true == MEDDLE(buf, 1024))
	{
		string cmd = buf;
		if ("show" == cmd)
		{
			Show();
		}
		else if ("on" == cmd)
		{
			m_intStrategyOnOff = 1;
			LOG("Strategy[%d] m_intStrategyOnOff=%d", g_StrategyId, m_intStrategyOnOff);
		}
		else if ("off" == cmd)
		{
			m_intStrategyOnOff = 0;
			LOG("Strategy[%d] m_intStrategyOnOff=%d", g_StrategyId, m_intStrategyOnOff);
		}
		else if ("inc_betrise" == cmd)
		{
			++m_intValidBetRise;
			LOG("Strategy[%d] m_intValidBetRise=%d", g_StrategyId, m_intValidBetRise);
		}
		else if ("inc_betdrop" == cmd)
		{
			++m_intValidBetDrop;
			LOG("Strategy[%d] m_intValidBetDrop=%d", g_StrategyId, m_intValidBetDrop);
		}
		else if ("dec_betrise" == cmd)
		{
			if(m_intValidBetRise>0)
				--m_intValidBetRise;
			LOG("Strategy[%d] m_intValidBetRise=%d", g_StrategyId, m_intValidBetRise);
		}
		else if ("dec_betdrop" == cmd)
		{
			if(m_intValidBetDrop>0)
				--m_intValidBetDrop;
			LOG("Strategy[%d] m_intValidBetDrop=%d", g_StrategyId, m_intValidBetDrop);
		}
		else if ("inc_upper" == cmd)
		{
			m_dbFixedUpperLine += 1.0;
		}
		else if ("dec_upper" == cmd)
		{
			m_dbFixedUpperLine -= 1.0;
		}
		else if ("inc_lower" == cmd)
		{
			m_dbFixedLowerLine += 1.0;
		}
		else if ("dec_lower" == cmd)
		{
			m_dbFixedLowerLine -= 1.0;
		}

	}
#pragma endregion
	m_adbFixedUpperLine.store(m_dbFixedUpperLine);
	m_adbFixedLowerLine.store(m_dbFixedLowerLine);
	m_adbUpperSpread.store(UpperSpread);
	m_adbLowerSpread.store(LowerSpread);

	

}


void COptionArbitrageStrategy::OnEndup()
{
}


void COptionArbitrageStrategy::OnTrade(
	TOrderRefIdType ref,
	TOrderSysIdType sys,
	TVolumeType volume,
	TPriceType price,
	TOrderDirectionType dir,
	TOrderOffsetType offset)
{
	if (m_mapTradedInfo.find(ref) != m_mapTradedInfo.end())
	{
		m_mapTradedInfo[ref].first += volume;
		m_mapTradedInfo[ref].second += volume*price;
		
		if (accumulate(m_mapTradedInfo.begin(), m_mapTradedInfo.end(), 0,
			[this](unsigned int a, decltype(m_mapTradedInfo)::value_type & b) {
			if (b.second.first == m_intVolumePerCopy)
				return a + 1;
			else return a;
		}) == m_mapTradedInfo.size())
		{//全部成交
			double TradedSpread =
				((m_mapTradedInfo[m_OrderRefs[MinuendCall]].second - m_mapTradedInfo[m_OrderRefs[MinuendPut]].second)
					-
					(m_mapTradedInfo[m_OrderRefs[SubtrahendCall]].second - m_mapTradedInfo[m_OrderRefs[SubtrahendPut]].second))
				/
				m_intVolumePerCopy * 10000;
			if (BetRiseEntering == m_enumOrderState)
			{
				m_deqBetRiseList.push_back(TradedSpread);
				m_intValidBetRise--;
				LOG("OptionArbitrage[%d] BetRised:%lf(%lf) Valid:%d", g_StrategyId, TradedSpread, TradedSpread - m_dbTargetSpread, m_intValidBetRise);
			}
			else if (BetDropEntering == m_enumOrderState)
			{
				m_deqBetDropList.push_back(TradedSpread);
				m_intValidBetDrop--;
				LOG("OptionArbitrage[%d] BetDroped:%lf(%lf) Valid:%d", g_StrategyId, TradedSpread, m_dbTargetSpread - TradedSpread, m_intValidBetDrop);
			}
			m_enumOrderState = Stable;
		}
	}
	
}

void COptionArbitrageStrategy::OnOrder(
	TOrderRefIdType ref,
	TOrderSysIdType sysId,
	TOrderDirectionType direction,
	TOrderStatusType Status,
	TPriceType LimitPrice,
	TTradedVolumeType VolumeTraded,
	TRemainVolumeType VolumeRemain)
{
}
