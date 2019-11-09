#include "CTP_FUTURE_TDPlugin.h"
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
const size_t SizeOfRef = sizeof(CThostFtdcInputOrderField::OrderRef)-1;
const TOrderRefIdType MaxOrderRef = (PowerOfTen<SizeOfRef>::val - 1);

const string CCTP_FUTURE_TDPlugin::s_strAccountKeyword = "serveraddress;brokerid;username;password;appid;authCode;investorid;userProductInfo;maxcancelperday;";

date CCTP_FUTURE_TDPlugin::GetTradeday(ptime _Current)
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


CCTP_FUTURE_TDPlugin::CCTP_FUTURE_TDPlugin() :
	m_StartAndStopCtrlTimer(m_IOservice), m_abIsPending(false)
{

}

CCTP_FUTURE_TDPlugin::~CCTP_FUTURE_TDPlugin()
{

}

bool CCTP_FUTURE_TDPlugin::IsPedding()
{
	return m_abIsPending.load();
}

bool CCTP_FUTURE_TDPlugin::IsOnline()
{
	return m_boolIsOnline;
}

void CCTP_FUTURE_TDPlugin::IncreaseRefCount()
{
	m_intRefCount++;
}

void CCTP_FUTURE_TDPlugin::DescreaseRefCount()
{
	m_intRefCount--;
}

int CCTP_FUTURE_TDPlugin::GetRefCount()
{
	return m_intRefCount;
}

void CCTP_FUTURE_TDPlugin::CheckSymbolValidity(const unordered_map<string, string> & insConfig)
{
	auto Type = insConfig.find("type");
	if (Type == insConfig.end())
		throw std::runtime_error("can not find the <type> of the the symbol.");
	if (Type->second != "future")
		throw std::runtime_error("ctp marketdatasource does not support this symbol.");
	auto InstrumentNode = insConfig.find("instrumentid");
	if (InstrumentNode == insConfig.end())
		throw std::runtime_error("<instrumentid> not found.");
	auto ExchangeIDNode = insConfig.find("exchangeid");
	if (ExchangeIDNode == insConfig.end())
		throw std::runtime_error("<exchangeid> not found.");
}

string CCTP_FUTURE_TDPlugin::GetCurrentKeyword()
{
	return "ctp_td&"+m_strUsername + "&" + m_strBrokerID;
}

string CCTP_FUTURE_TDPlugin::GetProspectiveKeyword(const ptree & in)
{
	string retKey = "ctp_td&";
	auto temp = in.find("username");
	if (temp != in.not_found())
	{
		
		if (temp->second.data().size()>(sizeof(CThostFtdcReqUserLoginField::UserID) - 1))
			throw std::runtime_error("ctp:username is too long");
		retKey += temp->second.data();
	}
	else
		throw std::runtime_error("ctp:can not find <username>");
	retKey += "&";

	temp = in.find("brokerid");
	if (temp != in.not_found())
	{
		
		if (temp->second.data().size()>(sizeof(CThostFtdcReqUserLoginField::BrokerID) - 1))
			throw std::runtime_error("ctp:brokerid is too long");
		retKey += temp->second.data();
	}
	else
		throw std::runtime_error("ctp:can not find <brokerid>");

	return retKey;
}

void CCTP_FUTURE_TDPlugin::GetState(ptree & out)
{
	if (m_boolIsOnline)
		out.put("online", "true");
	else
		out.put("online", "false");
	out.put("serveraddress", m_strServerAddress);
	out.put("brokerid", m_strBrokerID);
	out.put("username", m_strUsername);

	//out.put("appid", m_strAppID);
	//out.put("authcode", m_strAuthCode);
	//out.put("investorid", m_strInvestorID);
	//out.put("userProductinfo", m_strUserProductInfo);





	ptree CancelAmount;
	{
		boost::shared_lock<boost::shared_mutex> lg(m_mtxProtectCancelAmount);
		for (auto & p : m_mapCancelAmount)
			CancelAmount.put(p.first, m_intInitAmountOfCancelChancesPerDay - p.second);
	}
	out.put_child("cancelamount",CancelAmount);
}

