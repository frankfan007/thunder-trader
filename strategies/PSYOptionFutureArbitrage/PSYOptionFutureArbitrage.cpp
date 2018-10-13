#include "Order.h"
#include "StrategyDefine.h"
#include "StrategyData.h"
#include "Tick.h"
#include "StockTick.h"
#include "FutureTick.h"
#include "OptionTick.h"
#include <cmath>
#include <list>
#include <string>
#include <fstream>
#include <utility> 
#include <algorithm>
#include <numeric>
#include <tuple>
#include <functional>
#include <map>
using namespace std;
#define OPTION_LIMIT_ORDER_PRICE_LEVEL 4
enum _PositionType { _BetSpreadRisePosition, _BetSpreadDropPosition };
enum TOrderState
{
	ORDER_STATE_SellFutureBuyCallSellPut_Increase_SellFuture,
	ORDER_STATE_SellFutureBuyCallSellPut_Increase_BuyCallSellPut,
	ORDER_STATE_BuyFutureSellCallBuyPut_Increase_BuyFuture,
	ORDER_STATE_BuyFutureSellCallBuyPut_Increase_SellCallBuyPut,
	ORDER_STATE_BuyFutureSellCallBuyPut_Descrease_BuyFuture,
	ORDER_STATE_BuyFutureSellCallBuyPut_Descrease_SellCallBuyPut,
	ORDER_STATE_SellFutureBuyCallSellPut_Descrease_SellFuture,
	ORDER_STATE_SellFutureBuyCallSellPut_Descrease_BuyCallSellPut,
	ORDER_STATE_Free
};
#define FutureDataid 0
#define CallOptionDataid 1
#define PutOptionDataid 2
#define I_LowerSpread 0
#define I_UpperSpread 1

class CBasketElement
{
public:
	class CTarget
	{
	public:
		TOrderDirectionType m_enumDirection;
		TOrderOffsetType m_enumOffset;
		TVolumeType m_intTargetVol;
		CTick * m_ReferenceTickLink;

		BEGIN_SERIALIZATION
			SERIALIZATION(m_enumDirection)
			SERIALIZATION(m_enumOffset)
			SERIALIZATION(m_intTargetVol)
			SERIALIZATION(m_ReferenceTickLink)
		END_SERIALIZATION
	};
	class CCurrentState
	{
	public:
		ptime m_ptimeLimitOrderTime;
		TOrderRefIdType m_intCurrentRefid;
		string m_strOrderSysid;
		TVolumeType m_intTradedVol;
		TPriceType m_dbTradedTurnover;

		BEGIN_SERIALIZATION
			SERIALIZATION(m_ptimeLimitOrderTime)
			SERIALIZATION(m_intCurrentRefid)
			SERIALIZATION(m_strOrderSysid)
			SERIALIZATION(m_intTradedVol)
			SERIALIZATION(m_dbTradedTurnover)
		END_SERIALIZATION

	};

	
	TMarketDataIdType m_uTargetDataid;
	CTarget m_Target;
	CCurrentState m_Current;
	bool IsAlltraded() { return m_Current.m_intTradedVol == m_Target.m_intTargetVol; }
	CBasketElement():CBasketElement(ptime(not_a_date_time),0){}
	CBasketElement(const ptime& CurTime,TMarketDataIdType Dataid)
	{
		m_uTargetDataid = Dataid;
		m_Current.m_ptimeLimitOrderTime = CurTime;
		m_Current.m_intTradedVol = 0;
		m_Current.m_dbTradedTurnover = 0.0;
		m_Current.m_strOrderSysid = "";
	}

	BEGIN_SERIALIZATION
		SERIALIZATION(m_uTargetDataid)
		SERIALIZATION(m_Target)
		SERIALIZATION(m_Current)
	END_SERIALIZATION

	
};

typedef map<TOrderRefIdType, CBasketElement> TBasketType;


bool IsBasketAllTraded(TBasketType &basket)
{
	for (auto & Ele : basket)
		if (Ele.second.IsAlltraded() == false)
			return false;
	return true;
}

class CPSYOptionFutureArbitrage : public MStrategy
{
	
public:
	
	STRATEGY_TEMPLATE_DECLARE(CPSYOptionFutureArbitrage)

	BEGIN_SERIALIZATION
#pragma region 全局变量
	SERIALIZATION(m_dbLongSpread)
	SERIALIZATION(m_dbShortSpread)

	SERIALIZATION(m_FutureDataComplete)
	SERIALIZATION(m_OpCallDataComplete)
	SERIALIZATION(m_OpPutDataComplete)

	SERIALIZATION(m_enumCurrentState)

	SERIALIZATION(m_FutureTick)
	SERIALIZATION(m_CallOptionTick)
	SERIALIZATION(m_PutOptionTick)
	SERIALIZATION(m_bskOptionOrderBasket)
	SERIALIZATION(m_eleFutureOrderBasket)

