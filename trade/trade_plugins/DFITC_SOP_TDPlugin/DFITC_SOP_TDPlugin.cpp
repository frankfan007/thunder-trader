#include "DFITC_SOP_TDPlugin.h"
#include <stdarg.h>
#include <thread>
#include "OrderRefResolve.h"
#include "AutoPend.h"
#include <numeric>
#include <limits>
extern char ProcessName[256];
#define NOTIFY_LOGIN_SUCCEED {m_boolIsOnline = true; std::unique_lock<std::mutex> lk(m_mtxLoginSignal);m_cvLoginSignalCV.notify_all();}
#define NOTIFY_LOGIN_FAILED  {m_boolIsOnline = false;std::unique_lock<std::mutex> lk(m_mtxLoginSignal);m_cvLoginSignalCV.notify_all();}

const TOrderRefIdType MaxOrderRef = numeric_limits<DFITCSECLocalOrderIDType>::max(); //(PowerOfTwo<8*sizeof(DFITCSOPReqEntrustOrderField::localOrderID) - 2>::val - 1);

const string CDFITC_SOP_TDPlugin::s_strAccountKeyword = "serveraddress;accountid;password;maxcancelperday;";

date CDFITC_SOP_TDPlugin::GetTradeday(ptime _Current)
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

CDFITC_SOP_TDPlugin::CDFITC_SOP_TDPlugin():
	m_StartAndStopCtrlTimer(m_IOservice), m_abIsPending(false)
{

}

CDFITC_SOP_TDPlugin::~CDFITC_SOP_TDPlugin()
{

}

bool CDFITC_SOP_TDPlugin::IsPedding()
{
	return m_abIsPending.load();
}

bool CDFITC_SOP_TDPlugin::IsOnline()
{
	return m_boolIsOnline;
}

void CDFITC_SOP_TDPlugin::IncreaseRefCount()
{
	m_intRefCount++;
}

void CDFITC_SOP_TDPlugin::DescreaseRefCount()
{
	m_intRefCount--;
}

int CDFITC_SOP_TDPlugin::GetRefCount()
{
	return m_intRefCount;
}

void CDFITC_SOP_TDPlugin::CheckSymbolValidity(const unordered_map<string, string> & insConfig)
{
	auto Type = insConfig.find("type");
	if (Type == insConfig.end())
		throw std::runtime_error("can not find the <type> of the the symbol.");
	if (Type->second != "option")
		throw std::runtime_error("dfitc_sop marketdatasource does not support this symbol.");
	auto InstrumentNode = insConfig.find("instrumentid");
	if (InstrumentNode == insConfig.end())
		throw std::runtime_error("<instrumentid> not found.");
	auto ExchangeIDNode = insConfig.find("exchangeid");
	if (ExchangeIDNode == insConfig.end())
		throw std::runtime_error("<exchangeid> not found.");
}

string CDFITC_SOP_TDPlugin::GetCurrentKeyword()
{
	return "dfitc_sop_td&"+ m_strAccountid;
}

string CDFITC_SOP_TDPlugin::GetProspectiveKeyword(const ptree & in)
{
	string retKey = "dfitc_sop_td&";
	auto temp = in.find("accountid");
	if (temp != in.not_found())
	{
		
		if (temp->second.data().size()>(sizeof(DFITCSECReqUserLoginField::accountID) - 1))
			throw std::runtime_error("ctp:accountid is too long");
		retKey += temp->second.data();
	}
	else
		throw std::runtime_error("ctp:can not find <accountid>");
	return retKey;
}

void CDFITC_SOP_TDPlugin::GetState(ptree & out)
{
	if (m_boolIsOnline)
		out.put("online", "true");
	else
		out.put("online", "false");
	out.put("serveraddress", m_strServerAddress);
	out.put("accountid", m_strAccountid);
	ptree CancelAmount;
	{
		boost::shared_lock<boost::shared_mutex> lg(m_mtxProtectCancelAmount);
		for (auto & p : m_mapCancelAmount)
			CancelAmount.put(p.first, m_intInitAmountOfCancelChancesPerDay - p.second);
	}
	out.put_child("cancelamount", CancelAmount);
}

