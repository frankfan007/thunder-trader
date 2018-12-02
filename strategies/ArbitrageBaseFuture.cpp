// StrategyDemo.cpp : 定义 DLL 应用程序的导出函数。
//
#define _EXPORTS
#define EXPORT_STRATEGY
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
#include "ArbitrageBaseFuture.h"
#include "OrderRefResolve.h"
#include <numeric>
using namespace std;

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

bool CArbitrageBase::IsSupport(TStrategyTickType ticktype)
{
	return true;
};

CArbitrageBase::CArbitrageBase(MStrategyContext* context, TStrategyIdType strategyid) :
g_pStrategyContext(context),
g_StrategyId(strategyid)
{
	if (context == NULL)
		throw 1;
}

void CArbitrageBase::OnLoad(const char * fname)
{
	if (nullptr == fname)
		throw runtime_error("fname is nullptr.");
	ifstream is(fname, std::ios::binary);
	if (is.is_open())
	{
		boost::archive::binary_iarchive ia(is);
		ia >> *this;is.close();
	}
	else throw runtime_error("can not open file.");
}

void CArbitrageBase::OnSave(const char * fname)
{
	if (nullptr == fname)  
		throw runtime_error("fname is nullptr.");
	ofstream os(fname, std::ios::binary);
	if (os.is_open()) 
	{ 
		boost::archive::binary_oarchive oa(os);
		oa << *this;os.close(); 
	}
	else 
		throw runtime_error("can not open file.");
}

TLastErrorIdType CArbitrageBase::OnInit(ptime currentTime)
{
	double Value=0.0;
	if (SHAREDVALUE_GET(m_intSharedValueIndex, Value) == false)
	{
		LOG("Can not find shared value index %d", m_intSharedValueIndex);
		return TLastErrorIdType::LB1_INVALID_VAL;
	}
	LOG("Strategy:%d-Valid Copies=%lf",g_StrategyId ,Value);
#pragma region 全局变量区
	m_datePositionTradeDay = ptime(not_a_date_time).date();
	/*m_Position[Subtrahend_Index].Init();
	m_Position[Minuend_Index].Init();*/
	m_Position[Subtrahend_Index].Init();
	m_Position[Minuend_Index].Init();
	m_ptimeGlobalCurrentTime = min_date_time;
	m_Order[Subtrahend_Index].Init();
	m_Order[Minuend_Index].Init();
	m_booIsPedding = false;
	m_boolHasInitialize[0] = m_boolHasInitialize[1] = false;
#pragma endregion
	ShowPosition();
	SetPresupposedPosition(currentTime,PresupposedPositionType,PresupposedPositionTradeDayType);
	return LB1_NO_ERROR;
}

TLastErrorIdType CArbitrageBase::OnInit_FromArchive(ptime currentTime)
{
	SetPresupposedPosition(currentTime,PresupposedPositionType, PresupposedPositionTradeDayType);
	ShowPosition();
	return LB1_NO_ERROR;
}

bool CArbitrageBase::OnGetPositionInfo(int *)
{
	return false;
};

bool CArbitrageBase::OnGetCustomInfo(char * out_buf, size_t len)
{
	if (0 == GetMinuendPosition().m_uLongPosition
		&&
		0 == GetMinuendPosition().m_uShortPosition
		&&
		0 == GetSubtrahendPosition().m_uLongPosition
		&&
		0 == GetSubtrahendPosition().m_uShortPosition)
	{
		snprintf(out_buf, len, "empty");
	}
	else if (
		(0 == GetMinuendPosition().m_uLongPosition
			&&
			0 != GetMinuendPosition().m_uShortPosition
			&&
			0 != GetSubtrahendPosition().m_uLongPosition
			&&
			0 == GetSubtrahendPosition().m_uShortPosition)
		||
		(0 != GetMinuendPosition().m_uLongPosition
			&&
			0 == GetMinuendPosition().m_uShortPosition
			&&
			0 == GetSubtrahendPosition().m_uLongPosition
			&&
			0 != GetSubtrahendPosition().m_uShortPosition)
		)
	{
		snprintf(out_buf, len, "M(%.2u|%.2u) S(%.2u|%.2u)",
			GetMinuendPosition().m_uLongPosition,
			GetMinuendPosition().m_uShortPosition,
			GetSubtrahendPosition().m_uLongPosition,
			GetSubtrahendPosition().m_uShortPosition);
	}
	else
	{
		snprintf(out_buf, len, "error");
	}
	return true;
};

bool CArbitrageBase::OnGetFloatingProfit(double *)
{
	return false;
}

bool CArbitrageBase::OnGetStatus(char *, size_t)
{
	return false;
}


