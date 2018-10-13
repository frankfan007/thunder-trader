#include "CTP_FUTURE_MDPlugin.h"
#include <stdarg.h>
#include <thread>
#include "AutoPend.h"
const string CCTP_FUTURE_MDPlugin::s_strAccountKeyword = "serveraddress;brokerid;username;password;";
extern char ProcessName[256];

CCTP_FUTURE_MDPlugin::CCTP_FUTURE_MDPlugin() :m_StartAndStopCtrlTimer(m_IOservice), m_abIsPending(false), m_adbIsPauseed(false)
{
}

CCTP_FUTURE_MDPlugin::~CCTP_FUTURE_MDPlugin()
{
}

bool CCTP_FUTURE_MDPlugin::IsPedding()
{
	return m_abIsPending.load();
}

bool CCTP_FUTURE_MDPlugin::IsOnline()
{
	return m_boolIsOnline;
}

void CCTP_FUTURE_MDPlugin::IncreaseRefCount()
{
	m_intRefCount++;
}

void CCTP_FUTURE_MDPlugin::DescreaseRefCount()
{
	m_intRefCount--;
}

int CCTP_FUTURE_MDPlugin::GetRefCount()
{
	return m_intRefCount;
}

void CCTP_FUTURE_MDPlugin::CheckSymbolValidity(const unordered_map<string, string> & insConfig)
{
	auto Type = insConfig.find("type");
	if (Type == insConfig.end())
		throw std::runtime_error("Can not find the <type> of the the symbol.");
	if (Type->second != "future")
		throw std::runtime_error("ctp MarketDataSource does not support this symbol.");
	auto InstrumentNode = insConfig.find("instrumentid");
	if (InstrumentNode == insConfig.end())
		throw std::runtime_error("<instrumentid> not found.");
	auto ExchangeIDNode = insConfig.find("exchangeid");
	if (ExchangeIDNode == insConfig.end())
		throw std::runtime_error("<exchangeid> not found.");
}

string CCTP_FUTURE_MDPlugin::GetCurrentKeyword()
{
	return "ctp_md&"+m_strUsername+"&"+m_strBrokerID;
}