void CDFITC_SOP_TDPlugin::UpdateAccountInfo(const ptree & in)
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


	temp = in.find("accountid");
	if (temp != in.not_found())
	{
		m_strAccountid = temp->second.data();
		if (m_strAccountid.size()>(sizeof(DFITCSECReqUserLoginField::accountID) - 1))
			throw std::runtime_error("ctp:accountid is too long");
		else if (m_strAccountid.empty())
			throw std::runtime_error("ctp:accountid is empty");
	}
	else
		throw std::runtime_error("ctp:Can not find <accountid>");

	temp = in.find("password");
	if (temp != in.not_found())
	{
		m_strPassword = temp->second.data();
		if (m_strPassword.size()>(sizeof(DFITCSECReqUserLoginField::password) - 1))
			throw std::runtime_error("ctp:password is too long");
		else if (m_strPassword.empty())
			throw std::runtime_error("ctp:password is empty");
	}
	else
		throw std::runtime_error("ctp:Can not find <password>");
}

void CDFITC_SOP_TDPlugin::TDInit(const ptree & in, MTradePluginContextInterface * pTradePluginContext, unsigned int AccountNumber)
{
	m_uAccountNumber = AccountNumber;
	m_pTradePluginContext = pTradePluginContext;
	
	UpdateAccountInfo(in);
	auto MaxCancelPerDayNode = in.find("maxcancelperday");
	if (MaxCancelPerDayNode != in.not_found())
		m_intInitAmountOfCancelChancesPerDay = atoi(MaxCancelPerDayNode->second.data().c_str());
	else
		m_intInitAmountOfCancelChancesPerDay = std::numeric_limits<int>::max();;

	m_dateTradeDay = GetTradeday(second_clock::universal_time());
	map<string, int>().swap(m_mapCancelAmount);
	m_StartAndStopCtrlTimer.expires_from_now(boost::posix_time::seconds(3));
	m_StartAndStopCtrlTimer.async_wait(boost::bind(&CDFITC_SOP_TDPlugin::TimerHandler, this,
		&m_StartAndStopCtrlTimer,
		boost::asio::placeholders::error));
	m_futTimerThreadFuture = std::async(std::launch::async,[this] {
		this->m_IOservice.run();
		return true;
	});


}

void CDFITC_SOP_TDPlugin::TDHotUpdate(const ptree & in)
{
	UpdateAccountInfo(in);
	if (m_boolIsOnline)
	{
		Stop();
		Start();
	}
}

void CDFITC_SOP_TDPlugin::TimerHandler(boost::asio::deadline_timer* timer, const boost::system::error_code& err)
{
	if (err)
	{
		ShowMessage(normal, "%s: Timmer is canceled.%s", GetCurrentKeyword().c_str(), err.message().c_str());
	}
	else
	{
		time_duration tid = second_clock::universal_time().time_of_day();
		ptime nextActiveTime = not_a_date_time;
		if (tid >= time_duration(0, 0, 0, 0) && tid < time_duration(1, 15, 0, 0))
		{
			nextActiveTime = ptime(second_clock::universal_time().date(), time_duration(1, 15, 30, 0));
		}
		else if (tid >= time_duration(1, 15, 0, 0) && tid < time_duration(7, 0, 0, 0))
		{
			Start();
			nextActiveTime = ptime(second_clock::universal_time().date(), time_duration(7, 0, 30, 0));
		}
		else if (tid >= time_duration(7, 0, 0, 0) && tid < time_duration(23, 59, 59, 999))
		{
			Stop();
			nextActiveTime = ptime(second_clock::universal_time().date() + days(1), time_duration(1, 15, 30, 0));
		}
		m_StartAndStopCtrlTimer.expires_at(nextActiveTime);
		m_StartAndStopCtrlTimer.async_wait(boost::bind(&CDFITC_SOP_TDPlugin::TimerHandler, this,
			&m_StartAndStopCtrlTimer,
			boost::asio::placeholders::error));
		ShowMessage(normal, "%s: Next:%s", GetCurrentKeyword().c_str(), to_simple_string(nextActiveTime).c_str());
	}
}

bool CDFITC_SOP_TDPlugin::Start()
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
		m_pUserApi = DFITCSECTraderApi::CreateDFITCSECTraderApi();
		if (nullptr == m_pUserApi)
		{
			ShowMessage(
				severity_levels::error,
				"%s CreateDFITCSECTraderApi error",
				GetCurrentKeyword().c_str());
			return false;
		}
		char ServerAddress[65];
		strcpy(ServerAddress, m_strServerAddress.c_str());
		m_pUserApi->Init(ServerAddress, this);
		std::unique_lock<std::mutex> lk(m_mtxLoginSignal);
		m_cvLoginSignalCV.wait_for(lk, std::chrono::seconds(10));
		if (m_boolIsOnline)
			return true;
		else
		{
			m_pUserApi->Release();
			m_pUserApi = nullptr;
			m_boolIsOnline = false;
			return false;
		}
	}
	else return true;
}

