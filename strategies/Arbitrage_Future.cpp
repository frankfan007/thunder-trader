
//#include "stdafx.h"
#include "ArbitrageBaseFuture.h"
#include <numeric>
#include <tuple>
#include "boost/serialization/serialization.hpp"  
#include "boost/archive/binary_oarchive.hpp"  
#include "boost/archive/binary_iarchive.hpp"  
#include "boost/serialization/export.hpp"  
#include "boost/serialization/vector.hpp"  
#include "boost/serialization/deque.hpp" 
#include "boost/serialization/map.hpp" 
#include "boost/serialization/split_member.hpp"  
#include "boost/serialization/utility.hpp"
#include <functional>

using namespace std;
#define UpperI 0
#define LowerI 1
#define LastI 2

namespace boost {
	namespace serialization {
		template<class Archive, class F, class S, class T>
		inline void serialize(
			Archive & ar,
			std::tuple<F, S, T> & p,
			const unsigned int
			) {
			ar & get<0>(p);
			ar & get<1>(p);
			ar & get<2>(p);
		}
	}
}

class CMyStrategy : public CArbitrageBase
{
public:
	CMyStrategy();
	CMyStrategy(MStrategyContext* context, TStrategyIdType strategyid);

	bool IsSupport(TStrategyTickType ticktype)
	{
		return true;
	}

	bool OnInquiry(MStrategyInquiryDataInterface*) { return true; };

	void OnLoad(const char * fname)
	{
		if (nullptr == fname)
			throw runtime_error("fname is nullptr.");
		ifstream is(fname, std::ios::binary);
		if (is.is_open())
		{
			boost::archive::binary_iarchive ia(is);
			ia & boost::serialization::base_object<CArbitrageBase>(*this);
			ia & *this;
			is.close();
		}
		else throw runtime_error("can not open file.");
	};

	void OnSave(const char * fname) {
		if (nullptr == fname)
			throw runtime_error("fname is nullptr.");
		ofstream os(fname, std::ios::binary);
		if (os.is_open())
		{
			boost::archive::binary_oarchive oa(os);
			oa & boost::serialization::base_object<CArbitrageBase>(*this);
			oa & *this;
			os.close();
		}
		else
			throw runtime_error("can not open file.");
	};

	BEGIN_SERIALIZATION 
#pragma region 参数
		SERIALIZATION(BarTypeSecond)
		SERIALIZATION(m_intMinuendVolumePerCopy)
		SERIALIZATION(m_intSubtrahendVolumePerCopy)
		SERIALIZATION(m_intWindowsLen)
		SERIALIZATION(m_dbWidth)
		SERIALIZATION(m_dbInitMiddleWidth)
		SERIALIZATION(m_dbCurrentMiddleWidth)
		SERIALIZATION(m_intAlgorithmType)
		SERIALIZATION(m_intCopies)
		SERIALIZATION(m_tdMorningPeriodBegin)
		SERIALIZATION(m_tdMorningPeriodEnd)
		SERIALIZATION(m_tdAfternoonPeriodBegin)
		SERIALIZATION(m_tdAfternoonPeriodEnd)
		SERIALIZATION(m_tdNightPeriodBegin)
		SERIALIZATION(m_tdNightPeriodEnd)
#pragma endregion

#pragma region ForBar
		SERIALIZATION(m_intLastBarCycleNumber)
		SERIALIZATION(m_barUpperSpreadBar)
		SERIALIZATION(m_barLowerSpreadBar)
		SERIALIZATION(m_barLastSpreadBar)
		SERIALIZATION(m_dateCurrentDate)
		SERIALIZATION(m_uBarCount_Overall)
		SERIALIZATION(m_uBarCount_Inday)
#pragma endregion

#pragma region 全局变量
		SERIALIZATION(m_deqHisteryBars)
		SERIALIZATION(m_intbarCount)
		SERIALIZATION(m_ptimeGlobalCurrentTime)
		SERIALIZATION(m_boolStrategyOnOff)
#pragma endregion
	END_SERIALIZATION

