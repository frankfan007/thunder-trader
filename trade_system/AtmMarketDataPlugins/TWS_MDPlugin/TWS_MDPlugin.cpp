#include "TWS_MDPlugin.h"
#include <stdarg.h>
#include <thread>
#include "AutoPend.h"

const string CTWS_MDPlugin::s_strAccountKeyword = "serveraddress;port;clientid;";
extern char ProcessName[256];
void String2CTwsContractField(const string instru, CTwsContractField & req)
{
	memset(&req, 0, sizeof(CTwsContractField));
	stringstream ss;
	ss << instru;
	ptree instrumentTree;
	read_json(ss, instrumentTree);
	if(instrumentTree.find("symbol")->second.data()!="_null_")
		strcpy(req.m_strSymbol, instrumentTree.find("symbol")->second.data().c_str());
	auto SecType = instrumentTree.find("sectype")->second.data();
	if ("STK" == SecType)
		req.m_enumSecType = TTwsSecTypeType::Tws_STK;
	else if ("OPT" == SecType)
		req.m_enumSecType = TTwsSecTypeType::Tws_OPT;
	else if ("FUT" == SecType)
		req.m_enumSecType = TTwsSecTypeType::Tws_FUT;
	else if ("IND" == SecType)
		req.m_enumSecType = TTwsSecTypeType::Tws_IND;
	else if ("FOP" == SecType)
		req.m_enumSecType = TTwsSecTypeType::Tws_FOP;
	else if ("CASH" == SecType)
		req.m_enumSecType = TTwsSecTypeType::Tws_CASH;
	else if ("BAG" == SecType)
		req.m_enumSecType = TTwsSecTypeType::Tws_BAG;


	if(instrumentTree.find("expiry")->second.data()!="_null_")
		strcpy(req.m_strExpiry, instrumentTree.find("expiry")->second.data().c_str());
	if(instrumentTree.find("strike")->second.data()!="_null_")
		req.m_dbStrike = atof(instrumentTree.find("strike")->second.data().c_str());
	if(instrumentTree.find("multiplier")->second.data()!="_null_")
		strcpy(req.m_strMultiplier, instrumentTree.find("multiplier")->second.data().c_str());
	if(instrumentTree.find("exchange")->second.data()!="_null_")
		strcpy(req.m_strExchange, instrumentTree.find("exchange")->second.data().c_str());
	if(instrumentTree.find("primaryexchange")->second.data()!="_null_")
		strcpy(req.m_strPrimaryExchange, instrumentTree.find("primaryexchange")->second.data().c_str());
	if(instrumentTree.find("currency")->second.data() !="_null_")
		strcpy(req.m_strCurrency, instrumentTree.find("currency")->second.data().c_str());
	if(instrumentTree.find("localsymbol")->second.data()!="_null_")
		strcpy(req.m_strLocalSymbol, instrumentTree.find("localsymbol")->second.data().c_str());
	if(instrumentTree.find("tradingclass")->second.data()!="_null_")
		strcpy(req.m_strTradingClass, instrumentTree.find("tradingclass")->second.data().c_str());
}

CTWS_MDPlugin::CTWS_MDPlugin() :m_StartAndStopCtrlTimer(m_IOservice), m_abIsPending(false), m_adbIsPauseed(false)
{
}

CTWS_MDPlugin::~CTWS_MDPlugin()
{
}

bool CTWS_MDPlugin::IsPedding()
{
	return m_abIsPending.load();
}

bool CTWS_MDPlugin::IsOnline()
{
	return m_boolIsOnline;
}

void CTWS_MDPlugin::IncreaseRefCount()
{
	m_intRefCount++;
}

void CTWS_MDPlugin::DescreaseRefCount()
{
	m_intRefCount--;
}

int CTWS_MDPlugin::GetRefCount()
{
	return m_intRefCount;
}

void CTWS_MDPlugin::CheckSymbolValidity(const unordered_map<string, string> & insConfig)
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

string CTWS_MDPlugin::GetCurrentKeyword()
{
	string tempAddress = m_strServerAddress;
	for (unsigned int i = 0;i < tempAddress.size();i++)
		if (tempAddress[i] == '.')
			tempAddress[i] = '_';
	char buf[128];
	snprintf(buf, sizeof(buf), "tws_md&%s&%u&%u", tempAddress.c_str(), m_uPort, m_uClientID);
	return string(buf);
}

