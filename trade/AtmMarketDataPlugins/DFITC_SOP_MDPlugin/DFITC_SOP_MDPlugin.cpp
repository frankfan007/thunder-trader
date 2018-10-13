#include "DFITC_SOP_MDPlugin.h"
#include <stdarg.h>
#include <thread>
#include "AutoPend.h"
#define NAME "dfitc_sop_md&"
const string CDFITC_SOP_MDPlugin::s_strAccountKeyword= "serveraddress;accountid;password;";
extern char ProcessName[256];
CDFITC_SOP_MDPlugin::CDFITC_SOP_MDPlugin():m_StartAndStopCtrlTimer(m_IOservice), m_abIsPending(false), m_adbIsPauseed(false)
{
}

CDFITC_SOP_MDPlugin::~CDFITC_SOP_MDPlugin()
{
}

bool CDFITC_SOP_MDPlugin::IsPedding()
{
	return m_abIsPending.load();
}

bool CDFITC_SOP_MDPlugin::IsOnline()
{
	return m_boolIsOnline;
}

void CDFITC_SOP_MDPlugin::IncreaseRefCount()
{
	m_intRefCount++;
}

void CDFITC_SOP_MDPlugin::DescreaseRefCount()
{
	m_intRefCount--;
}

int CDFITC_SOP_MDPlugin::GetRefCount()
{
	return m_intRefCount;
}

void CDFITC_SOP_MDPlugin::CheckSymbolValidity(const unordered_map<string, string> & insConfig)
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

string CDFITC_SOP_MDPlugin::GetCurrentKeyword()
{
	return NAME + m_strAccountID;
}

string CDFITC_SOP_MDPlugin::GetProspectiveKeyword(const ptree & in)
{
	string retKey = NAME;
	auto temp = in.find("accountid");
	if (temp != in.not_found())
	{

		if (temp->second.data().size()>(sizeof(DFITCSECReqUserLoginField::accountID) - 1))
			throw std::runtime_error("dfitc:accountid is too long");
		retKey += temp->second.data();
	}
	else
		throw std::runtime_error("dfitc:Can not find <accountid>");

	return retKey;
}

void CDFITC_SOP_MDPlugin::GetState(ptree & out)
{
	if(m_boolIsOnline)
		out.put("online","true");
	else
		out.put("online", "false");
	out.put("serveraddress", m_strServerAddress);
	out.put("accountid", m_strAccountID);
}

void CDFITC_SOP_MDPlugin::MDInit(const ptree & in)
{
	auto temp = in.find("serveraddress");
	if (temp != in.not_found())
	{
		
		m_strServerAddress = temp->second.data();
		if (m_strServerAddress.size()>64)
			throw std::runtime_error("dfitc:serveraddress is too long(Max:64 char)");
		else if(m_strServerAddress.empty())
			throw std::runtime_error("dfitc:serveraddress is empty");
	}
	else
		throw std::runtime_error("dfitc:can not find <serveraddress>");


	temp = in.find("accountid");
	if (temp != in.not_found())
	{
		m_strAccountID = temp->second.data();
		if(m_strAccountID.size()>(sizeof(DFITCSECReqUserLoginField::accountID)-1))
			throw std::runtime_error("dfitc:accountid is too long");
		else if (m_strAccountID.empty())
			throw std::runtime_error("dfitc:accountid is empty");
	}
	else
		throw std::runtime_error("dfitc:can not find <accountid>");


	temp = in.find("password");
	if (temp != in.not_found())
	{
		m_strPassword = temp->second.data();
		if(m_strPassword.size()>(sizeof(DFITCSECReqUserLoginField::password)-1))
			throw std::runtime_error("dfitc:password is too long");
		else if (m_strPassword.empty())
			throw std::runtime_error("dfitc:password is empty");
	}
	else
		throw std::runtime_error("dfitc:Can not find <password>");

	m_StartAndStopCtrlTimer.expires_from_now(boost::posix_time::seconds(3));
	m_StartAndStopCtrlTimer.async_wait(boost::bind(
		&CDFITC_SOP_MDPlugin::TimerHandler,
		this,
		&m_StartAndStopCtrlTimer,
		boost::asio::placeholders::error));
	m_futTimerThreadFuture=std::async(std::launch::async,[this] {
		this->m_IOservice.run();
		return true;
	});

}

