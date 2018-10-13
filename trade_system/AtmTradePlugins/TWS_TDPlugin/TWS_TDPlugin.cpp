#include "TWS_TDPlugin.h"
#include <stdarg.h>
#include <thread>
#include "OrderRefResolve.h"
#include "AutoPend.h"
#include <numeric>
#include <limits>
extern char ProcessName[256];
#define NOTIFY_LOGIN_SUCCEED {m_boolIsOnline = true; std::unique_lock<std::mutex> lk(m_mtxLoginSignal);m_cvLoginSignalCV.notify_all();}
#define NOTIFY_LOGIN_FAILED  {m_boolIsOnline = false;std::unique_lock<std::mutex> lk(m_mtxLoginSignal);m_cvLoginSignalCV.notify_all();}
template<int N>
struct PowerOfTen {
	static const long long val = 10 * PowerOfTen<N - 1>::val;
};
template<>
struct PowerOfTen<0> {
	static const long long val = 1;
};

const TOrderRefIdType MaxOrderRef = std::numeric_limits<TTwsOrderIdType>::max(); //(PowerOfTwo<8 * sizeof(CTwsInputOrderField::m_longOrderId) - 2>::val - 1);

const string CTWS_TDPlugin::s_strAccountKeyword = "serveraddress;port;clientid;maxcancelperday;";

date CTWS_TDPlugin::GetTradeday(ptime _Current)
{
	if (_Current.time_of_day() < time_duration(12, 0, 0, 0))//这个地方不要卡的太死
		return _Current.date();
	else
	{
		if (_Current.date().day_of_week().as_enum() == Friday)
			return _Current.date() + days(3);
		else
			return _Current.date() + days(1);
	}
}

CTWS_TDPlugin::CTWS_TDPlugin() :
	m_StartAndStopCtrlTimer(m_IOservice), m_abIsPending(false)
{

}

CTWS_TDPlugin::~CTWS_TDPlugin()
{

}

bool CTWS_TDPlugin::IsPedding()
{
	return m_abIsPending.load();
}

bool CTWS_TDPlugin::IsOnline()
{
	return m_boolIsOnline;
}

void CTWS_TDPlugin::IncreaseRefCount()
{
	m_intRefCount++;
}

void CTWS_TDPlugin::DescreaseRefCount()
{
	m_intRefCount--;
}

int CTWS_TDPlugin::GetRefCount()
{
	return m_intRefCount;
}

void CTWS_TDPlugin::CheckSymbolValidity(const unordered_map<string, string> & insConfig)
{
	auto Type = insConfig.find("type");
	if (Type == insConfig.end())
		throw std::runtime_error("Can not find the <type> of the the symbol.");
	if (Type->second != "tws")
		throw std::runtime_error("tws MarketDataSource does not support this symbol.");

	auto symbolNode = insConfig.find("symbol");
	if (symbolNode == insConfig.end())
		throw std::runtime_error("<symbol> not found.");

	auto sectypeNode = insConfig.find("sectype");
	if (sectypeNode == insConfig.end())
		throw std::runtime_error("<sectype> not found.");

	auto expiryNode = insConfig.find("expiry");
	if (expiryNode == insConfig.end())
		throw std::runtime_error("<expiry> not found.");

	auto strikeNode = insConfig.find("strike");
	if (strikeNode == insConfig.end())
		throw std::runtime_error("<strike> not found.");

	auto rightNode = insConfig.find("right");
	if (rightNode == insConfig.end())
		throw std::runtime_error("<right> not found.");

	auto multiplierNode = insConfig.find("multiplier");
	if (multiplierNode == insConfig.end())
		throw std::runtime_error("<multiplier> not found.");

	auto exchangeNode = insConfig.find("exchange");
	if (exchangeNode == insConfig.end())
		throw std::runtime_error("<exchange> not found.");

	auto primaryexchangeNode = insConfig.find("primaryexchange");
	if (primaryexchangeNode == insConfig.end())
		throw std::runtime_error("<primaryexchange> not found.");

	auto currencyNode = insConfig.find("currency");
	if (currencyNode == insConfig.end())
		throw std::runtime_error("<currency> not found.");

	auto localsymbolNode = insConfig.find("localsymbol");
	if (localsymbolNode == insConfig.end())
		throw std::runtime_error("<localsymbol> not found.");

	auto tradingclassNode = insConfig.find("tradingclass");
	if (tradingclassNode == insConfig.end())
		throw std::runtime_error("<tradingclass> not found.");
}

