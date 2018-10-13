// StrategyDemo.cpp : 定义 DLL 应用程序的导出函数。
//


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
#include <numeric>
using namespace std;

#define BARSECONDNUMBER 60

date GetTradeday(ptime _Current)
{
	if (_Current.time_of_day() < time_duration(12, 0, 0, 0))
		return _Current.date();
	else
	{
		if (_Current.date().day_of_week().as_enum() == Friday)
			return _Current.date() + days(3);
		else
			return _Current.date() + days(1);
	}
}

struct T_MyBar
{
	deque<double> openPrice;
	deque<double> highPrice;
	deque<double> lowPrice;
	deque<double> closePrice;
	bool boolIsValid;
};

enum T_HasPosition { HasLongPosition, HasShortPosition, NoPosition };

enum T_CanceledOderStates { NoStates, EnterCanceledOrder, ExitCanceledOrder };

enum T_FlactionState
{
	PCL_Waiting,//场外等待中
	PCL_EnterWaiting,//进场未成交
	PCL_EnterPartlyTraded,//进场部分成交
	PCL_Inside,//进场已成交
	PCL_ExitWaiting,//出场未成交
	PCL_ExitPartlyTraded,//出场部分成交
	PCL_EnterCancelOrderWaiting,//入场撤单已发
	PCL_ExitCancelOrderWaiting//出场撤单已发
};

typedef int T_StrategyName;
#define ChannelBreakOut_UpperLowerClose 0
#define ChannelBreakOut_MiddleClose 1
#define KentnerBreakOut 2

typedef int T_StrMyDequeAverageFlag;
#define MA 0
#define EMA 1

typedef int T_StrFilterFlag;
#define NoFilter 0
#define ChannelFilter 1
#define SpreadFilter 2
#define MaFilter 3

enum T_StrMyDequeMaxMinFlag {ChannelMAX,ChannelMIN};

enum T_StrMyDequeKentnerFlag {KentnerUpper,KentnerLower};

class CInquiryMarkNetCurvePoint :public MStrategyInquiryDataInterface
{
public:
	virtual bool ValueToString(char * buf, size_t len)
	{
		if (strlen("netcurve") < len - 1)
		{
			strncpy(buf, "netcurve", len);
			return true;
		}
		else
			return false;

	}
	virtual void Release() { delete this; };
};

class MyStrategy : public MStrategy
{
public:
	//自定义区域
	CFutureTick m_tickData[2];
	T_MyBar m_BarData[2];
	bool m_boolInited[2];
	bool m_boolCanUpdateLine;
	bool m_boolLongOpenFlag;
	bool m_boolShortOpenFlag;
	bool m_boolLongCloseFlag;
	bool m_boolShortCloseFlag;
	bool m_boolSwitch;

	date m_dateOderTradeDay;

	string m_strStr = "";
	string m_strSysid = "";

	int m_intLastBarCycleNumber;
	int m_intLongOpenWindowsLen = 60;
	int m_intLongCloseWindowsLen = 60;
	int m_intShortCloseWindowsLen = 60;
	int m_intShortOpenWindowsLen = 60;
	int m_intFilterLongLen = 120;
	int m_intFilterShortLen = 60;

	int m_intMaxParam = 0;

	int m_intBadSlipTickCount = 20;
	int m_intCopies = 1;
	int m_intTradingCopies;
	int m_intCumulateTradedVolume = 0;
	int m_intTradingDataID = 1;

	int m_intOrderWaitTime = 1;

	double m_dbMinPriceTick = 1;
	double m_dbOpenWidth = 0;
	double m_dbCloseWidth = 0.5;

	double m_dbLongOpenLine[2];
	double m_dbShortCloseLine[2];
	double m_dbShortOpenLine[2];
	double m_dbLongCloseLine[2];

	double m_dbFilterLine[2];

	T_StrMyDequeMaxMinFlag m_enumMyDequeMaxMinFlag;

	T_StrMyDequeKentnerFlag m_enumMyDequeKentnerFlag;

	T_StrMyDequeAverageFlag m_enumMyDequeAverageFlag=EMA;

	T_StrategyName m_enumStrategyName = ChannelBreakOut_MiddleClose;

	T_StrFilterFlag m_enumFilterFlag=NoFilter;

	T_CanceledOderStates m_enumCanceledOrder;

	T_FlactionState m_enumState;

	TOrderRefIdType m_llOderNumber;

	T_HasPosition m_enumHasPosition;

	ptime m_ptimeGlobalLastTime;
	ptime m_ptimeGlobalMakeOderTime;

	time_duration m_tdDayPeriodBegin = time_duration(1, 0, 0, 0);
	time_duration m_tdDayPeriodEnd = time_duration(7, 0, 0, 0);
	time_duration m_tdNightPeriodBegin = time_duration(13, 0, 0, 0);
	time_duration m_tdNightPeriodEnd = time_duration(15, 0, 0, 0);

	//自定义区域
	BEGIN_SHOW(Show)

		SHOW_STRING(m_strStr.c_str())
		SHOW_STRING(m_strSysid.c_str())
		SHOW_INT(m_intLastBarCycleNumber)
		SHOW_INT(m_intLongOpenWindowsLen)
		SHOW_INT(m_intLongCloseWindowsLen)
		SHOW_INT(m_intShortCloseWindowsLen)
		SHOW_INT(m_intShortOpenWindowsLen)
		SHOW_INT(m_intFilterLongLen)
		SHOW_INT(m_intFilterShortLen)
		SHOW_INT(m_intMaxParam)
		SHOW_INT(m_intBadSlipTickCount)
		SHOW_INT(m_intCopies)
		SHOW_INT(m_intTradingCopies)
		SHOW_INT(m_intCumulateTradedVolume)
		SHOW_INT(m_intTradingDataID)
		SHOW_INT(m_intOrderWaitTime)
		SHOW_DOUBLE( m_dbMinPriceTick)
		SHOW_DOUBLE(m_dbOpenWidth)
		SHOW_DOUBLE(m_dbCloseWidth)
	
