#include "MDPluginImp.h"
#include <stdarg.h>
#include <thread>
const string CMDPluginImp::s_strAccountKeyword="serveraddress;brokerid;username;password;";

CMDPluginImp::CMDPluginImp():m_StartAndStopCtrlTimer(m_IOservice)
{
}

CMDPluginImp::~CMDPluginImp()
{
}

bool CMDPluginImp::IsOnline()
{
	return m_boolIsOnline;
}

void CMDPluginImp::IncreaseRefCount()
{
	m_intRefCount++;
}

void CMDPluginImp::DescreaseRefCount()
{
	m_intRefCount--;
}

int CMDPluginImp::GetRefCount()
{
	return m_intRefCount;
}

void CMDPluginImp::CheckSymbolValidity(const unordered_map<string, string> & insConfig)
{
	auto Type = insConfig.find("type");
	if (Type == insConfig.end())
		throw std::exception("Can not find the <type> of the the symbol.");
	if (Type->second != "ctp")
		throw std::exception("ctp MarketDataSource does not support this symbol.");
	auto InstrumentNode = insConfig.find("instrumentid");
	if (InstrumentNode == insConfig.end())
		throw std::exception("<instrumentid> not found.");
	auto ExchangeIDNode = insConfig.find("exchangeid");
	if (ExchangeIDNode == insConfig.end())
		throw std::exception("<exchangeid> not found.");
}

string CMDPluginImp::GetCurrentKeyword()
{
	return "ctp_md&"+m_strUsername+"&"+m_strBrokerID;
}

string CMDPluginImp::GetProspectiveKeyword(const ptree & in)
{
	string retKey = "ctp_md&";
	auto temp = in.find("username");
	if (temp != in.not_found())
	{

		if (temp->second.data().size()>(sizeof(CThostFtdcReqUserLoginField::UserID) - 1))
			throw std::exception("ctp:username is too long");
		retKey += temp->second.data();
	}
	else
		throw std::exception("ctp:can not find <username>");
	retKey += "&";

	temp = in.find("brokerid");
	if (temp != in.not_found())
	{

		if (temp->second.data().size()>(sizeof(CThostFtdcReqUserLoginField::BrokerID) - 1))
			throw std::exception("ctp:brokerid is too long");
		retKey += temp->second.data();
	}
	else
		throw std::exception("ctp:can not find <brokerid>");

	return retKey;
}

void CMDPluginImp::GetState(ptree & out)
{
	if(m_boolIsOnline)
		out.put("online","true");
	else
		out.put("online", "false");
	out.put("serveraddress", m_strServerAddress);
	out.put("brokerid", m_strBrokerID);
	out.put("username", m_strUsername);
}

void CMDPluginImp::MDInit(const ptree & in)
{
	auto temp = in.find("serveraddress");
	if (temp != in.not_found())
	{
		
		m_strServerAddress = temp->second.data();
		if (m_strServerAddress.size()>64)
			throw std::exception("ctp:serveraddress is too long(Max:64 char)");
	}
	else
		throw std::exception("ctp:Can not find <serveraddress>");


	temp = in.find("brokerid");
	if (temp != in.not_found())
	{
		m_strBrokerID = temp->second.data();
		if(m_strBrokerID.size()>(sizeof(CThostFtdcReqUserLoginField::BrokerID)-1))
			throw std::exception("ctp:brokerid is too long");
	}
	else
		throw std::exception("ctp:Can not find <brokerid>");

	temp = in.find("username");
	if (temp != in.not_found())
	{
		m_strUsername = temp->second.data();
		if(m_strUsername.size()>(sizeof(CThostFtdcReqUserLoginField::UserID)-1))
			throw std::exception("ctp:username is too long");
	}
	else
		throw std::exception("ctp:Can not find <username>");

	temp = in.find("password");
	if (temp != in.not_found())
	{
		m_strPassword = temp->second.data();
		if(m_strPassword.size()>(sizeof(CThostFtdcReqUserLoginField::Password)-1))
			throw std::exception("ctp:Password is too long");
	}
	else
		throw std::exception("ctp:Can not find <password>");

	m_StartAndStopCtrlTimer.expires_from_now(time_duration(0,0,3,0));
	m_StartAndStopCtrlTimer.async_wait(boost::bind(&CMDPluginImp::TimerHandler,this));
	m_futTimerThreadFuture=std::async([this] {
		this->m_IOservice.run();
		return true;
	});

}