	SERIALIZATION(m_ptimeGlobalUTCTime)
	SERIALIZATION(m_intValidBetRiseCount)
	SERIALIZATION(m_intValidBetDropCount)
	SERIALIZATION(m_mapPositionRecord)
	SERIALIZATION(m_ptimeLastTradeTime)
#pragma endregion

#pragma region 参数
	SERIALIZATION(m_dbStrike)//行权价格
	SERIALIZATION(m_dbToBetSpreadWillRise_Threshold)//
	SERIALIZATION(m_dbToBetSpreadWillDrop_Threshold)//
	SERIALIZATION(m_dbSpreadHasRisenSoClear_Threshold)//
	SERIALIZATION(m_dbSpreadHasDroppedSoClear_Threshold)//
	SERIALIZATION(m_intOptionLot)
	SERIALIZATION(m_intFutureLot)
	SERIALIZATION(m_intInitMaxBetSpreadRiseNumPerday)//最多做多价差的次数
	SERIALIZATION(m_intInitMaxBetSpreadDropNumPerday)//最多的做空价差的次数
	SERIALIZATION(m_intYestodayBetSpreadRisePosition)//价差多头昨仓
	SERIALIZATION(m_intYestodayBetSpreadDropPosition)//价差空头昨仓
	SERIALIZATION(m_intTodayBetSpreadRisePosition)//价差多头昨仓
	SERIALIZATION(m_intTodayBetSpreadDropPosition)//价差空头昨仓
	SERIALIZATION(m_boolStrategyState)//策略是否启动
#pragma endregion
	END_SERIALIZATION
		const map<string, function<void()>> m_mapMeddleMap=
	{
		{"increase_betspreadwillrise_threshold",		[this] {m_dbToBetSpreadWillRise_Threshold		+= 100.0;}},
		{"increase_spreadhasrisensoclear_threshold",	[this] {m_dbSpreadHasRisenSoClear_Threshold		+= 100.0;}},
		{"increase_betspreadwilldrop_threshold",		[this] {m_dbToBetSpreadWillDrop_Threshold		+= 100.0;}},
		{"increase_spreadhasdroppedsoclear_threshold",	[this] {m_dbSpreadHasDroppedSoClear_Threshold	+= 100.0;}},
		{"decrease_betspreadwillrise_threshold",		[this] {m_dbToBetSpreadWillRise_Threshold		-= 100.0;}},
		{"decrease_spreadhasrisensoclear_threshold",	[this] {m_dbSpreadHasRisenSoClear_Threshold		-= 100.0;}},
		{"decrease_betspreadwilldrop_threshold",		[this] {m_dbToBetSpreadWillDrop_Threshold		-= 100.0;}},
		{"decrease_spreadhasdroppedsoclear_threshold",	[this] {m_dbSpreadHasDroppedSoClear_Threshold	-= 100.0;}},
		{"off",		[this] {m_boolStrategyState = false;	LOG("psy: StrategyOff");	}},
		{"on",		[this] {m_boolStrategyState = true;		LOG("psy: StrategyOn");		}},
		{"increase_validbetrisecount",	[this] {if (m_intValidBetRiseCount < 5)	++m_intValidBetRiseCount;	LOG("psy: m_intValidBetRiseCount:%d", m_intValidBetRiseCount);}},
		{"decrease_validbetrisecount",	[this] {if (m_intValidBetRiseCount > 0)	--m_intValidBetRiseCount;	LOG("psy: m_intValidBetRiseCount:%d", m_intValidBetRiseCount);}},
		{"increase_validbetdropcount",	[this] {if (m_intValidBetDropCount < 5)	++m_intValidBetDropCount;	LOG("psy: m_intValidBetDropCount:%d", m_intValidBetDropCount);}},
		{"decrease_validbetdropcount",	[this] {if (m_intValidBetDropCount > 0)	--m_intValidBetDropCount;	LOG("psy: m_intValidBetDropCount:%d", m_intValidBetDropCount);}},
		{"show",	[this] {Show();ShowDetailPosition();}
		}
	};
	bool IsSupport(TStrategyTickType ticktype)
	{
		return true;
	}
	
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
		TVolumeType /*VolumeTraded*/,
		TVolumeType /*VolumeTotal*/
		);
	void OnEndup();
	TLastErrorIdType OnInit_FromArchive(ptime)
	{
		Show();
		return TLastErrorIdType::LB1_NO_ERROR;
	};