string CTWS_MDPlugin::GetProspectiveKeyword(const ptree & in)
{
	string retKey = "tws_md&";
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

void CTWS_MDPlugin::GetState(ptree & out)
{
	if(m_boolIsOnline)
		out.put("online","true");
	else
		out.put("online", "false");
	out.put("serveraddress", m_strServerAddress);
	out.put("port", m_uPort);
	out.put("clientid", m_uClientID);
}

void CTWS_MDPlugin::MDInit(const ptree & in)
{
	auto temp = in.find("serveraddress");
	if (temp != in.not_found())
	{
		
		m_strServerAddress = temp->second.data();
		if (m_strServerAddress.size()>64)
			throw std::runtime_error("tws:serveraddress is too long(Max:64 char)");
		else if(m_strServerAddress.empty())
			throw std::runtime_error("tws:serveraddress is empty");
	}
	else
		throw std::runtime_error("tws:Can not find <serveraddress>");


	temp = in.find("port");
	if (temp != in.not_found())
	{
		m_uPort = atoi(temp->second.data().c_str());
		if(0 == m_uPort)
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
		throw std::runtime_error("tws:Can not find <cliendid>");

	m_StartAndStopCtrlTimer.expires_from_now(boost::posix_time::seconds(3));
	m_StartAndStopCtrlTimer.async_wait(boost::bind(
		&CTWS_MDPlugin::TimerHandler,
		this,
		&m_StartAndStopCtrlTimer,
		boost::asio::placeholders::error));
	m_futTimerThreadFuture=std::async(std::launch::async,[this] {
		this->m_IOservice.run();
		return true;
	});

}

void CTWS_MDPlugin::MDHotUpdate(const ptree & NewConfig)
{
	MDUnload();
	m_IOservice.reset();
	MDInit(NewConfig);
}

void CTWS_MDPlugin::TimerHandler(boost::asio::deadline_timer* timer, const boost::system::error_code& err)
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
		else if (tid >= time_duration(20, 49, 0, 0) && tid < time_duration(23,59,59,999))
		{
			Start();
			nextActiveTime = ptime(second_clock::universal_time().date()+days(1), time_duration(20, 0, 30, 0));
		}
		
		m_StartAndStopCtrlTimer.expires_at(nextActiveTime);
		m_StartAndStopCtrlTimer.async_wait(boost::bind(
			&CTWS_MDPlugin::TimerHandler,
			this,
			&m_StartAndStopCtrlTimer,
			boost::asio::placeholders::error));
		ShowMessage(normal, "%s: Next:%s", GetCurrentKeyword().c_str(), to_simple_string(nextActiveTime).c_str());
	}
}

