#include "FEMAS_FUTURE_TDPlugin.h"
#include <stdarg.h>
#include <thread>
#include "OrderRefResolve.h"
#include "AutoPend.h"
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
const unsigned int SizeOfRef = sizeof(CUstpFtdcInputOrderField::UserOrderLocalID)-5;

const TOrderRefIdType MaxOrderRef = (PowerOfTen<SizeOfRef>::val - 1);

const string CFEMAS_FUTURE_TDPlugin::s_strAccountKeyword = "serveraddress;brokerid;investorid;username;password;maxcancelperday;";

date CFEMAS_FUTURE_TDPlugin::GetTradeday(ptime _Current)
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

CFEMAS_FUTURE_TDPlugin::CFEMAS_FUTURE_TDPlugin() :
	m_StartAndStopCtrlTimer(m_IOservice), m_abIsPending(false)
{
	snprintf(m_bufOrderRefFormat, sizeof(m_bufOrderRefFormat), "%%0%dd", SizeOfRef);
}

CFEMAS_FUTURE_TDPlugin::~CFEMAS_FUTURE_TDPlugin()
{

}

bool CFEMAS_FUTURE_TDPlugin::IsPedding()
{
	return m_abIsPending.load();
}

bool CFEMAS_FUTURE_TDPlugin::IsOnline()
{
	return m_boolIsOnline;
}

void CFEMAS_FUTURE_TDPlugin::IncreaseRefCount()
{
	m_intRefCount++;
}

void CFEMAS_FUTURE_TDPlugin::DescreaseRefCount()
{
	m_intRefCount--;
}

int CFEMAS_FUTURE_TDPlugin::GetRefCount()
{
	return m_intRefCount;
}

void CFEMAS_FUTURE_TDPlugin::CheckSymbolValidity(const unordered_map<string, string> & insConfig)
{
	auto Type = insConfig.find("type");
	if (Type == insConfig.end())
		throw std::runtime_error("can not find the <type> of the the symbol.");
	if (Type->second != "future")
		throw std::runtime_error("femas marketdatasource does not support this symbol.");
	auto InstrumentNode = insConfig.find("instrumentid");
	if (InstrumentNode == insConfig.end())
		throw std::runtime_error("<instrumentid> not found.");
	auto ExchangeIDNode = insConfig.find("exchangeid");
	if (ExchangeIDNode == insConfig.end())
		throw std::runtime_error("<exchangeid> not found.");
}

string CFEMAS_FUTURE_TDPlugin::GetCurrentKeyword()
{
	return "femas_td&"+m_strUsername + "&" + m_strBrokerID;
}

string CFEMAS_FUTURE_TDPlugin::GetProspectiveKeyword(const ptree & in)
{
	string retKey = "femas_td&";
	auto temp = in.find("username");
	if (temp != in.not_found())
	{
		
		if (temp->second.data().size()>(sizeof(CUstpFtdcReqUserLoginField::UserID) - 1))
			throw std::runtime_error("femas:username is too long");
		retKey += temp->second.data();
	}
	else
		throw std::runtime_error("femas:can not find <username>");
	retKey += "&";

	temp = in.find("brokerid");
	if (temp != in.not_found())
	{
		
		if (temp->second.data().size()>(sizeof(CUstpFtdcReqUserLoginField::BrokerID) - 1))
			throw std::runtime_error("femas:brokerid is too long");
		retKey += temp->second.data();
	}
	else
		throw std::runtime_error("femas:can not find <brokerid>");

	return retKey;
}

void CFEMAS_FUTURE_TDPlugin::GetState(ptree & out)
{
	if (m_boolIsOnline)
		out.put("online", "true");
	else
		out.put("online", "false");
	out.put("serveraddress", m_strServerAddress);
	out.put("brokerid", m_strBrokerID);
	out.put("username", m_strUsername);
	ptree CancelAmount;
	{
		boost::shared_lock<boost::shared_mutex> lg(m_mtxProtectCancelAmount);
		for (auto & p : m_mapCancelAmount)
			CancelAmount.put(p.first, m_intInitAmountOfCancelChancesPerDay-p.second);
	}
	out.put_child("cancelamount", CancelAmount);
}