#define P_Yesterday_Bet_Rise 0
#define P_Yesterday_Bet_Drop 1
#define P_Today_Bet_Rise 2
#define P_Today_Bet_Drop 3
	tuple<int, int, int, int> GetPosition();
	
	void ShowDetailPosition()
	{
		if (m_mapPositionRecord.empty())
			LOG("Strategy[%d]: NoYesterdayPosition");
		else
		{
			for (auto & P : m_mapPositionRecord)
			{
				LOG(
					"Strategy[%d]: %s BetSpreadRise:%d BetSpreadDrop:%d",
					g_StrategyId,
					to_simple_string(P.first).c_str(),
					P.second[_BetSpreadRisePosition],
					P.second[_BetSpreadDropPosition]
					);
			}
			auto posi = GetPosition();
			LOG("Strategy[%d]: Yesterday[%d %d] Today[%d,%d]",
				g_StrategyId,
				get<P_Yesterday_Bet_Rise>(posi),
				get<P_Yesterday_Bet_Drop>(posi),
				get<P_Today_Bet_Rise>(posi),
				get<P_Today_Bet_Drop>(posi)
				);
		}
	}

	tuple<double, double> ComputeSpread(
		const CFutureTick * my_FutureTick,
		const COptionTick * my_CallOpTick,
		const COptionTick * my_PutOpTick
		);
	BEGIN_PARAMETER_BIND
		PARAMETER("Strike", LB1_DoubleType, &m_dbStrike)
		PARAMETER("m_intOptionLot", LB1_IntType, &m_intOptionLot)
		PARAMETER("m_intFutureLot", LB1_IntType, &m_intFutureLot)
		PARAMETER("m_dbToBetSpreadWillRise_Threshold", LB1_DoubleType, &m_dbToBetSpreadWillRise_Threshold)
		PARAMETER("m_dbToBetSpreadWillDrop_Threshold", LB1_DoubleType, &m_dbToBetSpreadWillDrop_Threshold)
		PARAMETER("m_dbSpreadHasRisenSoClear_Threshold", LB1_DoubleType, &m_dbSpreadHasRisenSoClear_Threshold)
		PARAMETER("m_dbSpreadHasDroppedSoClear_Threshold", LB1_DoubleType, &m_dbSpreadHasDroppedSoClear_Threshold)
		PARAMETER("m_intYestodayBetSpreadRisePosition", LB1_IntType, &m_intYestodayBetSpreadRisePosition)
		PARAMETER("m_intYestodayBetSpreadDropPosition", LB1_IntType, &m_intYestodayBetSpreadDropPosition)
		PARAMETER("m_intTodayBetSpreadRisePosition", LB1_IntType, &m_intTodayBetSpreadRisePosition)
		PARAMETER("m_intTodayBetSpreadDropPosition", LB1_IntType, &m_intTodayBetSpreadDropPosition)
		PARAMETER("m_intInitMaxBetSpreadRiseNumPerday", LB1_IntType, &m_intInitMaxBetSpreadRiseNumPerday)
		PARAMETER("m_intInitMaxBetSpreadDropNumPerday", LB1_IntType, &m_intInitMaxBetSpreadDropNumPerday)
	END_PARAMETER_BIND

	BEGIN_PROBE_BIND
		BEGIN_GRAPH
			PROBE(&m_adbToBetSpreadWillRise, "m_adbToBetSpreadWillRise", TProbe_Color_Red)
			PROBE(&m_adbToBetSpreadWillDrop, "m_adbToBetSpreadWillDrop", TProbe_Color_Green)
			PROBE(&m_adbSpreadHasRisenSoClear, "m_adbSpreadHasRisenSoClear", TProbe_Color_Red_Weak)
			PROBE(&m_adbSpreadHasDroppedSoClear, "m_adbSpreadHasDroppedSoClear", TProbe_Color_Green_Weak)
			PROBE(&m_adbUpperSpread, "m_adbUpperSpread", TProbe_Color_Red)
			PROBE(&m_adbLowerSpread, "m_adbLowerSpread", TProbe_Color_Green)
		END_GRAPH("Line")
		/*BEGIN_GRAPH
			PROBE(&m_adbFutureAsk, "m_adbFutureAsk", TProbe_Color_Red)
			PROBE(&m_adbFutureBid, "m_adbFutureBid", TProbe_Color_Green)
		END_GRAPH("Line")
		BEGIN_GRAPH
			PROBE(&m_adbCallAsk, "m_adbCallAsk", TProbe_Color_Red)
			PROBE(&m_adbCallBid, "m_adbCallBid", TProbe_Color_Green)
		END_GRAPH("Line")
		BEGIN_GRAPH
			PROBE(&m_adbPutAsk, "m_adbPutAsk", TProbe_Color_Red)
			PROBE(&m_adbPutBid, "m_adbPutBid", TProbe_Color_Green)
		END_GRAPH("Line")*/
		
	END_PROBE_BIND

	BEGIN_SHOW(Show)
#pragma region 一些常量
		SHOW_INT(MaxCountPerLimitOrder)
		SHOW_UINT(OptionQty)
		SHOW_DOUBLE(OptionPriceTick)
		SHOW_UINT(FutureQty)
#pragma endregion

#pragma region 全局变量
		SHOW_DOUBLE(m_dbLongSpread)
		SHOW_DOUBLE(m_dbShortSpread)

		SHOW_UINT(m_FutureDataComplete)
		SHOW_UINT(m_OpCallDataComplete)
		SHOW_UINT(m_OpPutDataComplete)

		SHOW_INT(m_enumCurrentState)

		SHOW_UINT(m_bskOptionOrderBasket.size());
		SHOW_STRING(to_simple_string(m_ptimeGlobalUTCTime).c_str())
		SHOW_INT(m_intValidBetRiseCount)
		SHOW_INT(m_intValidBetDropCount)
		SHOW_UINT(m_mapPositionRecord.size())
#pragma endregion

#pragma region 参数
		SHOW_DOUBLE(m_dbStrike)
		SHOW_DOUBLE(m_dbToBetSpreadWillRise_Threshold)
		SHOW_DOUBLE(m_dbToBetSpreadWillDrop_Threshold)
		SHOW_DOUBLE(m_dbSpreadHasRisenSoClear_Threshold)
		SHOW_DOUBLE(m_dbSpreadHasDroppedSoClear_Threshold)
		SHOW_INT(m_intOptionLot)
		SHOW_INT(m_intFutureLot)
		SHOW_INT(m_intInitMaxBetSpreadRiseNumPerday)
		SHOW_INT(m_intInitMaxBetSpreadDropNumPerday)
		SHOW_INT(m_intYestodayBetSpreadRisePosition)
		SHOW_INT(m_intYestodayBetSpreadDropPosition)
		SHOW_INT(m_intTodayBetSpreadRisePosition)
		SHOW_INT(m_intTodayBetSpreadDropPosition)
		SHOW_INT(m_boolStrategyState)
#pragma endregion
	END_SHOW
public:

#pragma region 绘图变量
	atomic<double> m_adbToBetSpreadWillRise;
	atomic<double> m_adbToBetSpreadWillDrop;
	atomic<double> m_adbSpreadHasRisenSoClear;
	atomic<double> m_adbSpreadHasDroppedSoClear;
	atomic<double> m_adbUpperSpread;
	atomic<double> m_adbLowerSpread;

	/*atomic<double> m_adbFutureAsk;
	atomic<double> m_adbFutureBid;
	atomic<double> m_adbCallAsk;
	atomic<double> m_adbCallBid;
	atomic<double> m_adbPutAsk;
	atomic<double> m_adbPutBid;*/
#pragma endregion

#pragma region 一些常量
	const TVolumeType MaxCountPerLimitOrder = 10;
	const unsigned int OptionQty = 10000;
	const TPriceType OptionPriceTick = 0.0001;
	const unsigned int FutureQty = 300;
#pragma endregion