string CCTP_FUTURE_MDPlugin::GetProspectiveKeyword(const ptree & in)
{
	string retKey = "ctp_md&";
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

void CCTP_FUTURE_MDPlugin::GetState(ptree & out)
{
	if(m_boolIsOnline)
		out.put("online","true");
	else
		out.put("online", "false");
	out.put("serveraddress", m_strServerAddress);
	out.put("brokerid", m_strBrokerID);
	out.put("username", m_strUsername);
}

void CCTP_FUTURE_MDPlugin::MDInit(const ptree & in)
{
	auto temp = in.find("serveraddress");
	if (temp != in.not_found())
	{
		m_strServerAddress = temp->second.data();
		if (m_strServerAddress.size()>64)
			throw std::runtime_error("ctp:serveraddress is too long(Max:64 char)");
		else if(m_strServerAddress.empty())
			throw std::runtime_error("ctp:serveraddress is empty");
	}
	else
		throw std::runtime_error("ctp:Can not find <serveraddress>");


	temp = in.find("brokerid");
	if (temp != in.not_found())
	{
		m_strBrokerID = temp->second.data();
		if(m_strBrokerID.size()>(sizeof(CThostFtdcReqUserLoginField::BrokerID)-1))
			throw std::runtime_error("ctp:brokerid is too long");
		else if(m_strBrokerID.empty())
			throw std::runtime_error("ctp:brokerid is empty");
	}
	else
		throw std::runtime_error("ctp:Can not find <brokerid>");

	temp = in.find("username");
	if (temp != in.not_found())
	{
		m_strUsername = temp->second.data();
		if(m_strUsername.size()>(sizeof(CThostFtdcReqUserLoginField::UserID)-1))
			throw std::runtime_error("ctp:username is too long");
		else if(m_strUsername.empty())
			throw std::runtime_error("ctp:username is empty");
	}
	else
		throw std::runtime_error("ctp:Can not find <username>");

	

	temp = in.find("password");
	if (temp != in.not_found())
	{
		m_strPassword = temp->second.data();
		if(m_strPassword.size()>(sizeof(CThostFtdcReqUserLoginField::Password)-1))
			throw std::runtime_error("ctp:Password is too long");
		else if(m_strPassword.empty())
			throw std::runtime_error("ctp:Password is empty");
	}
	else
		throw std::runtime_error("ctp:Can not find <password>");

	m_StartAndStopCtrlTimer.expires_from_now(boost::posix_time::seconds(3));
	m_StartAndStopCtrlTimer.async_wait(boost::bind(
		&CCTP_FUTURE_MDPlugin::TimerHandler,
		this,
		&m_StartAndStopCtrlTimer,
		boost::asio::placeholders::error));
	m_futTimerThreadFuture=std::async(std::launch::async,[this] {
		this->m_IOservice.run();
		return true;
	});

}

void CCTP_FUTURE_MDPlugin::MDHotUpdate(const ptree & NewConfig)
{
	MDUnload();
	m_IOservice.reset();
	MDInit(NewConfig);
}

void CCTP_FUTURE_MDPlugin::TimerHandler(boost::asio::deadline_timer* timer, const boost::system::error_code& err)
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
		m_StartAndStopCtrlTimer.async_wait(boost::bind(
			&CCTP_FUTURE_MDPlugin::TimerHandler,
			this,
			&m_StartAndStopCtrlTimer,
			boost::asio::placeholders::error));
		ShowMessage(normal, "%s: Next:%s", GetCurrentKeyword().c_str(), to_simple_string(nextActiveTime).c_str());
	}
}

bool CCTP_FUTURE_MDPlugin::Start()
{
	CAutoPend pend(m_abIsPending);
	if (false == m_boolIsOnline)
	{
		m_uRequestID = 0;
		m_boolIsOnline = false;
		m_pUserApi = std::shared_ptr<CThostFtdcMdApi>(CThostFtdcMdApi::CreateFtdcMdApi(), [](CThostFtdcMdApi* p) {if (p) p->Release();});
		if (nullptr == m_pUserApi)
		{
			ShowMessage(
				severity_levels::error,
				"%s CreateFtdcMdApi error",
				GetCurrentKeyword().c_str());
			return false;
		}
		m_pUserApi->RegisterSpi(this);
		char ServerAddress[65];
		strcpy(ServerAddress, m_strServerAddress.c_str());
		m_pUserApi->RegisterFront(ServerAddress);
		m_pUserApi->Init();
		std::unique_lock<std::mutex> lk(m_mtxLoginSignal);
		m_cvLoginSignalCV.wait_for(lk, std::chrono::seconds(10));
		if (m_boolIsOnline)
			return true;
		else
		{
			m_pUserApi->RegisterSpi(nullptr);
			m_pUserApi.reset();
			m_boolIsOnline = false;
			return false;
		}
	}
	else return true;
}

void CCTP_FUTURE_MDPlugin::Stop()
{
	CAutoPend pend(m_abIsPending);
	if (true==m_boolIsOnline)
	{
		boost::shared_lock<boost::shared_mutex> lg(m_mapObserverStructProtector);//Ð´Ëø
		unsigned int InstrumentCount = m_mapInsid2Strategys.size();
		typedef char * NAME;
		char * * ppInstrumentID = new NAME[InstrumentCount];
		auto pos = m_mapInsid2Strategys.begin();
		for (unsigned int i = 0;i < InstrumentCount;++i, ++pos)
		{
			ppInstrumentID[i] = new char[31];//TThostFtdcInstrumentIDType
			strncpy(ppInstrumentID[i], pos->first.c_str(), 31);
		}
		if (0 != m_pUserApi->UnSubscribeMarketData(ppInstrumentID, InstrumentCount))
			ShowMessage(severity_levels::error, "send unsubscribemarketdata failed.");
		for (auto i = 0;i < InstrumentCount;i++)
			delete[] ppInstrumentID[i];
		delete[] ppInstrumentID;
	}
	if (m_pUserApi)
	{
		m_pUserApi->RegisterSpi(nullptr);
		m_pUserApi.reset();
	}
	ShowMessage(
		severity_levels::normal,
		"AccountInfo: %s loginout succeed!",
		GetCurrentKeyword().c_str());
	m_boolIsOnline = false;
}