	BEGIN_PARAMETER_BIND
		PARAMETER_INT(BarTypeSecond)
		PARAMETER_INT(m_intMinuendVolumePerCopy)
		PARAMETER_INT(m_intSubtrahendVolumePerCopy)
		PARAMETER_INT(m_intWindowsLen)
		PARAMETER_DOUBLE(m_dbWidth)
		PARAMETER_DOUBLE(m_dbInitMiddleWidth)
		PARAMETER_INT(m_intAlgorithmType)
		PARAMETER_INT(m_intCopies)
		PARAMETER_TIMEDURATION(m_tdMorningPeriodBegin)
		PARAMETER_TIMEDURATION(m_tdMorningPeriodEnd)
		PARAMETER_TIMEDURATION(m_tdAfternoonPeriodBegin)
		PARAMETER_TIMEDURATION(m_tdAfternoonPeriodEnd)
		PARAMETER_TIMEDURATION(m_tdNightPeriodBegin)
		PARAMETER_TIMEDURATION(m_tdNightPeriodEnd)
	END_PARAMETER_BIND_

	BEGIN_PROBE_BIND
		BEGIN_GRAPH
			PROBE(&m_adbUpperSpread_Close, "m_adbUpperSpread_close", TProbe_Color_Green)
			PROBE(&m_adbLowerSpread_Close, "m_adbLowerSpread_close", TProbe_Color_Blue)
			PROBE(&m_adbUpper, "m_adbUpper", TProbe_Color_Yellow)
			PROBE(&m_HasRisenSoClear, "m_HasRisenSoClear", TProbe_Color_Blue_Weak)
			PROBE(&m_HasDroppedSoClear, "m_HasDroppedSoClear", TProbe_Color_Green_Weak)
			PROBE(&m_adbLower, "m_adbLower", TProbe_Color_Yellow)
		END_GRAPH("Line")
	END_PROBE_BIND_

	BEGIN_SHOW(Show)
#pragma region 参数
		SHOW_UINT(BarTypeSecond)
		SHOW_INT(m_intMinuendVolumePerCopy)
		SHOW_INT(m_intSubtrahendVolumePerCopy)
		SHOW_INT(m_intWindowsLen)
		SHOW_DOUBLE(m_dbWidth)
		SHOW_DOUBLE(m_dbInitMiddleWidth)
		SHOW_DOUBLE(m_dbCurrentMiddleWidth)
		SHOW_INT(GetMinuendMultipNumber())
		SHOW_INT(GetSubtrahendMultipNumber())
		SHOW_INT(m_intAlgorithmType)
		SHOW_INT(m_intCopies)
		SHOW_STRING(to_simple_string(m_tdMorningPeriodBegin).c_str())
		SHOW_STRING(to_simple_string(m_tdMorningPeriodEnd).c_str())
		SHOW_STRING(to_simple_string(m_tdAfternoonPeriodBegin).c_str())
		SHOW_STRING(to_simple_string(m_tdAfternoonPeriodEnd).c_str())
		SHOW_STRING(to_simple_string(m_tdNightPeriodBegin).c_str())
		SHOW_STRING(to_simple_string(m_tdNightPeriodEnd).c_str())
#pragma endregion

#pragma region ForBar
		SHOW_INT(m_intLastBarCycleNumber)
		SHOW_UINT(m_uBarCount_Overall)
		SHOW_UINT(m_uBarCount_Inday)
#pragma endregion

#pragma region 全局变量
		SHOW_UINT(m_deqHisteryBars.size())
		SHOW_INT(m_intbarCount)
		SHOW_STRING(to_simple_string(m_ptimeGlobalCurrentTime).c_str())
		SHOW_INT(m_boolStrategyOnOff)
#pragma endregion
	END_SHOW