void CArbitrageBase::MakeOrder(TTradeSignalType Signal, unsigned int copies, TVolumeType MinuendVolume, TVolumeType SubtrahendVolume)
{
	if (TEnumBuyMinuend_SellSubtrahend_Increase == Signal
		||
		TEnumSellMinuend_BuySubtrahend_Increase == Signal)
	{
		if (SHAREDVALUE_DEC(m_intSharedValueIndex, copies, [&copies](double c) {
			return c >= copies;
		}))
		{
			m_uLockedCopies = copies;
		}
		else
			return;
	}
	m_Order[Minuend_Index].vol = MinuendVolume;
	m_Order[Subtrahend_Index].vol = SubtrahendVolume;

	m_Order[Minuend_Index].dealVol = 0;
	m_Order[Subtrahend_Index].dealVol = 0;

	m_booIsPedding = true;
	

	switch (Signal)
	{
	case TEnumBuyMinuend_SellSubtrahend_Increase://对应于LB1_BuyO
	{
		
		 
		auto temp_ref = g_pStrategyContext->MakeOrder(g_StrategyId, static_cast<TOrderType>(MakeOrderType),
		LB1_Buy,
		LB1_Increase,
		m_Order[Minuend_Index].vol,
		m_tickData[Minuend_Index].m_dbAskPrice[0] + MinuendBadSlipTickCount*MinuendMinPriceTick,
		Minuend_Index, 0);
		if (LB1_NullOrderRef == temp_ref)
		{
			if (SHAREDVALUE_INC(m_intSharedValueIndex, copies, [](double) {return true;}) == false)
			{
				LOG("Can not increase shared value[%d]", m_intSharedValueIndex);
			}
			return;
		}
		m_Order[Minuend_Index].inRef = temp_ref;
		get<TrR_BetTradedList>(m_lstTradedLog[Minuend_Index]).clear();
		get<TrR_BetTradedList>(m_lstTradedLog[Subtrahend_Index]).clear();

		get<TrR_Direction>(m_lstTradedLog[Minuend_Index]) = LB1_Buy;

		get<TrR_Direction>(m_lstTradedLog[Subtrahend_Index]) = LB1_Sell;

		get<TrR_BetTarPrice>(m_lstTradedLog[Minuend_Index]) = m_tickData[Minuend_Index].m_dbAskPrice[0];
		get<TrR_BetTime>(m_lstTradedLog[Minuend_Index]) = m_ptimeGlobalCurrentTime;

		LOG("CustomArbitrageStrategy[%d]: TEnumBuyMinuend_SellSubtrahend_Increase", g_StrategyId);
		m_Order[Minuend_Index].inSysID = "";
		m_Order[Minuend_Index].orderTime = m_ptimeGlobalCurrentTime;

		m_Order[Minuend_Index].tradeState = T_OI_W;
		m_Order[Subtrahend_Index].tradeState = T_OI_INI;

		m_Order[Minuend_Index].direction_In = LB1_Buy;
		m_Order[Subtrahend_Index].direction_In = LB1_Sell;

		m_Order[Minuend_Index].direction_Out = LB1_Sell;
		m_Order[Subtrahend_Index].direction_Out = LB1_Buy;
	};
	break;
	case TEnumSellMinuend_BuySubtrahend_Increase://对应于LB1_SellO
	{
		auto temp_ref = g_pStrategyContext->MakeOrder(g_StrategyId, static_cast<TOrderType>(MakeOrderType),
			LB1_Sell,
			LB1_Increase,
			m_Order[Minuend_Index].vol,
			m_tickData[Minuend_Index].m_dbBidPrice[0] - MinuendBadSlipTickCount*MinuendMinPriceTick,
			Minuend_Index, 0);
		if (LB1_NullOrderRef == temp_ref)
		{
			if (SHAREDVALUE_INC(m_intSharedValueIndex, copies, [](double) {return true;}) == false)
			{
				LOG("Can not increase shared value[%d]", m_intSharedValueIndex);
			}
			return;
		}
		m_Order[Minuend_Index].inRef = temp_ref;
		get<TrR_BetTradedList>(m_lstTradedLog[Minuend_Index]).clear();
		get<TrR_BetTradedList>(m_lstTradedLog[Subtrahend_Index]).clear();

		get<TrR_Direction>(m_lstTradedLog[Minuend_Index]) = LB1_Sell;

		get<TrR_Direction>(m_lstTradedLog[Subtrahend_Index]) = LB1_Buy;

		get<TrR_BetTarPrice>(m_lstTradedLog[Minuend_Index]) = m_tickData[Minuend_Index].m_dbBidPrice[0];
		get<TrR_BetTime>(m_lstTradedLog[Minuend_Index]) = m_ptimeGlobalCurrentTime;
		LOG("CustomArbitrageStrategy[%d]: TEnumSellMinuend_BuySubtrahend_Increase", g_StrategyId);
		m_Order[Minuend_Index].inSysID = "";
		m_Order[Minuend_Index].orderTime = m_ptimeGlobalCurrentTime;

		m_Order[Minuend_Index].tradeState = T_OI_W;
		m_Order[Subtrahend_Index].tradeState = T_OI_INI;

		m_Order[Minuend_Index].direction_In = LB1_Sell;
		m_Order[Subtrahend_Index].direction_In = LB1_Buy;

		m_Order[Minuend_Index].direction_Out = LB1_Buy;
		m_Order[Subtrahend_Index].direction_Out = LB1_Sell;
	}
	break;
	case TEnumBuyMinuend_SellSubtrahend_Descrease://对应于LB1_BuyC
	{
		TOrderOffsetType offset;
		if (GetTradeday(m_ptimeGlobalCurrentTime) == m_datePositionTradeDay)
			offset = LB1_DecreaseToday;
		else
			offset = LB1_Decrease;

		auto temp_ref = g_pStrategyContext->MakeOrder(g_StrategyId,static_cast<TOrderType>(MakeOrderType),
			LB1_Buy,
			offset,
			m_Order[Minuend_Index].vol,
			m_tickData[Minuend_Index].m_dbAskPrice[0] + MinuendBadSlipTickCount*MinuendMinPriceTick,
			Minuend_Index,
			static_cast<unsigned int>(offset)
			);//以最新价挂单下单
		if (LB1_NullOrderRef == temp_ref)
			return;
		m_Order[Minuend_Index].outRef = temp_ref;
		get<TrR_ClearTradedList>(m_lstTradedLog[Minuend_Index]).clear();
		get<TrR_ClearTradedList>(m_lstTradedLog[Subtrahend_Index]).clear();

		

		get<TrR_ClearTarPrice>(m_lstTradedLog[Minuend_Index]) = m_tickData[Minuend_Index].m_dbAskPrice[0];
		get<TrR_ClearTime>(m_lstTradedLog[Minuend_Index]) = m_ptimeGlobalCurrentTime;
		LOG("CustomArbitrageStrategy[%d]: TEnumBuyMinuend_SellSubtrahend_Descrease", g_StrategyId);
		m_Order[Minuend_Index].outSysID = "";
		m_Order[Minuend_Index].orderTime = m_ptimeGlobalCurrentTime;

		m_Order[Minuend_Index].tradeState = T_OO_W;
		m_Order[Subtrahend_Index].tradeState = T_OO_INI;

		m_Order[Minuend_Index].direction_In = LB1_Sell;
		m_Order[Subtrahend_Index].direction_In = LB1_Buy;

		m_Order[Minuend_Index].direction_Out = LB1_Buy;
		m_Order[Subtrahend_Index].direction_Out = LB1_Sell;
	};
	break;
	case TEnumSellMinuend_BuySubtrahend_Descrease://对应于LB1_SellC
	{
		TOrderOffsetType offset;
		if (GetTradeday(m_ptimeGlobalCurrentTime) == m_datePositionTradeDay)
			offset = LB1_DecreaseToday;
		else
			offset = LB1_Decrease;
		auto temp_ref = g_pStrategyContext->MakeOrder(g_StrategyId,static_cast<TOrderType>(MakeOrderType),
			LB1_Sell,
			offset,
			m_Order[Minuend_Index].vol,
			m_tickData[Minuend_Index].m_dbBidPrice[0] - MinuendBadSlipTickCount*MinuendMinPriceTick,
			Minuend_Index,
			static_cast<unsigned int>(offset)
			);//以最新价挂单下单
		if (LB1_NullOrderRef == temp_ref)
			return;
		m_Order[Minuend_Index].outRef = temp_ref;
		get<TrR_ClearTradedList>(m_lstTradedLog[Minuend_Index]).clear();
		get<TrR_ClearTradedList>(m_lstTradedLog[Subtrahend_Index]).clear();

		
		get<TrR_ClearTarPrice>(m_lstTradedLog[Minuend_Index]) = m_tickData[Minuend_Index].m_dbBidPrice[0];
		get<TrR_ClearTime>(m_lstTradedLog[Minuend_Index]) = m_ptimeGlobalCurrentTime;
		LOG("CustomArbitrageStrategy[%d]: TEnumSellMinuend_BuySubtrahend_Descrease", g_StrategyId);
		m_Order[Minuend_Index].outSysID = "";
		m_Order[Minuend_Index].orderTime = m_ptimeGlobalCurrentTime;

		m_Order[Minuend_Index].tradeState = T_OO_W;
		m_Order[Subtrahend_Index].tradeState = T_OO_INI;

		m_Order[Minuend_Index].direction_In = LB1_Buy;
		m_Order[Subtrahend_Index].direction_In = LB1_Sell;

		m_Order[Minuend_Index].direction_Out = LB1_Sell;
		m_Order[Subtrahend_Index].direction_Out = LB1_Buy;


	};
	break;
	default:break;
	}
}