void CDFITC_SOP_MDPlugin::MDHotUpdate(const ptree & NewConfig)
{
	MDUnload();
	m_IOservice.reset();
	MDInit(NewConfig);
}

void CDFITC_SOP_MDPlugin::TimerHandler(boost::asio::deadline_timer* timer, const boost::system::error_code& err)
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
		m_StartAndStopCtrlTimer.async_wait(boost::bind(
			&CDFITC_SOP_MDPlugin::TimerHandler,
			this,
			&m_StartAndStopCtrlTimer,
			boost::asio::placeholders::error));
		ShowMessage(normal, "%s: Next:%s", GetCurrentKeyword().c_str(), to_simple_string(nextActiveTime).c_str());
	}
}

bool CDFITC_SOP_MDPlugin::Start()
{
	CAutoPend pend(m_abIsPending);
	if (false == m_boolIsOnline)
	{
		m_uRequestID = 0;
		m_boolIsOnline = false;
		m_pUserApi = std::shared_ptr<DFITCSECMdApi>(DFITCSECMdApi::CreateDFITCMdApi(), [](DFITCSECMdApi* p) {if (p) p->Release();});
		if (nullptr == m_pUserApi)
		{
			ShowMessage(
				severity_levels::error,
				"%s CreateDFITCMdApi error",
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
			m_pUserApi.reset();
			m_boolIsOnline = false;
			return false;
		}
	}
	else return true;
}

void CDFITC_SOP_MDPlugin::Stop()
{
	CAutoPend pend(m_abIsPending);
	if (true==m_boolIsOnline)
	{
		boost::shared_lock<boost::shared_mutex> lg(m_mapObserverStructProtector);//Ð´Ëø
		unsigned int InstrumentCount = m_mapInsid2Strategys.size();
		typedef char * NAMETYPE;
		char * * ppInstrumentID = new NAMETYPE[InstrumentCount];
		auto pos = m_mapInsid2Strategys.begin();
		for (unsigned int i = 0;i < InstrumentCount;++i, ++pos)
		{
			ppInstrumentID[i] = new char[31];//TThostFtdcInstrumentIDType
			strncpy(ppInstrumentID[i], pos->first.c_str(), 31);
		}
		if (0 != m_pUserApi->UnSubscribeSOPMarketData(ppInstrumentID, InstrumentCount, ++m_uRequestID))
			ShowMessage(severity_levels::error, "send unsubscribemarketdata failed.");
		for (auto i = 0;i < InstrumentCount;i++)
			delete[] ppInstrumentID[i];
		delete[] ppInstrumentID;

		DFITCSECReqUserLogoutField req;
		memset(&req, 0, sizeof(req));
		req.requestID = ++m_uRequestID;
		strncpy(req.accountID, m_strAccountID.c_str(), sizeof(req.accountID));

		int res = m_pUserApi->ReqSOPUserLogout(&req);
		if (0 != res)
			ShowMessage(severity_levels::error, "%s:  requserlogout failed.", GetCurrentKeyword().c_str());
		else
		{
			ShowMessage(severity_levels::error, "%s:  requserlogout Succeed.", GetCurrentKeyword().c_str());
			std::unique_lock<std::mutex> lk(m_mtxLogoutSignal);
			m_cvLogoutSignalCV.wait_for(lk, std::chrono::seconds(10));
		}
	}
	if (m_pUserApi)
	{
		m_pUserApi = nullptr;
	}
	m_boolIsOnline = false;
}

void CDFITC_SOP_MDPlugin::MDUnload()
{
	
	Stop();
	m_StartAndStopCtrlTimer.cancel();
	m_IOservice.stop();
	m_futTimerThreadFuture.get();
}

void CDFITC_SOP_MDPlugin::Pause()
{
	m_adbIsPauseed.store(true);
}

void CDFITC_SOP_MDPlugin::Continue()
{
	m_adbIsPauseed.store(false);
}

void CDFITC_SOP_MDPlugin::ShowMessage(severity_levels lv, const char * fmt, ...)
{
	char buf[512];
	va_list arg;
	va_start(arg, fmt);
	vsnprintf(buf, 512, fmt, arg);
	va_end(arg);
	boost::log::sources::severity_logger< severity_levels > m_Logger;
	BOOST_LOG_SEV(m_Logger, lv) << ProcessName << ": " << buf << " [" << to_iso_string(microsec_clock::universal_time()) << "]";
}

void CDFITC_SOP_MDPlugin::MDAttachStrategy(
	MStrategy * strategy,
	TMarketDataIdType dataid,
	const unordered_map<string, string> & insConfig,
	boost::shared_mutex & mtx,
	atomic_uint_least64_t * updatetime)
{
	boost::unique_lock<boost::shared_mutex> lg(m_mapObserverStructProtector);//Ð´Ëø

	auto ExchangeID = insConfig.find("exchangeid")->second;
	auto SecurityID = insConfig.find("instrumentid")->second;
	auto InstrumentID = ExchangeID + SecurityID;

	auto findres = m_mapInsid2Strategys.find(InstrumentID);
	m_mapStrategy2Insids[strategy].push_back(InstrumentID);
	if (findres != m_mapInsid2Strategys.end())
		findres->second.second.push_back(make_tuple(strategy, dataid, &mtx, updatetime));
	else
	{
		m_mapInsid2Strategys[InstrumentID].second.push_back(make_tuple(strategy, dataid, &mtx, updatetime));
		auto & tick = m_mapInsid2Strategys[InstrumentID].first;
		#pragma region ³õÊ¼»¯tick
		memset(tick.m_strInstrumentID, 0, sizeof(TInstrumentIDType));
		tick.m_datetimeUTCDateTime = not_a_date_time;
		tick.m_dbLastPrice = 0;
		tick.m_intVolume = 0;
		for (unsigned int i = 0;i < MAX_QUOTATIONS_DEPTH;i++)
		{
			tick.m_dbBidPrice[i] = 0.0;
			tick.m_intBidVolume[i] = 0;
			tick.m_dbAskPrice[i] = 0.0;
			tick.m_intAskVolume[i] = 0;
		}


		tick.m_datetimeTradingDateTime=not_a_date_time;
		tick.m_dbOpenPrice = 0;
		tick.m_dbHighestPrice = 0;
		tick.m_dbLowestPrice = 0;
		tick.m_dbClosePrice = 0;
		#pragma endregion
		if (m_boolIsOnline)
		{
			typedef char * NAMETYPE;
			char * * ppInstrumentID = new NAMETYPE[1];
			ppInstrumentID[0] = new char[31];//TThostFtdcInstrumentIDType
			strncpy(ppInstrumentID[0], InstrumentID.c_str(), 31);
			if (0 != m_pUserApi->SubscribeSOPMarketData(ppInstrumentID, 1,++m_uRequestID))
				ShowMessage(
					severity_levels::error,
					"send subscribemarketdata(%s) failed.", 
					InstrumentID.c_str());
			else
				ShowMessage(
					severity_levels::normal,
					"trying to subscribe %s",
					InstrumentID.c_str());
			delete[] ppInstrumentID[0];
			delete[] ppInstrumentID;
		}
	}
	

}

void CDFITC_SOP_MDPlugin::MDDetachStrategy(MStrategy * strategy)
{
	boost::unique_lock<boost::shared_mutex> lg(m_mapObserverStructProtector);//Ð´Ëø

	for (auto & ins : m_mapStrategy2Insids[strategy])
	{
		for (auto pos = m_mapInsid2Strategys[ins].second.begin();pos != m_mapInsid2Strategys[ins].second.end();)
		{
			if (get<0>(*pos) == strategy)
				m_mapInsid2Strategys[ins].second.erase(pos++);
			else
				pos++;
		}
		if (m_mapInsid2Strategys[ins].second.empty())
		{
			m_mapInsid2Strategys.erase(ins);
			if (m_boolIsOnline)
			{
				typedef char * NAMETYPE;
				char * * ppInstrumentID = new NAMETYPE[1];
				ppInstrumentID[0] = new char[31];//TThostFtdcInstrumentIDType
				strncpy(ppInstrumentID[0], ins.c_str(), 31);
				if (0 != m_pUserApi->UnSubscribeSOPMarketData(ppInstrumentID, 1, ++m_uRequestID))
					ShowMessage(
						severity_levels::error,
						"%s: send unsubscribemarketdata(%s) failed.",
						GetCurrentKeyword().c_str(),
						ins.c_str());
				else
					ShowMessage(
						severity_levels::normal,
						"trying to unsubscribe %s",
						ins.c_str());
				delete[] ppInstrumentID[0];
				delete[] ppInstrumentID;
				
			}
		}
	}
	m_mapStrategy2Insids.erase(strategy);

}

#pragma region CThostFtdcMdSpi

void CDFITC_SOP_MDPlugin::OnFrontConnected()
{
	DFITCSECReqUserLoginField req;
	memset(&req, 0, sizeof(req));
	req.requestID = ++m_uRequestID;
	strncpy(req.accountID, m_strAccountID.c_str(), sizeof(req.accountID));
	strncpy(req.password, m_strPassword.c_str(), sizeof(req.password));
	if((nullptr == m_pUserApi)||(0 != m_pUserApi->ReqSOPUserLogin(&req)))
	{
		ShowMessage(
			severity_levels::error,
			"%s: send requserlogin failed.",
			GetCurrentKeyword().c_str());
		std::unique_lock<std::mutex> lk(m_mtxLoginSignal);
		m_cvLoginSignalCV.notify_all();
	}
}

void CDFITC_SOP_MDPlugin::OnFrontDisconnected(int nReason)
{
	ShowMessage(
		severity_levels::error, 
		"%s: OnFrontDisconnected(%d)",
		GetCurrentKeyword().c_str(),
		nReason);
}

void CDFITC_SOP_MDPlugin::OnRspSOPUserLogin(struct DFITCSECRspUserLoginField * pRspUserLogin, struct DFITCSECRspInfoField * pRspInfo)
{
	if ((pRspInfo) && (pRspInfo->errorMsg != 0))
	{
		m_boolIsOnline = false;
		ShowMessage(
			severity_levels::error,
			"AccountInfo: %s login failed.errorid=%d,errormsg=%s",
			GetCurrentKeyword().c_str(), 
			pRspInfo->errorID,
			pRspInfo->errorMsg);
	}
	else
	{
		m_boolIsOnline = true;
		ShowMessage(
			severity_levels::normal,
			"AccountInfo: %s login succeed!",
			GetCurrentKeyword().c_str());

		boost::shared_lock<boost::shared_mutex> lg(m_mapObserverStructProtector);//Ð´Ëø
		unsigned int InstrumentCount = m_mapInsid2Strategys.size();
		if (0 != InstrumentCount)
		{
			typedef char * NAMETYPE;
			char * * ppInstrumentID = new NAMETYPE[InstrumentCount];
			auto pos = m_mapInsid2Strategys.begin();
			for (unsigned int i = 0;i < InstrumentCount;++i, ++pos)
			{
				ppInstrumentID[i] = new char[31];//TThostFtdcInstrumentIDType
				strncpy(ppInstrumentID[i], pos->first.c_str(), 31);
				ShowMessage(
					severity_levels::normal,
					"trying to subscribe %s",
					ppInstrumentID[i]);
			}
			if (0 != m_pUserApi->SubscribeSOPMarketData(ppInstrumentID, InstrumentCount, ++m_uRequestID))
				ShowMessage(severity_levels::error, "send subscribemarketdata failed.");
			for (auto i = 0;i < InstrumentCount;i++)
				delete[] ppInstrumentID[i];
			delete[] ppInstrumentID;
		}
	}
	std::unique_lock<std::mutex> lk(m_mtxLoginSignal);
	m_cvLoginSignalCV.notify_all();
}

void CDFITC_SOP_MDPlugin::OnRspSOPUserLogout(struct DFITCSECRspUserLogoutField * pRspUsrLogout, struct DFITCSECRspInfoField * pRspInfo)
{
	ShowMessage(
		severity_levels::normal,
		"AccountInfo: %s loginout succeed!",
		GetCurrentKeyword().c_str());
	std::unique_lock<std::mutex> lk(m_mtxLogoutSignal);
	m_cvLogoutSignalCV.notify_all();
}

void CDFITC_SOP_MDPlugin::OnRspError(struct DFITCSECRspInfoField *pRspInfo)
{
	if ((pRspInfo) && (pRspInfo->errorID != 0))
	{
		//m_boolIsOnline=false;
		ShowMessage(severity_levels::error,
			"%s onrsperror.errorid=%d,errormsg=%s", 
			GetCurrentKeyword().c_str(), 
			pRspInfo->errorID,
			pRspInfo->errorMsg);
	}
}

void CDFITC_SOP_MDPlugin::OnRspSOPSubMarketData(struct DFITCSECSpecificInstrumentField * pSpecificInstrument, struct DFITCSECRspInfoField * pRspInfo)
{
	if (pRspInfo&&pRspInfo->errorID != 0)
	{
		ShowMessage(severity_levels::error,
			"%s errorid=%d,errormsg=%s",
			GetCurrentKeyword().c_str(),
			pRspInfo->errorID,
			pRspInfo->errorMsg);
	}
	else if(pSpecificInstrument&&pSpecificInstrument->securityID[0]!=0)
	{
		ShowMessage(severity_levels::normal,
			"submarketdata %s succeed",
			pSpecificInstrument->securityID);
	}
}

void CDFITC_SOP_MDPlugin::OnRspSOPUnSubMarketData(struct DFITCSECSpecificInstrumentField * pSpecificInstrument, struct DFITCSECRspInfoField * pRspInfo)
{
	if (pRspInfo&&pRspInfo->errorID != 0)
	{
		ShowMessage(severity_levels::error,
			"%s errorid=%d,errormsg=%s",
			GetCurrentKeyword().c_str(),
			pRspInfo->errorID,
			pRspInfo->errorMsg);
	}
	else if (pSpecificInstrument&&pSpecificInstrument->securityID[0] != 0)
	{
		ShowMessage(severity_levels::normal,
			"unsubmarketdata %s succeed.",
			pSpecificInstrument->securityID);
	}
}

void CDFITC_SOP_MDPlugin::OnSOPMarketData(struct DFITCSOPDepthMarketDataField * pDepthMarketData)
{
	if (m_adbIsPauseed)
		return;
	if (0 == pDepthMarketData->staticDataField.securityID[0])
		return;
	string InstrumentID = pDepthMarketData->staticDataField.exchangeID;
	InstrumentID+=pDepthMarketData->staticDataField.securityID;

	boost::shared_lock<boost::shared_mutex> lg(m_mapObserverStructProtector);//¶ÁËø
	auto & InstrumentNode = m_mapInsid2Strategys[InstrumentID];
	auto & tick = InstrumentNode.first;
	try {
		strncpy(tick.m_strInstrumentID, InstrumentID.c_str(), sizeof(tick.m_strInstrumentID));

		tick.m_intVolume = static_cast<TVolumeType>(pDepthMarketData->sharedDataField.tradeQty);
		tick.m_dbLastPrice = pDepthMarketData->sharedDataField.latestPrice;
		tick.m_datetimeUTCDateTime = microsec_clock::universal_time();

		tick.m_dbOpenPrice= pDepthMarketData->staticDataField.openPrice;
		
		tick.m_dbBidPrice[0] = pDepthMarketData->sharedDataField.bidPrice1;
		tick.m_dbBidPrice[1] = pDepthMarketData->sharedDataField.bidPrice2;
		tick.m_dbBidPrice[2] = pDepthMarketData->sharedDataField.bidPrice3;
		tick.m_dbBidPrice[3] = pDepthMarketData->sharedDataField.bidPrice4;
		tick.m_dbBidPrice[4] = pDepthMarketData->sharedDataField.bidPrice5;

		tick.m_intBidVolume[0] = static_cast<TVolumeType>(pDepthMarketData->sharedDataField.bidQty1);
		tick.m_intBidVolume[1] = static_cast<TVolumeType>(pDepthMarketData->sharedDataField.bidQty2);
		tick.m_intBidVolume[2] = static_cast<TVolumeType>(pDepthMarketData->sharedDataField.bidQty3);
		tick.m_intBidVolume[3] = static_cast<TVolumeType>(pDepthMarketData->sharedDataField.bidQty4);
		tick.m_intBidVolume[4] = static_cast<TVolumeType>(pDepthMarketData->sharedDataField.bidQty5);

		tick.m_dbAskPrice[0] = pDepthMarketData->sharedDataField.askPrice1;
		tick.m_dbAskPrice[1] = pDepthMarketData->sharedDataField.askPrice2;
		tick.m_dbAskPrice[2] = pDepthMarketData->sharedDataField.askPrice3;
		tick.m_dbAskPrice[3] = pDepthMarketData->sharedDataField.askPrice4;
		tick.m_dbAskPrice[4] = pDepthMarketData->sharedDataField.askPrice5;

		tick.m_intAskVolume[0] = static_cast<TVolumeType>(pDepthMarketData->sharedDataField.askQty1);
		tick.m_intAskVolume[1] = static_cast<TVolumeType>(pDepthMarketData->sharedDataField.askQty2);
		tick.m_intAskVolume[2] = static_cast<TVolumeType>(pDepthMarketData->sharedDataField.askQty3);
		tick.m_intAskVolume[3] = static_cast<TVolumeType>(pDepthMarketData->sharedDataField.askQty4);
		tick.m_intAskVolume[4] = static_cast<TVolumeType>(pDepthMarketData->sharedDataField.askQty5);

		tick.m_dbAuctionPrice= static_cast<TPriceType>(pDepthMarketData->specificDataField.AuctionPrice);
		switch (pDepthMarketData->sharedDataField.TradingPhaseCode[0])
		{
		case 'T':tick.m_enumPhase = TradingPhase;break;
		case 'V':tick.m_enumPhase = CircuitBreakingPhase;break;
		case 'S':
		case 'C':tick.m_enumPhase = AuctionPhase;break;
		}
	}
	catch (std::exception & err)
	{
		ShowMessage(severity_levels::error,"%s: error :%s", GetCurrentKeyword().c_str(), err.what());
		return;
	}
	catch (...)
	{
		ShowMessage(severity_levels::error, "%s: error :unknow", GetCurrentKeyword().c_str());
		return;
	}

	for(auto & node: InstrumentNode.second)
	{
		boost::unique_lock<boost::shared_mutex> lk(*get<2>(node), boost::try_to_lock);
		if (lk.owns_lock())
		{
			tick.m_uDataID = get<1>(node);
			get<0>(node)->OnTick(get<1>(node), &tick);
			if (0 == get<1>(node))
			{
				uint_least64_t current = 0;
				unsigned int i = 0;
				current = pDepthMarketData->staticDataField.tradingDay;
				while (pDepthMarketData->sharedDataField.updateTime[i])
				{
					if (pDepthMarketData->sharedDataField.updateTime[i] >= '0'&&pDepthMarketData->sharedDataField.updateTime[i] <= '9')
						current = current * 10 + pDepthMarketData->sharedDataField.updateTime[i] - '0';
					i++;
				}
				get<3>(node)->store(current);
			}
		}
		else
			ShowMessage(severity_levels::warning,
				"%s: onrtndepthmarketdata try lock failed:%s%s",
				GetCurrentKeyword().c_str(),
				pDepthMarketData->staticDataField.exchangeID, pDepthMarketData->staticDataField.securityID);
	}

}

#pragma endregion