void CMDPluginImp::TimerHandler()
{
	time_duration tid = second_clock::universal_time().time_of_day();
	ptime nextActiveTime=not_a_date_time;
	if (tid >= time_duration(0, 0, 0, 0) && tid < time_duration(0, 59, 0, 0))
	{
		nextActiveTime = ptime(second_clock::universal_time().date(), time_duration(0, 59, 30, 0));
	}
	else if (tid >= time_duration(0, 59, 0, 0) && tid < time_duration(7, 14, 0, 0))
	{
		if (false == m_boolIsOnline)
			Start();
		nextActiveTime = ptime(second_clock::universal_time().date(), time_duration(7, 16, 30, 0));
	}
	else if (tid >= time_duration(7, 14, 0, 0) && tid < time_duration(12, 59, 0, 0))
	{
		if (true == m_boolIsOnline)
			Stop();
		nextActiveTime = ptime(second_clock::universal_time().date(), time_duration(12, 59, 30, 0));
	}
	else if (tid >= time_duration(12, 59, 0, 0) && tid < time_duration(18, 29, 0, 0))
	{
		if (false == m_boolIsOnline)
			Start();
		nextActiveTime = ptime(second_clock::universal_time().date(), time_duration(18, 31, 0, 0));
	}
	else if (tid >= time_duration(18, 29, 0, 0) && tid < time_duration(23, 59, 59, 0))
	{
		if (true == m_boolIsOnline)
			Stop();
		nextActiveTime = ptime(second_clock::universal_time().date()+days(1), time_duration(0, 0, 30, 0));
	}
	m_StartAndStopCtrlTimer.expires_at(nextActiveTime);
	m_StartAndStopCtrlTimer.async_wait(boost::bind(&CMDPluginImp::TimerHandler, this));
	ShowMessage(normal, "%s: Next:%s", GetCurrentKeyword().c_str(), to_simple_string(nextActiveTime).c_str());
}

bool CMDPluginImp::Start()
{
	m_uRequestID = 0;
	m_boolIsOnline = false;
	m_pUserApi = std::shared_ptr<CThostFtdcMdApi>(CThostFtdcMdApi::CreateFtdcMdApi(), [](CThostFtdcMdApi* p) {if (p) p->Release();});
	m_pUserApi->RegisterSpi(this);
	char ServerAddress[65];
	strcpy(ServerAddress, m_strServerAddress.c_str());
	m_pUserApi->RegisterFront(ServerAddress);
	m_pUserApi->Init();
	std::unique_lock<std::mutex> lk(m_mtxLoginSignal);
	m_cvLoginSignalCV.wait_for(lk,std::chrono::seconds(10));
	if (m_boolIsOnline)
		return true;
	else
		return false;
}

void CMDPluginImp::Stop()
{
	if (m_boolIsOnline)
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
		m_pUserApi=nullptr;
	}
	ShowMessage(
		severity_levels::normal,
		"%s loginout succeed!",
		GetCurrentKeyword().c_str());
	m_boolIsOnline = false;
}

void CMDPluginImp::MDUnload()
{
	
	Stop();
	//m_StartAndStopCtrlTimer.cancel();
	m_IOservice.stop();
	m_futTimerThreadFuture.get();
}

void CMDPluginImp::ShowMessage(severity_levels lv, const char * fmt, ...)
{
	char buf[512];
	va_list arg;
	va_start(arg, fmt);
	vsnprintf(buf, 512, fmt, arg);
	va_end(arg);
	boost::log::sources::severity_logger< severity_levels > m_Logger;
	BOOST_LOG_SEV(m_Logger, lv) << buf;
}

void CMDPluginImp::MDAttachStrategy(MStrategy * strategy,TMarketDataIdType dataid,const unordered_map<string, string> & insConfig,boost::shared_mutex & mtx)
{
	boost::unique_lock<boost::shared_mutex> lg(m_mapObserverStructProtector);//Ð´Ëø

	auto InstrumentID = insConfig.find("instrumentid")->second;
	auto findres = m_mapInsid2Strategys.find(InstrumentID);
	m_mapStrategy2Insids[strategy].push_back(InstrumentID);
	if (findres != m_mapInsid2Strategys.end())
		findres->second.second.push_back(make_tuple(strategy, dataid, &mtx));
	else
	{
		m_mapInsid2Strategys[InstrumentID].second.push_back(make_tuple(strategy, dataid, &mtx));
		if (m_boolIsOnline&&(m_mapInsid2Strategys[InstrumentID].second.size()==1))
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
			delete[] ppInstrumentID[0];
			delete[] ppInstrumentID;
		}
	}
	

}

void CMDPluginImp::MDDetachStrategy(MStrategy * strategy)
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
		if (m_boolIsOnline && m_mapInsid2Strategys[ins].second.empty())
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
			delete[] ppInstrumentID[0];
			delete[] ppInstrumentID;
			m_mapInsid2Strategys.erase(ins);
		}
	}
	m_mapStrategy2Insids.erase(strategy);

}