string CTWS_TDPlugin::GetCurrentKeyword()
{
	string tempAddress = m_strServerAddress;
	for (unsigned int i = 0;i < tempAddress.size();i++)
		if (tempAddress[i] == '.')
			tempAddress[i] = '_';
	char buf[128];
	snprintf(buf, sizeof(buf), "tws_td&%s&%u&%u", tempAddress.c_str(), m_uPort, m_uClientID);
	return string(buf);
}

string CTWS_TDPlugin::GetProspectiveKeyword(const ptree & in)
{
	string retKey = "tws_td&";
	auto temp = in.find("serveraddress");
	if (temp != in.not_found())
	{
		if (temp->second.data().size()>64)
			throw std::runtime_error("tws:serveraddress is too long(Max:64 char)");
		string tempAddress = temp->second.data();
		for (unsigned int i = 0;i < tempAddress.size();i++)
			if (tempAddress[i] == '.')
				tempAddress[i] = '_';
		retKey += tempAddress;
	}
	else
		throw std::runtime_error("tws:can not find <serveraddress>");
	retKey += "&";

	temp = in.find("port");
	if (temp != in.not_found())
	{
		retKey += temp->second.data();
	}
	else
		throw std::runtime_error("tws:can not find <port>");
	retKey += "&";

	temp = in.find("clientid");
	if (temp != in.not_found())
	{
		retKey += temp->second.data();
	}
	else
		throw std::runtime_error("tws:can not find <clientid>");

	return retKey;
}

void CTWS_TDPlugin::GetState(ptree & out)
{
	if (m_boolIsOnline)
		out.put("online", "true");
	else
		out.put("online", "false");
	out.put("serveraddress", m_strServerAddress);
	out.put("port", m_uPort);
	out.put("clientid", m_uClientID);
}

void CTWS_TDPlugin::UpdateAccountInfo(const ptree & in)
{
	auto temp = in.find("serveraddress");
	if (temp != in.not_found())
	{

		m_strServerAddress = temp->second.data();
		if (m_strServerAddress.size()>64)
			throw std::runtime_error("tws:serveraddress is too long(Max:64 char)");
		else if (m_strServerAddress.empty())
			throw std::runtime_error("tws:serveraddress is empty");
	}
	else
		throw std::runtime_error("tws:Can not find <serveraddress>");


	temp = in.find("port");
	if (temp != in.not_found())
	{
		m_uPort = atoi(temp->second.data().c_str());
		if (0 == m_uPort)
			throw std::runtime_error("tws:port is invalid");
	}
	else
		throw std::runtime_error("tws:can not find <port>");

	temp = in.find("clientid");
	if (temp != in.not_found())
	{
		m_uClientID = atoi(temp->second.data().c_str());
	}
	else
		throw std::runtime_error("tws:Can not find <clientid>");
}

void CTWS_TDPlugin::TDInit(const ptree & in, MTradePluginContextInterface * pTradePluginContext, unsigned int AccountNumber)
{
	m_uAccountNumber = AccountNumber;
	m_pTradePluginContext = pTradePluginContext;

	UpdateAccountInfo(in);
	auto MaxCancelPerDayNode = in.find("maxcancelperday");
	if (MaxCancelPerDayNode != in.not_found())
		m_intInitAmountOfCancelChancesPerDay = atoi(MaxCancelPerDayNode->second.data().c_str());
	else
		m_intInitAmountOfCancelChancesPerDay = 400;

	m_dateTradeDay = GetTradeday(second_clock::universal_time());
	map<string, int>().swap(m_mapCancelAmount);
	m_StartAndStopCtrlTimer.expires_from_now(boost::posix_time::seconds(3));
	m_StartAndStopCtrlTimer.async_wait(boost::bind(&CTWS_TDPlugin::TimerHandler, this,
		&m_StartAndStopCtrlTimer,
		boost::asio::placeholders::error));
	m_futTimerThreadFuture = std::async(std::launch::async,[this] {
		this->m_IOservice.run();
		return true;
	});


}