	TLastErrorIdType OnInit(ptime);
	void OnTick(TMarketDataIdType, const CTick *);

#pragma region 参数
	int BarTypeSecond = 60;
	int m_intMinuendVolumePerCopy = 1;
	int m_intSubtrahendVolumePerCopy = 1;
	int m_intWindowsLen = 60;
	double m_dbWidth = 5.0;
	double m_dbInitMiddleWidth = 0;
	int m_intAlgorithmType = 0;
	int m_intCopies = 1;
	
	time_duration m_tdMorningPeriodBegin	=	time_duration(1,  2,  0,  0);
	time_duration m_tdMorningPeriodEnd		=	time_duration(3, 29, 30,  0);

	time_duration m_tdAfternoonPeriodBegin	=	time_duration(5, 28,  0,  0);
	time_duration m_tdAfternoonPeriodEnd	=	time_duration(6, 55,  0,  0);

	time_duration m_tdNightPeriodBegin		=	time_duration(13, 2,  0,  0);
	time_duration m_tdNightPeriodEnd		=	time_duration(18, 28, 0,  0);
	
#pragma endregion

#pragma region ForBar
	int m_intLastBarCycleNumber;
	CBar m_barUpperSpreadBar;
	CBar m_barLowerSpreadBar;
	CBar m_barLastSpreadBar;
	date m_dateCurrentDate;
	unsigned int m_uBarCount_Overall;
	unsigned int m_uBarCount_Inday;
#pragma endregion

#pragma region 全局变量
	double m_dbCurrentMiddleWidth = 0;
	deque<tuple<CBar, CBar, CBar>> m_deqHisteryBars;
	int m_intbarCount = -1;
	ptime m_ptimeGlobalCurrentTime;
	bool m_boolStrategyOnOff = true;
#pragma endregion

private:
	atomic<double> m_adbUpperSpread_Close;
	atomic<double> m_adbLowerSpread_Close;
	//atomic<double> m_adbLastSpread_Close;

	atomic<double> m_adbUpper;
	atomic<double> m_adbMiddle;
	atomic<double> m_adbLower; 
	atomic<double> m_HasRisenSoClear;
	atomic<double> m_HasDroppedSoClear;
	const vector< function<tuple<double,double,double> ()>> m_vecAlgorithms=
	{
		std::bind([](const deque<tuple<CBar, CBar, CBar>> & _deqHisteryBars,const double & _dbWidth)->tuple<double,double,double>
	{
		double _upper = accumulate(
			_deqHisteryBars.begin(),
			_deqHisteryBars.end(), 0.0,
			[](double a, const tuple<CBar, CBar, CBar> & b)->double
		{
			return a + get<LastI>(b).m_dbHighest;
		}
		) / _deqHisteryBars.size();
		double _lower = accumulate(
			_deqHisteryBars.begin(),
			_deqHisteryBars.end(), 0.0,
			[](double a, const tuple<CBar, CBar, CBar> & b)->double
		{
			return a + get<LastI>(b).m_dbLowest;
		}
		) / _deqHisteryBars.size();
		double _middle = (_upper + _lower) / 2;
		double UpperLine = _middle + (_upper - _middle)*_dbWidth;
		double MiddleLine = _middle;
		double LowerLine = _middle - (_middle - _lower)*_dbWidth;
		return make_tuple(UpperLine, MiddleLine, LowerLine);
	},std::ref(m_deqHisteryBars),std::ref(m_dbWidth)),
		std::bind([](const deque<tuple<CBar, CBar, CBar>> & _deqHisteryBars,const double &_dbWidth)->tuple<double,double,double>
	{
		auto _intWindowsLen = _deqHisteryBars.size();
		double _upper = accumulate(
			_deqHisteryBars.begin(),
			_deqHisteryBars.end(), 0.0,
			[](double a, const tuple<CBar, CBar, CBar> & b)->double
		{
			return a + get<LastI>(b).m_dbHighest;
		}
		) / _deqHisteryBars.size();
		double _lower = accumulate(
			_deqHisteryBars.begin(),
			_deqHisteryBars.end(), 0.0,
			[](double a, const tuple<CBar, CBar, CBar> & b)->double
		{
			return a + get<LastI>(b).m_dbLowest;
		}
		) / _deqHisteryBars.size();

		unsigned int BeginMulti = _deqHisteryBars.size();
		double _middle = accumulate(
			_deqHisteryBars.begin(),
			_deqHisteryBars.end(), 0.0,
			[&BeginMulti](double a, const tuple<CBar, CBar, CBar> & b)->double
		{
			return a + get<LastI>(b).m_dbClose*(BeginMulti--);
		}
		) / ((1 + (double)_intWindowsLen)*(double)_intWindowsLen / 2);
		double UpperLine = _middle + (_upper - _lower) / 2.0*_dbWidth;
		double MiddleLine = _middle;
		double LowerLine = _middle - (_upper - _lower) / 2.0*_dbWidth;
		return make_tuple(UpperLine, MiddleLine, LowerLine);
	},std::ref(m_deqHisteryBars),std::ref(m_dbWidth))
	};
};