void CFEMAS_FUTURE_TDPlugin::UpdateAccountInfo(const ptree & in)
{
	auto temp = in.find("serveraddress");
	if (temp != in.not_found())
	{
		m_strServerAddress = temp->second.data();
		if (m_strServerAddress.size()>64)
			throw std::runtime_error("femas:serveraddress is too long(Max:64 char)");
		else if (m_strServerAddress.empty())
			throw std::runtime_error("femas:serveraddress is empty");
	}
	else
		throw std::runtime_error("femas:can not find <serveraddress>");


	temp = in.find("brokerid");
	if (temp != in.not_found())
	{
		m_strBrokerID = temp->second.data();
		if (m_strBrokerID.size()>(sizeof(CUstpFtdcReqUserLoginField::BrokerID) - 1))
			throw std::runtime_error("femas:brokerid is too long");
		else if (m_strBrokerID.empty())
			throw std::runtime_error("femas:brokerid is empty");
	}
	else
		throw std::runtime_error("femas:can not find <brokerid>");

	temp = in.find("username");
	if (temp != in.not_found())
	{
		m_strUsername = temp->second.data();
		if (m_strUsername.size()>(sizeof(CUstpFtdcReqUserLoginField::UserID) - 1))
			throw std::runtime_error("femas:username is too long");
		else if (m_strUsername.empty())
			throw std::runtime_error("femas:username is empty");
	}
	else
		throw std::runtime_error("femas:Can not find <username>");

	temp = in.find("investorid");
	if (temp != in.not_found())
	{
		m_strInvestorID = temp->second.data();
		if (m_strInvestorID.size()>(sizeof(TUstpFtdcInvestorIDType) - 1))
			throw std::runtime_error("femas:invectorid is too long");
		else if (m_strInvestorID.empty())
			throw std::runtime_error("femas:invectorid is empty");
	}
	else
		throw std::runtime_error("femas:Can not find <invectorid>");

	temp = in.find("password");
	if (temp != in.not_found())
	{
		m_strPassword = temp->second.data();
		if (m_strPassword.size()>(sizeof(CUstpFtdcReqUserLoginField::Password) - 1))
			throw std::runtime_error("femas:password is too long");
		else if (m_strPassword.empty())
			throw std::runtime_error("femas:Password is empty");
	}
	else
		throw std::runtime_error("femas:Can not find <password>");
}

void CFEMAS_FUTURE_TDPlugin::TDInit(const ptree & in, MTradePluginContextInterface * pTradePluginContext, unsigned int AccountNumber)
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
	m_StartAndStopCtrlTimer.async_wait(boost::bind(&CFEMAS_FUTURE_TDPlugin::TimerHandler, this,
		&m_StartAndStopCtrlTimer,
		boost::asio::placeholders::error));
	m_futTimerThreadFuture = std::async(std::launch::async,[this] {
		this->m_IOservice.run();
		return true;
	});


}

void CFEMAS_FUTURE_TDPlugin::TDHotUpdate(const ptree & in)
{
	UpdateAccountInfo(in);
	if (m_boolIsOnline)
	{
		Stop();
		Start();
	}
}