bool CArbitrageBase::CanExcute()
{
	return false == m_booIsPedding;
}

bool CArbitrageBase::ParamAndProbeInit(CParNode * ParamStruct, TProbeStructType ProbeStruct)
{

	unsigned int m_uProbeBeginIndex = 0;
	unsigned int m_uParamBeginIndex = 0;
	if (NULL == ParamStruct)
		return false;
	m_uParamBeginIndex = 0;
	while (strlen(m_array2ParameterStruct[m_uParamBeginIndex].m_arrayParname) != 0)
		m_uParamBeginIndex++;

	for (auto i = m_uParamBeginIndex;i < MAXPARCOUNT;i++)
	{
		m_array2ParameterStruct[i] = (*(ParamStruct + i - m_uParamBeginIndex));
		if (strlen(m_array2ParameterStruct[i].m_arrayParname)==0)
			break;
	}

	if (ProbeStruct == NULL)
		return false;
	m_uProbeBeginIndex = 0;
	while (NULL != m_array2ProbeStruct[m_uProbeBeginIndex][0].m_AtomicDoublePointer)
		m_uProbeBeginIndex++;
	for (auto i = m_uProbeBeginIndex;i < MAX_GRAPH_COUNT;i++)
	{
		for (auto t = 0;t < MAX_SERIAL_PER_GRAPH;t++)
		{
			m_array2ProbeStruct[i][t] = (*(ProbeStruct + (i - m_uProbeBeginIndex)))[t];
			if (NULL == m_array2ProbeStruct[i][t].m_AtomicDoublePointer)
				break;
		}
		if (NULL == m_array2ProbeStruct[i][0].m_AtomicDoublePointer)
			break;
	};
	return true;
}

CPosition & CArbitrageBase::GetMinuendPosition()
{
	return m_Position[Minuend_Index];
}

CPosition & CArbitrageBase::GetSubtrahendPosition()
{
	return m_Position[Subtrahend_Index];
}

CFutureTick & CArbitrageBase::GetMinuendTick()
{
	return m_tickData[Minuend_Index];
}

CFutureTick & CArbitrageBase::GetSubtrahendTick()
{
	return m_tickData[Subtrahend_Index];
}

TPriceType  CArbitrageBase::GetMinuendMinPriceTick()
{
	return MinuendMinPriceTick;
}

TPriceType  CArbitrageBase::GetSubtrahendMinPriceTick()
{
	return SubtrahendMinPriceTick;
}

int  CArbitrageBase::GetMinuendMultipNumber()
{
	return m_intMinuendMultipNumber;
}

int  CArbitrageBase::GetSubtrahendMultipNumber()
{
	return m_intSubtrahendMultipNumber;
}

bool CArbitrageBase::IsTwoLegHasInited()
{
	if (false == m_boolHasInitialize[0] || false == m_boolHasInitialize[1])
		return false;
	else return true;
} 

void CArbitrageBase::ShowPosition()
{
	double Value = 0.0;
	if (SHAREDVALUE_GET(m_intSharedValueIndex, Value) == false)
		LOG("Can not find shared value index %d", m_intSharedValueIndex);
	else
		LOG("Strategy[%d]: ValidCopies[%d]=%lf,WriteCsv=%d", g_StrategyId,m_intSharedValueIndex, Value, WriteCsv);
	LOG("Strategy[%d]: PositionTime:%s", g_StrategyId, to_simple_string(m_datePositionTradeDay).c_str());
	LOG("Strategy[%d]: Minuend    [Long:%d(%.2lf)][Short:%d(%.2lf)]",
		g_StrategyId,
		GetMinuendPosition().m_uLongPosition,
		GetMinuendPosition().m_dbLongTurnover,
		GetMinuendPosition().m_uShortPosition,
		GetMinuendPosition().m_dbShortTurnover
		);
	LOG("Strategy[%d]: Subtrahend [Long:%d(%.2lf)][Short:%d(%.2lf)]",
		g_StrategyId,
		GetSubtrahendPosition().m_uLongPosition,
		GetSubtrahendPosition().m_dbLongTurnover,
		GetSubtrahendPosition().m_uShortPosition,
		GetSubtrahendPosition().m_dbShortTurnover
		);
}

