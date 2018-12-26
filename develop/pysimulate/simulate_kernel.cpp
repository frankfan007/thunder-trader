#include "stdafx.h"
//#include "SimulateKernelInterface.h"
#include "MySimulateKernel.h"
#include "DlgConfig.h"
#include <unordered_map>
#include <fstream>
#include <atomic>
#include "OrderRefResolve.h"
#include <boost/filesystem/path.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem.hpp>
#include "Tick.h"
#include "FutureTick.h"
#include "OptionTick.h"
#include "TwsTick.h"
#include "StockTick.h"
#include "ForexTick.h"
#include <limits>
#include <numeric>
#pragma region ptree
#define BOOST_SPIRIT_THREADSAFE
#include <boost/property_tree/ptree.hpp>  
#include <boost/property_tree/json_parser.hpp> 
using namespace boost::property_tree;
#pragma endregion
using namespace std::tr1;
using namespace std;
#include "QuantFundHFTBackTestTypedefine.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#endif
void AbandonOverlap(vector<CTick*>&result);
class MSimulateKernelInterface;
__declspec(dllexport) MSimulateKernelInterface * MSimulateKernelInterface::CreateSimulateKernel(MSimulateKernelEnvironment *_env)
{

	MSimulateKernelInterface * ret;
	try {
		ret = new CMySimulateKernel(_env);
	}
	catch (bad_alloc&)
	{
		return NULL;
	}
	return ret;
}

//template <typename T>
//T Mid(T a, T b, T c)
//{
//#define SWAP(x,y) (temp=x,x=y,y=temp)
//	T temp;
//	if (a > b)
//		SWAP(a, b);
//	if (b > c)//9 8 7
//		SWAP(b, c);
//	if (a > b)//9 8 7
//		SWAP(a, b);
//	return b;
//}
//
//#define CURRENTTIME_UINT64 (m_vecSortedTick->GetTickSerial()[m_uCurrentTickIndex]->m_datetimeUTCDateTime)



void ThrowException(const char * fmt, ...)
{
	va_list args;
	char buf[1024];
	va_start(args, fmt);
	vsprintf_s(buf, fmt, args);
	va_end(args);
	throw std::exception(buf);
}
#define SIM_KER_THROW(fmt,...) ThrowException(fmt"[file:%s line:%d]",__VA_ARGS__,__FILE__,__LINE__);

class CDataSerial
{
public:
	ifstream m_File;
	CFutureTick m_tickFutureTick;
	CStockTick m_tickStockTick;
	COptionTick m_tickOptionTick;
	CTwsTick m_tickTwsTick;
	CForexTick m_tickForexTick;
	boost::archive::binary_iarchive * m_pInputArchive = nullptr;
	TStrategyTickType m_enumTickType;
	size_t m_intLength = 0;
	size_t m_intRemainTickCount = 0;
	TMarketDataIdType m_uDataid = 0;
	bool Init(string filename)
	{
		try {
			m_File.open(filename, ios::binary|ios::in);
			if (m_File.is_open())
			{
				m_pInputArchive = new boost::archive::binary_iarchive(m_File);
				*m_pInputArchive & m_enumTickType;
				*m_pInputArchive & m_intRemainTickCount;
				m_intLength = m_intRemainTickCount;
				SetNextTick();
			}
			else
				return false;
			return true;
		}
		catch (...)
		{
			return false;
		}
	}
	bool SetNextTick()
	{
		try {
			switch (m_enumTickType)
			{
			case FutureTick:
			{*m_pInputArchive & m_tickFutureTick;	m_tickFutureTick.m_uDataID = m_uDataid;}
			break;
			case OptionTick:
			{*m_pInputArchive & m_tickOptionTick;	m_tickOptionTick.m_uDataID = m_uDataid;}
			break;
			case ForexTick:
			{*m_pInputArchive & m_tickForexTick;	m_tickForexTick.m_uDataID = m_uDataid;}
			break;
			case StockTick:
			{*m_pInputArchive & m_tickStockTick;	m_tickStockTick.m_uDataID = m_uDataid;}
			break;
			case TwsTick:
			{*m_pInputArchive & m_tickTwsTick;		m_tickTwsTick.m_uDataID = m_uDataid;}
			break;
			}
		}
		catch (std::exception & err)
		{
			return false;
		}
		return true;
	}