bool CTWS_MDPlugin::Start()
{
	CAutoPend pend(m_abIsPending);
	if (false == m_boolIsOnline)
	{
		m_uRequestID = 0;
		m_boolIsOnline = false;
		m_pUserApi = std::shared_ptr<MTwsApi>(MTwsApi::CreateApi(), [](MTwsApi* p) {if (p) p->Release();});
		if (nullptr == m_pUserApi)
		{
			ShowMessage(
				severity_levels::error,
				"%s CreateApi error",
				GetCurrentKeyword().c_str());
			return false;
		}
		m_pUserApi->RegisterSpi(this);
		char ServerAddress[65];
		strcpy(ServerAddress, m_strServerAddress.c_str());
		m_pUserApi->Connect(ServerAddress,m_uPort,m_uClientID);
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

void CTWS_MDPlugin::Stop()
{
	CAutoPend pend(m_abIsPending);
	if (true==m_boolIsOnline)
	{
		boost::shared_lock<boost::shared_mutex> lg(m_mapObserverStructProtector);//Ð´Ëø
		
		for (auto p : m_mapInsid2Strategys)
		{
			CTwsContractField req;
			String2CTwsContractField(p.first, req);
			if (0 != m_pUserApi->UnSubscribeMarketData(&req))
				ShowMessage(severity_levels::error, "send unsubscribemarketdata failed.");
		}
		m_pUserApi->DisConnect();
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

void CTWS_MDPlugin::MDUnload()
{
	
	Stop();
	auto Count=m_StartAndStopCtrlTimer.cancel();
	m_IOservice.stop();
	m_futTimerThreadFuture.get();
	
}

void CTWS_MDPlugin::Pause()
{
	m_adbIsPauseed.store(true);
}

void CTWS_MDPlugin::Continue()
{
	m_adbIsPauseed.store(false);
}

void CTWS_MDPlugin::ShowMessage(severity_levels lv, const char * fmt, ...)
{
	char buf[512];
	va_list arg;
	va_start(arg, fmt);
	vsnprintf(buf, 512, fmt, arg);
	va_end(arg);
	boost::log::sources::severity_logger< severity_levels > m_Logger;
	BOOST_LOG_SEV(m_Logger, lv) << ProcessName << ": " << buf << " [" << to_iso_string(microsec_clock::universal_time()) << "]";
}

void CTWS_MDPlugin::MDAttachStrategy(
	MStrategy * strategy,
	TMarketDataIdType dataid,
	const unordered_map<string, string> & insConfig,
	boost::shared_mutex & mtx,
	atomic_uint_least64_t * updatetime)
{
	boost::unique_lock<boost::shared_mutex> lg(m_mapObserverStructProtector);//Ð´Ëø
	ptree instrumentTree;
	stringstream ss;
	for(auto & node:insConfig)
		instrumentTree.put(node.first,node.second);
	instrumentTree.sort([](ptree::value_type &a, ptree::value_type &b) {return a.first < b.first;});
	write_json(ss,instrumentTree);
	auto InstrumentID = ss.str();
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


		tick.m_boolValid = false;
		tick.m_longConId = 0;
		memset(tick.m_strSecType,0,sizeof(tick.m_strSecType));
		memset(tick.m_strExpiry, 0, sizeof(tick.m_strExpiry));
		//tick.m_dbStrike = 0.0;
		//memset(tick.m_strRight, 0, sizeof(tick.m_strRight));
		//memset(tick.m_strMultiplier, 0, sizeof(tick.m_strMultiplier));
		memset(tick.m_strExchange, 0, sizeof(tick.m_strExchange));
		memset(tick.m_strPrimaryExchange, 0, sizeof(tick.m_strPrimaryExchange));
		memset(tick.m_strCurrency, 0, sizeof(tick.m_strCurrency));
		//memset(tick.m_strLocalSymbol, 0, sizeof(tick.m_strLocalSymbol));
		//memset(tick.m_strTradingClass, 0, sizeof(tick.m_strTradingClass));
		//tick.m_boolIncludeExpired = false;
		//memset(tick.m_strSecIdType, 0, sizeof(tick.m_strSecIdType));
		//memset(tick.m_strSecId, 0, sizeof(tick.m_strSecId));
		//tick.m_dbOpenPrice = 0.0;
		//tick.m_dbHighPrice = 0.0;
		//tick.m_dbLowPrice = 0.0;
		//tick.m_dbClosePrice = 0.0;
		//tick.m_intLastSize = 0;

		#pragma endregion	
		if (m_boolIsOnline)
		{
			CTwsContractField req;
			String2CTwsContractField(InstrumentID, req);
			if (0 != m_pUserApi->SubscribeMarketData(&req))
				ShowMessage(
					severity_levels::error,
					"send subscribemarketdata(%s) failed.", 
					InstrumentID.c_str());
			else
				ShowMessage(
					severity_levels::normal,
					"trying to subscribe %s",
					InstrumentID.c_str());
		}
	}
}

void CTWS_MDPlugin::MDDetachStrategy(MStrategy * strategy)
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

				CTwsContractField req;
				String2CTwsContractField(ins, req);
				if (0 != m_pUserApi->UnSubscribeMarketData(&req))
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
				
			}
		}
	}
	m_mapStrategy2Insids.erase(strategy);

}