void CArbitrageBase::SetPresupposedPosition(ptime _ptimeGlobalT,int _PresupposedPositionType, int _PresupposedPositionTradeDayType)
{
	if (PresupposedPosition_Default != _PresupposedPositionType)
	{//有预设仓位
		GetMinuendPosition().Init();
		GetSubtrahendPosition().Init();
		if (PresupposedPosition_MinuendLong_SubtrahendShort == _PresupposedPositionType)
		{
			GetMinuendPosition().m_uLongPosition = 1;
			GetSubtrahendPosition().m_uShortPosition = 1;
			if (PresupposedPositionTradeDay_Today == _PresupposedPositionTradeDayType)
			{
				if (_ptimeGlobalT.time_of_day() < time_duration(13, 0, 0, 0))
					m_datePositionTradeDay = _ptimeGlobalT.date();
				else
				{ 
					if(_ptimeGlobalT.date().day_of_week() == Friday)
						m_datePositionTradeDay = _ptimeGlobalT.date() + days(3);
					else
						m_datePositionTradeDay = _ptimeGlobalT.date() + days(1);
				}
			}
			else if (PresupposedPositionTradeDay_Yesterday == _PresupposedPositionTradeDayType)
			{
				if (_ptimeGlobalT.time_of_day() < time_duration(13, 0, 0, 0))
					m_datePositionTradeDay = _ptimeGlobalT.date() - days(1);
				else
					m_datePositionTradeDay = _ptimeGlobalT.date();
			}
		}
		else if (PresupposedPosition_MinuendShort_SubtrahendLong == _PresupposedPositionType)
		{
			GetMinuendPosition().m_uShortPosition = 1;
			GetSubtrahendPosition().m_uLongPosition = 1;
			if (PresupposedPositionTradeDay_Today == _PresupposedPositionTradeDayType)
			{
				if (_ptimeGlobalT.time_of_day() < time_duration(13, 0, 0, 0))
					m_datePositionTradeDay = _ptimeGlobalT.date();
				else
				{
					if (_ptimeGlobalT.date().day_of_week() == Friday)
						m_datePositionTradeDay = _ptimeGlobalT.date() + days(3);
					else
						m_datePositionTradeDay = _ptimeGlobalT.date() + days(1);
				}
			}
			else if (PresupposedPositionTradeDay_Yesterday == _PresupposedPositionTradeDayType)
			{
				if (_ptimeGlobalT.time_of_day() < time_duration(13, 0, 0, 0))
					m_datePositionTradeDay = _ptimeGlobalT.date() - days(1);
				else
					m_datePositionTradeDay = _ptimeGlobalT.date();
			}
		}
		ShowPosition();
	}
}

string CArbitrageBase::GetLogFileName()
{
	char buf[64];
	sprintf(buf, "%d-%s.csv", g_StrategyId, to_iso_string(m_ptimeGlobalCurrentTime.date()).c_str());
	return buf;
}

date CArbitrageBase::GetTradeday(ptime _Current)
{
	if (_Current.time_of_day() < time_duration(12, 0, 0, 0))//这个地方不要卡的太死
		return _Current.date();
	else
	{
		if(_Current.date().day_of_week().as_enum() == Friday)
			return _Current.date() + days(3);
		else
			return _Current.date() + days(1);
	}
}