bool CDFITC_SOP_TDPlugin::Stop()
{
	CAutoPend pend(m_abIsPending);
	if (true==m_boolIsOnline)
	{
		DFITCSECReqUserLogoutField req;
		memset(&req, 0, sizeof(req));
		req.requestID = ++m_uRequestID;
		strncpy(req.accountID, m_strAccountid.c_str(), sizeof(req.accountID));              //客户号(Y)
		auto res = m_pUserApi->ReqSOPUserLogout(&req);
		if (0 != res)
			ShowMessage(severity_levels::error, "%s:  ReqSOPUserLogout Failed.", GetCurrentKeyword().c_str());
		else
		{
			ShowMessage(severity_levels::error, "%s:  ReqSOPUserLogout Succeed.", GetCurrentKeyword().c_str());
			std::unique_lock<std::mutex> lk(m_mtxLogoutSignal);
			m_cvLogoutSignalCV.wait_for(lk, std::chrono::seconds(10));
		}
	}
	if (m_pUserApi)
	{
		m_pUserApi->Release();
		m_pUserApi = nullptr;
	}
	m_boolIsOnline=false;
	return true;

}

void CDFITC_SOP_TDPlugin::TDUnload()
{
	
	Stop();
	m_StartAndStopCtrlTimer.cancel();
	m_IOservice.stop();
	m_futTimerThreadFuture.get();
}

void CDFITC_SOP_TDPlugin::ShowMessage(severity_levels lv, const char * fmt, ...)
{
	char buf[512];
	va_list arg;
	va_start(arg, fmt);
	vsnprintf(buf, 512, fmt, arg);
	va_end(arg);
	boost::log::sources::severity_logger< severity_levels > m_Logger;
	BOOST_LOG_SEV(m_Logger, lv) << ProcessName << ": " << buf << " [" << to_iso_string(microsec_clock::universal_time()) << "]";
}

TOrderRefIdType CDFITC_SOP_TDPlugin::TDBasicMakeOrder(
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
			"%s: limitorder error: not trading time.", GetCurrentKeyword().c_str());
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
	DFITCSOPReqEntrustOrderField req;
	memset(&req, 0, sizeof(req));
	req.requestID = ++m_uRequestID;                //请求ID(Y)
	req.localOrderID = static_cast<DFITCSECLocalOrderIDType>(Ref);             //本地委托号(Y)
	strncpy(req.accountID, m_strAccountid.c_str(), sizeof(req.accountID));                //客户号(Y)
	strncpy(req.exchangeID, instrument["exchangeid"].c_str(), sizeof(req.exchangeID));               //交易所代码(Y)
	strncpy(req.securityID, instrument["instrumentid"].c_str(), sizeof(req.securityID));              //证劵代码(Y)
																								  //req.subAccountID;             //子账户编码(N)预留字段
	req.entrustQty = volume;               //委托数量(Y)
	req.entrustPrice = LimitPrice;             //委托价格(N)
	req.entrustDirection = ((LB1_Buy == direction) ? (DFITCSEC_ED_Buy) : (DFITCSEC_ED_Sell));         //委托类别(Y)
	req.openCloseFlag = ((LB1_Increase == offset) ? (DFITCSEC_OCF_Open) : (DFITCSEC_OCF_Close));            //开平标志(Y)
	req.coveredFlag = DFITCSEC_CF_UnCovered;              //备兑类型(Y)
	req.orderType = 1;//DFITCSEC_OT_LimitPrice;                //订单类型(Y)
	req.orderExpiryDate = DFITCSEC_OE_NONE;          //订单时效限制(N)
	req.orderCategory = DFITCSEC_OC_GeneralOrder;            //委托单类别(Y)
															 //req.serialID;                 //扩展流水号(N)预留字段
															 //req.devID;                    //厂商ID(N)
															 //req.devDecInfo;               //用户自定义字段(N)
	int ReqOrderInsertResult = m_pUserApi->ReqSOPEntrustOrder(&req);
	if (0 != ReqOrderInsertResult)
	{
		ShowMessage(severity_levels::error, "[!][T]Send ReqOrderInsert Failed for %d.[Ref:]", ReqOrderInsertResult, Ref);
		return LB1_NullOrderRef;
	}

	return Ref;
}