void CFEMAS_FUTURE_TDPlugin::TimerHandler(boost::asio::deadline_timer* timer, const boost::system::error_code& err)
{
	if (err)
	{
		ShowMessage(normal, "%s: Timmer is canceled.%s", GetCurrentKeyword().c_str(), err.message().c_str());
	}
	else
	{
		time_duration tid = second_clock::universal_time().time_of_day();
		ptime nextActiveTime = not_a_date_time;
		if (tid >= time_duration(0, 0, 0, 0) && tid < time_duration(0, 45, 0, 0))
		{
			nextActiveTime = ptime(second_clock::universal_time().date(), time_duration(0, 45, 30, 0));
		}
		else if (tid >= time_duration(0, 45, 0, 0) && tid < time_duration(7, 15, 0, 0))
		{
			Start();
			nextActiveTime = ptime(second_clock::universal_time().date(), time_duration(7, 15, 30, 0));
		}
		else if (tid >= time_duration(7, 15, 0, 0) && tid < time_duration(12, 45, 0, 0))
		{
			Stop();
			nextActiveTime = ptime(second_clock::universal_time().date(), time_duration(12, 45, 30, 0));
		}
		else if (tid >= time_duration(12, 45, 0, 0) && tid < time_duration(18, 30, 0, 0))
		{
			Start();
			nextActiveTime = ptime(second_clock::universal_time().date(), time_duration(18, 30, 30, 0));
		}
		else if (tid >= time_duration(18, 30, 0, 0) && tid < time_duration(23, 59, 59, 999))
		{
			Stop();
			nextActiveTime = ptime(second_clock::universal_time().date() + days(1), time_duration(0, 45, 30, 0));
		}
		m_StartAndStopCtrlTimer.expires_at(nextActiveTime);
		m_StartAndStopCtrlTimer.async_wait(boost::bind(&CFEMAS_FUTURE_TDPlugin::TimerHandler, this,
			&m_StartAndStopCtrlTimer,
			boost::asio::placeholders::error));
		ShowMessage(normal, "%s: Next:%s", GetCurrentKeyword().c_str(), to_simple_string(nextActiveTime).c_str());
	}
}

bool CFEMAS_FUTURE_TDPlugin::Start()
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
		m_pUserApi = CUstpFtdcTraderApi::CreateFtdcTraderApi();
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
		m_pUserApi->RegisterFront(ServerAddress);
		
		//m_pUserApi->SubscribeUserTopic(USTP_TERT_QUICK);//不能写这一条，否则OnOrder与OnTrade都会调用两次
		m_pUserApi->SubscribePrivateTopic(USTP_TERT_QUICK);// 注册公有流
		m_pUserApi->SubscribePublicTopic(USTP_TERT_QUICK);// 注册私有流
		m_pUserApi->Init();
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