void CArbitrageBase::OnTick(TMarketDataIdType dataid, const CTick * pDepthMarketData)
{
	if (0 != dataid
		&&
		1 != dataid)//双腿策略
		return;
	if (pDepthMarketData->m_dbAskPrice[0] <= 0 || pDepthMarketData->m_dbAskPrice[0] >= 10e9
		||
		pDepthMarketData->m_dbBidPrice[0] <= 0 || pDepthMarketData->m_dbBidPrice[0] >= 10e9
		||
		pDepthMarketData->m_dbLastPrice <= 0 || pDepthMarketData->m_dbLastPrice >= 10e9
		)
	{
		//LOG("Strategy[%d]: dataid(%d) error", g_StrategyId, dataid);
	}
	else
	{
		m_boolHasInitialize[dataid] = true;
		m_tickData[dataid] = *static_cast<const CFutureTick*>(pDepthMarketData);
	}
	if (false == m_boolHasInitialize[0] || false == m_boolHasInitialize[1])
		return;
	m_ptimeGlobalCurrentTime = max(m_ptimeGlobalCurrentTime, pDepthMarketData->m_datetimeUTCDateTime);

	#pragma region 撤单!!
	if (
		(m_Order[Subtrahend_Index].tradeState == T_OI_W)
		&&
		(m_ptimeGlobalCurrentTime - m_Order[Subtrahend_Index].orderTime > milliseconds(OrderWaitTime))
		&&
		(m_Order[Subtrahend_Index].inSysID.size() > 0)
		)
			CANCEL(m_Order[Subtrahend_Index].inRef,(char*)m_Order[Subtrahend_Index].inSysID.c_str(), 0);
	if (
		(m_Order[Subtrahend_Index].tradeState == T_OO_W)
		&&
		(m_ptimeGlobalCurrentTime - m_Order[Subtrahend_Index].orderTime > milliseconds(OrderWaitTime))
		&& 
		(m_Order[Subtrahend_Index].outSysID.size() > 0)
		)
			CANCEL(m_Order[Subtrahend_Index].outRef,(char*)m_Order[Subtrahend_Index].outSysID.c_str(), 0);
	if (
		(m_Order[Minuend_Index].tradeState == T_OI_W)
		&&
		(m_ptimeGlobalCurrentTime - m_Order[Minuend_Index].orderTime > milliseconds(OrderWaitTime))
		&&
		(m_Order[Minuend_Index].inSysID.size() > 0)
		)
			CANCEL(m_Order[Minuend_Index].inRef,(char*)m_Order[Minuend_Index].inSysID.c_str(), 1);
	if (
		(m_Order[Minuend_Index].tradeState == T_OO_W)
		&&
		(m_ptimeGlobalCurrentTime - m_Order[Minuend_Index].orderTime > milliseconds(OrderWaitTime))
			&&
		(m_Order[Minuend_Index].outSysID.size() > 0)
		)
			CANCEL(m_Order[Minuend_Index].outRef,(char*)m_Order[Minuend_Index].outSysID.c_str(), 1);
	#pragma endregion

	# pragma region 结算重置
	if (
		m_Order[Subtrahend_Index].tradeState == T_OI_F
		&&
		m_Order[Minuend_Index].tradeState == T_OI_F
		)
	{
		m_Order[Subtrahend_Index].tradeState = T_INI;//T_OO_INI
		m_Order[Minuend_Index].tradeState = T_INI;
		m_booIsPedding=false;
		m_datePositionTradeDay = GetTradeday(m_ptimeGlobalCurrentTime);
		ShowPosition();
		
	}

	if (
		(
			m_Order[Subtrahend_Index].tradeState == T_OO_INI
			&&
			m_Order[Minuend_Index].tradeState == T_OO_INI)
		||
			(m_Order[Subtrahend_Index].tradeState == T_OI_INI
			&&
			m_Order[Minuend_Index].tradeState == T_OI_INI
			)
		)
	{
		m_Order[Subtrahend_Index].tradeState = T_INI;
		m_Order[Minuend_Index].tradeState = T_INI;
		m_booIsPedding = false;
		m_Order[Subtrahend_Index].inSysID = "";
		m_Order[Subtrahend_Index].outSysID = "";
		m_Order[Minuend_Index].inSysID = "";
		m_Order[Minuend_Index].outSysID = "";
	}

	if (
		m_Order[Subtrahend_Index].tradeState == T_OO_F
		&&
		m_Order[Minuend_Index].tradeState == T_OO_F
		)
	{
		m_Order[Subtrahend_Index].tradeState = T_INI;
		m_Order[Minuend_Index].tradeState = T_INI;
		m_booIsPedding = false;

		if (SHAREDVALUE_INC(m_intSharedValueIndex, m_uLockedCopies, [](double) {return true;}) == false)
		{
			LOG("Can not increase shared value[%d]", m_intSharedValueIndex);
		}
		double GainAndLose = 0;
		if (
			abs(m_Position[Minuend_Index].m_dbLongTurnover) > 10e-8
			||
			abs(m_Position[Subtrahend_Index].m_dbShortTurnover) > 10e-8
			)
		{//如果Minuend持有多头，Subtrahend就持有空头->做多价差
			GainAndLose =
			m_Position[Subtrahend_Index].m_dbShortTurnover*GetSubtrahendMultipNumber()
				-
			m_Position[Minuend_Index].m_dbLongTurnover*GetMinuendMultipNumber();
			
		}
		else
		{
			GainAndLose =
				m_Position[Minuend_Index].m_dbShortTurnover*GetMinuendMultipNumber()
				-
				m_Position[Subtrahend_Index].m_dbLongTurnover*GetSubtrahendMultipNumber();
		}
		double MinuendFee =
			m_Position[Minuend_Index].m_dbAccuFee[0]
			+
			m_Position[Minuend_Index].m_dbAccuFee[1]
			+
			m_Position[Minuend_Index].m_dbAccuFee[2]
			+
			m_Position[Minuend_Index].m_dbAccuFee[3];

		double SubtrahendFee =
			m_Position[Subtrahend_Index].m_dbAccuFee[0]
			+
			m_Position[Subtrahend_Index].m_dbAccuFee[1]
			+
			m_Position[Subtrahend_Index].m_dbAccuFee[2]
			+
			m_Position[Subtrahend_Index].m_dbAccuFee[3];

		ptime MinuendBetTime = get<TrR_BetTime>(m_lstTradedLog[Minuend_Index]);
		ptime SubtrahendBetTime = get<TrR_BetTime>(m_lstTradedLog[Subtrahend_Index]);
		ptime MinuendClearTime = get<TrR_ClearTime>(m_lstTradedLog[Minuend_Index]);
		ptime SubtrahendClearTime = get<TrR_ClearTime>(m_lstTradedLog[Subtrahend_Index]);
#pragma region 入场价格
		TPriceType MinuendBetTarPrice = get<TrR_BetTarPrice>(m_lstTradedLog[Minuend_Index]);
		TPriceType SubtrahendBetTarPrice = get<TrR_BetTarPrice>(m_lstTradedLog[Subtrahend_Index]);


		TVolumeType MinuendBetTradedVolume= accumulate(
			get<TrR_BetTradedList>(m_lstTradedLog[Minuend_Index]).begin(),
			get<TrR_BetTradedList>(m_lstTradedLog[Minuend_Index]).end(),
			0,
			[](int a, list< pair<TPriceType, TVolumeType> >::value_type & b)->int
		{
			return a + b.second;
		});
		TPriceType MinuendBetTradedPrice = accumulate(
			get<TrR_BetTradedList>(m_lstTradedLog[Minuend_Index]).begin(),
			get<TrR_BetTradedList>(m_lstTradedLog[Minuend_Index]).end(),
			0.0,
			[](double a, list< pair<TPriceType, TVolumeType> >::value_type & b)->double
		{
			return a + b.first*static_cast<double>(b.second);
		})
			/ MinuendBetTradedVolume;




		TVolumeType SubtrahendBetTradedVolume= accumulate(
			get<TrR_BetTradedList>(m_lstTradedLog[Subtrahend_Index]).begin(),
			get<TrR_BetTradedList>(m_lstTradedLog[Subtrahend_Index]).end(),
			0,
			[](int a, list< pair<TPriceType, TVolumeType> >::value_type & b)->int
		{
			return a + b.second;
		});
		TPriceType SubtrahendBetTradedPrice = 
			accumulate(
				get<TrR_BetTradedList>(m_lstTradedLog[Subtrahend_Index]).begin(),
				get<TrR_BetTradedList>(m_lstTradedLog[Subtrahend_Index]).end(),
				0.0,
				[](double a, list< pair<TPriceType, TVolumeType> >::value_type & b)->double
		{
			return a + b.first*static_cast<double>(b.second);
		})
			/ SubtrahendBetTradedVolume;
#pragma endregion


#pragma region 出场价格
		TPriceType MinuendClearTarPrice = get<TrR_ClearTarPrice>(m_lstTradedLog[Minuend_Index]);
		TPriceType SubtrahendClearTarPrice = get<TrR_ClearTarPrice>(m_lstTradedLog[Subtrahend_Index]);


		TVolumeType MinuendClearTradedVolume= accumulate(
			get<TrR_ClearTradedList>(m_lstTradedLog[Minuend_Index]).begin(),
			get<TrR_ClearTradedList>(m_lstTradedLog[Minuend_Index]).end(),
			0,
			[](int a, list< pair<TPriceType, TVolumeType> >::value_type & b)->int
		{
			return a + b.second;
		});
		TPriceType MinuendClearTradedPrice =
			accumulate(
				get<TrR_ClearTradedList>(m_lstTradedLog[Minuend_Index]).begin(),
				get<TrR_ClearTradedList>(m_lstTradedLog[Minuend_Index]).end(),
				0.0,
				[](double a, list< pair<TPriceType, TVolumeType> >::value_type & b)->double
		{
			return a + b.first*static_cast<double>(b.second);
		})
			/ MinuendClearTradedVolume;

		TVolumeType SubtrahendClearTradedVolume= accumulate(
			get<TrR_ClearTradedList>(m_lstTradedLog[Subtrahend_Index]).begin(),
			get<TrR_ClearTradedList>(m_lstTradedLog[Subtrahend_Index]).end(),
			0,
			[](int a, list< pair<TPriceType, TVolumeType> >::value_type & b)->int
		{
			return a + b.second;
		});
		TPriceType SubtrahendClearTradedPrice = 
			accumulate(
				get<TrR_ClearTradedList>(m_lstTradedLog[Subtrahend_Index]).begin(),
				get<TrR_ClearTradedList>(m_lstTradedLog[Subtrahend_Index]).end(),
				0.0,
				[](double a, list< pair<TPriceType, TVolumeType> >::value_type & b)->double
		{
			return a + b.first*static_cast<double>(b.second);
		})
			/ SubtrahendClearTradedVolume;
#pragma endregion
		//表头:操作类型,净利润,损益,M手续费,S手续费,M入场滑点,S入场滑点,M出场滑点,S出场滑点,
		//M入场时间,M出场时间,M手数,S手数,M入场盘口,M入场均价,S入场盘口,S入场均价,M出场盘口,M出场均价,S出场盘口,S出场均价
		char buf[2048];
		sprintf(buf,
			"%s,%.4lf,%.4lf,%.4lf,%.4lf,%.4lf,%.4lf,%.4lf,%.4lf,%s,%s,%d,%d,%.4lf,%.4lf,%.4lf,%.4lf,%.4lf,%.4lf,%.4lf,%.4lf",
			get<TrR_Direction>(m_lstTradedLog[Minuend_Index])==LB1_Buy?"BetRise":"BetDrop",
			GainAndLose - MinuendFee- SubtrahendFee,
			GainAndLose,
			MinuendFee, 
			SubtrahendFee, 
			abs(MinuendBetTarPrice- MinuendBetTradedPrice),
			abs(SubtrahendBetTarPrice - SubtrahendBetTradedPrice),
			abs(MinuendClearTarPrice - MinuendClearTradedPrice),
			abs(SubtrahendClearTarPrice - SubtrahendClearTradedPrice),
			to_iso_string(MinuendBetTime).c_str(),
			to_iso_string(MinuendClearTime).c_str(),
			MinuendBetTradedVolume, SubtrahendBetTradedVolume,

			MinuendBetTarPrice, MinuendBetTradedPrice,
			SubtrahendBetTarPrice, SubtrahendBetTradedPrice,
			MinuendClearTarPrice, MinuendClearTradedPrice,
			SubtrahendClearTarPrice,SubtrahendClearTradedPrice);

		LOG(buf);
		MEDDLERESPONSE(buf);
		if (WriteCsv != 0)
		{
			ofstream log(GetLogFileName(), ios::app);
			if (log.is_open())
			{
				log << buf << endl;
				log.close();
			}
		}
		m_Position[Minuend_Index].Init();
		m_Position[Subtrahend_Index].Init();

		ShowPosition();
		
		INQUIRY(new CInquiryMarkNetCurvePoint());
	}
	#pragma endregion

}