void CTWS_TDPlugin::TDHotUpdate(const ptree & in)
{
	UpdateAccountInfo(in);
	if (m_boolIsOnline)
	{
		Stop();
		Start();
	}
}

void CTWS_TDPlugin::TimerHandler(boost::asio::deadline_timer* timer, const boost::system::error_code& err)
{
	if (err)
	{
		ShowMessage(normal, "%s: Timmer is canceled.%s", GetCurrentKeyword().c_str(), err.message().c_str());
	}
	else
	{
		time_duration tid = second_clock::universal_time().time_of_day();
		ptime nextActiveTime = not_a_date_time;
		if (tid >= time_duration(0, 0, 0, 0) && tid < time_duration(20, 0, 0, 0))
		{
			Start();
			nextActiveTime = ptime(second_clock::universal_time().date(), time_duration(20, 0, 30, 0));
		}
		else if (tid >= time_duration(20, 0, 0, 0) && tid < time_duration(20, 49, 0, 0))
		{
			Stop();
			nextActiveTime = ptime(second_clock::universal_time().date(), time_duration(20, 49, 30, 0));
		}
		else if (tid >= time_duration(20, 49, 0, 0) && tid < time_duration(23, 59, 59, 999))
		{
			Start();
			nextActiveTime = ptime(second_clock::universal_time().date() + days(1), time_duration(20, 0, 30, 0));
		}
		m_StartAndStopCtrlTimer.expires_at(nextActiveTime);
		m_StartAndStopCtrlTimer.async_wait(boost::bind(&CTWS_TDPlugin::TimerHandler, this,
			&m_StartAndStopCtrlTimer,
			boost::asio::placeholders::error));
		ShowMessage(normal, "%s: Next:%s", GetCurrentKeyword().c_str(), to_simple_string(nextActiveTime).c_str());
	}
}

bool CTWS_TDPlugin::Start()
{
	CAutoPend pend(m_abIsPending);
	if (false==m_boolIsOnline)
	{
		m_uRequestID = 0;
		m_boolIsOnline = false;
		if (m_pUserApi)
		{
			m_pUserApi->Release();
			m_pUserApi = nullptr;
		}
		m_pUserApi = MTwsApi::CreateApi();
		if (nullptr == m_pUserApi)
		{
			ShowMessage(
				severity_levels::error,
				"%s CreateFtdcTraderApi error",
				GetCurrentKeyword().c_str());
			return false;
		}
		m_pUserApi->RegisterSpi(this);
		
		char ServerAddress[65];
		strcpy(ServerAddress, m_strServerAddress.c_str());
		m_pUserApi->Connect(ServerAddress, m_uPort, m_uClientID);
		std::unique_lock<std::mutex> lk(m_mtxLoginSignal);
		m_cvLoginSignalCV.wait_for(lk, std::chrono::seconds(10));
		if (m_boolIsOnline)
			return true;
		else
		{
			m_pUserApi->RegisterSpi(nullptr);
			m_pUserApi->Release();
			m_pUserApi = nullptr;
			m_boolIsOnline = false;
			return false;
		}
	}
	else
		return true;
}

bool CTWS_TDPlugin::Stop()
{
	CAutoPend pend(m_abIsPending);
	if (true==m_boolIsOnline)
	{
		m_pUserApi->DisConnect();
	}
	if (m_pUserApi)
	{
		m_pUserApi->RegisterSpi(nullptr);
		m_pUserApi->Release();
		m_pUserApi = nullptr;
	}
	m_boolIsOnline=false;
	return true;

}

void CTWS_TDPlugin::TDUnload()
{
	Stop();
	m_StartAndStopCtrlTimer.cancel();
	m_IOservice.stop();
	m_futTimerThreadFuture.get();
}

void CTWS_TDPlugin::ShowMessage(severity_levels lv, const char * fmt, ...)
{
	char buf[512];
	va_list arg;
	va_start(arg, fmt);
	vsnprintf(buf, 512, fmt, arg);
	va_end(arg);
	boost::log::sources::severity_logger< severity_levels > m_Logger;
	BOOST_LOG_SEV(m_Logger, lv) << ProcessName << ": " << buf << " [" << to_iso_string(microsec_clock::universal_time()) << "]";
}