void CCTP_FUTURE_MDPlugin::MDUnload()
{
	
	Stop();
	auto Count=m_StartAndStopCtrlTimer.cancel();
	m_IOservice.stop();
	m_futTimerThreadFuture.get();
	
}

void CCTP_FUTURE_MDPlugin::Pause()
{
	m_adbIsPauseed.store(true);
}

void CCTP_FUTURE_MDPlugin::Continue()
{
	m_adbIsPauseed.store(false);
}

void CCTP_FUTURE_MDPlugin::ShowMessage(severity_levels lv, const char * fmt, ...)
{
	char buf[512];
	va_list arg;
	va_start(arg, fmt);
	vsnprintf(buf, 512, fmt, arg);
	va_end(arg);
	boost::log::sources::severity_logger< severity_levels > m_Logger;
	BOOST_LOG_SEV(m_Logger, lv) << ProcessName<<": "<< buf << " [" << to_iso_string(microsec_clock::universal_time()) << "]";
}

void CCTP_FUTURE_MDPlugin::MDAttachStrategy(
	MStrategy * strategy,
	TMarketDataIdType dataid,
	const unordered_map<string, string> & insConfig,
	boost::shared_mutex & mtx,
	atomic_uint_least64_t * updatetime)
{
	boost::unique_lock<boost::shared_mutex> lg(m_mapObserverStructProtector);//Ð´Ëø

	auto InstrumentID = insConfig.find("instrumentid")->second;
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
		tick.m_dbLastPrice=0;
		tick.m_intVolume = 0;
		for (unsigned int i = 0;i < MAX_QUOTATIONS_DEPTH;i++)
		{
			tick.m_dbBidPrice[i] = 0.0;
			tick.m_intBidVolume[i] = 0;
			tick.m_dbAskPrice[i] = 0.0;
			tick.m_intAskVolume[i] = 0;
		}


		tick.m_dbLowerLimitPrice = 0;
		tick.m_dbUpperLimitPrice = 0;
		tick.m_dbAveragePrice = 0;
		tick.m_dbTurnover = 0;
		tick.m_dbOpenInterest = 0;
		tick.m_dbOpenPrice = 0;
		tick.m_dbHighestPrice = 0;
		tick.m_dbLowestPrice = 0;
		tick.m_dbClosePrice = 0;
		tick.m_dbPreSettlementPrice = 0;
		tick.m_dbPreClosePrice = 0;
		#pragma endregion	
		if (m_boolIsOnline)
		{
			typedef char * NAME;
			char * * ppInstrumentID = new NAME[1];
			ppInstrumentID[0] = new char[31];//TThostFtdcInstrumentIDType
			strncpy(ppInstrumentID[0], InstrumentID.c_str(), 31);
			if (0 != m_pUserApi->SubscribeMarketData(ppInstrumentID, 1))
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

void CCTP_FUTURE_MDPlugin::MDDetachStrategy(MStrategy * strategy)
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
				typedef char * NAME;
				char * * ppInstrumentID = new NAME[1];
				ppInstrumentID[0] = new char[31];//TThostFtdcInstrumentIDType
				strncpy(ppInstrumentID[0], ins.c_str(), 31);
				if (0 != m_pUserApi->UnSubscribeMarketData(ppInstrumentID, 1))
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

void CCTP_FUTURE_MDPlugin::OnFrontConnected()
{
	CThostFtdcReqUserLoginField req;
	memset(&req, 0, sizeof(req));
	strncpy(req.BrokerID, m_strBrokerID.c_str(), sizeof(req.BrokerID));
	strncpy(req.UserID, m_strUsername.c_str(), sizeof(req.UserID));
	strncpy(req.Password, m_strPassword.c_str(), sizeof(req.Password));

	if ((nullptr == m_pUserApi)||(0 != m_pUserApi->ReqUserLogin(&req, ++m_uRequestID)))
	{
		ShowMessage(
			severity_levels::error,
			"%s: send requserlogin failed.",
			GetCurrentKeyword().c_str());
		std::unique_lock<std::mutex> lk(m_mtxLoginSignal);
		m_cvLoginSignalCV.notify_all();
	}
}

void CCTP_FUTURE_MDPlugin::OnFrontDisconnected(int nReason)
{
	ShowMessage(
		severity_levels::error, 
		"%s: OnFrontDisconnected(%d)",
		GetCurrentKeyword().c_str(),
		nReason);
}

void CCTP_FUTURE_MDPlugin::OnHeartBeatWarning(int nTimeLapse)
{
	ShowMessage(
		severity_levels::error,
		"%s: onheartbeatwarning(%d)",
		GetCurrentKeyword().c_str(),
		nTimeLapse);
}

void CCTP_FUTURE_MDPlugin::OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	if ((pRspInfo) && (pRspInfo->ErrorID != 0))
	{
		m_boolIsOnline = false;
		ShowMessage(
			severity_levels::error,
			"AccountInfo: %s login failed.errorid=%d,errormsg=%s",
			GetCurrentKeyword().c_str(), 
			pRspInfo->ErrorID,
			pRspInfo->ErrorMsg);
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
			typedef char * NAME;
			char * * ppInstrumentID = new NAME[InstrumentCount];
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
			if (0 != m_pUserApi->SubscribeMarketData(ppInstrumentID, InstrumentCount))
				ShowMessage(severity_levels::error, "send subscribemarketdata failed.");

			for (auto i = 0;i < InstrumentCount;i++)
				delete[] ppInstrumentID[i];
			delete[] ppInstrumentID;
		}
	}
	std::unique_lock<std::mutex> lk(m_mtxLoginSignal);
	m_cvLoginSignalCV.notify_all();
}