void CArbitrageBase::OnTrade(
    TOrderRefIdType ref,
    TOrderSysIdType sys,
	TVolumeType volume, 
	TPriceType price, 
	TOrderDirectionType dir, 
	TOrderOffsetType offset) 
{
	if (ref == m_Order[Minuend_Index].outRef)
	{

		get<TrR_ClearTradedList>(m_lstTradedLog[Minuend_Index]).push_back(make_pair(price, volume));

		if (m_Order[Subtrahend_Index].tradeState == T_OO_INI)
		{
			m_Order[Subtrahend_Index].outRef = g_pStrategyContext->MakeOrder(g_StrategyId,static_cast<TOrderType>(MakeOrderType),
				m_Order[Subtrahend_Index].direction_Out,
				static_cast<TOrderOffsetType>(_OrderRef2StrategyCustomPart(ref)),//LB1_Decrease,
				m_Order[Subtrahend_Index].vol,
				(LB1_Buy == m_Order[Subtrahend_Index].direction_Out
				?
					m_tickData[Subtrahend_Index].m_dbAskPrice[0] + SubtrahendBadSlipTickCount*SubtrahendMinPriceTick
				: 
					m_tickData[Subtrahend_Index].m_dbBidPrice[0] - SubtrahendBadSlipTickCount*SubtrahendMinPriceTick
				),
				Subtrahend_Index,
				_OrderRef2StrategyCustomPart(ref));

			get<TrR_ClearTarPrice>(m_lstTradedLog[Subtrahend_Index]) = (LB1_Buy == m_Order[Subtrahend_Index].direction_Out
				?
				m_tickData[Subtrahend_Index].m_dbAskPrice[0]
				:
				m_tickData[Subtrahend_Index].m_dbBidPrice[0]
				);
			get<TrR_ClearTime>(m_lstTradedLog[Subtrahend_Index]) = m_ptimeGlobalCurrentTime;
			m_Order[Subtrahend_Index].outSysID = "";
			m_Order[Subtrahend_Index].orderTime = m_ptimeGlobalCurrentTime;
			m_Order[Subtrahend_Index].tradeState = T_OO_W;
		}

		m_Order[Minuend_Index].dealVol += volume;

		if (LB1_Buy == m_Order[Minuend_Index].direction_Out)
		{
			m_Position[Minuend_Index].m_dbShortTurnover -= price*volume;
			m_Position[Minuend_Index].m_uShortPosition -= volume;
		}
		else {
			m_Position[Minuend_Index].m_dbLongTurnover -= price*volume;
			m_Position[Minuend_Index].m_uLongPosition -= volume;
		}

		auto Offset = static_cast<TOrderOffsetType>(_OrderRef2StrategyCustomPart(ref));
		m_Position[Minuend_Index].m_dbAccuFee[Offset] +=
			FEE_TYPE_FIX == m_intFeeType[Minuend_Index] ?
			m_dbFeeRatio[Offset][Minuend_Index] * volume
			:
			m_dbFeeRatio[Offset][Minuend_Index] * price*volume*m_intMinuendMultipNumber;


		if (m_Order[Minuend_Index].dealVol >= m_Order[Minuend_Index].vol)											
			m_Order[Minuend_Index].tradeState = T_OO_F;
	}
	else if (ref == m_Order[Minuend_Index].inRef)
	{
		get<TrR_BetTradedList>(m_lstTradedLog[Minuend_Index]).push_back(make_pair(price, volume));


		if (m_Order[Subtrahend_Index].tradeState == T_OI_INI) {
			m_Order[Subtrahend_Index].inRef = g_pStrategyContext->MakeOrder(g_StrategyId,static_cast<TOrderType>(MakeOrderType),
				m_Order[Subtrahend_Index].direction_In,
				LB1_Increase,
				m_Order[Subtrahend_Index].vol,
				(LB1_Buy == m_Order[Subtrahend_Index].direction_In
				?
					m_tickData[Subtrahend_Index].m_dbAskPrice[0] + SubtrahendBadSlipTickCount*SubtrahendMinPriceTick
				:
					m_tickData[Subtrahend_Index].m_dbBidPrice[0] - SubtrahendBadSlipTickCount*SubtrahendMinPriceTick
				),
				Subtrahend_Index,0);
			get<TrR_BetTarPrice>(m_lstTradedLog[Subtrahend_Index]) = (LB1_Buy == m_Order[Subtrahend_Index].direction_In
				?
				m_tickData[Subtrahend_Index].m_dbAskPrice[0]
				:
				m_tickData[Subtrahend_Index].m_dbBidPrice[0]
				);
			get<TrR_BetTime>(m_lstTradedLog[Subtrahend_Index]) = m_ptimeGlobalCurrentTime;

			m_Order[Subtrahend_Index].inSysID = "";
			m_Order[Subtrahend_Index].orderTime = m_ptimeGlobalCurrentTime;
			m_Order[Subtrahend_Index].tradeState = T_OI_W;
		}
		m_Order[Minuend_Index].dealVol += volume;
		if (LB1_Buy == m_Order[Minuend_Index].direction_In)
		{
			m_Position[Minuend_Index].m_dbLongTurnover += price*volume;
			m_Position[Minuend_Index].m_uLongPosition += volume;
		}
		else 
		{
			m_Position[Minuend_Index].m_dbShortTurnover += price*volume;
			m_Position[Minuend_Index].m_uShortPosition += volume;
		}

		auto Offset = static_cast<TOrderOffsetType>(_OrderRef2StrategyCustomPart(ref));
		m_Position[Minuend_Index].m_dbAccuFee[Offset] +=
			FEE_TYPE_FIX == m_intFeeType[Minuend_Index] ?
			m_dbFeeRatio[Offset][Minuend_Index] * volume
			:
			m_dbFeeRatio[Offset][Minuend_Index] * price*volume*m_intMinuendMultipNumber;

		if (m_Order[Minuend_Index].dealVol >= m_Order[Minuend_Index].vol)					
			m_Order[Minuend_Index].tradeState = T_OI_F;
	}
	else if (ref ==	m_Order[Subtrahend_Index].outRef)
	{
		get<TrR_ClearTradedList>(m_lstTradedLog[Subtrahend_Index]).push_back(make_pair(price, volume));


		m_Order[Subtrahend_Index].dealVol += volume;
		if (LB1_Buy == m_Order[Subtrahend_Index].direction_Out)
		{
			m_Position[Subtrahend_Index].m_dbShortTurnover -= price*volume;
			m_Position[Subtrahend_Index].m_uShortPosition -= volume;
		}
		else
		{
			m_Position[Subtrahend_Index].m_dbLongTurnover -= price*volume;
			m_Position[Subtrahend_Index].m_uLongPosition -= volume;
		}
		auto Offset = static_cast<TOrderOffsetType>(_OrderRef2StrategyCustomPart(ref));
		m_Position[Subtrahend_Index].m_dbAccuFee[Offset] +=
			FEE_TYPE_FIX == m_intFeeType[Subtrahend_Index] ?
			m_dbFeeRatio[Offset][Subtrahend_Index] * volume
			:
			m_dbFeeRatio[Offset][Subtrahend_Index] * price * volume * m_intSubtrahendMultipNumber;

		if (m_Order[Subtrahend_Index].dealVol >= m_Order[Subtrahend_Index].vol)
			m_Order[Subtrahend_Index].tradeState = T_OO_F;
	}
	else if (ref ==	m_Order[Subtrahend_Index].inRef)
	{
		get<TrR_BetTradedList>(m_lstTradedLog[Subtrahend_Index]).push_back(make_pair(price, volume));


		m_Order[Subtrahend_Index].dealVol += volume;
		if (LB1_Buy == m_Order[Subtrahend_Index].direction_In)
		{
			m_Position[Subtrahend_Index].m_dbLongTurnover += price*volume;
			m_Position[Subtrahend_Index].m_uLongPosition += volume;
		}
		else 
		{
			m_Position[Subtrahend_Index].m_dbShortTurnover += price*volume;
			m_Position[Subtrahend_Index].m_uShortPosition += volume;
		}

		auto Offset = static_cast<TOrderOffsetType>(_OrderRef2StrategyCustomPart(ref));
		m_Position[Subtrahend_Index].m_dbAccuFee[Offset] +=
			FEE_TYPE_FIX == m_intFeeType[Subtrahend_Index] ?
			m_dbFeeRatio[Offset][Subtrahend_Index] * volume
			:
			m_dbFeeRatio[Offset][Subtrahend_Index] * price*volume*m_intSubtrahendMultipNumber;

		if (m_Order[Subtrahend_Index].dealVol >= m_Order[Subtrahend_Index].vol)
			m_Order[Subtrahend_Index].tradeState = T_OI_F;	
	}
	else
	{
		LOG("[!][OnTrade][!four]ThisRef=%d m_Order[Minuend_Index].outRef=%d m_Order[Minuend_Index].inRef=%d m_Order[Subtrahend_Index].outRef=%d m_Order[Subtrahend_Index].inRef=%d",
			ref,
			m_Order[Minuend_Index].outRef,
			m_Order[Minuend_Index].inRef,
			m_Order[Subtrahend_Index].outRef,
			m_Order[Subtrahend_Index].inRef
			);
		LOG("[!][OnTrade][!four]ThisRef=%d m_Order[Minuend_Index].dealVol=%d m_Order[Minuend_Index].dealVol=%d m_Order[Subtrahend_Index].dealVol=%d m_Order[Subtrahend_Index].dealVol=%d",
			ref,
			m_Order[Minuend_Index].dealVol,
			m_Order[Minuend_Index].dealVol,
			m_Order[Subtrahend_Index].dealVol,
			m_Order[Subtrahend_Index].dealVol
			);
	}
	
}