#pragma region 全局变量
	double m_dbLongSpread;
	double m_dbShortSpread;

	bool m_FutureDataComplete;
	bool m_OpCallDataComplete;
	bool m_OpPutDataComplete;

	TOrderState m_enumCurrentState;

	CFutureTick m_FutureTick;
	COptionTick m_CallOptionTick;
	COptionTick m_PutOptionTick;
	TBasketType m_bskOptionOrderBasket;
	CBasketElement m_eleFutureOrderBasket;

	ptime m_ptimeGlobalUTCTime;
	int m_intValidBetRiseCount;
	int m_intValidBetDropCount;
	map < date, map<_PositionType, int > > m_mapPositionRecord;
	ptime m_ptimeLastTradeTime=min_date_time;
#pragma endregion

#pragma region 参数
	double	m_dbStrike = 2.15;//行权价格
	double	m_dbToBetSpreadWillRise_Threshold = 0;//
	double	m_dbToBetSpreadWillDrop_Threshold =3000;//
	double	m_dbSpreadHasRisenSoClear_Threshold =2500;//
	double	m_dbSpreadHasDroppedSoClear_Threshold =500;//
	int		m_intOptionLot = 30;
	int		m_intFutureLot = 1;
	int		m_intInitMaxBetSpreadRiseNumPerday = 3;//最多做多价差的次数
	int		m_intInitMaxBetSpreadDropNumPerday = 3;//最多的做空价差的次数
	int		m_intYestodayBetSpreadRisePosition = 0;//价差多头昨仓
	int		m_intYestodayBetSpreadDropPosition = 0;//价差空头昨仓
	int		m_intTodayBetSpreadRisePosition = 0;//价差多头昨仓
	int		m_intTodayBetSpreadDropPosition = 0;//价差空头昨仓
	bool	m_boolStrategyState = true;//策略是否启动
#pragma endregion

};

STRATEGY_TEMPLATE_DEFINITION(CPSYOptionFutureArbitrage)

bool CPSYOptionFutureArbitrage::OnInquiry(MStrategyInquiryDataInterface * data)
{
	return true;
}

TLastErrorIdType CPSYOptionFutureArbitrage::OnInit(ptime CurrentTime)
{

#pragma region 全局变量
	m_dbLongSpread = 0;
	m_dbShortSpread = 0;

	m_FutureDataComplete=false;
	m_OpCallDataComplete=false;
	m_OpPutDataComplete=false;

	m_enumCurrentState= ORDER_STATE_Free;

	
	m_bskOptionOrderBasket.clear();
	 

	m_ptimeGlobalUTCTime= CurrentTime;
	m_mapPositionRecord.clear();
	m_intValidBetRiseCount=m_intInitMaxBetSpreadRiseNumPerday;
	m_intValidBetDropCount=m_intInitMaxBetSpreadDropNumPerday;
#pragma endregion
	m_mapPositionRecord[m_ptimeGlobalUTCTime.date() - days(1)][_BetSpreadRisePosition] = m_intYestodayBetSpreadRisePosition;
	m_mapPositionRecord[m_ptimeGlobalUTCTime.date() - days(1)][_BetSpreadDropPosition] = m_intYestodayBetSpreadDropPosition;
	m_mapPositionRecord[m_ptimeGlobalUTCTime.date()][_BetSpreadRisePosition] = m_intTodayBetSpreadRisePosition;
	m_mapPositionRecord[m_ptimeGlobalUTCTime.date()][_BetSpreadDropPosition] = m_intTodayBetSpreadDropPosition;
	ShowDetailPosition();
	return LB1_NO_ERROR;
}