TLastErrorIdType CDFITC_SOP_TDPlugin::TDBasicCancelOrder(TOrderRefIdType Ref, unordered_map<string, string> & instrument, TOrderSysIdType orderSysId)
{
	if (false == m_boolIsOnline)
	{
		ShowMessage(severity_levels::error,
			"%s: cancel order error: not trading time.", GetCurrentKeyword().c_str());
		return LB1_INVALID_VAL;
	}

	DFITCSECReqWithdrawOrderField Req;
	memset(&Req, 0, sizeof(Req));
	Req.requestID = ++m_uRequestID;                      //请求ID(Y)
	strncpy(Req.accountID, m_strAccountid.c_str(), sizeof(Req.accountID));
	Req.localOrderID = static_cast<DFITCSECLocalOrderIDType>(Ref);
	if (0 != m_pUserApi->ReqSOPWithdrawOrder(&Req))
	{
		ShowMessage(severity_levels::error, "[!][T]Cancel Order %s Failed", orderSysId);
		return LB1_INVALID_VAL;
	}
	return LB1_NO_ERROR;
}

int CDFITC_SOP_TDPlugin::TDGetRemainAmountOfCancelChances(const char * ins)
{
	return (std::numeric_limits<int>::max)();
}


void CDFITC_SOP_TDPlugin::OnFrontConnected()
{
	DFITCSECReqUserLoginField req;
	memset(&req, 0, sizeof(req));
	strncpy(req.accountID, m_strAccountid.c_str(), sizeof(req.accountID));
	strncpy(req.password, m_strPassword.c_str(), sizeof(req.password));
	req.requestID = ++m_uRequestID;
	if ((nullptr == m_pUserApi)||(0 != m_pUserApi->ReqSOPUserLogin(&req)))
	{
		ShowMessage(
			severity_levels::error, 
			"%s: Send ReqSOPUserLogin Failed.",
			GetCurrentKeyword().c_str());
		NOTIFY_LOGIN_FAILED;
	}
}

void CDFITC_SOP_TDPlugin::OnFrontDisconnected(int nReason)
{
	m_boolIsOnline = false;
	ShowMessage(severity_levels::error, "%s: FrontDisconnected for nReason %d",GetCurrentKeyword().c_str(),nReason);
}

void CDFITC_SOP_TDPlugin::OnRspSOPUserLogin(DFITCSECRspUserLoginField *pRspUserLogin, DFITCSECRspInfoField *pRspInfo)
{
	if ((pRspInfo) && (pRspInfo->errorID != 0))
	{
		
		ShowMessage(
			severity_levels::error, 
			"AccountInfo: %s login failed.errorid=%d,errormsg=%s",
			GetCurrentKeyword().c_str(), 
			pRspInfo->errorID,
			pRspInfo->errorMsg);
		NOTIFY_LOGIN_FAILED;
	}
	else
	{
		ShowMessage(
			severity_levels::normal,
			"AccountInfo: %s login succeed!",
			GetCurrentKeyword().c_str());
		m_intFrontID = pRspUserLogin->frontID;
		m_intSessionID = pRspUserLogin->sessionID;

		m_uIncreasePart = _OrderRef2OrderIncreasePart(pRspUserLogin->localOrderID)+1;
		NOTIFY_LOGIN_SUCCEED
	}
	
}

void CDFITC_SOP_TDPlugin::OnRspSOPUserLogout(DFITCSECRspUserLogoutField *pData, DFITCSECRspInfoField *pRspInfo)
{
	ShowMessage(
		severity_levels::normal,
		"AccountInfo: %s loginout succeed!",
		GetCurrentKeyword().c_str());
	std::unique_lock<std::mutex> lk(m_mtxLogoutSignal);
	m_cvLogoutSignalCV.notify_all();
}