		SHOW_INT(m_enumMyDequeMaxMinFlag)
		SHOW_INT(m_enumMyDequeKentnerFlag)
		SHOW_INT(m_enumMyDequeAverageFlag)
		SHOW_INT(m_enumStrategyName)
		SHOW_INT(m_enumFilterFlag)
		SHOW_INT(m_enumCanceledOrder)
		SHOW_INT(m_enumState)
		SHOW_INT(m_enumHasPosition)
		SHOW_INT(static_cast<int>(m_boolSwitch))

		SHOW_STRING(to_simple_string(m_ptimeGlobalLastTime).c_str())
		SHOW_STRING(to_simple_string(m_ptimeGlobalMakeOderTime).c_str())

		SHOW_STRING(to_simple_string(m_dateOderTradeDay).c_str())

	END_SHOW

	BEGIN_SHOW(position)

		SHOW_INT(m_enumHasPosition)

	END_SHOW

	STRATEGY_TEMPLATE_DECLARE(MyStrategy)

	BEGIN_SERIALIZATION
		SERIALIZATION(m_dateOderTradeDay)
		SERIALIZATION(m_intLongOpenWindowsLen)
		SERIALIZATION(m_intLongCloseWindowsLen)
		SERIALIZATION(m_intShortOpenWindowsLen)
		SERIALIZATION(m_intShortCloseWindowsLen)
		SERIALIZATION(m_intFilterLongLen)
		SERIALIZATION(m_intFilterShortLen)
		SERIALIZATION(m_dbOpenWidth)
		SERIALIZATION(m_dbCloseWidth)
		SERIALIZATION(m_intBadSlipTickCount)
		SERIALIZATION(m_dbMinPriceTick)
		SERIALIZATION(m_intCopies)
		SERIALIZATION(m_intTradingDataID)
		SERIALIZATION(m_enumMyDequeAverageFlag)
		SERIALIZATION(m_enumStrategyName)
		SERIALIZATION(m_enumFilterFlag)
		SERIALIZATION(m_intOrderWaitTime)
		SERIALIZATION(m_tdDayPeriodBegin)
		SERIALIZATION(m_tdDayPeriodEnd)
		SERIALIZATION(m_tdNightPeriodBegin)
		SERIALIZATION(m_tdNightPeriodEnd)
	END_SERIALIZATION

	BEGIN_PARAMETER_BIND
		PARAMETER("m_intLongOpenWindowsLen", LB1_IntType, &m_intLongOpenWindowsLen)
		PARAMETER("m_intLongCloseWindowsLen", LB1_IntType, &m_intLongCloseWindowsLen)
		PARAMETER("m_intShortOpenWindowsLen", LB1_IntType, &m_intShortOpenWindowsLen)
		PARAMETER("m_intShortCloseWindowsLen", LB1_IntType, &m_intShortCloseWindowsLen)
		PARAMETER("m_intFilterLongLen", LB1_IntType, &m_intFilterLongLen)
		PARAMETER("m_intFilterShortLen", LB1_IntType, &m_intFilterShortLen)
		PARAMETER("m_dbOpenWidth", LB1_DoubleType, &m_dbOpenWidth)
		PARAMETER("m_dbCloseWidth", LB1_DoubleType, &m_dbCloseWidth)
		PARAMETER("m_intBadSlipTickCount", LB1_IntType, &m_intBadSlipTickCount)
		PARAMETER("m_dbMinPriceTick", LB1_DoubleType, &m_dbMinPriceTick)
		PARAMETER("m_intCopies", LB1_IntType, &m_intCopies)
		PARAMETER("m_intTradingDataID", LB1_IntType, &m_intTradingDataID)
		PARAMETER("m_enumMyDequeAverageFlag", LB1_IntType, &m_enumMyDequeAverageFlag)
		PARAMETER("m_enumStrategyName", LB1_IntType, &m_enumStrategyName)
		PARAMETER("m_enumFilterFlag", LB1_IntType, &m_enumFilterFlag)
		PARAMETER("m_intOrderWaitTime", LB1_IntType, &m_intOrderWaitTime)
		PARAMETER("m_tdDayPeriodBegin", LB1_TimeDurationType, &m_tdDayPeriodBegin)
		PARAMETER("m_tdDayPeriodEnd", LB1_TimeDurationType, &m_tdDayPeriodEnd)
		PARAMETER("m_tdNightPeriodBegin", LB1_TimeDurationType, &m_tdNightPeriodBegin)
		PARAMETER("m_tdNightPeriodEnd", LB1_TimeDurationType, &m_tdNightPeriodEnd)
	END_PARAMETER_BIND

		bool IsSupport(TStrategyTickType ticktype)
	{
		return true;
	}

	BEGIN_PROBE_BIND
		BEGIN_GRAPH
		PROBE(&m_adbAskPrice0, "AskPrice0", TProbe_Color_Green)
		PROBE(&m_adbBidPrice0, "BidPrice0", TProbe_Color_Blue)

		PROBE(&m_adbLongOpenLine0, "LongOpenLine0", TProbe_Color_Yellow)
		PROBE(&m_adbLongCloseLine0, "LongCloseLine0", TProbe_Color_Red)
		PROBE(&m_adbShortCloseLine0, "ShortCloseLine0", TProbe_Color_Green)
		PROBE(&m_adbShortOpenLine0, "ShortOpenLine0", TProbe_Color_Yellow)

		PROBE(&m_adbFilterLine0, "m_adbFilterLine0", TProbe_Color_Purple)
		PROBE(&m_adbFilterLine1, "m_adbFilterLine1", TProbe_Color_Purple)
		END_GRAPH("Line")

		BEGIN_GRAPH
		PROBE(&m_adbAskPrice1, "AskPrice1", TProbe_Color_Green)
		PROBE(&m_adbBidPrice1, "BidPrice1", TProbe_Color_Blue)

		PROBE(&m_adbLongOpenLine1, "LongOpenLine1", TProbe_Color_Yellow)
		PROBE(&m_adbLongCloseLine1, "LongCloseLine1", TProbe_Color_Red)
		PROBE(&m_adbShortCloseLine1, "ShortCloseLine1", TProbe_Color_Green)
		PROBE(&m_adbShortOpenLine1, "ShortOpenLine1", TProbe_Color_Yellow)
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