CMyStrategy::CMyStrategy() :
	CArbitrageBase(nullptr, 0)
{
}

CMyStrategy::CMyStrategy(MStrategyContext* context, TStrategyIdType strategyid):
	CArbitrageBase(context, strategyid)
{
	ParamAndProbeInit(m_array2ParameterStruct, m_array2ProbeStruct);
}

TLastErrorIdType CMyStrategy::OnInit(ptime CurrentTime)
{
#pragma region 参数合法性检查
	
#pragma endregion

#pragma region ForBar
	m_intLastBarCycleNumber = -1;//当前周期编号：当前时间距离凌晨0：0：0的总秒数除以Bar周期并且取整,当值为-1时表示未初始化
	m_barUpperSpreadBar.Init(BarTypeSecond);
	m_barLowerSpreadBar.Init(BarTypeSecond);
	m_barLastSpreadBar.Init(BarTypeSecond);
	
	m_uBarCount_Overall = 0;
	m_uBarCount_Inday = 0;
#pragma endregion

#pragma region 全局变量
	m_dbCurrentMiddleWidth = m_dbInitMiddleWidth;
	m_deqHisteryBars.clear();
	m_deqHisteryBars.push_back(tuple<CBar, CBar, CBar>());
	m_intbarCount = -1;
	m_dateCurrentDate = m_ptimeGlobalCurrentTime.date();
	m_boolStrategyOnOff = true;
#pragma endregion
	
	m_adbUpper.store(PROBE_NULL_VALUE);
	m_adbMiddle.store(PROBE_NULL_VALUE);
	m_adbLower.store(PROBE_NULL_VALUE);
	m_HasRisenSoClear.store(PROBE_NULL_VALUE);
	m_HasDroppedSoClear.store(PROBE_NULL_VALUE);


	m_adbUpperSpread_Close.store(PROBE_NULL_VALUE);
	m_adbLowerSpread_Close.store(PROBE_NULL_VALUE);

	return CArbitrageBase::OnInit(CurrentTime);
}