void CMDPluginImp::OnFrontConnected()
{
	CThostFtdcReqUserLoginField req;
	memset(&req, 0, sizeof(req));
	strncpy(req.BrokerID, m_strBrokerID.c_str(), sizeof(req.BrokerID));
	strncpy(req.UserID, m_strUsername.c_str(), sizeof(req.UserID));
	strncpy(req.Password, m_strPassword.c_str(), sizeof(req.Password));
	int res = m_pUserApi->ReqUserLogin(&req, ++m_uRequestID);
	if (0 != res)
	{
		ShowMessage(
			severity_levels::error,
			"%s: send requserlogin failed.",
			GetCurrentKeyword().c_str());
		std::unique_lock<std::mutex> lk(m_mtxLoginSignal);
		m_cvLoginSignalCV.notify_all();
	}
}

void CMDPluginImp::OnFrontDisconnected(int nReason)
{
	ShowMessage(
		severity_levels::error, 
		"%s: OnFrontDisconnected(%d)",
		GetCurrentKeyword().c_str(),
		nReason);
}

void CMDPluginImp::OnHeartBeatWarning(int nTimeLapse)
{
	ShowMessage(
		severity_levels::error,
		"%s: onheartbeatwarning(%d)",
		GetCurrentKeyword().c_str(),
		nTimeLapse);
}

void CMDPluginImp::OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	if ((pRspInfo) && (pRspInfo->ErrorID != 0))
	{
		m_boolIsOnline = false;
		ShowMessage(
			severity_levels::error,
			"%s login failed.errorid=%d,errormsg=%s",
			GetCurrentKeyword().c_str(), 
			pRspInfo->ErrorID,
			pRspInfo->ErrorMsg);
	}
	else
	{
		m_boolIsOnline = true;
		ShowMessage(
			severity_levels::normal,
			"%s login succeed!",
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

void CMDPluginImp::OnRspUserLogout(CThostFtdcUserLogoutField *pUserLogout, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{}

void CMDPluginImp::OnRspError(CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	if ((pRspInfo) && (pRspInfo->ErrorID != 0))
	{
		m_boolIsOnline=false;
		ShowMessage(severity_levels::error,
			"%s onrsperror.errorid=%d,errormsg=%s", 
			GetCurrentKeyword().c_str(), 
			pRspInfo->ErrorID,
			pRspInfo->ErrorMsg);
	}
}

void CMDPluginImp::OnRspSubMarketData(CThostFtdcSpecificInstrumentField *pSpecificInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
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

void CMDPluginImp::OnRspUnSubMarketData(CThostFtdcSpecificInstrumentField *pSpecificInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
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

void CMDPluginImp::OnRspSubForQuoteRsp(CThostFtdcSpecificInstrumentField *pSpecificInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{}

void CMDPluginImp::OnRspUnSubForQuoteRsp(CThostFtdcSpecificInstrumentField *pSpecificInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{}

void CMDPluginImp::OnRtnDepthMarketData(CThostFtdcDepthMarketDataField *pDepthMarketData)
{
	if (0 == pDepthMarketData->InstrumentID[0])
		return;
	boost::shared_lock<boost::shared_mutex> lg(m_mapObserverStructProtector);//¶ÁËø
	auto & InstrumentNode = m_mapInsid2Strategys[pDepthMarketData->InstrumentID];
	auto & tick = InstrumentNode.first;
	try {
		strncpy(tick.m_strInstrumentID, pDepthMarketData->InstrumentID, sizeof(tick.m_strInstrumentID));
		tick.m_intVolume = pDepthMarketData->Volume;
		tick.m_dbLastPrice = pDepthMarketData->LastPrice;
		tick.m_datetimeUTCDateTime = ptime(from_undelimited_string(pDepthMarketData->TradingDay), duration_from_string(pDepthMarketData->UpdateTime) + milliseconds(pDepthMarketData->UpdateMillisec));//pDepthMarketData-;
		tick.m_datetimeUTCDateTime -= hours(8);
		tick.m_dbLowerLimitPrice = pDepthMarketData->LowerLimitPrice;
		tick.m_dbUpperLimitPrice = pDepthMarketData->UpperLimitPrice;
		tick.m_dbLowestPrice = pDepthMarketData->LastPrice;
		tick.m_dbBidPrice1 = pDepthMarketData->BidPrice1;
		tick.m_intBidVolume1 = pDepthMarketData->BidVolume1;
		tick.m_dbAskPrice1 = pDepthMarketData->AskPrice1;
		tick.m_intAskVolume1 = pDepthMarketData->AskVolume1;
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
			get<0>(node)->OnTick(get<1>(node), &tick);
		else
			ShowMessage(severity_levels::warning,
				"%s: onrtndepthmarketdata try lock failed:%s",
				GetCurrentKeyword().c_str(),
				pDepthMarketData->InstrumentID);
	}

}

void CMDPluginImp::OnRtnForQuoteRsp(CThostFtdcForQuoteRspField *pForQuoteRsp)
{}