void CPSYOptionFutureArbitrage::OnTick(TMarketDataIdType dataid, const CTick *tick)
{
	
	if (tick->m_datetimeUTCDateTime.date() != m_ptimeGlobalUTCTime.date())
	{
		m_intValidBetRiseCount = m_intInitMaxBetSpreadRiseNumPerday;
		m_intValidBetDropCount = m_intInitMaxBetSpreadDropNumPerday;

	}
	m_ptimeGlobalUTCTime = tick->m_datetimeUTCDateTime;
	

	switch (dataid)
	{
	case 0:
	{
		m_FutureTick = *static_cast<const CFutureTick*>(tick);
		m_FutureDataComplete = true;
	}
		break;
	case 1:
	{
		
		m_CallOptionTick = *static_cast<const COptionTick*>(tick);
		if (
			m_CallOptionTick.m_dbAskPrice[0] < 10e-8
			||
			m_CallOptionTick.m_dbBidPrice[0] < 10e-8)
			return;

		for (unsigned int i = 1;i < MAX_QUOTATIONS_DEPTH;i++)
		{//检测数据五档行情是否有错误
			if (abs(m_CallOptionTick.m_dbAskPrice[i]) < 10e-5)
				m_CallOptionTick.m_dbAskPrice[i] = m_CallOptionTick.m_dbAskPrice[i - 1] + 2 * OptionPriceTick;
			if (abs(m_CallOptionTick.m_dbBidPrice[i]) < 10e-5)
				m_CallOptionTick.m_dbBidPrice[i] = m_CallOptionTick.m_dbBidPrice[i - 1] - 2 * OptionPriceTick;
		}
		m_OpCallDataComplete = true;
	}
		break;
	case 2:
	{
		m_PutOptionTick = *static_cast<const COptionTick*>(tick);
		if (
			m_PutOptionTick.m_dbAskPrice[0] < 10e-8
			||
			m_PutOptionTick.m_dbBidPrice[0] < 10e-8)
			return;

		for (unsigned int i = 1;i < MAX_QUOTATIONS_DEPTH;i++)
		{//检测数据五档行情是否有错误
			if (abs(m_PutOptionTick.m_dbAskPrice[i]) < 10e-5)
				m_PutOptionTick.m_dbAskPrice[i] = m_PutOptionTick.m_dbAskPrice[i - 1] + 2 * OptionPriceTick;
			if (abs(m_PutOptionTick.m_dbBidPrice[i]) < 10e-5)
				m_PutOptionTick.m_dbBidPrice[i] = m_PutOptionTick.m_dbBidPrice[i - 1] - 2 * OptionPriceTick;
		}
		m_OpPutDataComplete = true;
	}
		break;
	}

	
	if (
		m_ptimeGlobalUTCTime.time_of_day() < time_duration(1, 31, 15, 0)
		||
		m_ptimeGlobalUTCTime.time_of_day() > time_duration(6, 55, 0, 0))
		return;
	if (true == m_FutureDataComplete
		&&
		true == m_OpCallDataComplete
		&&
		true == m_OpPutDataComplete)
	{

		//计算价差
		auto Spread=ComputeSpread(&m_FutureTick, &m_CallOptionTick, &m_PutOptionTick);
		
		m_adbToBetSpreadWillRise.store(m_dbToBetSpreadWillRise_Threshold);
		m_adbToBetSpreadWillDrop.store(m_dbToBetSpreadWillDrop_Threshold);
		m_adbSpreadHasRisenSoClear.store(m_dbSpreadHasRisenSoClear_Threshold);
		m_adbSpreadHasDroppedSoClear.store(m_dbSpreadHasDroppedSoClear_Threshold);
		m_adbUpperSpread.store(get<I_UpperSpread>(Spread));
		m_adbLowerSpread.store(get<I_LowerSpread>(Spread));

		if (true == m_boolStrategyState)
		{
			switch (m_enumCurrentState)
			{
			case ORDER_STATE_Free:
			{
				//如果距离上一次下单还未超过3秒钟，就不进行第二次下单
				if (tick->m_datetimeUTCDateTime - m_ptimeLastTradeTime < time_duration(0, 0, 3, 0))
					break;
				auto Direction = LB1_UnknownDirection;
				auto Offset = LB1_UnknownOffset;
				
				if (get<I_UpperSpread>(Spread) < m_dbSpreadHasDroppedSoClear_Threshold)//寻找是否有机会平昨做空价差仓位
				{
					auto posi = GetPosition();
					if(
						0 < get<P_Yesterday_Bet_Drop>(posi)//
						&&
						0 == get<P_Today_Bet_Drop>(posi)
						)
					{
						Direction = LB1_Sell;
						Offset = LB1_Decrease;
						m_enumCurrentState = ORDER_STATE_SellFutureBuyCallSellPut_Descrease_SellFuture;
						goto LimitOrder;
					}
				}
				
				if (get<I_LowerSpread>(Spread) > m_dbSpreadHasRisenSoClear_Threshold)//寻找是否有机会平昨做多价差仓位)
				{//如果今天还没有开仓，才可以平仓
					auto posi = GetPosition();
					if (
						0 < get<P_Yesterday_Bet_Rise>(posi)
						&&
						0 == get<P_Today_Bet_Rise>(posi)
						)
					{
						Direction = LB1_Buy;
						Offset = LB1_Decrease;
						m_enumCurrentState = ORDER_STATE_BuyFutureSellCallBuyPut_Descrease_BuyFuture;
						goto LimitOrder;
					}
				}
				
				if (get<I_LowerSpread>(Spread) > m_dbToBetSpreadWillDrop_Threshold) //寻找是否有机会做空价差，建立新仓位
				{
					if (m_intValidBetDropCount > 0)
					{
						Direction = LB1_Buy;
						Offset = LB1_Increase;
						m_enumCurrentState = ORDER_STATE_BuyFutureSellCallBuyPut_Increase_BuyFuture;
						goto LimitOrder;
					}
				}
				
				if (get<I_UpperSpread>(Spread) < m_dbToBetSpreadWillRise_Threshold)//寻找是否有机会做多价差，建立新仓位
				{
					if (m_intValidBetRiseCount > 0)
					{
						Direction = LB1_Sell;
						Offset = LB1_Increase;
						m_enumCurrentState = ORDER_STATE_SellFutureBuyCallSellPut_Increase_SellFuture;
						goto LimitOrder;
					}
				}
LimitOrder:
				if ((LB1_UnknownDirection != Direction) && (LB1_UnknownOffset != Offset))
				{	
					m_eleFutureOrderBasket= CBasketElement(m_ptimeGlobalUTCTime, FutureDataid);
					m_eleFutureOrderBasket.m_Current.m_intCurrentRefid = LIMITORDER(
						m_eleFutureOrderBasket.m_Target.m_enumDirection = Direction,
						m_eleFutureOrderBasket.m_Target.m_enumOffset = Offset,
						m_eleFutureOrderBasket.m_Target.m_intTargetVol = m_intFutureLot,
						Direction == LB1_Buy ?
						(m_eleFutureOrderBasket.m_Target.m_ReferenceTickLink = &m_FutureTick)->m_dbAskPrice[0]
						:
						(m_eleFutureOrderBasket.m_Target.m_ReferenceTickLink = &m_FutureTick)->m_dbBidPrice[0],
						m_eleFutureOrderBasket.m_uTargetDataid);
					m_ptimeLastTradeTime = tick->m_datetimeUTCDateTime;
				}
			};
			break;
			case ORDER_STATE_SellFutureBuyCallSellPut_Increase_SellFuture:
			case ORDER_STATE_BuyFutureSellCallBuyPut_Increase_BuyFuture:
			case ORDER_STATE_BuyFutureSellCallBuyPut_Descrease_BuyFuture:
			case ORDER_STATE_SellFutureBuyCallSellPut_Descrease_SellFuture:
			{
				
				if (
					m_eleFutureOrderBasket.m_Current.m_intTradedVol<m_eleFutureOrderBasket.m_Target.m_intTargetVol
					&&
					m_ptimeGlobalUTCTime - m_eleFutureOrderBasket.m_Current.m_ptimeLimitOrderTime>milliseconds(3000)
					&&
					m_eleFutureOrderBasket.m_Current.m_strOrderSysid.empty() == false
					)
				{
					CANCEL(
						m_eleFutureOrderBasket.m_Current.m_intCurrentRefid,
						(char *)m_eleFutureOrderBasket.m_Current.m_strOrderSysid.c_str(),
						m_eleFutureOrderBasket.m_uTargetDataid);
				}
			};
			break;
			case ORDER_STATE_SellFutureBuyCallSellPut_Increase_BuyCallSellPut:
			case ORDER_STATE_BuyFutureSellCallBuyPut_Increase_SellCallBuyPut:
			case ORDER_STATE_BuyFutureSellCallBuyPut_Descrease_SellCallBuyPut:
			case ORDER_STATE_SellFutureBuyCallSellPut_Descrease_BuyCallSellPut:
			{
				for (auto & order : m_bskOptionOrderBasket)
				{
					if (
						false == order.second.IsAlltraded()
						&&
						m_ptimeGlobalUTCTime - order.second.m_Current.m_ptimeLimitOrderTime>milliseconds(3000)
						)
					{
						CANCEL(
							order.first, 
							(char *)order.second.m_Current.m_strOrderSysid.c_str(), 
							order.second.m_uTargetDataid);
					}
				}
			}
			break;
			}
		}
	}

#pragma region 干预策略
	char buf[1024];
	if (false == MEDDLE(buf, 1024))
		return;
	string cmd = buf;
	auto FindResult = m_mapMeddleMap.find(cmd);
	if (m_mapMeddleMap.end() != FindResult)
		(FindResult->second)();
#pragma endregion
}