void CMyStrategy::OnTick(TMarketDataIdType dataid, const CTick *pDepthMarketData)
{
	CArbitrageBase::OnTick(dataid, pDepthMarketData);
	if (IsTwoLegHasInited() == false)
		return;
	
	#pragma region 更新K线
	m_barUpperSpreadBar.m_dbClose =
		GetMinuendTick().m_dbAskPrice[0] * m_intMinuendVolumePerCopy*GetMinuendMultipNumber()
		-
		GetSubtrahendTick().m_dbBidPrice[0] * m_intSubtrahendVolumePerCopy*GetSubtrahendMultipNumber();

	m_barLowerSpreadBar.m_dbClose =
		GetMinuendTick().m_dbBidPrice[0] * m_intMinuendVolumePerCopy*GetMinuendMultipNumber()
		-
		GetSubtrahendTick().m_dbAskPrice[0] * m_intSubtrahendVolumePerCopy*GetSubtrahendMultipNumber();
	m_barLastSpreadBar.m_dbClose =
		GetMinuendTick().m_dbLastPrice * m_intMinuendVolumePerCopy*GetMinuendMultipNumber()
		-
		GetSubtrahendTick().m_dbLastPrice * m_intSubtrahendVolumePerCopy*GetSubtrahendMultipNumber();

	m_ptimeGlobalCurrentTime = max(GetMinuendTick().m_datetimeUTCDateTime, GetSubtrahendTick().m_datetimeUTCDateTime);


	if (-1 == m_intLastBarCycleNumber)
	{
		//这段程序只执行两次
		m_barUpperSpreadBar.m_dbOpen =
			m_barUpperSpreadBar.m_dbHighest =
			m_barUpperSpreadBar.m_dbLowest =
			m_barUpperSpreadBar.m_dbClose;

		m_barLowerSpreadBar.m_dbOpen =
			m_barLowerSpreadBar.m_dbHighest =
			m_barLowerSpreadBar.m_dbLowest =
			m_barLowerSpreadBar.m_dbClose;

		m_barLastSpreadBar.m_dbOpen =
			m_barLastSpreadBar.m_dbHighest =
			m_barLastSpreadBar.m_dbLowest =
			m_barLastSpreadBar.m_dbClose;

		m_uBarCount_Overall = 0;
		m_uBarCount_Inday = 0;
		m_intLastBarCycleNumber = CYCLE_NUM_OF_UTC(m_ptimeGlobalCurrentTime, BarTypeSecond);
		m_dateCurrentDate = m_ptimeGlobalCurrentTime.date();
	}

	if (m_ptimeGlobalCurrentTime.date() != m_dateCurrentDate)
	{
		m_uBarCount_Inday = -1;

	}

	m_dateCurrentDate = m_ptimeGlobalCurrentTime.date();

	auto SelfCycleNum = CYCLE_NUM_OF_UTC(m_ptimeGlobalCurrentTime, BarTypeSecond);
	if (SelfCycleNum>m_intLastBarCycleNumber)
	{
		m_intLastBarCycleNumber = SelfCycleNum;



		m_barUpperSpreadBar.m_dbOpen =
			m_barUpperSpreadBar.m_dbHighest =
			m_barUpperSpreadBar.m_dbLowest =
			m_barUpperSpreadBar.m_dbClose;

		m_barLowerSpreadBar.m_dbOpen =
			m_barLowerSpreadBar.m_dbHighest =
			m_barLowerSpreadBar.m_dbLowest =
			m_barLowerSpreadBar.m_dbClose;

		m_barLastSpreadBar.m_dbOpen =
			m_barLastSpreadBar.m_dbHighest =
			m_barLastSpreadBar.m_dbLowest =
			m_barLastSpreadBar.m_dbClose;

		++m_uBarCount_Overall;
		++m_uBarCount_Inday;

	}
	else
	{
		if (m_barUpperSpreadBar.m_dbClose > m_barUpperSpreadBar.m_dbHighest)
			m_barUpperSpreadBar.m_dbHighest = m_barUpperSpreadBar.m_dbClose;
		if (m_barUpperSpreadBar.m_dbClose<m_barUpperSpreadBar.m_dbLowest)
			m_barUpperSpreadBar.m_dbLowest = m_barUpperSpreadBar.m_dbClose;

		if (m_barLowerSpreadBar.m_dbClose > m_barLowerSpreadBar.m_dbHighest)
			m_barLowerSpreadBar.m_dbHighest = m_barLowerSpreadBar.m_dbClose;
		if (m_barLowerSpreadBar.m_dbClose<m_barLowerSpreadBar.m_dbLowest)
			m_barLowerSpreadBar.m_dbLowest = m_barLowerSpreadBar.m_dbClose;

		if (m_barLastSpreadBar.m_dbClose > m_barLastSpreadBar.m_dbHighest)
			m_barLastSpreadBar.m_dbHighest = m_barLastSpreadBar.m_dbClose;
		if (m_barLastSpreadBar.m_dbClose<m_barLastSpreadBar.m_dbLowest)
			m_barLastSpreadBar.m_dbLowest = m_barLastSpreadBar.m_dbClose;
	}
	#pragma endregion

	m_adbUpper.store(PROBE_NULL_VALUE);
	m_adbMiddle.store(PROBE_NULL_VALUE);
	m_adbLower.store(PROBE_NULL_VALUE);
	m_HasRisenSoClear.store(PROBE_NULL_VALUE);
	m_HasDroppedSoClear.store(PROBE_NULL_VALUE);

	m_adbUpperSpread_Close.store(m_barUpperSpreadBar.m_dbClose);
	m_adbLowerSpread_Close.store(m_barLowerSpreadBar.m_dbClose);
	//m_adbLastSpread_Close.store(m_barLastSpreadBar.m_dbClose);

	#pragma region 保存HisteryBarLength根历史bar
	if (m_intbarCount != m_uBarCount_Overall
		&&
		-1 != m_intbarCount)
	{
		m_deqHisteryBars.push_front(make_tuple(m_barUpperSpreadBar, m_barLowerSpreadBar, m_barLastSpreadBar));
		if (m_deqHisteryBars.size()> m_intWindowsLen)
			m_deqHisteryBars.pop_back();
	}
	else
		m_deqHisteryBars.front() = make_tuple(m_barUpperSpreadBar, m_barLowerSpreadBar, m_barLastSpreadBar);
	m_intbarCount = m_uBarCount_Overall;
	#pragma endregion

	auto res = m_vecAlgorithms[m_intAlgorithmType%m_vecAlgorithms.size()]();
	double UpperLine = get<0>(res);
	double MiddleLine = get<1>(res);
	double LowerLine = get<2>(res);

	m_adbUpper.store(UpperLine);
	m_adbMiddle.store(MiddleLine);
	m_adbLower.store(LowerLine);
	m_HasRisenSoClear.store(MiddleLine - m_dbCurrentMiddleWidth);
	m_HasDroppedSoClear.store(MiddleLine + m_dbCurrentMiddleWidth);

	if (m_boolStrategyOnOff&&m_deqHisteryBars.size() >= m_intWindowsLen)
	{
		bool IsNearLimitPrice = 
			GetMinuendTick().m_dbLastPrice > static_cast<CFutureTick*>(&GetMinuendTick())->m_dbUpperLimitPrice - GetMinuendMinPriceTick() * 10
			||
			GetMinuendTick().m_dbLastPrice < static_cast<CFutureTick*>(&GetMinuendTick())->m_dbLowerLimitPrice + GetMinuendMinPriceTick() * 10
			||
			GetSubtrahendTick().m_dbLastPrice > static_cast<CFutureTick*>(&GetSubtrahendTick())->m_dbUpperLimitPrice - GetSubtrahendMinPriceTick() * 10
			||
			GetSubtrahendTick().m_dbLastPrice < static_cast<CFutureTick*>(&GetSubtrahendTick())->m_dbLowerLimitPrice + GetSubtrahendMinPriceTick() * 10;
		bool IsTimeOk =
			(m_ptimeGlobalCurrentTime.time_of_day() > m_tdMorningPeriodBegin&&m_ptimeGlobalCurrentTime.time_of_day() < m_tdMorningPeriodEnd)
			||
			(m_ptimeGlobalCurrentTime.time_of_day() > m_tdAfternoonPeriodBegin&&m_ptimeGlobalCurrentTime.time_of_day() < m_tdAfternoonPeriodEnd)
			||
			(m_ptimeGlobalCurrentTime.time_of_day() > m_tdNightPeriodBegin&&m_ptimeGlobalCurrentTime.time_of_day() < m_tdNightPeriodEnd);
		
		if (CanExcute() && (false == IsNearLimitPrice) && (IsTimeOk))
		{
			if (//目前无仓位
				0 == GetSubtrahendPosition().m_uLongPosition
				&&
				0 == GetMinuendPosition().m_uLongPosition
				&&
				0 == GetSubtrahendPosition().m_uShortPosition
				&&
				0 == GetMinuendPosition().m_uShortPosition
				&&
				(REMAINCANCELAMOUNT(0)>50)
				&&
				(REMAINCANCELAMOUNT(1)>50)
				)
			{
				if (m_barLowerSpreadBar.m_dbClose > UpperLine)
					MakeOrder(TEnumSellMinuend_BuySubtrahend_Increase, m_intCopies, m_intMinuendVolumePerCopy*m_intCopies, m_intSubtrahendVolumePerCopy*m_intCopies);//M-S 卖出M 买入S 赌价差会下降
				else if (m_barUpperSpreadBar.m_dbClose < LowerLine)
					MakeOrder(TEnumBuyMinuend_SellSubtrahend_Increase, m_intCopies, m_intMinuendVolumePerCopy*m_intCopies, m_intSubtrahendVolumePerCopy*m_intCopies);//M-S 买入M 卖出S 赌价差会上升
			}
			else if (//M-S M持空头 S持多头 价差下降到中轨就平仓
				0 != GetMinuendPosition().m_uShortPosition
				&&
				0 != GetSubtrahendPosition().m_uLongPosition
				&&
				m_barUpperSpreadBar.m_dbClose < MiddleLine + m_dbCurrentMiddleWidth
				)
			{
				MakeOrder(TEnumBuyMinuend_SellSubtrahend_Descrease, m_intCopies, m_intMinuendVolumePerCopy*m_intCopies, m_intSubtrahendVolumePerCopy*m_intCopies);
			}
			else if (//M-S M持多头 S持空头 价差上升到中轨就平仓
				0 != GetMinuendPosition().m_uLongPosition
				&&
				0 != GetSubtrahendPosition().m_uShortPosition
				&&
				m_barLowerSpreadBar.m_dbClose > MiddleLine - m_dbCurrentMiddleWidth
				)
			{
				MakeOrder(TEnumSellMinuend_BuySubtrahend_Descrease, m_intCopies, m_intMinuendVolumePerCopy*m_intCopies, m_intSubtrahendVolumePerCopy*m_intCopies);
			}
		}

	}

	#pragma region 干预策略
	char buf[1024];
	if (true == MEDDLE(buf, 1024))
	{
		string cmd = buf;
		if ("betrise" == cmd)
		{
			if (CanExcute())
			{
				if (//目前无仓位
					0 == GetSubtrahendPosition().m_uLongPosition
					&&
					0 == GetMinuendPosition().m_uLongPosition
					&&
					0 == GetSubtrahendPosition().m_uShortPosition
					&&
					0 == GetMinuendPosition().m_uShortPosition
					)
				{
					MakeOrder(TEnumBuyMinuend_SellSubtrahend_Increase, m_intCopies,
						m_intMinuendVolumePerCopy*m_intCopies,
						m_intSubtrahendVolumePerCopy*m_intCopies);//M-S 买入M 卖出S 赌价差会上升
				}
				else
					LOG("strategy[%d]: Strategy has position so can not increase", g_StrategyId);
			}
		}
		if ("betdrop" == cmd)
		{
			if (CanExcute())
			{
				if (//目前无仓位
					0 == GetSubtrahendPosition().m_uLongPosition
					&&
					0 == GetMinuendPosition().m_uLongPosition
					&&
					0 == GetSubtrahendPosition().m_uShortPosition
					&&
					0 == GetMinuendPosition().m_uShortPosition
					)
				{
					MakeOrder(TEnumSellMinuend_BuySubtrahend_Increase, m_intCopies,
						m_intMinuendVolumePerCopy*m_intCopies, 
						m_intSubtrahendVolumePerCopy*m_intCopies);//M-S 卖出M 买入S 赌价差会下降
				}
				else
					LOG("strategy[%d]: Strategy has position so can not increase", g_StrategyId);
			}
		}
		if ("clear" == cmd)
		{
			if (//M-S M持空头 S持多头 价差下降到中轨就平仓
				0 != GetMinuendPosition().m_uShortPosition
				&&
				0 != GetSubtrahendPosition().m_uLongPosition
				)
			{
				MakeOrder(TEnumBuyMinuend_SellSubtrahend_Descrease, m_intCopies,
					m_intMinuendVolumePerCopy*m_intCopies, 
					m_intSubtrahendVolumePerCopy*m_intCopies);
			}
			else if (//M-S M持多头 S持空头 价差上升到中轨就平仓
				0 != GetMinuendPosition().m_uLongPosition
				&&
				0 != GetSubtrahendPosition().m_uShortPosition
				)
			{
				MakeOrder(TEnumSellMinuend_BuySubtrahend_Descrease, m_intCopies,
					m_intMinuendVolumePerCopy*m_intCopies,
					m_intSubtrahendVolumePerCopy*m_intCopies);
			}
			else
				LOG("strategy[%d]: Strategy has no position so can not decrease", g_StrategyId);
		}
		if ("position" == cmd)
		{
			ShowPosition();
		}
		else if ("show" == cmd)
		{
			Show();
		}
		else if ("on" == cmd)
		{
			m_boolStrategyOnOff = true;
		}
		else if ("off" == cmd)
		{
			m_boolStrategyOnOff = false;
		}
		else if ("inc_copies" == cmd)
		{
			if (0 == GetSubtrahendPosition().m_uLongPosition
				&&
				0 == GetMinuendPosition().m_uLongPosition
				&&
				0 == GetSubtrahendPosition().m_uShortPosition
				&&
				0 == GetMinuendPosition().m_uShortPosition)
			{
				m_intCopies++;
				LOG("Strategy[%d]: Current Copies:%d",g_StrategyId, m_intCopies);
			}
			else
			{
				LOG("Strategy[%d]: Cannot modify copies.", g_StrategyId);
			}
		}
		else if ("dec_copies" == cmd)
		{
			if (0 == GetSubtrahendPosition().m_uLongPosition
				&&
				0 == GetMinuendPosition().m_uLongPosition
				&&
				0 == GetSubtrahendPosition().m_uShortPosition
				&&
				0 == GetMinuendPosition().m_uShortPosition)
			{
				if(m_intCopies>1)
					m_intCopies--;
				LOG("Strategy[%d]: Current Copies:%d", g_StrategyId, m_intCopies);
			}
			else
			{
				LOG("Strategy[%d]: Cannot modify copies.", g_StrategyId);
			}
		}
		else if ("inc_middlewidth" == cmd)
		{
			double dt = 0;
			switch (m_intAlgorithmType%m_vecAlgorithms.size())
			{
			case 0:dt = (UpperLine - LowerLine) / m_dbWidth / 2 * 0.1;break;
			case 1:dt = (UpperLine - LowerLine) / m_dbWidth * 0.1;break;
			}
			m_dbCurrentMiddleWidth += dt;
		}
		else if ("dec_middlewidth" == cmd)
		{
			double dt = 0;
			switch (m_intAlgorithmType%m_vecAlgorithms.size())
			{
			case 0:dt = (UpperLine - LowerLine) / m_dbWidth / 2 * 0.1;break;
			case 1:dt = (UpperLine - LowerLine) / m_dbWidth * 0.1;break;
			}
			m_dbCurrentMiddleWidth -= dt;
		}
		else if ("reset_middlewidth" == cmd)
		{
			m_dbCurrentMiddleWidth = m_dbInitMiddleWidth;
		}
	}
	#pragma endregion
}

#pragma region 导出函数
extern "C" {
	STRATEGY_INTERFACE  MStrategy * CreateStrategyObject(MStrategyContext*context, TStrategyIdType stid)\
	{
		MStrategy * ret;

		try {
			ret = new CMyStrategy(context, stid);
		}
		catch (...)
		{
			return NULL;
		}
		return ret;
	}
}
#pragma endregion