	ptime & GetTime()
	{
		switch (m_enumTickType)
		{
		case TStrategyTickType::FutureTick:	return	m_tickFutureTick.m_datetimeUTCDateTime;
		case TStrategyTickType::OptionTick:	return	m_tickOptionTick.m_datetimeUTCDateTime;
		case TStrategyTickType::ForexTick:	return	m_tickForexTick.m_datetimeUTCDateTime;
		case TStrategyTickType::StockTick:	return	m_tickStockTick.m_datetimeUTCDateTime;
		case TStrategyTickType::TwsTick:	return	m_tickTwsTick.m_datetimeUTCDateTime;
		}
		return	m_tickFutureTick.m_datetimeUTCDateTime;
	}
	CTick * GetTick()
	{
		switch (m_enumTickType)
		{
		case TStrategyTickType::FutureTick:	return	dynamic_cast<CTick * >(&m_tickFutureTick);
		case TStrategyTickType::OptionTick:	return	dynamic_cast<CTick * >(&m_tickOptionTick);
		case TStrategyTickType::ForexTick:	return	dynamic_cast<CTick * >(&m_tickForexTick);
		case TStrategyTickType::StockTick:	return	dynamic_cast<CTick * >(&m_tickStockTick);
		case TStrategyTickType::TwsTick:	return	dynamic_cast<CTick * >(&m_tickTwsTick);
		}
		return	dynamic_cast<CTick * >(&m_tickFutureTick);
	}
	~CDataSerial()
	{
		if (m_pInputArchive)
			delete m_pInputArchive;
		if (m_File.is_open())
			m_File.close();
	}
};

auto MergeTickCompaire = [](CDataSerial * a, CDataSerial * b)->bool {
	if (a->GetTime() > b->GetTime())
		return true;
	else if (a->GetTime() < b->GetTime())
		return false;
	else
		return strcmp(a->GetTick()->m_strInstrumentID, b->GetTick()->m_strInstrumentID) > 0;
};

CMySimulateKernel::CMySimulateKernel(MSimulateKernelEnvironment * _env):m_environment(_env)
{
	m_enumTradePriceType = TradePrice_BestPrice;
}

void CMySimulateKernel::Release()
{
	delete this;
}