void CDFITC_SOP_TDPlugin::OnSOPTradeRtn(DFITCSOPTradeRtnField * pTrade)
{
	if (nullptr == pTrade)
		return;
	if (pTrade->sessionID != m_intSessionID)
		return;
	if (
		DFITCSEC_DR_PartTrade == pTrade->declareResult ///部分成交
		||
		DFITCSEC_DR_TotalTrade == pTrade->declareResult ///全部成交
		||
		DFITCSEC_DR_TradeAWithdraw == pTrade->declareResult ///部成部撤
		)
	{
		TOrderRefIdType Ref = pTrade->localOrderID;
		char sysid[64];
		snprintf(sysid, 64, "%llu", Ref);
		TOrderSysIdType Sys = sysid;
		TPriceType Price = pTrade->tradePrice;
		TVolumeType Volume = pTrade->tradeQty;
		m_pTradePluginContext->OnTrade(Ref, Sys, Price, Volume);
	}
	const char * type = m_mapDeclareResult[pTrade->declareResult];
	if(type)
	ShowMessage(severity_levels::normal, 
		"%s: OnSOPTradeRtn:%s.",
		GetCurrentKeyword().c_str(),
		type);
}

void CDFITC_SOP_TDPlugin::OnRspSOPWithdrawOrder(DFITCSECRspWithdrawOrderField * pData, DFITCSECRspInfoField * pRspInfo)
{
	if (nullptr == pData)
		return;
	if (pRspInfo&&pRspInfo->errorID != 0)
	{
		unsigned int OrderRef = pData->localOrderID;
			
		ShowMessage(
			severity_levels::error,
			"%s: onrsporderaction:errorid=%d errormsg=%s orderref=%u",
			GetCurrentKeyword().c_str(),
			pRspInfo->errorID,
			pRspInfo->errorMsg,
			OrderRef);
	}
}

void CDFITC_SOP_TDPlugin::OnSOPWithdrawOrderRtn(DFITCSOPWithdrawOrderRtnField * pData)
{
	if (nullptr == pData)
		return;
	if (pData->sessionID != m_intSessionID)
		return;
	if (
		DFITCSEC_DR_TotalWithdraw == pData->declareResult ///全部撤单
		||
		DFITCSEC_DR_TradeAWithdraw == pData->declareResult ///部成部撤
		||
		DFITCSEC_DR_UnDeclare == pData->declareResult ///错误(废单错误) 
		)
	{
		
		{
			boost::unique_lock<boost::shared_mutex> lg(m_mtxProtectCancelAmount);
			if (m_dateTradeDay != GetTradeday(second_clock::universal_time()))
			{
				m_dateTradeDay = GetTradeday(second_clock::universal_time());
				m_mapCancelAmount.clear();
				map<string, int>().swap(m_mapCancelAmount);
			}
			m_mapCancelAmount[string(pData->exchangeID) + string(pData->securityID)]++;
		}

		TOrderRefIdType Ref = pData->localOrderID;
		char sysid[64];
		snprintf(sysid, 64, "%llu", Ref);
		TOrderSysIdType Sys = sysid;
		TOrderStatusType Status = LB1_StatusCanceled;
		TPriceType Price = 0;
		TTradedVolumeType TraderVolume = pData->tradeQty;
		TRemainVolumeType RemainVolume = pData->withdrawQty;
		m_pTradePluginContext->OnOrder(Ref, Sys, Status, Price, TraderVolume, RemainVolume);
	}
	const char * type = m_mapDeclareResult[pData->declareResult];
	if (type)
		ShowMessage(severity_levels::normal,
			"%s: OnSOPWithdrawOrderRtn:%s.",
			GetCurrentKeyword().c_str(),
			type);
}

void CDFITC_SOP_TDPlugin::OnRspSOPEntrustOrder(DFITCSOPRspEntrustOrderField *pData, DFITCSECRspInfoField *pRspInfo)
{

	if (pRspInfo&&pRspInfo->errorID != 0)
	{
		unsigned int OrderRef = (nullptr== pData ?0:pData->localOrderID);

		ShowMessage(
			severity_levels::error, 
			"%s: onrsporderinsert:errorid=%d errormsg=%s orderref=%u.",
			GetCurrentKeyword().c_str(),
			pRspInfo->errorID,
			pRspInfo->errorMsg,
			OrderRef);
	}
}

void CDFITC_SOP_TDPlugin::OnRspError(DFITCSECRspInfoField *pRspInfo)
{
	if (pRspInfo&&pRspInfo->errorID != 0)
	{
		ShowMessage(
			severity_levels::error,
			"%s: onrsperror:errorid=%d errormsg=%s.",
			GetCurrentKeyword().c_str(),
			pRspInfo->errorID,
			pRspInfo->errorMsg);
	}
}