void CPSYOptionFutureArbitrage::OnTrade(
	TOrderRefIdType ref,
	TOrderSysIdType sys,
	TVolumeType volume,
	TPriceType price,
	TOrderDirectionType dir,
	TOrderOffsetType offset)
{
	if (ref == m_eleFutureOrderBasket.m_Current.m_intCurrentRefid)
	{
		m_eleFutureOrderBasket.m_Current.m_intTradedVol += volume;
		m_eleFutureOrderBasket.m_Current.m_dbTradedTurnover += price*double(volume);
		if (m_eleFutureOrderBasket.IsAlltraded())
		{
			m_bskOptionOrderBasket.clear();
			TOrderDirectionType CallDir;
			TOrderDirectionType PutDir;
			TOrderOffsetType Offset;

			switch (m_enumCurrentState)
			{
			case ORDER_STATE_SellFutureBuyCallSellPut_Increase_SellFuture:
				CallDir = LB1_Buy;PutDir = LB1_Sell;Offset = LB1_Increase;
				m_enumCurrentState = ORDER_STATE_SellFutureBuyCallSellPut_Increase_BuyCallSellPut;
				break;
			case ORDER_STATE_BuyFutureSellCallBuyPut_Increase_BuyFuture:
				CallDir = LB1_Sell;PutDir = LB1_Buy;Offset = LB1_Increase;
				m_enumCurrentState = ORDER_STATE_BuyFutureSellCallBuyPut_Increase_SellCallBuyPut;
				break;
			case ORDER_STATE_BuyFutureSellCallBuyPut_Descrease_BuyFuture:
				CallDir = LB1_Sell;PutDir = LB1_Buy;Offset = LB1_Decrease;
				m_enumCurrentState = ORDER_STATE_BuyFutureSellCallBuyPut_Descrease_SellCallBuyPut;
				break;
			case ORDER_STATE_SellFutureBuyCallSellPut_Descrease_SellFuture:
				CallDir = LB1_Buy;PutDir = LB1_Sell;Offset = LB1_Decrease;
				m_enumCurrentState = ORDER_STATE_SellFutureBuyCallSellPut_Descrease_BuyCallSellPut;
				break;
			}
			for (unsigned int i = 0;i < m_intOptionLot/MaxCountPerLimitOrder;i++)
			{
				char buf[64];
				CBasketElement eleCall(m_ptimeGlobalUTCTime, CallOptionDataid);
				eleCall.m_Current.m_intCurrentRefid = LIMITORDER(
					eleCall.m_Target.m_enumDirection = CallDir,
					eleCall.m_Target.m_enumOffset = Offset,
					eleCall.m_Target.m_intTargetVol = MaxCountPerLimitOrder,
					LB1_Buy == CallDir ?
					(eleCall.m_Target.m_ReferenceTickLink = &m_CallOptionTick)->m_dbAskPrice[OPTION_LIMIT_ORDER_PRICE_LEVEL]
					:
					(eleCall.m_Target.m_ReferenceTickLink = &m_CallOptionTick)->m_dbBidPrice[OPTION_LIMIT_ORDER_PRICE_LEVEL],
					eleCall.m_uTargetDataid);
				sprintf(buf, "%lld", eleCall.m_Current.m_intCurrentRefid);
				eleCall.m_Current.m_strOrderSysid = buf;

				CBasketElement elePut(m_ptimeGlobalUTCTime, PutOptionDataid);
				elePut.m_Current.m_intCurrentRefid = LIMITORDER(
					elePut.m_Target.m_enumDirection = PutDir,
					elePut.m_Target.m_enumOffset = Offset,
					elePut.m_Target.m_intTargetVol = MaxCountPerLimitOrder,
					LB1_Buy == PutDir?
					(elePut.m_Target.m_ReferenceTickLink = &m_PutOptionTick)->m_dbAskPrice[OPTION_LIMIT_ORDER_PRICE_LEVEL]
					:
					(elePut.m_Target.m_ReferenceTickLink = &m_PutOptionTick)->m_dbBidPrice[OPTION_LIMIT_ORDER_PRICE_LEVEL],
					elePut.m_uTargetDataid);
				sprintf(buf, "%lld", elePut.m_Current.m_intCurrentRefid);
				elePut.m_Current.m_strOrderSysid = buf;
				m_bskOptionOrderBasket[eleCall.m_Current.m_intCurrentRefid] = eleCall;
				m_bskOptionOrderBasket[elePut.m_Current.m_intCurrentRefid] = elePut;

			}
		}
	}
	else
	{
		auto FindResult = m_bskOptionOrderBasket.find(ref);
		if (m_bskOptionOrderBasket.end() != FindResult)
		{
			FindResult->second.m_Current.m_intTradedVol += volume;
			FindResult->second.m_Current.m_dbTradedTurnover += volume*price;
		}
		if (IsBasketAllTraded(m_bskOptionOrderBasket))
		{
			switch (m_enumCurrentState)
			{
			case ORDER_STATE_SellFutureBuyCallSellPut_Increase_BuyCallSellPut:
				m_mapPositionRecord[m_ptimeGlobalUTCTime.date()][_BetSpreadRisePosition]++;
				m_intValidBetRiseCount--;
				break;
			case ORDER_STATE_BuyFutureSellCallBuyPut_Increase_SellCallBuyPut:
				m_mapPositionRecord[m_ptimeGlobalUTCTime.date()][_BetSpreadDropPosition]++;
				m_intValidBetDropCount--;
				break;
			case ORDER_STATE_BuyFutureSellCallBuyPut_Descrease_SellCallBuyPut:
				m_mapPositionRecord[m_ptimeGlobalUTCTime.date() - days(1)][_BetSpreadRisePosition]--;
				break;
			case ORDER_STATE_SellFutureBuyCallSellPut_Descrease_BuyCallSellPut:
				m_mapPositionRecord[m_ptimeGlobalUTCTime.date() - days(1)][_BetSpreadDropPosition]--;
				break;
			default:;
			}
			ShowDetailPosition();
			m_enumCurrentState = ORDER_STATE_Free;
			double CallTradedAvgPrice=accumulate(
				m_bskOptionOrderBasket.begin(),
				m_bskOptionOrderBasket.end(),0.0,
				[](double a, TBasketType::value_type & b)->double
			{
				if (CallOptionDataid == b.second.m_uTargetDataid)
					return a += b.second.m_Current.m_dbTradedTurnover;
				else
					return a;
			}
			)/
				accumulate(
					m_bskOptionOrderBasket.begin(),
					m_bskOptionOrderBasket.end(),0,
					[](int a, TBasketType::value_type & b)->int
			{
				if (CallOptionDataid == b.second.m_uTargetDataid)
					return a += b.second.m_Current.m_intTradedVol;
				else
					return a;
			}
			);
			double PutTradedAvgPrice = accumulate(
				m_bskOptionOrderBasket.begin(),
				m_bskOptionOrderBasket.end(), 0.0,
				[](double a, TBasketType::value_type & b)->double
			{
				if (PutOptionDataid == b.second.m_uTargetDataid)
					return a += b.second.m_Current.m_dbTradedTurnover;
				else
					return a;
			}
				) /
				accumulate(
					m_bskOptionOrderBasket.begin(),
					m_bskOptionOrderBasket.end(), 0,
					[](int a, TBasketType::value_type & b)->int
			{
				if (PutOptionDataid == b.second.m_uTargetDataid)
					return a += b.second.m_Current.m_intTradedVol;
				else
					return a;
			}
			);
			

			double FutureTradedAvgPrice = m_eleFutureOrderBasket.m_Current.m_dbTradedTurnover / m_eleFutureOrderBasket.m_Current.m_intTradedVol;

			LOG("Future:%.2lf Call:%.4lf Put:%.4lf TradedSpread:%4lf",
				FutureTradedAvgPrice,
				CallTradedAvgPrice, PutTradedAvgPrice,
				CallTradedAvgPrice* OptionQty*m_intOptionLot
				+
				m_dbStrike * 1000 * FutureQty
				-
				PutTradedAvgPrice* OptionQty*m_intOptionLot
				-
				FutureTradedAvgPrice * m_intFutureLot*FutureQty
				);
		}
	}
	
}