bool CFEMAS_FUTURE_TDPlugin::Stop()
{
	CAutoPend pend(m_abIsPending);
	if (true==m_boolIsOnline)
	{
		CUstpFtdcReqUserLogoutField req;
		memset(&req, 0, sizeof(req));
		strncpy(req.BrokerID, m_strBrokerID.c_str(), sizeof(req.BrokerID));
		strncpy(req.UserID, m_strUsername.c_str(), sizeof(req.UserID));
		int res = m_pUserApi->ReqUserLogout(&req, ++m_uRequestID);
		if (0 != res)
			ShowMessage(severity_levels::error, "%s:  ReqUserLogout Failed.", GetCurrentKeyword().c_str());
		else
		{
			ShowMessage(severity_levels::error, "%s:  ReqUserLogout Succeed.", GetCurrentKeyword().c_str());
			std::unique_lock<std::mutex> lk(m_mtxLogoutSignal);
			m_cvLogoutSignalCV.wait_for(lk, std::chrono::seconds(10));
		}
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

void CFEMAS_FUTURE_TDPlugin::TDUnload()
{
	
	Stop();
	m_StartAndStopCtrlTimer.cancel();
	m_IOservice.stop();
	m_futTimerThreadFuture.get();
}

void CFEMAS_FUTURE_TDPlugin::ShowMessage(severity_levels lv, const char * fmt, ...)
{
	char buf[512];
	va_list arg;
	va_start(arg, fmt);
	vsnprintf(buf, 512, fmt, arg);
	va_end(arg);
	boost::log::sources::severity_logger< severity_levels > m_Logger;
	BOOST_LOG_SEV(m_Logger, lv) << ProcessName << ": " << buf << " [" << to_iso_string(microsec_clock::universal_time()) << "]";
}

TOrderRefIdType CFEMAS_FUTURE_TDPlugin::TDBasicMakeOrder(
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
			"%s: The OrderRef is run out of MaxRoderRef(%d).", GetCurrentKeyword().c_str(), MaxOrderRef);
		return LB1_NullOrderRef;
	}
	CUstpFtdcInputOrderField req;
	memset(&req, 0, sizeof(req));
	snprintf(req.UserOrderLocalID, sizeof(req.UserOrderLocalID), m_bufOrderRefFormat, Ref);
	strncpy(req.BrokerID, m_strBrokerID.c_str(), sizeof(req.BrokerID));
	strncpy(req.InvestorID, m_strInvestorID.c_str(), sizeof(req.InvestorID));
	strncpy(req.UserID, m_strUsername.c_str(), sizeof(req.UserID));
	strncpy(req.InstrumentID, instrument["instrumentid"].c_str(), sizeof(req.InstrumentID)); //合约代码
	strncpy(req.ExchangeID, instrument["exchangeid"].c_str(), sizeof(req.InstrumentID)); //合约代码

	req.Direction = (direction == LB1_Buy ? USTP_FTDC_D_Buy : USTP_FTDC_D_Sell);
	switch (offset)
	{
	case LB1_Increase:req.OffsetFlag = USTP_FTDC_OF_Open;break;
	case LB1_Decrease:req.OffsetFlag = USTP_FTDC_OF_Close;break;
	case LB1_DecreaseYesterday:req.OffsetFlag = USTP_FTDC_OF_CloseYesterday;break;
	case LB1_DecreaseToday:req.OffsetFlag = USTP_FTDC_OF_CloseToday;break;
	}
	req.Volume = volume;
	req.HedgeFlag = USTP_FTDC_CHF_Speculation;
	req.ForceCloseReason = USTP_FTDC_FCR_NotForceClose;
	req.IsAutoSuspend = 0;

	switch (ordertype)
	{
	case LB1_NormalLimitOrderType:
	{
		req.OrderPriceType = USTP_FTDC_OPT_LimitPrice;
		req.LimitPrice = LimitPrice;
		req.TimeCondition = USTP_FTDC_TC_GFD;//THOST_FTDC_TC_IOC;//
		req.VolumeCondition = USTP_FTDC_VC_AV;//THOST_FTDC_VC_CV;//
		req.MinVolume = 1;//volume;//

	}
		break;
	case LB1_MarketOrderType:
	{
		req.OrderPriceType = USTP_FTDC_OPT_AnyPrice;
		req.LimitPrice = 0.0;
		req.TimeCondition = USTP_FTDC_TC_IOC;//THOST_FTDC_TC_IOC;//
		req.VolumeCondition = USTP_FTDC_VC_AV;//THOST_FTDC_VC_CV;//
		req.MinVolume = 1;//volume;//
	}
		break;
	case LB1_FOKLimitOrderType:
	{
		req.OrderPriceType = USTP_FTDC_OPT_LimitPrice;
		req.LimitPrice = LimitPrice;
		req.TimeCondition = USTP_FTDC_TC_IOC;
		req.VolumeCondition = USTP_FTDC_VC_CV;
		req.MinVolume = 1;//volume;//
	}
		break;
	case LB1_FAKLimitOrderType:
	{
		req.OrderPriceType = USTP_FTDC_OPT_LimitPrice;
		req.LimitPrice = LimitPrice;
		req.TimeCondition = USTP_FTDC_TC_IOC;
		req.VolumeCondition = USTP_FTDC_VC_MV;
		req.MinVolume = 1;//volume;//
	}
		break;
	}
	int ReqOrderInsertResult = m_pUserApi->ReqOrderInsert(&req, ++m_uRequestID);
	if (0 != ReqOrderInsertResult)
	{
		ShowMessage(severity_levels::error, "[!][T]Send ReqOrderInsert Failed for %d.[Ref:]", ReqOrderInsertResult, Ref);
		return LB1_NullOrderRef;
	}
	return Ref;
}