void CArbitrageBase::OnOrder(
        TOrderRefIdType ref,
        TOrderSysIdType sysId,
        TOrderDirectionType direction,
        TOrderStatusType Status,
        TPriceType LimitPrice,
		TTradedVolumeType VolumeTraded,
		TRemainVolumeType VolumeRemain)
{
    if (Status == LB1_StatusCanceled)
    {
		if (ref==m_Order[Minuend_Index].inRef)
        {
			if (VolumeTraded == 0 && m_Order[Minuend_Index].dealVol == 0)
			{
				m_Order[Minuend_Index].tradeState = T_OI_INI;
				m_Order[Subtrahend_Index].tradeState = T_OI_INI;
				if (SHAREDVALUE_INC(m_intSharedValueIndex, m_uLockedCopies, [](double) {return true;}) == false)
				{
					LOG("Can not increase shared value[%d]", m_intSharedValueIndex);
				}
			}
			else
			{
				m_Order[Minuend_Index].inRef = g_pStrategyContext->MakeOrder(g_StrategyId,static_cast<TOrderType>(MakeOrderType),
					m_Order[Minuend_Index].direction_In,
					LB1_Increase,
					VolumeRemain,
					LB1_Buy == m_Order[Minuend_Index].direction_In
					?
					m_tickData[Minuend_Index].m_dbAskPrice[0] + (MinuendBadSlipTickCount + 1) * MinuendMinPriceTick
					:
					m_tickData[Minuend_Index].m_dbBidPrice[0] - (MinuendBadSlipTickCount + 1) * MinuendMinPriceTick,
					Minuend_Index,0);
				m_Order[Minuend_Index].inSysID = "";
				m_Order[Minuend_Index].orderTime = m_ptimeGlobalCurrentTime;
			}
			
			
            return;
        }
        else if (ref==m_Order[Minuend_Index].outRef)
        {
			if (VolumeTraded == 0 && m_Order[Minuend_Index].dealVol == 0)
			{
				m_Order[Minuend_Index].tradeState = T_OO_INI;
				m_Order[Subtrahend_Index].tradeState = T_OO_INI;
			}
			else
			{
				m_Order[Minuend_Index].outRef = g_pStrategyContext->MakeOrder(g_StrategyId,static_cast<TOrderType>(MakeOrderType),//bug20150715
					m_Order[Minuend_Index].direction_Out,//bug20150715
					static_cast<TOrderOffsetType>(_OrderRef2StrategyCustomPart(ref)), //LB1_Decrease,
					VolumeRemain,
					LB1_Buy == m_Order[Minuend_Index].direction_Out
					?
					m_tickData[Minuend_Index].m_dbAskPrice[0] + (MinuendBadSlipTickCount + 1) * MinuendMinPriceTick
					:
					m_tickData[Minuend_Index].m_dbBidPrice[0] - (MinuendBadSlipTickCount + 1) * MinuendMinPriceTick,
					1,
					_OrderRef2StrategyCustomPart(ref));
				m_Order[Minuend_Index].outSysID = "";
				m_Order[Minuend_Index].orderTime = m_ptimeGlobalCurrentTime;
			}

			
            return;
        }
		else if (ref==m_Order[Subtrahend_Index].inRef)
        {
            m_Order[Subtrahend_Index].inRef = g_pStrategyContext->MakeOrder(g_StrategyId,static_cast<TOrderType>(MakeOrderType),
                m_Order[Subtrahend_Index].direction_In,
                LB1_Increase,
                VolumeRemain,
					LB1_Buy == m_Order[Subtrahend_Index].direction_In
				? 
					m_tickData[Subtrahend_Index].m_dbAskPrice[0] + (SubtrahendBadSlipTickCount+1) * SubtrahendMinPriceTick
				: 
					m_tickData[Subtrahend_Index].m_dbBidPrice[0] - (SubtrahendBadSlipTickCount+1) * SubtrahendMinPriceTick,
				Subtrahend_Index,0);
			m_Order[Subtrahend_Index].inSysID = "";
			m_Order[Subtrahend_Index].orderTime = m_ptimeGlobalCurrentTime;
            return;
        }
		else if (ref==m_Order[Subtrahend_Index].outRef)
        {
            m_Order[Subtrahend_Index].outRef = g_pStrategyContext->MakeOrder(g_StrategyId,static_cast<TOrderType>(MakeOrderType),
                m_Order[Subtrahend_Index].direction_Out,
				static_cast<TOrderOffsetType>(_OrderRef2StrategyCustomPart(ref)),//LB1_Decrease,
                VolumeRemain,
					LB1_Buy == m_Order[Subtrahend_Index].direction_Out
				?
					m_tickData[Subtrahend_Index].m_dbAskPrice[0] + (SubtrahendBadSlipTickCount+1) * SubtrahendMinPriceTick
				: 
					m_tickData[Subtrahend_Index].m_dbBidPrice[0] - (SubtrahendBadSlipTickCount+1) * SubtrahendMinPriceTick,
                0,
				_OrderRef2StrategyCustomPart(ref));
			m_Order[Subtrahend_Index].outSysID = "";
			m_Order[Subtrahend_Index].orderTime = m_ptimeGlobalCurrentTime;
            return;
        }
    }
	else
	{
		if (ref == m_Order[Subtrahend_Index].inRef)
			m_Order[Subtrahend_Index].inSysID = sysId;
		else if (ref == m_Order[Subtrahend_Index].outRef)
			m_Order[Subtrahend_Index].outSysID = sysId;
		else if (ref == m_Order[Minuend_Index].inRef)
			m_Order[Minuend_Index].inSysID = sysId;
		else if (ref == m_Order[Minuend_Index].outRef)
			m_Order[Minuend_Index].outSysID = sysId;
    }
}

void CArbitrageBase::OnEndup()
{
}

void CArbitrageBase::OnRelease()
{
	delete this;
}

CArbitrageBase::~CArbitrageBase() {}