void CPSYOptionFutureArbitrage::OnOrder(
	TOrderRefIdType ref,
	TOrderSysIdType sysId,
	TOrderDirectionType direction,
	TOrderStatusType Status,
	TPriceType LimitPrice,
	TVolumeType VolumeTraded,
	TVolumeType VolumeTotal)
{
	if (ref == m_eleFutureOrderBasket.m_Current.m_intCurrentRefid) //记录m_strFuture_OrderSysid
	{
		m_eleFutureOrderBasket.m_Current.m_strOrderSysid = sysId;
		if ((LB1_StatusCanceled == Status))//期货撤单成功
		{
			if(0 == m_eleFutureOrderBasket.m_Current.m_intTradedVol)
				m_enumCurrentState = ORDER_STATE_Free; //期货撤单成功 从等待状态退出，切换到寻找交易机会状态
			else
			{
				m_eleFutureOrderBasket.m_Current.m_intCurrentRefid = LIMITORDER(
					m_eleFutureOrderBasket.m_Target.m_enumDirection,
					m_eleFutureOrderBasket.m_Target.m_enumOffset,
					m_eleFutureOrderBasket.m_Target.m_intTargetVol- m_eleFutureOrderBasket.m_Current.m_intTradedVol,
					m_eleFutureOrderBasket.m_Target.m_enumDirection == LB1_Buy ?
					m_eleFutureOrderBasket.m_Target.m_ReferenceTickLink->m_dbAskPrice[0]
					:
					m_eleFutureOrderBasket.m_Target.m_ReferenceTickLink->m_dbBidPrice[0],
					m_eleFutureOrderBasket.m_uTargetDataid);
			}
		}
	}
	else
	{
		if (LB1_StatusCanceled == Status)
		{
			auto FindResult = m_bskOptionOrderBasket.find(ref);
			if (m_bskOptionOrderBasket.end() != FindResult)
			{
				CBasketElement ele(m_ptimeGlobalUTCTime, FindResult->second.m_uTargetDataid);
				ele.m_Current.m_intCurrentRefid = LIMITORDER(
					ele.m_Target.m_enumDirection = FindResult->second.m_Target.m_enumDirection,
				ele.m_Target.m_enumOffset = FindResult->second.m_Target.m_enumOffset,
				ele.m_Target.m_intTargetVol = FindResult->second.m_Target.m_intTargetVol - FindResult->second.m_Current.m_intTradedVol,
					FindResult->second.m_Target.m_enumDirection == LB1_Buy ?
					(ele.m_Target.m_ReferenceTickLink = FindResult->second.m_Target.m_ReferenceTickLink)->m_dbAskPrice[OPTION_LIMIT_ORDER_PRICE_LEVEL]
					:
					(ele.m_Target.m_ReferenceTickLink = FindResult->second.m_Target.m_ReferenceTickLink)->m_dbBidPrice[OPTION_LIMIT_ORDER_PRICE_LEVEL],
					ele.m_uTargetDataid = FindResult->second.m_uTargetDataid
					);
				char buf[64];
				sprintf(buf, "%lld", ele.m_Current.m_intCurrentRefid);
				ele.m_Current.m_strOrderSysid = buf;
				m_bskOptionOrderBasket.erase(ref);
				m_bskOptionOrderBasket[ele.m_Current.m_intCurrentRefid] = ele;
			}
		}
	}
}