TLastErrorIdType CFEMAS_FUTURE_TDPlugin::TDBasicCancelOrder(TOrderRefIdType Ref, unordered_map<string, string> & instrument, TOrderSysIdType orderSysId)
{
	if (false == m_boolIsOnline)
	{
		ShowMessage(severity_levels::error,
			"%s: cancel order error: not trading time.", GetCurrentKeyword().c_str());
		return LB1_INVALID_VAL;
	}
	TOrderRefIdType ActionLocalID = _OrderIncreasePart2OrderRefPart(++m_uIncreasePart) +
		_AccountNumberPart2OrderRefPart(m_uAccountNumber);
	CUstpFtdcOrderActionField Req;
	memset(&Req, 0, sizeof(Req));
	Req.ActionFlag = USTP_FTDC_AF_Delete;
	snprintf(Req.UserOrderActionLocalID, sizeof(Req.UserOrderActionLocalID), m_bufOrderRefFormat, ActionLocalID);
	strncpy(Req.BrokerID, m_strBrokerID.c_str(), sizeof(Req.BrokerID));
	strncpy(Req.ExchangeID, instrument["exchangeid"].c_str(), sizeof(Req.ExchangeID));
	strncpy(Req.InvestorID, m_strInvestorID.c_str(), sizeof(Req.InvestorID));
	strncpy(Req.OrderSysID, orderSysId, sizeof(Req.OrderSysID));
	strncpy(Req.UserID, m_strUsername.c_str(), sizeof(Req.UserID));
	if( 0 != m_pUserApi->ReqOrderAction(&Req, ++m_uRequestID))
	{
		ShowMessage(severity_levels::error, "[!][T]Cancel Order %lld(%s) Failed", ActionLocalID,orderSysId);
		return LB1_INVALID_VAL;
	}
	return LB1_NO_ERROR;

}

int CFEMAS_FUTURE_TDPlugin::TDGetRemainAmountOfCancelChances(const char * ins)
{
	if (nullptr == ins)
		return 0;
	else
	{
		boost::unique_lock<boost::shared_mutex> lg(m_mtxProtectCancelAmount);
		if (m_dateTradeDay != GetTradeday(second_clock::universal_time()))
		{
			m_dateTradeDay = GetTradeday(second_clock::universal_time());
			m_mapCancelAmount.clear();
			map<string, int>().swap(m_mapCancelAmount);
		}
		return m_intInitAmountOfCancelChancesPerDay - m_mapCancelAmount[ins];
	}
}


void CFEMAS_FUTURE_TDPlugin::OnFrontConnected()
{
	CUstpFtdcReqUserLoginField req;
	memset(&req, 0, sizeof(req));
	strncpy(req.BrokerID, m_strBrokerID.c_str(), sizeof(req.BrokerID));
	strncpy(req.UserID, m_strUsername.c_str(), sizeof(req.UserID));
	strncpy(req.Password, m_strPassword.c_str(), sizeof(req.Password));
	if ((nullptr == m_pUserApi)||(0 != m_pUserApi->ReqUserLogin(&req, ++m_uRequestID)))
	{
		ShowMessage(
			severity_levels::error, 
			"%s: Send ReqUserLogin Failed.",
			GetCurrentKeyword().c_str());
		NOTIFY_LOGIN_FAILED;
	}
}

void CFEMAS_FUTURE_TDPlugin::OnFrontDisconnected(int nReason)
{
	m_boolIsOnline = false;
	ShowMessage(severity_levels::error, "%s: FrontDisconnected for nReason %d",GetCurrentKeyword().c_str(),nReason);
}

void CFEMAS_FUTURE_TDPlugin::OnHeartBeatWarning(int nTimeLapse)
{
	ShowMessage(severity_levels::error, "%s heartbeatwarning %d", GetCurrentKeyword().c_str(), nTimeLapse);
}