void CMySimulateKernel::StartBackTest(
	MStrategy * pStrategy,/*IN*/
	vector<string> tickFiles,
	string strInArchiveFile,/*IN*/
	string strOutArchiveFile,/*IN*/
	const ptree config,/*IN*/
	unsigned int flags,/*IN*/
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	CBackTestResult * out/*OUT*/
	)
{
	if (nullptr == pStrategy)
		SIM_KER_THROW("MStrategy pointer is nullptr.");
	if (nullptr == out)
		SIM_KER_THROW("out pointer is nullptr.");
	if(tickFiles.empty())
		SIM_KER_THROW("tickFiles is empty.");


	m_uFlags = flags;
	m_Out = out;

	m_vecProbeUpdateList.clear();
	m_listOrdersTobeHandle.clear();
	mapSysId2OrderRef.clear();
	m_vecOnTickTimeConsuming.clear();
	

	
	out->Init();
	out->m_vecNGraphProbeInfo.clear();
	out->m_vecKGraphProbeInfo.clear();
	out->m_vecTimeSerial.clear();
	out->m_vecMessages.clear();
	out->m_mapOrders.clear();
	out->m_mapCriterions.clear();
	out->m_vecInqueries.clear();
	out->m_uSerialLength = 0;
	time_duration SubmitDelay = milliseconds(200);
	time_duration CancelDelay = milliseconds(90);
	size_t OverAllTickCount = 0;
	ptime InitTime = max_date_time;
	LARGE_INTEGER Freq;
	LARGE_INTEGER WinBegin;
	LARGE_INTEGER WinEnd;

	unsigned int m_uCurrentTickIndex = 0;

#pragma region 读取配置
	auto SimulateKernelNode = config.find("SimulateKernel");
	if (config.not_found() != SimulateKernelNode)
	{
		auto TradePriceTypeNode = SimulateKernelNode->second.find("TradePriceType");
		if (TradePriceTypeNode != SimulateKernelNode->second.not_found())
		{
			string TradePriceTypeString = TradePriceTypeNode->second.data();
			if ("FixPrice" == TradePriceTypeString)
			{
				m_enumTradePriceType = TradePrice_FixPrice;
			}
			else if ("BestPrice" == TradePriceTypeString)
			{
				m_enumTradePriceType = TradePrice_BestPrice;
			}
		}
		else
			m_enumTradePriceType = TradePrice_BestPrice;

		auto AutoUpdateChartNode = SimulateKernelNode->second.find("AutoUpdateChart");
		if (AutoUpdateChartNode != SimulateKernelNode->second.not_found())
		{
			string AutoUpdateChartNodeString = AutoUpdateChartNode->second.data();
			if ("On" == AutoUpdateChartNodeString)
			{
				m_boolAutoUpdateChart = true;
			}
			else if ("Off" == AutoUpdateChartNodeString)
			{
				m_boolAutoUpdateChart = false;
			}
		}
		else m_boolAutoUpdateChart = true;

		auto SharedValueInitNode = SimulateKernelNode->second.find("SharedValueInit");
		if (SharedValueInitNode != SimulateKernelNode->second.not_found())
		{
			for (auto & node : SharedValueInitNode->second)
				m_Out->m_mapSharedValue[atoi(node.first.c_str())] = atof(node.second.data().c_str());
		}

		auto SubmitDelayMillisNode = SimulateKernelNode->second.find("SubmitDelayMillis");
		if (SubmitDelayMillisNode != SimulateKernelNode->second.not_found())
			SubmitDelay = milliseconds(atoi(SubmitDelayMillisNode->second.data().c_str()));

		auto CancelDelayMillisNode = SimulateKernelNode->second.find("CancelDelayMillis");
		if (CancelDelayMillisNode != SimulateKernelNode->second.not_found())
			CancelDelay = milliseconds(atoi(CancelDelayMillisNode->second.data().c_str()));
	}
#pragma endregion

	priority_queue<CDataSerial *, vector<CDataSerial *>, decltype(MergeTickCompaire) > que(MergeTickCompaire);
	TProbeStructType ProbeMatrix = pStrategy->GetProbeStruct();

	
	if (flags&Simulate_HasOnTickTimeConsuming)
		QueryPerformanceFrequency(&Freq); // 获取时钟周期

	
	for (size_t i = 0;i < tickFiles.size();i++)
	{
		auto temp = new CDataSerial();
		if(false == temp->Init(tickFiles[i]))
			SIM_KER_THROW("Could not open file %s", tickFiles[i].c_str());
		temp->m_uDataid = i;
		que.push(temp);
		--temp->m_intRemainTickCount;
		OverAllTickCount += temp->m_intLength;
		out->m_vecInstruments.push_back(temp->GetTick()->m_strInstrumentID);
		if (temp->GetTick()->m_datetimeUTCDateTime < InitTime)
			InitTime = temp->GetTick()->m_datetimeUTCDateTime;
	}
	if(OverAllTickCount == 0)
		SIM_KER_THROW("There is no tick date.");
	m_pCurrentTop = que.top();
	out->m_vecLastTick.resize(tickFiles.size());
	if ((flags&Simulate_HasProbes)&&nullptr != ProbeMatrix)
	{
		m_Out->m_vecTimeSerial.reserve(OverAllTickCount);
		for (auto _Graph = 0;;_Graph++)
		{
			if (nullptr == ProbeMatrix[_Graph][0].m_AtomicDoublePointer)
				break;
			else
			{
				vector<
					std::tuple<
					string/*name*/,
					TProbeColorType/*Color*/,
					std::shared_ptr<vector<unsigned int>>/*Index*/,
					std::shared_ptr<vector<float>>/*Value*/
					>
				> tempInfo;
				TProbeNodeType temp;
				for (auto _Serial = 0;;_Serial++)
				{
					if (nullptr == ProbeMatrix[_Graph][_Serial].m_AtomicDoublePointer)
					{
						if (strcmp("Candlesticks", ProbeMatrix[_Graph][_Serial].m_strProbeName) == 0)
						{
							m_Out->m_vecKGraphProbeInfo.push_back(tempInfo);
						}
						else
						{
							m_Out->m_vecNGraphProbeInfo.push_back(make_pair(ProbeMatrix[_Graph][_Serial].m_strProbeName, tempInfo));
						}
						break;
					}
					else
					{
						tempInfo.push_back(
							make_tuple(
								ProbeMatrix[_Graph][_Serial].m_strProbeName,
								ProbeMatrix[_Graph][_Serial].m_enumColor,
								std::shared_ptr<vector<unsigned int>>(new vector<unsigned int>(), [](vector<unsigned int> * p) {if (p)delete p;}),
								std::shared_ptr<vector<float>>(new vector<float>(), [](vector<float> * p) {if (p)delete p;})
								)
							);
						m_vecProbeUpdateList.push_back(
							make_tuple(
								get<2>(tempInfo[tempInfo.size() - 1]),
								get<3>(tempInfo[tempInfo.size() - 1]),
								ProbeMatrix[_Graph][_Serial].AtomicCounterOverallPointer,
								ProbeMatrix[_Graph][_Serial].m_AtomicDoublePointer
								)
							);
						//vecProbeUpdateList.push_back(make_pair(make_pair(y, x), make_pair(ProbeMatrix[y][x].AtomicCounterOverallPointer,ProbeMatrix[y][x].m_AtomicDoublePointer)));
					}
				}
			}
		}
	}
	if (
		(flags&Simulate_HasProbes)
		&&
		(flags&Simulate_HasCustomFloatProfit)
		)
	{
		m_Out->m_vecCustomFloatingProfit.reserve(OverAllTickCount);
		m_Out->m_vecCustomFloatingProfitTimeSerial.reserve(OverAllTickCount);
	}
	m_environment->OnUpdateProgress(0, OverAllTickCount);
	if(flags&Simulate_HasOnTickTimeConsuming)
		m_vecOnTickTimeConsuming.resize(OverAllTickCount);
	if (strInArchiveFile.empty())
	{
		if (TLastErrorIdType::LB1_NO_ERROR != pStrategy->OnInit(InitTime - microseconds(100)))
		{
			SIM_KER_THROW("OnInit Failed.");
		}
	}
	else
	{
		try {
			pStrategy->OnLoad(strInArchiveFile.c_str());
		}
		catch (std::exception & err)
		{
			AfxMessageBox(L"载入文档错误");
		}
		if (TLastErrorIdType::LB1_NO_ERROR != pStrategy->OnInit_FromArchive(InitTime - microseconds(100)))
			throw std::exception("OnInit_FromArchive Failed.");
	}

	while (que.empty() == false)
	{
		m_pCurrentTop = que.top();
		que.pop();
		{
			m_pLastTick_Global = m_pCurrentTop->GetTick();
			string temp = to_iso_string(m_pLastTick_Global->m_datetimeUTCDateTime) + " " + m_pLastTick_Global->m_strInstrumentID;

#pragma region 推送给策略
			if (flags&Simulate_HasOnTickTimeConsuming)
				QueryPerformanceCounter(&WinBegin);
			pStrategy->OnTick(
				static_cast<TMarketDataIdType>(m_pLastTick_Global->m_uDataID),
				m_pLastTick_Global
				);
			if (flags&Simulate_HasOnTickTimeConsuming)
			{
				QueryPerformanceCounter(&WinEnd);
				m_vecOnTickTimeConsuming[m_uCurrentTickIndex] = (WinEnd.QuadPart - WinBegin.QuadPart) * 1000000 / Freq.QuadPart;
			}
			if (
				(flags&Simulate_HasProbes)
				&&
				(flags&Simulate_HasCustomFloatProfit)
				)
			{
				double FloatingProfit=0;
				if (pStrategy->OnGetFloatingProfit(&FloatingProfit)&&abs(FloatingProfit)>0.0001)
				{
					m_Out->m_vecCustomFloatingProfit.push_back(FloatingProfit);
					m_Out->m_vecCustomFloatingProfitTimeSerial.push_back(m_pLastTick_Global->m_datetimeUTCDateTime);
				}
			}
#pragma endregion	

			for (auto ord : m_listOrdersTobeHandle)
			{//检查待处理列表里是否有报单被激活
				if (
					false == ord->m_boolActived
					&&
					m_pLastTick_Global->m_datetimeUTCDateTime >= ord->m_u64UTCDateTime + SubmitDelay
					)
					ord->m_boolActived = true;
			}

			if (m_listOrdersTobeHandle.size() != 0)
			{
				do
				{
					m_boolNewOrder = false;
					m_boolNewCancel = false;

					auto ord = m_listOrdersTobeHandle.begin();
					while (ord != m_listOrdersTobeHandle.end())
					{
						switch ((*ord)->m_enumOrderStatus)
						{
						case LB1_StatusUnknown:
						{
							(*ord)->m_enumOrderStatus = LB1_StatusNoTradeQueueing;
							ShowMessage(
								-1,
								"%s OnRtnOrder (Before) OrderID:%lld CancelID:%s Price:%10.4lf VolumeTraded:%10d VolumeRemaind:%10d Status:%d",
								to_iso_string(m_pLastTick_Global->m_datetimeUTCDateTime).c_str(),
								(*ord)->m_OrderID,
								(TOrderSysIdType)(*ord)->m_strOrderSysID.c_str(),
								(*ord)->m_LimitPrice,
								(*ord)->m_TradedVolume,
								(*ord)->m_Volume,
								(*ord)->m_enumOrderStatus);
							pStrategy->OnOrder(
								(*ord)->m_OrderID,
								(TOrderSysIdType)(*ord)->m_strOrderSysID.c_str(),
								(*ord)->m_enumDirection,
								TOrderStatusType::LB1_StatusNoTradeQueueing,
								0,
								(*ord)->m_TradedVolume,
								(*ord)->m_Volume
								);
							//++ord;
						}
						case LB1_StatusPartTradedQueueing:
						case LB1_StatusNoTradeQueueing:
						{
							if (
								((*ord)->m_boolActived)//如果报单已经被激活
								&&
								((*ord)->m_uDataId == m_pLastTick_Global->m_uDataID)//如果当前tick与该报单的DataID一致
								)
							{
								CTick * DeterminantTick_Channel = m_pLastTick_Global;
								bool CanDeal = false;
								switch (m_enumTradePriceType)
								{
								case TradePrice_FixPrice:
									CanDeal = (*ord)->m_enumDirection == LB1_Buy
										?
										(
											(*ord)->m_LimitPrice >= DeterminantTick_Channel->m_dbAskPrice[0]
											||
											(*ord)->m_LimitPrice > DeterminantTick_Channel->m_dbLastPrice
											)
										:
										(
											(*ord)->m_LimitPrice <= DeterminantTick_Channel->m_dbBidPrice[0]
											||
											(*ord)->m_LimitPrice < DeterminantTick_Channel->m_dbLastPrice
											);
									break;
								case TradePrice_BestPrice:
									CanDeal = (*ord)->m_enumDirection == LB1_Buy
										?
										((*ord)->m_LimitPrice >= DeterminantTick_Channel->m_dbAskPrice[0])
										:
										((*ord)->m_LimitPrice <= DeterminantTick_Channel->m_dbBidPrice[0]);
									break;
								}

								if (CanDeal)
								{
									TPriceType		TrPriThisTime = 0.0;
									TVolumeType		TrVolThisTime = 0;

									switch (m_enumTradePriceType)
									{
									case TradePrice_FixPrice:
									{
										TrPriThisTime = (*ord)->m_LimitPrice;//成交价格就是下单的价格
										if (LB1_Buy == (*ord)->m_enumDirection)
											TrVolThisTime = min(
												(*ord)->m_Volume - (*ord)->m_TradedVolume,
												DeterminantTick_Channel->m_intAskVolume[0]
												);//成交量按照第一档成交
										else
											TrVolThisTime = min(
												(*ord)->m_Volume - (*ord)->m_TradedVolume,
												DeterminantTick_Channel->m_intBidVolume[0]
												);//成交量按照第一档成交
									}
									break;
									case TradePrice_BestPrice:
									{
										TVolumeType tarVolume = ((*ord)->m_Volume - (*ord)->m_TradedVolume);
										TVolumeType accVolume = 0;
										double tarSum = 0;
										if (LB1_Buy == (*ord)->m_enumDirection)
										{
											for (unsigned int i = 0;i < MAX_QUOTATIONS_DEPTH;i++)
											{
												if ((*ord)->m_LimitPrice < DeterminantTick_Channel->m_dbAskPrice[i])
													break;
												if (tarVolume == accVolume)
													break;
												TVolumeType thisLevelVolume = min(tarVolume - accVolume, DeterminantTick_Channel->m_intAskVolume[i]);
												tarSum += thisLevelVolume*DeterminantTick_Channel->m_dbAskPrice[i];
												accVolume += thisLevelVolume;


											}
										}
										else if (LB1_Sell == (*ord)->m_enumDirection)
										{
											for (unsigned int i = 0;i < MAX_QUOTATIONS_DEPTH;i++)
											{
												if ((*ord)->m_LimitPrice > DeterminantTick_Channel->m_dbBidPrice[i])
													break;
												if (tarVolume == accVolume)
													break;
												TVolumeType thisLevelVolume = min(tarVolume - accVolume, DeterminantTick_Channel->m_intBidVolume[i]);
												tarSum += thisLevelVolume*DeterminantTick_Channel->m_dbBidPrice[i];
												accVolume += thisLevelVolume;

											}
										}

										TrPriThisTime = tarSum / accVolume;
										TrVolThisTime = accVolume;
									}
									break;
									}

									if (0 != TrVolThisTime)
									{
										(*ord)->m_TradeLimitPrice = TrPriThisTime;
										(*ord)->m_TradedVolume += TrVolThisTime;
										(*ord)->m_u64TradeUTCDateTime = m_pLastTick_Global->m_datetimeUTCDateTime;
										if ((*ord)->m_TradedVolume == (*ord)->m_Volume)
											(*ord)->m_enumOrderStatus = LB1_StatusAllTraded;
										else
											(*ord)->m_enumOrderStatus = LB1_StatusPartTradedQueueing;

										ShowMessage(
											-1,
											"%s OnRtnTrade (Before) OrderID:%lld CancelID:%s Price:%10.4lf VolumeTraded:%10d",
											to_iso_string((*ord)->m_u64TradeUTCDateTime).c_str(),
											(*ord)->m_OrderID,
											(TOrderSysIdType)(*ord)->m_strOrderSysID.c_str(),
											(*ord)->m_TradeLimitPrice,
											TrVolThisTime);//(*ord)->m_TradedVolume
										pStrategy->OnTrade(
											(*ord)->m_OrderID,
											(TOrderSysIdType)(*ord)->m_strOrderSysID.c_str(),
											TrVolThisTime,//(*ord)->m_TradedVolume,
											(*ord)->m_TradeLimitPrice,
											(*ord)->m_enumDirection,
											(*ord)->m_enumOffset
											);
										ShowMessage(
											-1,
											"%s OnRtnOrder (Before) OrderID:%lld CancelID:%s Price:%10.4lf VolumeTraded:%10d VolumeRemaind:%10d Status:%d ",
											to_iso_string(m_pLastTick_Global->m_datetimeUTCDateTime).c_str(),
											(*ord)->m_OrderID,
											(TOrderSysIdType)(*ord)->m_strOrderSysID.c_str(),
											(*ord)->m_LimitPrice,
											(*ord)->m_TradedVolume,
											(*ord)->m_Volume - (*ord)->m_TradedVolume,
											(*ord)->m_enumOrderStatus);
										pStrategy->OnOrder(
											(*ord)->m_OrderID,
											(TOrderSysIdType)(*ord)->m_strOrderSysID.c_str(),
											(*ord)->m_enumDirection,
											(*ord)->m_enumOrderStatus,
											(*ord)->m_TradeLimitPrice,
											(*ord)->m_TradedVolume,
											(*ord)->m_Volume - (*ord)->m_TradedVolume
											);
										if (LB1_StatusAllTraded == (*ord)->m_enumOrderStatus)
											m_listOrdersTobeHandle.erase(ord++);
										else
											++ord;
									}
									else
										++ord;
								}
								else
									++ord;
							}
							else
								++ord;
						}
						break;
						case LB1_StatusCanceled:
						{
							if (m_pLastTick_Global->m_datetimeUTCDateTime >= (*ord)->m_u64TryCancelUTCDateTime + CancelDelay)
							{
								(*ord)->m_u64CanceledUTCDateTime = m_pLastTick_Global->m_datetimeUTCDateTime;
								ShowMessage(
									-1,
									"%s OnRtnOrder (Before) OrderID:%lld CancelID:%s Price:%10.4lf VolumeTraded:%10d VolumeRemaind:%10d Status:%d ",
									to_iso_string(m_pLastTick_Global->m_datetimeUTCDateTime).c_str(),
									(*ord)->m_OrderID,
									(TOrderSysIdType)(*ord)->m_strOrderSysID.c_str(),
									(*ord)->m_LimitPrice,
									(*ord)->m_TradedVolume,
									(*ord)->m_Volume - (*ord)->m_TradedVolume,
									LB1_StatusCanceled);
								pStrategy->OnOrder(
									(*ord)->m_OrderID,
									(TOrderSysIdType)(*ord)->m_strOrderSysID.c_str(),
									(*ord)->m_enumDirection,
									LB1_StatusCanceled,
									0,
									(*ord)->m_TradedVolume,
									(*ord)->m_Volume - (*ord)->m_TradedVolume);
								m_listOrdersTobeHandle.erase(ord++);
							}
							else
								++ord;
						}
						break;
						}
					}

				} while (m_boolNewOrder || m_boolNewCancel);
			}

			m_uCurrentTickIndex++;

			if (m_boolAutoUpdateChart)
				UpdateChart();

			
			if (m_uCurrentTickIndex % 1000 == 0)
				m_environment->OnUpdateProgress(m_uCurrentTickIndex, OverAllTickCount);
		}

		if (m_pCurrentTop->m_intRemainTickCount > 0)
		{
			m_pCurrentTop->SetNextTick();
			que.push(m_pCurrentTop);
			if (1 == m_pCurrentTop->m_intRemainTickCount)
				out->m_vecLastTick[m_pCurrentTop->m_uDataid] = *m_pCurrentTop->GetTick();
			--m_pCurrentTop->m_intRemainTickCount;
		}else
			delete m_pCurrentTop;
	}
	pStrategy->OnEndup();
	if (false == strOutArchiveFile.empty())
	{
		try {
			pStrategy->OnSave(strOutArchiveFile.c_str());
		}
		catch (std::exception & err)
		{
			AfxMessageBox(L"保存错误");
		}
	}
	char buf[128];
	if (flags&Simulate_HasOnTickTimeConsuming)
	{
		double SumConsume = 0;
		for (auto i : m_vecOnTickTimeConsuming)
			SumConsume += i;
		double AverageConsume = SumConsume / m_vecOnTickTimeConsuming.size();
		
		sprintf_s(buf, "%lf", AverageConsume);
		m_Out->m_mapCriterions["OnTick函数耗时平均值(微秒)"] = buf;
	}

	double PositiveSlip = 0;
	unsigned int CancelCount = 0;
	for (auto & ord : m_Out->m_mapOrders)
	{
		if (ord.second.m_TradedVolume > 0)
			PositiveSlip += abs(ord.second.m_LimitPrice - ord.second.m_TradeLimitPrice);
		else
			CancelCount++;
	}
	sprintf_s(buf, "%lf", PositiveSlip);
	m_Out->m_mapCriterions["有利滑点"] = buf;
	sprintf_s(buf, "%d", CancelCount);
	m_Out->m_mapCriterions["撤单次数"] = buf;
	m_environment->OnUpdateProgress(m_Out->m_uSerialLength, m_Out->m_uSerialLength);
	m_environment->OnBackTestFinished();

}