#pragma region CThostFtdcMdSpi
void CTWS_MDPlugin::OnRspUserLogin(CTwsRspUserLoginField * loginField, bool IsSucceed)
{
	
	ShowMessage(
		severity_levels::normal,
		"%s NextValidId:%d ServerTime:%d ManagedAccounts:%s",
		GetCurrentKeyword().c_str(), 
		loginField->m_intNextValidId, 
		loginField->m_LongServerTime,
		loginField->m_strManagedAccounts);

	ShowMessage(
		severity_levels::normal,
		"AccountInfo: %s login succeed!",
		GetCurrentKeyword().c_str());
	m_boolIsOnline = true;
	std::unique_lock<std::mutex> lk(m_mtxLoginSignal);
	m_cvLoginSignalCV.notify_all();
}

void CTWS_MDPlugin::OnRtnDepthMarketData(CTwsDepthMarketDataField * pDepthMarketData)
{
	if (m_adbIsPauseed)
		return;
	if (0 == pDepthMarketData->m_strSymbol[0])
		return;
	boost::shared_lock<boost::shared_mutex> lg(m_mapObserverStructProtector);//¶ÁËø
	ptree instrumentTree;
	if(strlen(pDepthMarketData->m_strSymbol)!=0)
		instrumentTree.put("symbol", pDepthMarketData->m_strSymbol);
	else
		instrumentTree.put("symbol", "_null_");

	if(strlen(pDepthMarketData->m_strSecType)!=0)
		instrumentTree.put("sectype", pDepthMarketData->m_strSecType);
	else
		instrumentTree.put("sectype", "_null_");

	if(strlen(pDepthMarketData->m_strExpiry)!=0)
		instrumentTree.put("expiry", pDepthMarketData->m_strExpiry);
	else
		instrumentTree.put("expiry", "_null_");
	if(abs(pDepthMarketData->m_dbStrike)>10e-5)
		instrumentTree.put("strike", pDepthMarketData->m_dbStrike);
	else
		instrumentTree.put("strike", "_null_");

	if(strlen(pDepthMarketData->m_strRight)!=0)
		instrumentTree.put("right", pDepthMarketData->m_strRight);
	else
		instrumentTree.put("right", "_null_");

	if(strlen(pDepthMarketData->m_strMultiplier)!=0)
		instrumentTree.put("multiplier", pDepthMarketData->m_strMultiplier);
	else
		instrumentTree.put("multiplier", "_null_");

	if(strlen(pDepthMarketData->m_strExchange)!=0)
		instrumentTree.put("exchange", pDepthMarketData->m_strExchange);
	else
		instrumentTree.put("exchange", "_null_");

	if(strlen(pDepthMarketData->m_strPrimaryExchange)!=0)
		instrumentTree.put("primaryexchange", pDepthMarketData ->m_strPrimaryExchange);
	else
		instrumentTree.put("primaryexchange", "_null_");

	if(strlen(pDepthMarketData->m_strCurrency)!=0)
		instrumentTree.put("currency", pDepthMarketData ->m_strCurrency);
	else
		instrumentTree.put("currency", "_null_");

	if(strlen(pDepthMarketData->m_strLocalSymbol)!=0)
		instrumentTree.put("localsymbol", pDepthMarketData ->m_strLocalSymbol);
	else
		instrumentTree.put("localsymbol", "_null_");

	if(strlen(pDepthMarketData->m_strTradingClass)!=0)
		instrumentTree.put("tradingclass", pDepthMarketData ->m_strTradingClass);
	else
		instrumentTree.put("tradingclass", "_null_");

	instrumentTree.put("type", "tws");
	instrumentTree.sort([](ptree::value_type &a, ptree::value_type &b) {return a.first < b.first;});
	stringstream ss;
	write_json(ss, instrumentTree);
	auto InstrumentID = ss.str();
	auto & InstrumentNode = m_mapInsid2Strategys[InstrumentID];
	auto & tick = InstrumentNode.first;
	try {
		tick.m_uDataID;

		snprintf(tick.m_strInstrumentID,sizeof(tick.m_strInstrumentID),"%s%s", pDepthMarketData->m_strSymbol, pDepthMarketData->m_strExpiry);
		tick.m_datetimeUTCDateTime= microsec_clock::universal_time();
		tick.m_dbLastPrice = pDepthMarketData->m_dbLastPrice;
		tick.m_intVolume = pDepthMarketData->m_intLastSize;
		tick.m_dbBidPrice[0] = pDepthMarketData->m_dbBidPrice1;
		tick.m_dbAskPrice[0] = pDepthMarketData->m_dbAskPrice1;
		tick.m_intBidVolume[0] = pDepthMarketData->m_intBidVolume1;
		tick.m_intAskVolume[0] = pDepthMarketData->m_intAskVolume1;



		tick.m_boolValid = pDepthMarketData->m_boolValid;
		tick.m_longConId = pDepthMarketData->m_longConId ;
		strcpy(tick.m_strSecType,pDepthMarketData->m_strSecType);
		strcpy(tick.m_strExpiry,pDepthMarketData->m_strExpiry) ;
		//tick.m_dbStrike = pDepthMarketData->m_dbStrike ;
		//strcpy(tick.m_strRight,pDepthMarketData->m_strRight);
		//strcpy(tick.m_strMultiplier, pDepthMarketData->m_strMultiplier);
		strcpy(tick.m_strExchange,pDepthMarketData->m_strExchange);
		strcpy(tick.m_strPrimaryExchange,pDepthMarketData->m_strPrimaryExchange);
		strcpy(tick.m_strCurrency,pDepthMarketData->m_strCurrency);
		//strcpy(tick.m_strLocalSymbol,pDepthMarketData->m_strLocalSymbol);
		//strcpy(tick.m_strTradingClass,pDepthMarketData->m_strTradingClass);
		//tick.m_boolIncludeExpired = pDepthMarketData->m_boolIncludeExpired ;
		//strcpy(tick.m_strSecIdType,pDepthMarketData->m_strSecIdType);
		//strcpy(tick.m_strSecId,pDepthMarketData->m_strSecId);
		//tick.m_dbOpenPrice = pDepthMarketData->m_dbOpenPrice;
		//tick.m_dbHighPrice = pDepthMarketData->m_dbHighPrice;
		//tick.m_dbLowPrice = pDepthMarketData->m_dbLowPrice;
		//tick.m_dbClosePrice = pDepthMarketData->m_dbClosePrice;
		tick.m_intLastSize = pDepthMarketData->m_intLastSize;
	}
	catch (std::exception & err)
	{
		ShowMessage(severity_levels::error, "%s: error :%s", GetCurrentKeyword().c_str(), err.what());
		return;
	}
	catch (...)
	{
		ShowMessage(severity_levels::error, "%s: error :unknow", GetCurrentKeyword().c_str());
		return;
	}

	for (auto & node : InstrumentNode.second)
	{
		boost::unique_lock<boost::shared_mutex> lk(*get<2>(node), boost::try_to_lock);
		if (lk.owns_lock())
		{
			tick.m_uDataID = get<1>(node);
			get<0>(node)->OnTick(get<1>(node), &tick);
			if (0 == get<1>(node))
			{
				ptime t = pDepthMarketData->m_datetimeUTCDateTime;
				uint_least64_t current = ((uint_least64_t)t.date().year()) * 10000000000000;
				current += ((uint_least64_t)t.date().month().as_number()) * 100000000000;
				current += ((uint_least64_t)t.date().day().as_number()) * 1000000000;
				current += ((uint_least64_t)t.time_of_day().hours()) * 10000000;
				current += ((uint_least64_t)t.time_of_day().minutes()) * 100000;
				current += ((uint_least64_t)t.time_of_day().seconds()) * 1000;
				current += ((uint_least64_t)t.time_of_day().fractional_seconds()) / 1000;
				get<3>(node)->store(current);
			}
		}
		else
			ShowMessage(severity_levels::warning,
				"%s: onrtndepthmarketdata try lock failed:%s",
				GetCurrentKeyword().c_str(),
				tick.m_strInstrumentID);
	}
}

void CTWS_MDPlugin::OnRspError(int ErrID, int ErrCode, const char * ErrMsg)
{
		ShowMessage(severity_levels::error,
			"%s onrsperror.errorid=%d, errorcode=%d,errormsg=%s",
			GetCurrentKeyword().c_str(),
			ErrID,
			ErrCode, ErrMsg==nullptr?"unknown": ErrMsg);
}

void CTWS_MDPlugin::OnDisconnected()
{
	ShowMessage(severity_levels::error,
		"AccountInfo: %s OnDisconnected",
		GetCurrentKeyword().c_str());
	m_boolIsOnline = false;
}

#pragma endregion