TOrderRefIdType CTWS_TDPlugin::TDBasicMakeOrder(
	TOrderType ordertype,
	unordered_map<string, string> & instrument,
	TOrderDirectionType direction,
	TOrderOffsetType offset,
	TVolumeType volume,
	TPriceType LimitPrice,
	TOrderRefIdType orderRefBase)
{
	if (false == m_boolIsOnline)
	{
		ShowMessage(severity_levels::error,
			"%s: makeorder error: not trading time.", GetCurrentKeyword().c_str());
		return LB1_NullOrderRef;
	}
	TOrderRefIdType Ref = _OrderIncreasePart2OrderRefPart(++m_uIncreasePart) +
		_AccountNumberPart2OrderRefPart(m_uAccountNumber)
		+ orderRefBase;

	if (Ref > MaxOrderRef)
	{
		ShowMessage(severity_levels::error,
			"%s: The OrderRef is run out of MaxRoderRef(%lld).", GetCurrentKeyword().c_str(), MaxOrderRef);
		return LB1_NullOrderRef;
	}
	CTwsContractField contr;
	CTwsInputOrderField ord;
	memset(&contr, 0, sizeof(CTwsContractField));
	memset(&ord, 0, sizeof(CTwsInputOrderField));
	
	if(instrument.find("symbol")->second!="_null_")
		strcpy(contr.m_strSymbol, instrument.find("symbol")->second.c_str());

	auto SecType = instrument.find("sectype")->second;
	if ("STK" == SecType)
		contr.m_enumSecType = TTwsSecTypeType::Tws_STK;
	else if ("OPT" == SecType)
		contr.m_enumSecType = TTwsSecTypeType::Tws_OPT;
	else if ("FUT" == SecType)
		contr.m_enumSecType = TTwsSecTypeType::Tws_FUT;
	else if ("IND" == SecType)
		contr.m_enumSecType = TTwsSecTypeType::Tws_IND;
	else if ("FOP" == SecType)
		contr.m_enumSecType = TTwsSecTypeType::Tws_FOP;
	else if ("CASH" == SecType)
		contr.m_enumSecType = TTwsSecTypeType::Tws_CASH;
	else if ("BAG" == SecType)
		contr.m_enumSecType = TTwsSecTypeType::Tws_BAG;
	if(instrument.find("expiry")->second!="_null_")
		strcpy(contr.m_strExpiry, instrument.find("expiry")->second.c_str());

	if(instrument.find("strike")->second!="_null_")
		contr.m_dbStrike=atof(instrument.find("strike")->second.c_str());

	if (instrument.find("multiplier")->second != "_null_")
		strcpy(contr.m_strMultiplier, instrument.find("multiplier")->second.c_str());

	if (instrument.find("exchange")->second != "_null_")
		strcpy(contr.m_strExchange,instrument.find("exchange")->second.c_str());

	if (instrument.find("primaryexchange")->second != "_null_")
		strcpy(contr.m_strPrimaryExchange, instrument.find("primaryexchange")->second.c_str());

	if (instrument.find("currency")->second != "_null_")
		strcpy(contr.m_strCurrency, instrument.find("currency")->second.c_str());

	if (instrument.find("localsymbol")->second != "_null_")
		strcpy(contr.m_strLocalSymbol, instrument.find("localsymbol")->second.c_str());

	if (instrument.find("tradingclass")->second != "_null_")
		strcpy(contr.m_strTradingClass, instrument.find("tradingclass")->second.c_str());
	
	ord.m_longOrderId= Ref;
	ord.m_enumAction= (direction == LB1_Buy ? Tws_BUY : Tws_SELL);//BUY， SELL， SSHORT
	ord.m_longTotalQuantity = volume;//定单数量。
	ord.m_enumTimeInForceType= Tws_GTC;//确认定单类型。取消前有效GTC
	ord.m_dbLmtPrice = LimitPrice;//限价价格，用于限价单、止损限价单和相对定单。 所有其它情形下为零。 对没有限价价格的相对定单，也为零。
	ord.m_dbAuxPrice=0;
	ord.m_longMinQty = 1;
	
	auto RtnOrderRef = m_pUserApi->ReqOrderInsert(&contr, &ord);;
	return RtnOrderRef;
	
}