void CFEMAS_FUTURE_TDPlugin::OnPackageStart(int nTopicID, int nSequenceNo)
{
}

void CFEMAS_FUTURE_TDPlugin::OnPackageEnd(int nTopicID, int nSequenceNo)
{
}

void CFEMAS_FUTURE_TDPlugin::OnRspError(CUstpFtdcRspInfoField * pRspInfo, int nRequestID, bool bIsLast)
{
	if (pRspInfo&&pRspInfo->ErrorID != 0)
	{
		ShowMessage(
			severity_levels::error,
			"%s: onrsperror:errorid=%d errormsg=%s.",
			GetCurrentKeyword().c_str(),
			pRspInfo->ErrorID,
			pRspInfo->ErrorMsg);
	}
}

void CFEMAS_FUTURE_TDPlugin::OnRspUserLogin(CUstpFtdcRspUserLoginField * pRspUserLogin, CUstpFtdcRspInfoField * pRspInfo, int nRequestID, bool bIsLast)
{
	if ((pRspInfo) && (pRspInfo->ErrorID != 0))
	{
		ShowMessage(
			severity_levels::error,
			"AccountInfo: %s: login failed.errorid=%d,errormsg=%s",
			GetCurrentKeyword().c_str(),
			pRspInfo->ErrorID,
			pRspInfo->ErrorMsg);
		NOTIFY_LOGIN_FAILED;
	}
	else
	{
		ShowMessage(
			severity_levels::normal,
			"AccountInfo: %s login succeed!",
			GetCurrentKeyword().c_str());
		m_uIncreasePart = _OrderRef2OrderIncreasePart(atoi(pRspUserLogin->MaxOrderLocalID))+1;
		ShowMessage(severity_levels::normal, "MaxOrderLocalID=%s StartOrderRef=%u", pRspUserLogin->MaxOrderLocalID, m_uIncreasePart);
		NOTIFY_LOGIN_SUCCEED;
	}
}

void CFEMAS_FUTURE_TDPlugin::OnRspUserLogout(CUstpFtdcRspUserLogoutField * pRspUserLogout, CUstpFtdcRspInfoField * pRspInfo, int nRequestID, bool bIsLast)
{
	ShowMessage(
		severity_levels::normal,
		"AccountInfo: %s loginout succeed!",
		GetCurrentKeyword().c_str());
	std::unique_lock<std::mutex> lk(m_mtxLogoutSignal);
	m_cvLogoutSignalCV.notify_all();
}

void CFEMAS_FUTURE_TDPlugin::OnRspOrderInsert(CUstpFtdcInputOrderField * pInputOrder, CUstpFtdcRspInfoField * pRspInfo, int nRequestID, bool bIsLast)
{
	if (pRspInfo&&pRspInfo->ErrorID != 0)
	{
		unsigned int OrderRef = (nullptr == pInputOrder ? 0 : atoi(pInputOrder->UserOrderLocalID));

		ShowMessage(
			severity_levels::error,
			"%s: onrsporderinsert:errorid=%d errormsg=%s orderref=%u.",
			GetCurrentKeyword().c_str(),
			pRspInfo->ErrorID,
			pRspInfo->ErrorMsg,
			OrderRef);
	}
}

void CFEMAS_FUTURE_TDPlugin::OnRspOrderAction(CUstpFtdcOrderActionField * pOrderAction, CUstpFtdcRspInfoField * pRspInfo, int nRequestID, bool bIsLast)
{
	if (pRspInfo&&pRspInfo->ErrorID != 0)
	{
		unsigned int OrderRef = (nullptr==pOrderAction?0:atoi(pOrderAction->UserOrderActionLocalID));
		ShowMessage(
			severity_levels::error,
			"%s: onrsporderaction:errorid=%d errormsg=%s orderref=%u",
			GetCurrentKeyword().c_str(),
			pRspInfo->ErrorID,
			pRspInfo->ErrorMsg,
			OrderRef);
	}
}