#pragma region StrategyContext
bool CMySimulateKernel::Inquery(TStrategyIdType stid, MStrategyInquiryDataInterface * inquery)
{
	m_Out->m_vecInqueries.push_back(make_pair(m_pCurrentTop->GetTick()->m_datetimeUTCDateTime,inquery));
	return true;
}

bool CMySimulateKernel::MeddleResponse(TStrategyIdType, const char *, ...)
{
	return true;
}

bool CMySimulateKernel::ShowMessage(TStrategyIdType, const char * fmt, ...)
{
	if (Simulate_HasMessage&m_uFlags)
	{
		va_list args;
		char buf[1024];
		va_start(args, fmt);
		vsprintf_s(buf, fmt, args);
		va_end(args);
		wstring mes = CA2W(buf);
		m_Out->m_vecMessages.push_back(
			make_pair(
				m_pCurrentTop->GetTick()->m_datetimeUTCDateTime,
				mes
				)
			);
	}
	return true;

}

bool CMySimulateKernel::GetNextMeddle(TStrategyIdType, char * retbuffer, unsigned int maxlength)
{
	return false;
}

TOrderRefIdType CMySimulateKernel::MakeOrder(
	TStrategyIdType strategyid,
	TOrderType,
	TOrderDirectionType direction,
	TOrderOffsetType offset,
	TVolumeType volume,
	TPriceType LimitPrice,
	TMarketDataIdType DataId,
	TCustomRefPartType custom)
{
	char sysid[30];
	m_boolNewOrder = true;
	COrder ord;
	ord.m_boolActived = false;
	ord.m_u64TradeUTCDateTime = not_a_date_time;//成交时间
	ord.m_enumOrderStatus = LB1_StatusUnknown;
	ord.m_u64TryCancelUTCDateTime = not_a_date_time;
	ord.m_u64CanceledUTCDateTime = not_a_date_time;
	ord.m_TradeLimitPrice = -1;//实际成交
	ord.m_TradedVolume = 0;//实际成交手数
	ord.m_enumOrderType = LB1_NormalLimitOrderType;
	ord.m_enumDirection = direction;
	ord.m_enumOffset = offset;
	ord.m_Volume = volume;
	ord.m_LimitPrice = LimitPrice;
	ord.m_uDataId = DataId;
	ord.m_u64UTCDateTime =m_pCurrentTop->GetTick()->m_datetimeUTCDateTime;
	ord.m_OrderID = (m_Out->m_mapOrders.size()<< _StrategyCustomBitCount)+ (custom & _MaskFor4Bit);

	sprintf_s(sysid, "%lld", ord.m_OrderID);
	ord.m_strOrderSysID = sysid;
	mapSysId2OrderRef[sysid] = ord.m_OrderID;
	m_Out->m_mapOrders[ord.m_OrderID] = ord;
	m_listOrdersTobeHandle.push_back(&m_Out->m_mapOrders[ord.m_OrderID]);

	ShowMessage(
		-1,
		"%s LimitOrder (Before) OrderID:%lld DataID:%1d Price:%10.4lf Direction:%s Offset:%s Volume:%10d",
		to_iso_string(ord.m_u64UTCDateTime).c_str(),
		ord.m_OrderID,
		DataId,
		LimitPrice,
		direction == LB1_Buy ? "Buy" : "Sell",
		offset == LB1_Increase ? "+" : "-",
		volume);



	return ord.m_OrderID;
}