void CCTP_FUTURE_MDPlugin::OnRspUserLogout(CThostFtdcUserLogoutField *pUserLogout, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{}

void CCTP_FUTURE_MDPlugin::OnRspError(CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	if ((pRspInfo) && (pRspInfo->ErrorID != 0))
	{
		//m_boolIsOnline=false;
		ShowMessage(severity_levels::error,
			"%s onrsperror.errorid=%d,errormsg=%s", 
			GetCurrentKeyword().c_str(), 
			pRspInfo->ErrorID,
			pRspInfo->ErrorMsg);
	}
}

void CCTP_FUTURE_MDPlugin::OnRspSubMarketData(CThostFtdcSpecificInstrumentField *pSpecificInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	if (pRspInfo&&pRspInfo->ErrorID != 0)
	{
		ShowMessage(severity_levels::error,
			"%s errorid=%d,errormsg=%s",
			GetCurrentKeyword().c_str(),
			pRspInfo->ErrorID,
			pRspInfo->ErrorMsg);
	}
	else if(pSpecificInstrument&&pSpecificInstrument->InstrumentID[0]!=0)
	{
		ShowMessage(severity_levels::normal,
			"submarketdata %s succeed",
			pSpecificInstrument->InstrumentID);
	}
}

void CCTP_FUTURE_MDPlugin::OnRspUnSubMarketData(CThostFtdcSpecificInstrumentField *pSpecificInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	if (pRspInfo&&pRspInfo->ErrorID != 0)
	{
		ShowMessage(severity_levels::error,
			"%s errorid=%d,errormsg=%s",
			GetCurrentKeyword().c_str(),
			pRspInfo->ErrorID,
			pRspInfo->ErrorMsg);
	}
	else if (pSpecificInstrument&&pSpecificInstrument->InstrumentID[0] != 0)
	{
		ShowMessage(severity_levels::normal,
			"unsubmarketdata %s succeed.",
			pSpecificInstrument->InstrumentID);
	}
}

void CCTP_FUTURE_MDPlugin::OnRspSubForQuoteRsp(CThostFtdcSpecificInstrumentField *pSpecificInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{}

void CCTP_FUTURE_MDPlugin::OnRspUnSubForQuoteRsp(CThostFtdcSpecificInstrumentField *pSpecificInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{}

void CCTP_FUTURE_MDPlugin::OnRtnDepthMarketData(CThostFtdcDepthMarketDataField *pDepthMarketData)
{
	if (m_adbIsPauseed)
		return;
	if (0 == pDepthMarketData->InstrumentID[0])
		return;
	boost::shared_lock<boost::shared_mutex> lg(m_mapObserverStructProtector);//¶ÁËø
	auto & InstrumentNode = m_mapInsid2Strategys[pDepthMarketData->InstrumentID];
	auto & tick = InstrumentNode.first;
	try {
		strncpy(tick.m_strInstrumentID, pDepthMarketData->InstrumentID, sizeof(tick.m_strInstrumentID));
		tick.m_intVolume = pDepthMarketData->Volume;
		tick.m_dbLastPrice = pDepthMarketData->LastPrice;
		tick.m_datetimeUTCDateTime = microsec_clock::universal_time();
		tick.m_dbAveragePrice = pDepthMarketData->AveragePrice;
		tick.m_dbTurnover = pDepthMarketData->Turnover;
		tick.m_dbOpenInterest = pDepthMarketData->OpenInterest;
		tick.m_dbLowerLimitPrice = pDepthMarketData->LowerLimitPrice;
		tick.m_dbUpperLimitPrice = pDepthMarketData->UpperLimitPrice;
		tick.m_dbOpenPrice = pDepthMarketData->OpenPrice;
		tick.m_dbHighestPrice = pDepthMarketData->HighestPrice;
		tick.m_dbLowestPrice = pDepthMarketData->LowestPrice;
		tick.m_dbClosePrice = pDepthMarketData->ClosePrice;
		tick.m_dbBidPrice[0] = pDepthMarketData->BidPrice1;
		tick.m_intBidVolume[0] = pDepthMarketData->BidVolume1;
		tick.m_dbAskPrice[0] = pDepthMarketData->AskPrice1;
		tick.m_intAskVolume[0] = pDepthMarketData->AskVolume1;
		tick.m_dbPreSettlementPrice = pDepthMarketData->PreSettlementPrice;
		tick.m_dbPreClosePrice = pDepthMarketData->PreClosePrice;
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
				while (pDepthMarketData->ActionDay[i])
				{
					current = current * 10 + pDepthMarketData->ActionDay[i] - '0';
					i++;
				}
				i = 0;
				while (pDepthMarketData->UpdateTime[i])
				{
					if(pDepthMarketData->UpdateTime[i]>='0'&&pDepthMarketData->UpdateTime[i]<='9')
						current = current * 10 + pDepthMarketData->UpdateTime[i] - '0';
					i++;
				}
				current = current * 1000 + pDepthMarketData->UpdateMillisec;
				get<3>(node)->store(current);
			}
		}
		else
			ShowMessage(severity_levels::warning,
				"%s: onrtndepthmarketdata try lock failed:%s",
				GetCurrentKeyword().c_str(),
				pDepthMarketData->InstrumentID);
	}
}

void CCTP_FUTURE_MDPlugin::OnRtnForQuoteRsp(CThostFtdcForQuoteRspField *pForQuoteRsp)
{}

#pragma endregion