void CCTP_FUTURE_TDPlugin::UpdateAccountInfo(const ptree & in)
{
	auto temp = in.find("serveraddress");
	if (temp != in.not_found())
	{
		m_strServerAddress = temp->second.data();
		if (m_strServerAddress.size()>64)
			throw std::runtime_error("ctp:serveraddress is too long(Max:64 char)");
		else if (m_strServerAddress.empty())
			throw std::runtime_error("ctp:serveraddress is empty");
	}
	else
		throw std::runtime_error("ctp:can not find <serveraddress>");


	temp = in.find("brokerid");
	if (temp != in.not_found())
	{
		m_strBrokerID = temp->second.data();
		if (m_strBrokerID.size()>(sizeof(CThostFtdcReqUserLoginField::BrokerID) - 1))
			throw std::runtime_error("ctp:brokerid is too long");
		else if (m_strBrokerID.empty())
			throw std::runtime_error("ctp:brokerid is empty");
	}
	else
		throw std::runtime_error("ctp:can not find <brokerid>");

	temp = in.find("username");
	if (temp != in.not_found())
	{
		m_strUsername = temp->second.data();
		if (m_strUsername.size()>(sizeof(CThostFtdcReqUserLoginField::UserID) - 1))
			throw std::runtime_error("ctp:username is too long");
		else if (m_strUsername.empty())
			throw std::runtime_error("ctp:username is empty");
	}
	else
		throw std::runtime_error("ctp:Can not find <username>");

	temp = in.find("password");
	if (temp != in.not_found())
	{
		m_strPassword = temp->second.data();
		if (m_strPassword.size()>(sizeof(CThostFtdcReqUserLoginField::Password) - 1))
			throw std::runtime_error("ctp:password is too long");
		else if (m_strPassword.empty())
			throw std::runtime_error("ctp:Password is empty");
	}
	else
		throw std::runtime_error("ctp:Can not find <password>");
}

void CCTP_FUTURE_TDPlugin::TDInit(const ptree & in, MTradePluginContextInterface * pTradePluginContext, unsigned int AccountNumber)
{
	m_uAccountNumber = AccountNumber;
	m_pTradePluginContext = pTradePluginContext;

	UpdateAccountInfo(in);

	auto MaxCancelPerDayNode = in.find("maxcancelperday");
	if (MaxCancelPerDayNode != in.not_found())
		m_intInitAmountOfCancelChancesPerDay = atoi(MaxCancelPerDayNode->second.data().c_str());
	else
		m_intInitAmountOfCancelChancesPerDay = 400;
	
	m_dateTradeDay=GetTradeday(second_clock::universal_time());
	map<string, int>().swap(m_mapCancelAmount);

	m_StartAndStopCtrlTimer.expires_from_now(boost::posix_time::seconds(3));
	m_StartAndStopCtrlTimer.async_wait(boost::bind(&CCTP_FUTURE_TDPlugin::TimerHandler, this,
		&m_StartAndStopCtrlTimer,
		boost::asio::placeholders::error));
	m_futTimerThreadFuture = std::async(std::launch::async,[this] {
		this->m_IOservice.run();
		return true;
	});


}

void CCTP_FUTURE_TDPlugin::TDHotUpdate(const ptree & in)
{
	UpdateAccountInfo(in);
	if (m_boolIsOnline)
	{
		Stop();
		Start();
	}
}

void CCTP_FUTURE_TDPlugin::TimerHandler(boost::asio::deadline_timer* timer, const boost::system::error_code& err)
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
		m_StartAndStopCtrlTimer.async_wait(boost::bind(&CCTP_FUTURE_TDPlugin::TimerHandler, this,
			&m_StartAndStopCtrlTimer,
			boost::asio::placeholders::error));
		ShowMessage(normal, "%s: Next:%s", GetCurrentKeyword().c_str(), to_simple_string(nextActiveTime).c_str());
	}
}

bool CCTP_FUTURE_TDPlugin::Start()
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
		m_pUserApi = CThostFtdcTraderApi::CreateFtdcTraderApi();
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
		m_pUserApi->SubscribePrivateTopic(THOST_TERT_QUICK);
		m_pUserApi->SubscribePublicTopic(THOST_TERT_QUICK);
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