	double MyDequeMaxMin(const T_MyBar & MyBar, int Index, T_StrMyDequeMaxMinFlag Str);

	double MyDequeKentner(const T_MyBar & MyBar, int Index, T_StrMyDequeKentnerFlag Str);

	double MyDequeAverage(const T_MyBar & MyBar, int Index, T_StrMyDequeAverageFlag Str);

	TLastErrorIdType OnInit_FromArchive(ptime)
	{
		return TLastErrorIdType::LB1_NO_ERROR;
	};

private:

	atomic<double>  m_adbLongOpenLine0;
	atomic<double>  m_adbLongCloseLine0;
	atomic<double>  m_adbShortOpenLine0;
	atomic<double>  m_adbShortCloseLine0;

	atomic<double>  m_adbLongOpenLine1;
	atomic<double>  m_adbLongCloseLine1;
	atomic<double>  m_adbShortOpenLine1;
	atomic<double>  m_adbShortCloseLine1;

	atomic<double> m_adbAskPrice0;
	atomic<double> m_adbBidPrice0;

	atomic<double> m_adbAskPrice1;
	atomic<double> m_adbBidPrice1;

	atomic<double> m_adbFilterLine0;
	atomic<double> m_adbFilterLine1;
};

STRATEGY_TEMPLATE_DEFINITION(MyStrategy)


bool MyStrategy::OnInquiry(MStrategyInquiryDataInterface * data)
{
	return true;
}

TLastErrorIdType MyStrategy::OnInit(ptime pt)
{
	//自定义区域
	m_dateOderTradeDay = pt.date();

	m_tickData[0].m_datetimeUTCDateTime = m_tickData[1].m_datetimeUTCDateTime = not_a_date_time;

	m_intMaxParam = max(max(max(max(max(m_intMaxParam, m_intLongOpenWindowsLen), m_intLongCloseWindowsLen), m_intShortCloseWindowsLen), m_intShortOpenWindowsLen), m_intFilterLongLen);

	for (int i = 0;i < 2;i++)
	{
		for (int j = 0;j < 5;j++)
		{
			m_tickData[i].m_dbAskPrice[j] = NAN;
			m_tickData[i].m_dbBidPrice[j] = NAN;
			m_tickData[i].m_intAskVolume[j] = NAN;
			m_tickData[i].m_intBidVolume[j] = NAN;
			m_tickData[i].m_dbUpperLimitPrice = NAN;
			m_tickData[i].m_dbLowerLimitPrice = NAN;
		}
		m_tickData[i].m_dbLastPrice = NAN;
		m_tickData[i].m_intVolume = NAN;
		m_tickData[i].m_uDataID = NAN;
	}

	m_BarData[0].boolIsValid = m_BarData[1].boolIsValid = false;
	for (int i = 0;i < 2;i++)
	{
		m_BarData[i].openPrice.clear();
		m_BarData[i].highPrice.clear();
		m_BarData[i].lowPrice.clear();
		m_BarData[i].closePrice.clear();
	}

	m_boolInited[0] = m_boolInited[1] = false;
	m_boolCanUpdateLine = false;
	m_boolLongOpenFlag = false;
	m_boolShortOpenFlag = false;
	m_boolLongCloseFlag = false;
	m_boolShortCloseFlag = false;

	m_strStr = "";
	m_strSysid = "";

	m_intTradingCopies = 0;
	m_intCumulateTradedVolume = 0;

	m_dbLongOpenLine[0] = m_dbLongOpenLine[1] = NAN;
	m_dbLongCloseLine[0] = m_dbLongCloseLine[1] = NAN;
	m_dbShortOpenLine[0] = m_dbShortOpenLine[1] = NAN;
	m_dbShortCloseLine[0] = m_dbShortCloseLine[1] = NAN;
	m_dbFilterLine[0] = m_dbFilterLine[1] = NAN;

	m_ptimeGlobalLastTime = pt;
	m_ptimeGlobalMakeOderTime = not_a_date_time;

	m_enumHasPosition = NoPosition;

	m_enumState = PCL_Waiting;

	m_enumCanceledOrder = NoStates;
	//自定义区域

	return LB1_NO_ERROR;
}