TLastErrorIdType CTWS_TDPlugin::TDBasicCancelOrder(TOrderRefIdType Ref, unordered_map<string, string> & instrument, TOrderSysIdType orderSysId)
{
	if (false == m_boolIsOnline)
	{
		ShowMessage(severity_levels::error,
			"%s: cancel order error: not trading time.", GetCurrentKeyword().c_str());
		return LB1_INVALID_VAL;
	}
	
	m_pUserApi->ReqOrderCancel(Ref);
	return LB1_NO_ERROR;
}

int CTWS_TDPlugin::TDGetRemainAmountOfCancelChances(const char *)
{
	return std::numeric_limits<int>::max();
}

#pragma region CThostFtdcTraderSpi

void CTWS_TDPlugin::OnRspUserLogin(CTwsRspUserLoginField * loginField, bool IsSucceed)
{
	m_intNextValidId = loginField->m_intNextValidId;
	ShowMessage(
		severity_levels::error, 
		"AccountInfo: %s login succeed ServerTime:%ld",
		GetCurrentKeyword().c_str(),loginField->m_LongServerTime);
	m_uIncreasePart = _OrderRef2OrderIncreasePart(m_intNextValidId) + 1;
	NOTIFY_LOGIN_SUCCEED;
}

void CTWS_TDPlugin::OnRspError(int ErrID, int ErrCode, const char * ErrMsg)
{
	ShowMessage(severity_levels::error,
		"%s onrsperror.errorid=%d, errorcode=%d,errormsg=%s",
		GetCurrentKeyword().c_str(),
		ErrID,
		ErrCode, ErrMsg == nullptr ? "unknown" : ErrMsg);
}

void CTWS_TDPlugin::OnDisconnected()
{
	ShowMessage(severity_levels::error,
		"AccountInfo: %s OnDisconnected",
		GetCurrentKeyword().c_str());
	m_boolIsOnline = false;
}

void CTWS_TDPlugin::OnRtnOrder(CTwsOrderField * pOrder)
{
	if (nullptr == pOrder)
		return;
	if (pOrder->m_uClientID != m_uClientID)
		return;
	TOrderRefIdType Ref = pOrder->m_longOrderId;
	char Sys[64];
	sprintf(Sys, "%ld", pOrder->m_longOrderId);
	
	TOrderStatusType Status = LB1_StatusUnknown;
	switch (pOrder->m_enumStatus)
	{
		case Tws_PendingSubmit:Status = LB1_StatusUnknown;			break;
		case Tws_PendingCancel:Status = LB1_StatusUnknown;			break;
		case Tws_PreSubmitted:Status = LB1_StatusUnknown;			break;
		case Tws_Submitted:Status = LB1_StatusNoTradeQueueing;		break;
		case Tws_Cancelled:Status = LB1_StatusCanceled;				break;
		case Tws_Filled:Status = LB1_StatusPartTradedQueueing;		break;
		case Tws_Inactive:Status = LB1_StatusUnknown;				break;
	};
	TPriceType Price = pOrder->m_dbAvgFillPrice;
	TTradedVolumeType TraderVolume = pOrder->m_longFilledVolume;
	TRemainVolumeType RemainVolume = pOrder->m_longRemainingVolume;

	m_pTradePluginContext->OnOrder(Ref, Sys, Status, Price, TraderVolume, RemainVolume);
}

void CTWS_TDPlugin::OnRtnTrade(CTwsTradeField * pTrade)
{
	if (nullptr == pTrade)
		return;
	if (pTrade->m_uClientID != m_uClientID)
		return;
	TOrderRefIdType Ref = pTrade->m_longOrderId;
	char Sys[64];
	sprintf(Sys, "%ld", pTrade->m_longOrderId);
	TPriceType Price = pTrade->m_dbFilledPrice;
	TVolumeType Volume = pTrade->m_longFilledVolume;
	m_pTradePluginContext->OnTrade(Ref, Sys, Price, Volume);
}

#pragma endregion