bool CCTP_FUTURE_TDPlugin::Stop()
{
	CAutoPend pend(m_abIsPending);
	if (true==m_boolIsOnline)
	{
		CThostFtdcUserLogoutField req;
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

void CCTP_FUTURE_TDPlugin::TDUnload()
{
	Stop();
	m_StartAndStopCtrlTimer.cancel();
	m_IOservice.stop();
	m_futTimerThreadFuture.get();
}

void CCTP_FUTURE_TDPlugin::ShowMessage(severity_levels lv, const char * fmt, ...)
{
	char buf[512];
	va_list arg;
	va_start(arg, fmt);
	vsnprintf(buf, 512, fmt, arg);
	va_end(arg);
	boost::log::sources::severity_logger< severity_levels > m_Logger;
	BOOST_LOG_SEV(m_Logger, lv) << ProcessName << ": " << buf << " [" << to_iso_string(microsec_clock::universal_time()) << "]";
}

TOrderRefIdType CCTP_FUTURE_TDPlugin::TDBasicMakeOrder(
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
	CThostFtdcInputOrderField req;
	memset(&req, 0, sizeof(req));
	snprintf(req.OrderRef, sizeof(req.OrderRef), "%.12d", static_cast<int>(Ref));
	strncpy(req.BrokerID, m_strBrokerID.c_str(), sizeof(req.BrokerID));
	strncpy(req.InvestorID, m_strUsername.c_str(), sizeof(req.InvestorID));
	strncpy(req.InstrumentID, instrument["instrumentid"].c_str(), sizeof(req.InstrumentID)); //合约代码
	switch (ordertype)
	{
	case LB1_NormalLimitOrderType:
	{
		req.OrderPriceType = THOST_FTDC_OPT_LimitPrice;
		req.LimitPrice = LimitPrice;
		req.TimeCondition = THOST_FTDC_TC_GFD;//THOST_FTDC_TC_IOC;//
		req.VolumeCondition = THOST_FTDC_VC_AV;//THOST_FTDC_VC_CV;//
		req.MinVolume = 1;//volume;//
	}
		break;
	case LB1_MarketOrderType:
	{
		req.OrderPriceType = THOST_FTDC_OPT_AnyPrice;
		req.LimitPrice = 0.0;
		req.TimeCondition = THOST_FTDC_TC_IOC;//THOST_FTDC_TC_IOC;//
		req.VolumeCondition = THOST_FTDC_VC_AV;//THOST_FTDC_VC_CV;//
		req.MinVolume = 1;//volume;//
	}
		break;
	case LB1_FOKLimitOrderType:
	{
		req.OrderPriceType = THOST_FTDC_OPT_LimitPrice;
		req.LimitPrice = LimitPrice;
		req.TimeCondition = THOST_FTDC_TC_IOC;
		req.VolumeCondition = THOST_FTDC_VC_CV;
		req.MinVolume = 1;//volume;//
	}
		break;
	case LB1_FAKLimitOrderType:
	{
		req.OrderPriceType = THOST_FTDC_OPT_LimitPrice;
		req.LimitPrice = LimitPrice;
		req.TimeCondition = THOST_FTDC_TC_IOC;
		req.VolumeCondition = THOST_FTDC_VC_MV;
		req.MinVolume = 1;//volume;//
	}
		break;
	default://默认下限价单 
	{
		req.OrderPriceType = THOST_FTDC_OPT_LimitPrice;
		req.LimitPrice = LimitPrice;
		req.TimeCondition = THOST_FTDC_TC_GFD;//THOST_FTDC_TC_IOC;//
		req.VolumeCondition = THOST_FTDC_VC_AV;//THOST_FTDC_VC_CV;//
		req.MinVolume = 1;//volume;//
	}
			 break;
	}
	
	req.VolumeTotalOriginal = volume;	///数量
	req.CombHedgeFlag[0] = THOST_FTDC_HF_Speculation;
	req.ContingentCondition = THOST_FTDC_CC_Immediately;
	req.Direction = (direction == LB1_Buy ? THOST_FTDC_D_Buy : THOST_FTDC_D_Sell);
	switch (offset)
	{
	case LB1_Increase:req.CombOffsetFlag[0] = THOST_FTDC_OF_Open;break;
	case LB1_Decrease:req.CombOffsetFlag[0] = THOST_FTDC_OF_Close;break;
	case LB1_DecreaseYesterday:req.CombOffsetFlag[0] = THOST_FTDC_OF_CloseYesterday;break;
	case LB1_DecreaseToday:req.CombOffsetFlag[0] = THOST_FTDC_OF_CloseToday;break;
	default:req.CombOffsetFlag[0] = '0';break;
	}
	//TThostFtdcPriceType	StopPrice;  //止损价
	req.ForceCloseReason = THOST_FTDC_FCC_NotForceClose;
	req.IsAutoSuspend = 0;
	req.UserForceClose = 0;
	int ReqOrderInsertResult = m_pUserApi->ReqOrderInsert(&req, ++m_uRequestID);
	if (0 != ReqOrderInsertResult)
	{
		ShowMessage(severity_levels::error, "[!][T]Send ReqOrderInsert Failed for %d.[Ref:]", ReqOrderInsertResult, Ref);
		return LB1_NullOrderRef;
	}

	return Ref;
}

TLastErrorIdType CCTP_FUTURE_TDPlugin::TDBasicCancelOrder(TOrderRefIdType Ref, unordered_map<string, string> & instrument, TOrderSysIdType orderSysId)
{
	if (false == m_boolIsOnline)
	{
		ShowMessage(severity_levels::error,
			"%s: cancel order error: not trading time.", GetCurrentKeyword().c_str());
		return LB1_INVALID_VAL;
	}
	CThostFtdcInputOrderActionField Req;
	memset(&Req, 0, sizeof(Req));
	Req.ActionFlag = THOST_FTDC_AF_Delete;
	Req.FrontID = m_intFrontID;
	Req.SessionID = m_intSessionID;
	snprintf(Req.OrderRef, sizeof(Req.OrderRef), "%.12ld", static_cast<long>(Ref));
	Req.OrderActionRef =  _OrderIncreasePart2OrderRefPart(++m_uIncreasePart) +
		_AccountNumberPart2OrderRefPart(m_uAccountNumber);
	if (Ref > MaxOrderRef)
	{
		ShowMessage(severity_levels::error,
			"%s: The OrderRef is run out of MaxRoderRef(%d) C.", GetCurrentKeyword().c_str(), MaxOrderRef);
		return LB1_INVALID_VAL;
	}
	strncpy(Req.BrokerID, m_strBrokerID.c_str(), sizeof(Req.BrokerID));
	strncpy(Req.ExchangeID, instrument["exchangeid"].c_str(), sizeof(Req.ExchangeID));
	strncpy(Req.InvestorID, m_strUsername.c_str(), sizeof(Req.InvestorID));
	strncpy(Req.OrderSysID, orderSysId, sizeof(Req.OrderSysID));


	if (0 != m_pUserApi->ReqOrderAction(&Req, ++m_uRequestID))
	{
		ShowMessage(severity_levels::error, "[!][T]Cancel Order %s Failed", orderSysId);
		return LB1_INVALID_VAL;
	}
	return LB1_NO_ERROR;
}

int CCTP_FUTURE_TDPlugin::TDGetRemainAmountOfCancelChances(const char * ins)
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


void CCTP_FUTURE_TDPlugin::OnFrontConnected()
{
	CThostFtdcReqUserLoginField req;
	memset(&req, 0, sizeof(req));
	strncpy(req.BrokerID, m_strBrokerID.c_str(), sizeof(req.BrokerID));
	strncpy(req.UserID, m_strUsername.c_str(), sizeof(req.UserID));
	strncpy(req.Password, m_strPassword.c_str(), sizeof(req.Password));

    strncpy(req.AppID, m_strAppID.c_str(), sizeof(req.AppID));
	strncpy(req.AuthCode, m_strAuthCode.c_str(), sizeof(req.AuthCode));
	strncpy(req.InvestorID, m_strInvestorID.c_str(), sizeof(req.InvestorID));
	strncpy(req.UserProductinfo, m_strUserProductInfo.c_str(), sizeof(req.UserProductInfo));





	if ((nullptr == m_pUserApi)||(0 != m_pUserApi->ReqUserLogin(&req, ++m_uRequestID)))
	{
		ShowMessage(
			severity_levels::error,
			"%s: Send ReqUserLogin Failed.",
			GetCurrentKeyword().c_str());
		NOTIFY_LOGIN_FAILED;
		
	};
}

void CCTP_FUTURE_TDPlugin::OnFrontDisconnected(int nReason)
{
	m_boolIsOnline = false;
	ShowMessage(severity_levels::error, "%s: FrontDisconnected for nReason %d",GetCurrentKeyword().c_str(),nReason);
}

void CCTP_FUTURE_TDPlugin::OnHeartBeatWarning(int nTimeLapse)
{
	ShowMessage(severity_levels::error, "%s heartbeatwarning %d", GetCurrentKeyword().c_str(), nTimeLapse);
}

void CCTP_FUTURE_TDPlugin::OnRspUserLogin(CThostFtdcRspUserLoginField * pRspUserLogin, CThostFtdcRspInfoField * pRspInfo, int nRequestID, bool bIsLast)
{
	if ((pRspInfo) && (pRspInfo->ErrorID != 0))
	{
		
		ShowMessage(
			severity_levels::error, 
			"%s: trade login failed.errorid=%d,errormsg=%s",
			GetCurrentKeyword().c_str(), 
			pRspInfo->ErrorID, 
			pRspInfo->ErrorMsg);
		NOTIFY_LOGIN_FAILED;
	}
	else
	{
		m_intFrontID = pRspUserLogin->FrontID;
		m_intSessionID = pRspUserLogin->SessionID;
		unsigned int _CTPMaxorderRef = atoi(pRspUserLogin->MaxOrderRef);

		m_uIncreasePart = _OrderRef2OrderIncreasePart(_CTPMaxorderRef)+1;
		CThostFtdcSettlementInfoConfirmField req;
		memset(&req, 0, sizeof(req));
		strncpy(req.BrokerID, m_strBrokerID.c_str(), sizeof(req.BrokerID));
		strncpy(req.InvestorID, m_strUsername.c_str(), sizeof(req.InvestorID));
		if (0 != m_pUserApi->ReqSettlementInfoConfirm(&req, ++m_uRequestID))
		{
			
			ShowMessage(severity_levels::error,
				"%s: send reqsettlementinfoconfirm failed!",
				GetCurrentKeyword().c_str()
				);
			NOTIFY_LOGIN_FAILED;
		}
	}
	
}

void CCTP_FUTURE_TDPlugin::OnRspSettlementInfoConfirm(CThostFtdcSettlementInfoConfirmField * pSettlementInfoConfirm, CThostFtdcRspInfoField * pRspInfo, int nRequestID, bool bIsLast)
{
	ShowMessage(
		severity_levels::normal,
		"AccountInfo: %s login succeed!",
		GetCurrentKeyword().c_str());
	NOTIFY_LOGIN_SUCCEED;
}

void CCTP_FUTURE_TDPlugin::OnRspUserLogout(CThostFtdcUserLogoutField * pUserLogout, CThostFtdcRspInfoField * pRspInfo, int nRequestID, bool bIsLast)
{
	ShowMessage(
		severity_levels::normal,
		"AccountInfo: %s loginout succeed!",
		GetCurrentKeyword().c_str());
	std::unique_lock<std::mutex> lk(m_mtxLogoutSignal);
	m_cvLogoutSignalCV.notify_all();
}

void CCTP_FUTURE_TDPlugin::OnRtnOrder(CThostFtdcOrderField * pOrder)
{
	if (nullptr == pOrder)
		return;
	if (THOST_FTDC_OST_Canceled == pOrder->OrderStatus)
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
	if (pOrder->SessionID != m_intSessionID)
		return;
	TOrderRefIdType Ref = atoi(pOrder->OrderRef);
	TOrderSysIdType Sys = pOrder->OrderSysID;
	TOrderStatusType Status = LB1_StatusUnknown;
	switch (pOrder->OrderStatus)
	{
	case THOST_FTDC_OST_AllTraded:				Status = LB1_StatusAllTraded;			break;
	case THOST_FTDC_OST_PartTradedQueueing:		Status = LB1_StatusPartTradedQueueing;	break;
	case THOST_FTDC_OST_PartTradedNotQueueing:	Status = LB1_StatusUnknown;				break;
	case THOST_FTDC_OST_NoTradeQueueing:		Status = LB1_StatusNoTradeQueueing;		break;
	case THOST_FTDC_OST_NoTradeNotQueueing:		Status = LB1_StatusUnknown;				break;
	case THOST_FTDC_OST_Canceled:				Status = LB1_StatusCanceled;			break;
	case THOST_FTDC_OST_Unknown:				Status = LB1_StatusUnknown;				break;
	case THOST_FTDC_OST_NotTouched:				Status = LB1_StatusUnknown;				break;
	case THOST_FTDC_OST_Touched:				Status = LB1_StatusUnknown;				break;
	default:									Status = LB1_StatusUnknown;				break;
	}
	TPriceType Price = pOrder->LimitPrice;
	TTradedVolumeType TraderVolume = pOrder->VolumeTraded;
	TRemainVolumeType RemainVolume = pOrder->VolumeTotal;

	m_pTradePluginContext->OnOrder(Ref, Sys, Status, Price, TraderVolume, RemainVolume);
}

void CCTP_FUTURE_TDPlugin::OnRtnTrade(CThostFtdcTradeField * pTrade)
{
	if (nullptr == pTrade)
		return;
	TOrderRefIdType Ref = atoi(pTrade->OrderRef);
	TOrderSysIdType Sys = pTrade->OrderSysID;
	TPriceType Price = pTrade->Price;
	TVolumeType Volume = pTrade->Volume;
	m_pTradePluginContext->OnTrade(Ref, Sys, Price, Volume);
}

void CCTP_FUTURE_TDPlugin::OnRspOrderInsert(CThostFtdcInputOrderField * pInputOrder, CThostFtdcRspInfoField * pRspInfo, int nRequestID, bool bIsLast)
{

	if (pRspInfo&&pRspInfo->ErrorID != 0)
	{
		unsigned int OrderRef = nullptr==pInputOrder?0:atoi(pInputOrder->OrderRef);

		ShowMessage(
			severity_levels::error, 
			"%s: onrsporderinsert:errorid=%d errormsg=%s orderref=%u.",
			GetCurrentKeyword().c_str(),
			pRspInfo->ErrorID,
			pRspInfo->ErrorMsg,
			OrderRef);
	}
}

void CCTP_FUTURE_TDPlugin::OnRspOrderAction(CThostFtdcInputOrderActionField * pInputOrderAction, CThostFtdcRspInfoField * pRspInfo, int nRequestID, bool bIsLast)
{
	if (nullptr == pInputOrderAction)
		return;
	if (pInputOrderAction->SessionID != m_intSessionID)
		return;
	if (pRspInfo&&pRspInfo->ErrorID != 0)
	{
		unsigned int OrderRef = atoi(pInputOrderAction->OrderRef);
		ShowMessage(
			severity_levels::error,
			"%s: onrsporderaction:errorid=%d errormsg=%s orderref=%u",
			GetCurrentKeyword().c_str(),
			pRspInfo->ErrorID,
			pRspInfo->ErrorMsg,
			OrderRef);
	}
}

void CCTP_FUTURE_TDPlugin::OnRspError(CThostFtdcRspInfoField * pRspInfo, int nRequestID, bool bIsLast)
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

void CCTP_FUTURE_TDPlugin::OnErrRtnOrderInsert(CThostFtdcInputOrderField * pInputOrder, CThostFtdcRspInfoField * pRspInfo)
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

void CCTP_FUTURE_TDPlugin::OnErrRtnOrderAction(CThostFtdcOrderActionField * pOrderAction, CThostFtdcRspInfoField * pRspInfo)
{
	if (pOrderAction->SessionID != m_intSessionID)
		return;
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