void MyStrategy::OnTick(TMarketDataIdType dataid, const CTick *pDepthMarketData)
{

	//自定义区域
	m_ptimeGlobalLastTime = pDepthMarketData->m_datetimeUTCDateTime;

	if (pDepthMarketData->m_dbAskPrice[0] < 10e-8
		||
		pDepthMarketData->m_dbBidPrice[0] < 10e-8
		||
		pDepthMarketData->m_dbLastPrice < 10e-8
		)
		return;

	m_boolInited[dataid] = true;
	m_tickData[dataid] = *static_cast<const CFutureTick*>(pDepthMarketData);

	if (false == m_boolInited[0] || false == m_boolInited[1])
		return;

	if (
		(pDepthMarketData->m_datetimeUTCDateTime.time_of_day() > m_tdDayPeriodBegin
			&&
			pDepthMarketData->m_datetimeUTCDateTime.time_of_day() < m_tdDayPeriodEnd)
		||
		(pDepthMarketData->m_datetimeUTCDateTime.time_of_day() > m_tdNightPeriodBegin
			&&
			pDepthMarketData->m_datetimeUTCDateTime.time_of_day() < m_tdNightPeriodEnd)
		)
	{
		char buf[1024];
		if (true == MEDDLE(buf, 1024))
		{
			string cmd = buf;
			if ("show" == cmd)
				Show();
			else if ("position" == cmd)
			{
				position();
			}
			else if ("inc_copies" == cmd)
			{
				m_intCopies += 1;
			}
			else if ("dec_copies" == cmd)
			{
				m_intCopies -= 1;
			}
			else if ("on" == cmd)
			{
				m_boolSwitch = true;
			}
			else if ("off" == cmd)
			{
				m_boolSwitch = false;
			}
		}
	}
	else
		return;

	if (false == m_BarData[0].boolIsValid || false == m_BarData[1].boolIsValid)
	{

		m_BarData[0].openPrice.push_back(m_tickData[0].m_dbLastPrice);
		m_BarData[0].highPrice.push_back(m_tickData[0].m_dbLastPrice);
		m_BarData[0].lowPrice.push_back(m_tickData[0].m_dbLastPrice);
		m_BarData[0].closePrice.push_back(m_tickData[0].m_dbLastPrice);
		m_BarData[0].boolIsValid = true;

		m_BarData[1].openPrice.push_back(m_tickData[1].m_dbLastPrice);
		m_BarData[1].highPrice.push_back(m_tickData[1].m_dbLastPrice);
		m_BarData[1].lowPrice.push_back(m_tickData[1].m_dbLastPrice);
		m_BarData[1].closePrice.push_back(m_tickData[1].m_dbLastPrice);
		m_BarData[1].boolIsValid = true;

		m_intLastBarCycleNumber = (int)(pDepthMarketData->m_datetimeUTCDateTime.time_of_day().total_seconds() / BARSECONDNUMBER);

	}

	int SelfCycleNum = (int)(pDepthMarketData->m_datetimeUTCDateTime.time_of_day().total_seconds() / 60);

	if (SelfCycleNum == m_intLastBarCycleNumber)
	{

		m_BarData[0].highPrice[size(m_BarData[0].highPrice) - 1]
			= max(m_BarData[0].highPrice[size(m_BarData[0].highPrice) - 1], m_tickData[0].m_dbLastPrice);
		m_BarData[0].lowPrice[size(m_BarData[0].lowPrice) - 1]
			= min(m_BarData[0].lowPrice[size(m_BarData[0].lowPrice) - 1], m_tickData[0].m_dbLastPrice);
		m_BarData[0].closePrice[size(m_BarData[0].closePrice) - 1] = m_tickData[0].m_dbLastPrice;

		m_BarData[1].highPrice[size(m_BarData[1].highPrice) - 1]
			= max(m_BarData[1].highPrice[size(m_BarData[1].highPrice) - 1], m_tickData[1].m_dbLastPrice);
		m_BarData[1].lowPrice[size(m_BarData[1].lowPrice) - 1]
			= min(m_BarData[1].lowPrice[size(m_BarData[1].lowPrice) - 1], m_tickData[1].m_dbLastPrice);
		m_BarData[1].closePrice[size(m_BarData[1].closePrice) - 1] = m_tickData[1].m_dbLastPrice;

		m_boolCanUpdateLine = false;

	}
	else
	{

		m_BarData[0].openPrice.push_back(m_tickData[0].m_dbLastPrice);
		m_BarData[0].highPrice.push_back(m_tickData[0].m_dbLastPrice);
		m_BarData[0].lowPrice.push_back(m_tickData[0].m_dbLastPrice);
		m_BarData[0].closePrice.push_back(m_tickData[0].m_dbLastPrice);

		m_BarData[1].openPrice.push_back(m_tickData[1].m_dbLastPrice);
		m_BarData[1].highPrice.push_back(m_tickData[1].m_dbLastPrice);
		m_BarData[1].lowPrice.push_back(m_tickData[1].m_dbLastPrice);
		m_BarData[1].closePrice.push_back(m_tickData[1].m_dbLastPrice);

		m_intLastBarCycleNumber = SelfCycleNum;

		m_boolCanUpdateLine = true;

	}

	if (m_intMaxParam > size(m_BarData[0].closePrice))
	{
		return;
	}
	else if (m_intMaxParam < size(m_BarData[0].closePrice))
	{
		m_BarData[0].openPrice.erase(m_BarData[0].openPrice.begin());
		m_BarData[0].highPrice.erase(m_BarData[0].highPrice.begin());
		m_BarData[0].lowPrice.erase(m_BarData[0].lowPrice.begin());
		m_BarData[0].closePrice.erase(m_BarData[0].closePrice.begin());

		m_BarData[1].openPrice.erase(m_BarData[1].openPrice.begin());
		m_BarData[1].highPrice.erase(m_BarData[1].highPrice.begin());
		m_BarData[1].lowPrice.erase(m_BarData[1].lowPrice.begin());
		m_BarData[1].closePrice.erase(m_BarData[1].closePrice.begin());
	}

	m_dbFilterLine[0] =
		(accumulate(m_BarData[0].highPrice.end() - m_intFilterLongLen, m_BarData[0].highPrice.end(), 0)) / (double)m_intFilterLongLen;
	m_dbFilterLine[1] =
		(accumulate(m_BarData[0].highPrice.end() - m_intFilterShortLen, m_BarData[0].highPrice.end(), 0)) / (double)m_intFilterShortLen;

	if (true == m_boolCanUpdateLine)
	{

		if (ChannelBreakOut_UpperLowerClose == m_enumStrategyName)
		{
			m_enumMyDequeMaxMinFlag = ChannelMAX;
			m_dbLongOpenLine[0] = MyDequeMaxMin(m_BarData[0], m_intLongOpenWindowsLen, m_enumMyDequeMaxMinFlag);
			m_dbLongOpenLine[1] = MyDequeMaxMin(m_BarData[1], m_intLongOpenWindowsLen, m_enumMyDequeMaxMinFlag);
			m_dbShortCloseLine[0] = MyDequeMaxMin(m_BarData[0], m_intShortCloseWindowsLen, m_enumMyDequeMaxMinFlag);
			m_dbShortCloseLine[1] = MyDequeMaxMin(m_BarData[1], m_intShortCloseWindowsLen, m_enumMyDequeMaxMinFlag);

			m_enumMyDequeMaxMinFlag = ChannelMIN;
			m_dbShortOpenLine[0] = MyDequeMaxMin(m_BarData[0], m_intShortOpenWindowsLen, m_enumMyDequeMaxMinFlag);
			m_dbShortOpenLine[1] = MyDequeMaxMin(m_BarData[1], m_intShortOpenWindowsLen, m_enumMyDequeMaxMinFlag);
			m_dbLongCloseLine[0] = MyDequeMaxMin(m_BarData[0], m_intLongCloseWindowsLen, m_enumMyDequeMaxMinFlag);
			m_dbLongCloseLine[1] = MyDequeMaxMin(m_BarData[1], m_intLongCloseWindowsLen, m_enumMyDequeMaxMinFlag);
		}
		else if (ChannelBreakOut_MiddleClose == m_enumStrategyName)
		{

			double Middle[2];
			double Diff[2];

			m_enumMyDequeMaxMinFlag = ChannelMAX;
			m_dbLongOpenLine[0] = MyDequeMaxMin(m_BarData[0], m_intLongOpenWindowsLen, m_enumMyDequeMaxMinFlag);
			m_dbLongOpenLine[1] = MyDequeMaxMin(m_BarData[1], m_intLongOpenWindowsLen, m_enumMyDequeMaxMinFlag);

			m_enumMyDequeMaxMinFlag = ChannelMIN;
			m_dbShortOpenLine[0] = MyDequeMaxMin(m_BarData[0], m_intShortOpenWindowsLen, m_enumMyDequeMaxMinFlag);
			m_dbShortOpenLine[1] = MyDequeMaxMin(m_BarData[1], m_intShortOpenWindowsLen, m_enumMyDequeMaxMinFlag);

			Middle[0] = (m_dbLongOpenLine[0] + m_dbShortOpenLine[0]) / 2;
			Middle[1] = (m_dbLongOpenLine[1] + m_dbShortOpenLine[1]) / 2;
			Diff[0] = (m_dbLongOpenLine[0] - m_dbShortOpenLine[0]) / 2;
			Diff[1] = (m_dbLongOpenLine[1] - m_dbShortOpenLine[1]) / 2;

			m_dbLongOpenLine[0] += Diff[0] * m_dbOpenWidth;
			m_dbLongOpenLine[1] += Diff[1] * m_dbOpenWidth;
			m_dbShortOpenLine[0] -= Diff[0] * m_dbOpenWidth;
			m_dbShortOpenLine[1] -= Diff[1] * m_dbOpenWidth;

			m_dbLongCloseLine[0] = Middle[0] - Diff[0] * m_dbCloseWidth;
			m_dbShortCloseLine[0] = Middle[0] + Diff[0] * m_dbCloseWidth;

			m_dbLongCloseLine[1] = Middle[1] - Diff[1] * m_dbCloseWidth;
			m_dbShortCloseLine[1] = Middle[1] + Diff[1] * m_dbCloseWidth;

		}
		else if (KentnerBreakOut == m_enumStrategyName)
		{

			double Upper[2];
			double Lower[2];
			double Middle[2];

			m_enumMyDequeKentnerFlag = KentnerUpper;
			Upper[0] = MyDequeKentner(m_BarData[0], m_intLongOpenWindowsLen, m_enumMyDequeKentnerFlag);
			Upper[1] = MyDequeKentner(m_BarData[1], m_intLongOpenWindowsLen, m_enumMyDequeKentnerFlag);
			
			m_enumMyDequeKentnerFlag = KentnerLower;
			Lower[0] = MyDequeKentner(m_BarData[0], m_intLongOpenWindowsLen, m_enumMyDequeKentnerFlag);
			Lower[1] = MyDequeKentner(m_BarData[1], m_intLongOpenWindowsLen, m_enumMyDequeKentnerFlag);

			Middle[0] = MyDequeAverage(m_BarData[0], m_intLongOpenWindowsLen, m_enumMyDequeAverageFlag);
			Middle[1] = MyDequeAverage(m_BarData[1], m_intLongOpenWindowsLen, m_enumMyDequeAverageFlag);

			m_dbLongOpenLine[0] = Middle[0] + m_dbOpenWidth*(Upper[0] - Lower[0]);
			m_dbLongOpenLine[1] = Middle[1] + m_dbOpenWidth*(Upper[1] - Lower[1]);

			m_dbShortOpenLine[0] = Middle[0] - m_dbOpenWidth*(Upper[0] - Lower[0]);
			m_dbShortOpenLine[1] = Middle[1] - m_dbOpenWidth*(Upper[1] - Lower[1]);

			m_dbLongCloseLine[0] = Middle[0] + m_dbCloseWidth*(Upper[0] - Lower[0]);
			m_dbLongCloseLine[1] = Middle[1] + m_dbCloseWidth*(Upper[1] - Lower[1]);

			m_dbShortCloseLine[0] = Middle[0] - m_dbCloseWidth*(Upper[0] - Lower[0]);
			m_dbShortCloseLine[1] = Middle[1] - m_dbCloseWidth*(Upper[1] - Lower[1]);

		}

	}

#pragma region

	m_adbLongOpenLine0 = m_dbLongOpenLine[0];
	m_adbLongCloseLine0 = m_dbLongCloseLine[0];
	m_adbShortOpenLine0 = m_dbShortOpenLine[0];
	m_adbShortCloseLine0 = m_dbShortCloseLine[0];

	m_adbAskPrice0 = m_tickData[0].m_dbAskPrice[0];
	m_adbBidPrice0 = m_tickData[0].m_dbBidPrice[0];

	m_adbLongOpenLine1 = m_dbLongOpenLine[1];
	m_adbLongCloseLine1 = m_dbLongCloseLine[1];
	m_adbShortOpenLine1 = m_dbShortOpenLine[1];
	m_adbShortCloseLine1 = m_dbShortCloseLine[1];

	m_adbAskPrice1 = m_tickData[1].m_dbAskPrice[0];
	m_adbBidPrice1 = m_tickData[1].m_dbBidPrice[0];

	m_adbFilterLine0 = m_dbFilterLine[0];
	m_adbFilterLine1 = m_dbFilterLine[1];

#pragma endregion

	switch (m_enumFilterFlag)
	{
	case NoFilter:
	{

		if (m_tickData[0].m_dbBidPrice[0] > m_dbLongOpenLine[0])
		{
			m_boolLongOpenFlag = true;
		}
		else
		{
			m_boolLongOpenFlag = false;
		}

		if (m_tickData[0].m_dbAskPrice[0] < m_dbShortOpenLine[0])
		{
			m_boolShortOpenFlag = true;
		}
		else
		{
			m_boolShortOpenFlag = false;
		}

		if (m_tickData[0].m_dbAskPrice[0] < m_dbLongCloseLine[0])
		{
			m_boolLongCloseFlag = true;
		}
		else
		{
			m_boolLongCloseFlag = false;
		}

		if (m_tickData[0].m_dbBidPrice[0] > m_dbShortCloseLine[0])
		{
			m_boolShortCloseFlag = true;
		}
		else
		{
			m_boolShortCloseFlag = false;
		}

	}break;
	case ChannelFilter:
	{

		if (m_dbFilterLine[0] < m_dbFilterLine[1])
		{
			if (m_tickData[0].m_dbBidPrice[0] > m_dbLongOpenLine[0] && m_tickData[1].m_dbBidPrice[1] < m_dbLongOpenLine[1])
			{
				m_boolLongOpenFlag = true;
			}
			else
			{
				m_boolLongOpenFlag = false;
			}

			if (m_tickData[0].m_dbAskPrice[0] < m_dbShortOpenLine[0] && m_tickData[1].m_dbAskPrice[1] > m_dbShortOpenLine[1])
			{
				m_boolShortOpenFlag = true;
			}
			else
			{
				m_boolShortOpenFlag = false;
			}
		}
		else
		{
			if (m_tickData[0].m_dbBidPrice[0] > m_dbLongOpenLine[0] && m_tickData[1].m_dbBidPrice[1] > m_dbLongOpenLine[1])
			{
				m_boolLongOpenFlag = true;
			}
			else
			{
				m_boolLongOpenFlag = false;
			}

			if (m_tickData[0].m_dbAskPrice[0] < m_dbShortOpenLine[0] && m_tickData[1].m_dbAskPrice[1] < m_dbShortOpenLine[1])
			{
				m_boolShortOpenFlag = true;
			}
			else
			{
				m_boolShortOpenFlag = false;
			}
		}


		if (m_tickData[0].m_dbAskPrice[0] < m_dbLongCloseLine[0])
		{
			m_boolLongCloseFlag = true;
		}
		else
		{
			m_boolLongCloseFlag = false;
		}

		if (m_tickData[0].m_dbBidPrice[0] > m_dbShortCloseLine[0])
		{
			m_boolShortCloseFlag = true;
		}
		else
		{
			m_boolShortCloseFlag = false;
		}

	}break;
	}

	if (m_boolSwitch == false)
		return;

	if (
		(pDepthMarketData->m_datetimeUTCDateTime.time_of_day() > m_tdDayPeriodBegin
			&&
			pDepthMarketData->m_datetimeUTCDateTime.time_of_day() < m_tdDayPeriodEnd)
		||
		(pDepthMarketData->m_datetimeUTCDateTime.time_of_day() > m_tdNightPeriodBegin
			&&
			pDepthMarketData->m_datetimeUTCDateTime.time_of_day() < m_tdNightPeriodEnd)
		)
	{
		switch (m_enumState)
		{

		case PCL_Waiting: {

			if (
				(pDepthMarketData->m_datetimeUTCDateTime.time_of_day() > m_tdDayPeriodBegin
					&&
					pDepthMarketData->m_datetimeUTCDateTime.time_of_day() < m_tdDayPeriodEnd - minutes(1))
				||
				(pDepthMarketData->m_datetimeUTCDateTime.time_of_day() > m_tdNightPeriodBegin
					&&
					pDepthMarketData->m_datetimeUTCDateTime.time_of_day() < m_tdNightPeriodEnd)
				)
			{
				if (NoPosition == m_enumHasPosition && m_boolLongOpenFlag)
				{
					m_intTradingCopies = m_intCopies;
					m_llOderNumber =
						LIMITORDER(LB1_Buy, LB1_Increase, m_intTradingCopies,
							min(m_tickData[m_intTradingDataID].m_dbLastPrice + m_intBadSlipTickCount*m_dbMinPriceTick, m_tickData[m_intTradingDataID].m_dbUpperLimitPrice),
							m_intTradingDataID);
					m_strSysid = "";
					m_ptimeGlobalMakeOderTime = pDepthMarketData->m_datetimeUTCDateTime;
					m_dateOderTradeDay = GetTradeday(m_ptimeGlobalMakeOderTime);
					m_intCumulateTradedVolume = 0;
					m_enumState = PCL_EnterWaiting;
					m_enumHasPosition = HasLongPosition;
					return;
				}//多单进场

				if (NoPosition == m_enumHasPosition && m_boolShortOpenFlag)
				{
					m_intTradingCopies = m_intCopies;
					m_llOderNumber =
						LIMITORDER(LB1_Sell, LB1_Increase, m_intTradingCopies,
							max(m_tickData[m_intTradingDataID].m_dbLastPrice - m_intBadSlipTickCount*m_dbMinPriceTick, m_tickData[m_intTradingDataID].m_dbLowerLimitPrice),
							m_intTradingDataID);
					m_strSysid = "";
					m_ptimeGlobalMakeOderTime = pDepthMarketData->m_datetimeUTCDateTime;
					m_dateOderTradeDay = GetTradeday(m_ptimeGlobalMakeOderTime);
					m_intCumulateTradedVolume = 0;
					m_enumState = PCL_EnterWaiting;
					m_enumHasPosition = HasShortPosition;
					return;
				}//空单进场
			}

		};break;//场外等待中

		case PCL_EnterWaiting: {

			if (m_ptimeGlobalLastTime - m_ptimeGlobalMakeOderTime > seconds(m_intOrderWaitTime))
			{
				CANCEL(m_llOderNumber, (char *)m_strSysid.c_str(), m_intTradingDataID);
				m_enumState = PCL_EnterCancelOrderWaiting;
			}

		};break;//进场未成交

		case PCL_EnterPartlyTraded: {

			if (m_ptimeGlobalLastTime - m_ptimeGlobalMakeOderTime > seconds(m_intOrderWaitTime))
			{
				CANCEL(m_llOderNumber, (char *)m_strSysid.c_str(), m_intTradingDataID);
				m_enumState = PCL_EnterCancelOrderWaiting;
			}

		};break;//进场部分成交

		case PCL_Inside: {

			if (HasLongPosition == m_enumHasPosition && m_boolLongCloseFlag)
			{
				m_llOderNumber =
					LIMITORDER(LB1_Sell,
						GetTradeday(m_ptimeGlobalLastTime) == m_dateOderTradeDay ? LB1_DecreaseToday : LB1_Decrease,
						m_intCopies,
						max(m_tickData[m_intTradingDataID].m_dbLastPrice - m_intBadSlipTickCount*m_dbMinPriceTick, m_tickData[m_intTradingDataID].m_dbLowerLimitPrice),
						m_intTradingDataID);
				m_ptimeGlobalMakeOderTime = pDepthMarketData->m_datetimeUTCDateTime;
				m_strSysid = "";
				m_intCumulateTradedVolume = 0;
				m_enumState = PCL_ExitWaiting;
				return;
			}//多单出场

			if (HasShortPosition == m_enumHasPosition && m_boolShortCloseFlag)
			{
				m_llOderNumber =
					LIMITORDER(LB1_Buy, 
						GetTradeday(m_ptimeGlobalLastTime) == m_dateOderTradeDay ? LB1_DecreaseToday : LB1_Decrease, 
						m_intCopies,
						min(m_tickData[m_intTradingDataID].m_dbLastPrice + m_intBadSlipTickCount*m_dbMinPriceTick, m_tickData[m_intTradingDataID].m_dbUpperLimitPrice),
						m_intTradingDataID);
				m_ptimeGlobalMakeOderTime = pDepthMarketData->m_datetimeUTCDateTime;
				m_strSysid = "";
				m_intCumulateTradedVolume = 0;
				m_enumState = PCL_ExitWaiting;
				return;
			}//空单出场

		};break;//进场已成交

		case PCL_ExitWaiting: {

			if (m_ptimeGlobalLastTime - m_ptimeGlobalMakeOderTime > seconds(m_intOrderWaitTime))
			{
				if (m_strSysid.empty() == false)
				{
					CANCEL(m_llOderNumber, (char *)m_strSysid.c_str(), m_intTradingDataID);
					m_enumState = PCL_ExitCancelOrderWaiting;
				}
			}

		};break;//出场未成交

		case PCL_ExitPartlyTraded: {

			if (m_ptimeGlobalLastTime - m_ptimeGlobalMakeOderTime > seconds(m_intOrderWaitTime))
			{
				if (m_strSysid.empty() == false)
				{
					CANCEL(m_llOderNumber, (char *)m_strSysid.c_str(), m_intTradingDataID);
					m_enumState = PCL_ExitCancelOrderWaiting;
				}
			}

		};break;//出场部分成交

		case PCL_EnterCancelOrderWaiting: {

			if (0 < m_intCumulateTradedVolume)
			{
				if (HasLongPosition == m_enumHasPosition && EnterCanceledOrder == m_enumCanceledOrder)
				{
					m_intTradingCopies = m_intCopies - m_intCumulateTradedVolume;
					m_llOderNumber =
						LIMITORDER(LB1_Buy, LB1_Increase, m_intTradingCopies,
							min(m_tickData[m_intTradingDataID].m_dbLastPrice + m_intBadSlipTickCount*m_dbMinPriceTick, m_tickData[m_intTradingDataID].m_dbUpperLimitPrice),
							m_intTradingDataID);
					m_strSysid = "";
					m_ptimeGlobalMakeOderTime = pDepthMarketData->m_datetimeUTCDateTime;
					m_dateOderTradeDay = GetTradeday(m_ptimeGlobalMakeOderTime);
					m_enumState = PCL_EnterWaiting;
					return;
				}
				else if (HasShortPosition == m_enumHasPosition)
				{
					m_intTradingCopies = m_intCopies - m_intCumulateTradedVolume;
					m_llOderNumber =
						LIMITORDER(LB1_Sell, LB1_Increase, m_intTradingCopies,
							max(m_tickData[m_intTradingDataID].m_dbLastPrice - m_intBadSlipTickCount*m_dbMinPriceTick, m_tickData[m_intTradingDataID].m_dbLowerLimitPrice),
							m_intTradingDataID);
					m_strSysid = "";
					m_ptimeGlobalMakeOderTime = pDepthMarketData->m_datetimeUTCDateTime;
					m_dateOderTradeDay = GetTradeday(m_ptimeGlobalMakeOderTime);
					m_enumState = PCL_EnterWaiting;
					m_enumHasPosition = HasShortPosition;
					return;
				}
			}
			else if (0 == m_intCumulateTradedVolume && EnterCanceledOrder == m_enumCanceledOrder)
			{
				m_enumState = PCL_Waiting;
				m_enumHasPosition = NoPosition;
				return;
			}

		};break;//入场撤单已发

		case PCL_ExitCancelOrderWaiting: {

			if (HasLongPosition == m_enumHasPosition && ExitCanceledOrder == m_enumCanceledOrder)
			{
				m_intTradingCopies = m_intCopies - m_intCumulateTradedVolume;
				m_llOderNumber =
					LIMITORDER(LB1_Sell, 
						GetTradeday(m_ptimeGlobalLastTime) == m_dateOderTradeDay ? LB1_DecreaseToday : LB1_Decrease, 
						m_intTradingCopies,
						max(m_tickData[m_intTradingDataID].m_dbLastPrice - m_intBadSlipTickCount*m_dbMinPriceTick, m_tickData[m_intTradingDataID].m_dbLowerLimitPrice),
						m_intTradingDataID);
				m_ptimeGlobalMakeOderTime = pDepthMarketData->m_datetimeUTCDateTime;
				m_strSysid = "";
				m_enumState = PCL_ExitWaiting;
				return;
			}
			else if (HasShortPosition == m_enumHasPosition && ExitCanceledOrder == m_enumCanceledOrder)
			{
				m_intTradingCopies = m_intCopies - m_intCumulateTradedVolume;
				m_llOderNumber =
					LIMITORDER(LB1_Buy, 
						GetTradeday(m_ptimeGlobalLastTime) == m_dateOderTradeDay ? LB1_DecreaseToday : LB1_Decrease, 
						m_intTradingCopies,
						min(m_tickData[m_intTradingDataID].m_dbLastPrice + m_intBadSlipTickCount*m_dbMinPriceTick, m_tickData[m_intTradingDataID].m_dbUpperLimitPrice),
						m_intTradingDataID);
				m_ptimeGlobalMakeOderTime = pDepthMarketData->m_datetimeUTCDateTime;
				m_strSysid = "";
				m_enumState = PCL_ExitWaiting;
				return;
			}

		};break;//出场撤单已发

		}
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
	switch (m_enumState)
	{
	case PCL_Waiting: {};break;//场外等待中
	case PCL_EnterWaiting: {
		m_intCumulateTradedVolume += volume;
		if (m_intCopies == m_intCumulateTradedVolume)
		{
			m_enumState = PCL_Inside;
		}
		else
		{
			m_enumState = PCL_EnterPartlyTraded;
		}
	};break;//进场未成交
	case PCL_EnterPartlyTraded: {
		m_intCumulateTradedVolume += volume;
		if (m_intCopies == m_intCumulateTradedVolume)
		{
			m_enumState = PCL_Inside;
		}
		else
		{
			m_enumState = PCL_EnterPartlyTraded;
		}

	};break;//进场部分成交
	case PCL_Inside: {};break;//进场已成交
	case PCL_ExitWaiting: {
		m_intCumulateTradedVolume += volume;
		if (m_intCopies == m_intCumulateTradedVolume)
		{
			m_enumState = PCL_Waiting;
			m_enumHasPosition = NoPosition;
			INQUIRY(new CInquiryMarkNetCurvePoint());
		}
		else
		{
			m_enumState = PCL_ExitPartlyTraded;
		}
	};break;//出场未成交
	case PCL_ExitPartlyTraded: {
		m_intCumulateTradedVolume += volume;
		if (m_intCopies == m_intCumulateTradedVolume)
		{
			m_enumState = PCL_Waiting;
			m_enumHasPosition = NoPosition;
			INQUIRY(new CInquiryMarkNetCurvePoint());
		}
		else
		{
			m_enumState = PCL_ExitPartlyTraded;
		}
	};break;//出场部分成交
	case PCL_EnterCancelOrderWaiting: {
		m_intCumulateTradedVolume += volume;
		if (m_intCopies == m_intCumulateTradedVolume)
		{
			m_enumState = PCL_Inside;
		}
		else
		{
			m_enumState = PCL_EnterPartlyTraded;
		}

	};break;//入场撤单已发
	case PCL_ExitCancelOrderWaiting: {
		m_intCumulateTradedVolume += volume;
		if (m_intCopies == m_intCumulateTradedVolume)
		{
			m_enumState = PCL_Waiting;
			m_enumHasPosition = NoPosition;
			INQUIRY(new CInquiryMarkNetCurvePoint());
		}
		else
		{
			m_enumState = PCL_ExitPartlyTraded;
		}

	};break;//出场撤单已发
	}

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
	{
		switch (m_enumState)
		{
		case PCL_Waiting: {};break;//场外等待中
		case PCL_EnterWaiting: {};break;//进场未成交
		case PCL_EnterPartlyTraded: {};break;//进场部分成交
		case PCL_Inside: {};break;//进场已成交
		case PCL_ExitWaiting: {};break;//出场未成交
		case PCL_ExitPartlyTraded: {};break;//出场部分成交
		case PCL_EnterCancelOrderWaiting: {

			m_enumCanceledOrder = EnterCanceledOrder;

		};break;//入场撤单已发
		case PCL_ExitCancelOrderWaiting: {

			m_enumCanceledOrder = ExitCanceledOrder;

		};break;//出场撤单已发
		}
	}
	else
	{
		m_strSysid = sysId;
	}
}

double MyStrategy::MyDequeMaxMin(const T_MyBar & MyBar, int Index, T_StrMyDequeMaxMinFlag Str)
{
	deque<double>::const_iterator Iter;
	if (ChannelMAX == Str)
	{
		auto result = minmax_element(MyBar.highPrice.end() - Index, MyBar.highPrice.end() - 1);
		return *result.second;
	}
	else if (ChannelMIN == Str)
	{
		auto result = minmax_element(MyBar.lowPrice.end() - Index, MyBar.lowPrice.end() - 1);
		return *result.first;
	}
	else
	{
		LOG("MyVectorMaxMin Function String input is Error");
		return 0.0;
	}
}

double MyStrategy::MyDequeKentner(const T_MyBar & MyBar, int Index, T_StrMyDequeKentnerFlag Str)
{
	deque<double>::const_iterator Iter;

	double Upper = 0;
	double Lower = 0;

	if (KentnerUpper == Str)
	{
		Upper = accumulate(MyBar.highPrice.end() - Index, MyBar.highPrice.end(), 0);
		Upper /= Index;
		return Upper;
	}
	else if (KentnerLower == Str)
	{
		Lower = accumulate(MyBar.lowPrice.end() - Index, MyBar.lowPrice.end(), 0);
		Lower /= Index;
		return Lower;
	}
	else
	{
		LOG("MyDequeKentner Function String input is Error");
		return 0.0;
	}
}

double MyStrategy::MyDequeAverage(const T_MyBar & MyBar, int Index, T_StrMyDequeAverageFlag Str)
{
	deque<double>::const_iterator Iter;

	double Mean = 0;

	if (MA == Str)
	{
		Mean = accumulate(MyBar.closePrice.end() - Index, MyBar.closePrice.end() , 0);
		Mean /= Index;
		return Mean;
	}
	else if(EMA == Str)
	{
		double IndexSum = 0;
		for (int i = 1;i <= Index;i++)
		{
			Mean += MyBar.closePrice[MyBar.closePrice.size() - i]*(Index-i+1);
			IndexSum += Index - i + 1;
		}
		Mean /= IndexSum;
		return Mean;
	}
	else
	{
		LOG("MyDequeAverage Function String input is Error");
		return 0.0;
	}
}