TLastErrorIdType CMySimulateKernel::CancelOrder(
	TStrategyIdType,
	TOrderRefIdType,
	TOrderSysIdType sysid,
	TMarketDataIdType dataid)
{
	m_boolNewCancel = true;
	if (mapSysId2OrderRef.find(sysid) == mapSysId2OrderRef.end())
		return LB1_INVALID_VAL;//如果策略撤销一个不存在的单子，则返回无效

	if (
		find_if(
			m_listOrdersTobeHandle.begin(),
			m_listOrdersTobeHandle.end(),
			[sysid](const COrder* ord)
	{
		return ord->m_strOrderSysID == string(sysid);
	}) == m_listOrdersTobeHandle.end()
		)//如果策略撤销一个已经稳定的单子，则忽略
		return LB1_NO_ERROR;

	if (LB1_StatusCanceled == m_Out->m_mapOrders[mapSysId2OrderRef[sysid]].m_enumOrderStatus)
		return LB1_NO_ERROR;//如果策略撤销一个已经被撤消过一次的单子，则忽略

	m_Out->m_mapOrders[mapSysId2OrderRef[sysid]].m_u64TryCancelUTCDateTime = m_pCurrentTop->GetTick()->m_datetimeUTCDateTime;
	m_Out->m_mapOrders[mapSysId2OrderRef[sysid]].m_enumOrderStatus = LB1_StatusCanceled;
	ShowMessage(
		-1,
		"%s CancOrder (Before) CancelID:%s",
		to_iso_string(m_Out->m_mapOrders[mapSysId2OrderRef[sysid]].m_u64TryCancelUTCDateTime).c_str(),
		sysid);
	return LB1_NO_ERROR;
}