void CPSYOptionFutureArbitrage::OnEndup()
{
}

tuple<int, int, int, int> CPSYOptionFutureArbitrage::GetPosition()
{
	TOrderDirectionType Direction = LB1_UnknownDirection;
	TOrderOffsetType Offset = LB1_UnknownOffset;
	int YesterdayBetRiseP = accumulate(
		m_mapPositionRecord.begin(),
		m_mapPositionRecord.end(), 0,
		[this](int a, map < date, map<_PositionType, int > >::value_type & b)->int
	{
		if (b.first != m_ptimeGlobalUTCTime.date())
			return a + b.second[_PositionType::_BetSpreadRisePosition];
		else return a;
	}
	);
	int YesterdayBetDropP = accumulate(
		m_mapPositionRecord.begin(),
		m_mapPositionRecord.end(), 0,
		[this](int a, map < date, map<_PositionType, int > >::value_type & b)->int
	{
		if (b.first != m_ptimeGlobalUTCTime.date())
			return a + b.second[_PositionType::_BetSpreadDropPosition];
		else return a;
	}
	);
	int TodayBetRiseP = m_mapPositionRecord[m_ptimeGlobalUTCTime.date()][_PositionType::_BetSpreadRisePosition];
	int TodayBetDropP = m_mapPositionRecord[m_ptimeGlobalUTCTime.date()][_PositionType::_BetSpreadDropPosition];
	return make_tuple(YesterdayBetRiseP, YesterdayBetDropP, TodayBetRiseP, TodayBetDropP);
};

tuple<double,double> CPSYOptionFutureArbitrage::ComputeSpread(
	const CFutureTick * my_FutureTick,
	const COptionTick * my_CallOpTick,
	const COptionTick * my_PutOpTick
	)
{
	auto CaluAskSpread = [this](TVolumeType Target,const COptionTick * tick) ->TPriceType
	{
		TPriceType AskAverage = 0;
		TVolumeType Remain = Target;
		for (
			unsigned int i = 0;
			i < MAX_QUOTATIONS_DEPTH&&Remain>0 && tick->m_intAskVolume[i]>0;
			i++)
		{
			auto ThisQty = min(tick->m_intAskVolume[i], Remain);
			AskAverage += tick->m_dbAskPrice[i] * ThisQty;
			Remain -= ThisQty;
		}
		if (Remain != 0)
		{
			AskAverage += Remain*(tick->m_dbAskPrice[0] + 200 * OptionPriceTick);
		}
		return AskAverage;
	};

	auto CaluBidSpread = [this](TVolumeType Target, const COptionTick * tick) ->TPriceType
	{
		TPriceType BidAverage = 0;
		TVolumeType Remain = Target;
		for (
			unsigned int i = 0;
			i < MAX_QUOTATIONS_DEPTH&&Remain>0 && tick->m_intBidVolume[i]>0;
			i++)
		{
			auto ThisQty = min(tick->m_intBidVolume[i], Remain);
			BidAverage += tick->m_dbBidPrice[i] * ThisQty;
			Remain -= ThisQty;
		}
		if (Remain != 0)
		{
			BidAverage += Remain*(tick->m_dbBidPrice[0] - 200 * OptionPriceTick);
		}
		return BidAverage;
	};
	auto Lower_Bid = CaluBidSpread(m_intOptionLot, my_CallOpTick);
	auto Lower_Ask = CaluAskSpread(m_intOptionLot, my_PutOpTick);
	auto Upper_Ask = CaluAskSpread(m_intOptionLot, my_CallOpTick);
	auto Upper_Bid = CaluBidSpread(m_intOptionLot, my_PutOpTick);
	double  LowerSpread = 
		Lower_Bid* OptionQty+ m_dbStrike * 1000 * FutureQty - Lower_Ask* OptionQty - my_FutureTick->m_dbAskPrice[0] * m_intFutureLot*FutureQty;
	double  UpperSpread = 
		Upper_Ask * OptionQty+ m_dbStrike * 1000 * FutureQty- Upper_Bid* OptionQty - my_FutureTick->m_dbBidPrice[0] * m_intFutureLot*FutureQty;
	return make_tuple(LowerSpread, UpperSpread);
}