void CFEMAS_FUTURE_TDPlugin::OnRtnTrade(CUstpFtdcTradeField * pTrade)
{
	if (nullptr == pTrade)
		return;
	TOrderRefIdType Ref = atoi(pTrade->UserOrderLocalID);
	TOrderSysIdType Sys = pTrade->OrderSysID;
	TPriceType Price = pTrade->TradePrice;
	TVolumeType Volume = pTrade->TradeVolume;
	m_pTradePluginContext->OnTrade(Ref, Sys, Price, Volume);
}

void CFEMAS_FUTURE_TDPlugin::OnRtnOrder(CUstpFtdcOrderField * pOrder)
{
	if (nullptr == pOrder)
		return;
	if (USTP_FTDC_OS_Canceled == pOrder->OrderStatus)
	{
		boost::unique_lock<boost::shared_mutex> lg(m_mtxProtectCancelAmount);
		if (m_dateTradeDay != GetTradeday(second_clock::universal_time()))
		{
			m_dateTradeDay = GetTradeday(second_clock::universal_time());
			m_mapCancelAmount.clear();
			map<string, int>().swap(m_mapCancelAmount);
		}
		m_mapCancelAmount[pOrder->InstrumentID]++;
	}
	TOrderRefIdType Ref = atoi(pOrder->UserOrderLocalID);
	TOrderSysIdType Sys = pOrder->OrderSysID;
	TOrderStatusType Status = LB1_StatusUnknown;
	switch (pOrder->OrderStatus)
	{
	case USTP_FTDC_OS_AllTraded:				Status = LB1_StatusAllTraded;			break;
	case USTP_FTDC_OS_PartTradedQueueing:		Status = LB1_StatusPartTradedQueueing;	break;
	case USTP_FTDC_OS_PartTradedNotQueueing:	Status = LB1_StatusUnknown;				break;
	case USTP_FTDC_OS_NoTradeQueueing:			Status = LB1_StatusNoTradeQueueing;		break;
	case USTP_FTDC_OS_NoTradeNotQueueing:		Status = LB1_StatusUnknown;				break;
	case USTP_FTDC_OS_Canceled:					Status = LB1_StatusCanceled;			break;
	case USTP_FTDC_OS_AcceptedNoReply:			Status = LB1_StatusUnknown;				break;
	}
	TPriceType Price = pOrder->LimitPrice;
	TTradedVolumeType TraderVolume = pOrder->VolumeTraded;
	TRemainVolumeType RemainVolume = pOrder->VolumeRemain;

	m_pTradePluginContext->OnOrder(Ref, Sys, Status, Price, TraderVolume, RemainVolume);
}

void CFEMAS_FUTURE_TDPlugin::OnErrRtnOrderInsert(CUstpFtdcInputOrderField * pInputOrder, CUstpFtdcRspInfoField * pRspInfo)
{
	if (pRspInfo&&pRspInfo->ErrorID != 0)
	{
		ShowMessage(
			severity_levels::error,
			"%s: onerrrtnorderinsert:Errorid=%d errormsg=%s.",
			GetCurrentKeyword().c_str(),
			pRspInfo->ErrorID,
			pRspInfo->ErrorMsg);
	}
}

void CFEMAS_FUTURE_TDPlugin::OnErrRtnOrderAction(CUstpFtdcOrderActionField * pOrderAction, CUstpFtdcRspInfoField * pRspInfo)
{
	if (pRspInfo&&pRspInfo->ErrorID != 0)
	{
		ShowMessage(
			severity_levels::error,
			"%s: onerrrtnorderaction:errorid=%d errormsg=%s.",
			GetCurrentKeyword().c_str(),
			pRspInfo->ErrorID,
			pRspInfo->ErrorMsg);
	}
}

void CFEMAS_FUTURE_TDPlugin::OnRspSubscribeTopic(CUstpFtdcDisseminationField * pDissemination, CUstpFtdcRspInfoField * pRspInfo, int nRequestID, bool bIsLast)
{
}