void CMySimulateKernel::UpdateChart()
{
#pragma region 取探针数据
	if (Simulate_HasProbes&m_uFlags)
	{
		m_Out->m_uSerialLength++;
		for (auto & data : m_vecProbeUpdateList)
		{
			get<Index_ResBuff>(data)->push_back(get<IndexAddr>(data) == nullptr ? 0 : get<IndexAddr>(data)->load());
			get<Value_ResBuff>(data)->push_back(get<ValueAddr>(data)->load());
		}
		m_Out->m_vecTimeSerial.push_back(m_pLastTick_Global->m_datetimeUTCDateTime);
	}
#pragma endregion
}

bool CMySimulateKernel::GetSharedValue(TSharedIndexType i, double & ret)
{
	if (m_Out->m_mapSharedValue.find(i) != m_Out->m_mapSharedValue.end())
	{
		ret = m_Out->m_mapSharedValue[i];
		return true;
	}
	else
		return false;
}

bool CMySimulateKernel::IncreaseSharedValue(TSharedIndexType i, double dt, function<bool(double)> issatisfy)
{
	if (m_Out->m_mapSharedValue.find(i) == m_Out->m_mapSharedValue.end())
		return false;
	if (issatisfy(m_Out->m_mapSharedValue[i]))
	{
		m_Out->m_mapSharedValue[i] += dt;
		ShowMessage(-1,"SharedValue[%d]=%lf", i, m_Out->m_mapSharedValue[i]);
		return true;
	}
	else
		return false;
}

bool CMySimulateKernel::DecreaseSharedValue(TSharedIndexType i, double dt, function<bool(double)> issatisfy)
{
	if (m_Out->m_mapSharedValue.find(i) == m_Out->m_mapSharedValue.end())
		return false;
	if (issatisfy(m_Out->m_mapSharedValue[i]))
	{
		m_Out->m_mapSharedValue[i] -= dt;
		ShowMessage(-1, "SharedValue[%d]=%lf", i, m_Out->m_mapSharedValue[i]);
		return true;
	}
	else
		return false;
}

bool CMySimulateKernel::SetSharedValue(TSharedIndexType i, double newvalue, function<bool(double)> issatisfy)
{
	if (issatisfy(m_Out->m_mapSharedValue[i]))
	{
		m_Out->m_mapSharedValue[i] = newvalue;
		ShowMessage(-1, "SharedValue[%d]=%lf", i, m_Out->m_mapSharedValue[i]);
		return true;
	}
	else
		return false;
}

int CMySimulateKernel::GetRemainCancelAmount(TStrategyIdType, TMarketDataIdType)
{
	return (std::numeric_limits<int>::max)();
}
#pragma  endregion
