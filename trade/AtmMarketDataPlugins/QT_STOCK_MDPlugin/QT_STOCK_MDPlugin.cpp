#include "QT_STOCK_MDPlugin.h"
#include <stdarg.h>
#include <thread>
#include "AutoPend.h"
const string CQT_STOCK_MDPlugin::s_strAccountKeyword = "ip;port;";
extern char ProcessName[256];
#define NAME ("quant_tech_stock_md&")

CQT_STOCK_MDPlugin::CQT_STOCK_MDPlugin():m_StartAndStopCtrlTimer(m_IOservice), m_abIsPending(false), m_adbIsPauseed(false)
{
}

CQT_STOCK_MDPlugin::~CQT_STOCK_MDPlugin()
{
}

bool CQT_STOCK_MDPlugin::IsPedding()
{
	return m_abIsPending.load();
}

bool CQT_STOCK_MDPlugin::IsOnline()
{
	return m_boolIsOnline;
}

void CQT_STOCK_MDPlugin::IncreaseRefCount()
{
	m_intRefCount++;
}

void CQT_STOCK_MDPlugin::DescreaseRefCount()
{
	m_intRefCount--;
}

int CQT_STOCK_MDPlugin::GetRefCount()
{
	return m_intRefCount;
}

void CQT_STOCK_MDPlugin::CheckSymbolValidity(const unordered_map<string, string> & insConfig)
{
	auto Type = insConfig.find("type");
	if (Type == insConfig.end())
		throw std::runtime_error("Can not find the <type> of the the symbol.");
	if (Type->second != "stock")
		throw std::runtime_error("Quanttech MarketDataSource does not support this symbol.");
	auto InstrumentNode = insConfig.find("instrumentid");
	if (InstrumentNode == insConfig.end())
		throw std::runtime_error("<instrumentid> not found.");
	auto ExchangeidNode = insConfig.find("exchangeid");
	if (ExchangeidNode == insConfig.end())
		throw std::runtime_error("<exchangeid> not found.");
}

string CQT_STOCK_MDPlugin::GetCurrentKeyword()
{
	return NAME;
}

string CQT_STOCK_MDPlugin::GetProspectiveKeyword(const ptree & in)
{
	string retKey = NAME;
	return retKey;
}

void CQT_STOCK_MDPlugin::GetState(ptree & out)
{
	if(m_boolIsOnline)
		out.put("online","true");
	else
		out.put("online", "false");
}

void CQT_STOCK_MDPlugin::MDInit(const ptree & in)
{
	auto IPNode = in.find("ip");
	if (IPNode != in.not_found())
		m_strIP = IPNode->second.data();
	else
		throw std::runtime_error("Can not find the <ip>.");
	auto PortNode = in.find("port");
	if (PortNode != in.not_found())
		m_strPort = PortNode->second.data();
	else 
		throw std::runtime_error("Can not find the <port>.");
	m_StartAndStopCtrlTimer.expires_from_now(boost::posix_time::seconds(3));
	m_StartAndStopCtrlTimer.async_wait(boost::bind(
		&CQT_STOCK_MDPlugin::TimerHandler,
		this,
		&m_StartAndStopCtrlTimer,
		boost::asio::placeholders::error));
	m_futTimerThreadFuture=std::async(std::launch::async,[this] {
		this->m_IOservice.run();
		return true;
	});

}

void CQT_STOCK_MDPlugin::MDHotUpdate(const ptree & NewConfig)
{
	MDUnload();
	m_IOservice.reset();
	MDInit(NewConfig);
	
}

void CQT_STOCK_MDPlugin::TimerHandler(boost::asio::deadline_timer* timer, const boost::system::error_code& err)
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
		else if (tid >= time_duration(1, 15, 0, 0) && tid < time_duration(3, 30, 0, 0))
		{
			Start();
			nextActiveTime = ptime(second_clock::universal_time().date(), time_duration(3, 30, 30, 0));
		}
		else if (tid >= time_duration(3, 30, 0, 0) && tid < time_duration(4, 59, 0, 0))
		{
			Stop();
			nextActiveTime = ptime(second_clock::universal_time().date(), time_duration(4, 59, 30, 0));
		}
		else if (tid >= time_duration(4, 59, 0, 0) && tid < time_duration(7, 0, 0, 0))
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
			&CQT_STOCK_MDPlugin::TimerHandler,
			this,
			&m_StartAndStopCtrlTimer,
			boost::asio::placeholders::error));
		ShowMessage(normal, "%s: Next:%s", GetCurrentKeyword().c_str(), to_simple_string(nextActiveTime).c_str());
	}
}

bool CQT_STOCK_MDPlugin::Start()
{
	CAutoPend pend(m_abIsPending);
	if (false == m_boolIsOnline)
	{
		m_uRequestID = 0;
		m_boolIsOnline = false;
		m_pUserApi = std::shared_ptr<MQuantTechStockQuotationInterface>(
			MQuantTechStockQuotationInterface::Create(),
			[](MQuantTechStockQuotationInterface* p) {if (p) p->Release();});
		if (nullptr == m_pUserApi)
		{
			ShowMessage(
				severity_levels::error,
				"%s Create error",
				GetCurrentKeyword().c_str());
			return false;
		}
		try {
			m_pUserApi->Init(m_strIP,m_strPort,this);
		}catch (std::exception &err)
		{
			ShowMessage(severity_levels::error, "Init failed:%s", err.what());
			return false;
		}
		m_boolIsOnline = true;
		ShowMessage(
			severity_levels::normal,
			"AccountInfo: %s login succeed!",
			GetCurrentKeyword().c_str());

		boost::shared_lock<boost::shared_mutex> lg(m_mapObserverStructProtector);//Ð´Ëø
		for (auto & ins : m_mapInsid2Strategys)
		{
			try {
				m_pUserApi->SubscribeMarketData(ins.first.c_str());
			}
			catch (std::exception &err)
			{
				ShowMessage(severity_levels::error, "send subscribemarketdata failed:", err.what());
			}
		}
		return true;
	}
	else return true;
	
}

void CQT_STOCK_MDPlugin::Stop()
{
	CAutoPend pend(m_abIsPending);
	if (m_boolIsOnline)
	{
		boost::shared_lock<boost::shared_mutex> lg(m_mapObserverStructProtector);//Ð´Ëø
		for (auto & ins : m_mapInsid2Strategys)
		{

			try {
				m_pUserApi->UnSubscribeMarketData(ins.first.c_str());
			}
			catch (std::exception &err)
			{
				ShowMessage(severity_levels::error, "send unsubscribemarketdata failed:", err.what());
			}
		}
	}
	if (m_pUserApi)
	{
		try {
			m_pUserApi->UnInit();
		}
		catch (std::exception &err)
		{
			ShowMessage(severity_levels::error, "UnInit failed:", err.what());
		}
		m_pUserApi=nullptr;
	}
	ShowMessage(
		severity_levels::normal,
		"AccountInfo: %s loginout succeed!",
		GetCurrentKeyword().c_str());
	m_boolIsOnline = false;
}

void CQT_STOCK_MDPlugin::MDUnload()
{
	
	Stop();
	m_StartAndStopCtrlTimer.cancel();
	m_IOservice.stop();
	auto a=m_futTimerThreadFuture.get();
}

void CQT_STOCK_MDPlugin::Pause()
{
	m_adbIsPauseed.store(true);
}

void CQT_STOCK_MDPlugin::Continue()
{
	m_adbIsPauseed.store(false);
}

void CQT_STOCK_MDPlugin::ShowMessage(severity_levels lv, const char * fmt, ...)
{
	char buf[512];
	va_list arg;
	va_start(arg, fmt);
	vsnprintf(buf, 512, fmt, arg);
	va_end(arg);
	boost::log::sources::severity_logger< severity_levels > m_Logger;
	BOOST_LOG_SEV(m_Logger, lv) << ProcessName << ": " << buf<<" ["<<to_iso_string(microsec_clock::universal_time())<<"]";
}

void CQT_STOCK_MDPlugin::MDAttachStrategy(
	MStrategy * strategy,
	TMarketDataIdType dataid,
	const unordered_map<string, string> & insConfig,
	boost::shared_mutex & mtx,
	atomic_uint_least64_t * updatetime)
{
	boost::unique_lock<boost::shared_mutex> lg(m_mapObserverStructProtector);//Ð´Ëø

	auto InstrumentID = insConfig.find("exchangeid")->second+insConfig.find("instrumentid")->second;
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

		tick.m_dbUpperLimitPrice = 0;
		tick.m_dbLowerLimitPrice = 0;
		tick.m_dbOpenPrice = 0;
		tick.m_dbHighestPrice = 0;
		tick.m_dbLowestPrice = 0;
		tick.m_dbPreClosePrice = 0;
#pragma endregion
		if (m_boolIsOnline)
		{
			
			try {
				m_pUserApi->SubscribeMarketData(InstrumentID.c_str());
			}
			catch (std::exception & err)
			{
				ShowMessage(
					severity_levels::error,
					"send subscribemarketdata(%s) failed.",
					InstrumentID.c_str());
			}
			ShowMessage(
				severity_levels::normal,
				"trying to subscribe %s",
				InstrumentID.c_str());
		}
	}
}

void CQT_STOCK_MDPlugin::MDDetachStrategy(MStrategy * strategy)
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
				try {
					m_pUserApi->UnSubscribeMarketData(ins.c_str());
				}
				catch (std::exception & err)
				{
					ShowMessage(
						severity_levels::error,
						"send subscribemarketdata(%s) failed.",
						ins.c_str());
				}
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

void CQT_STOCK_MDPlugin::OnRspError(const char * errormsg)
{
	if (errormsg)
	{
		//m_boolIsOnline=false;
		ShowMessage(severity_levels::error,
			"%s onrsperror.errormsg=%s", 
			GetCurrentKeyword().c_str(), errormsg);
	}
}

void CQT_STOCK_MDPlugin::OnRspSubMarketData(const char * symbol)
{
	if (symbol)
	{
		ShowMessage(severity_levels::normal,
			"submarketdata %s succeed.",
			symbol);
	}
}

void CQT_STOCK_MDPlugin::OnRspUnSubMarketData(const char * symbol)
{
	if (symbol)
	{
		ShowMessage(severity_levels::normal,
			"unsubmarketdata %s succeed.",
			symbol);
	}
}

void CQT_STOCK_MDPlugin::OnRtnDepthMarketData(const CStockTick * pDepthMarketData)
{
	if (m_adbIsPauseed)
		return;
	boost::shared_lock<boost::shared_mutex> lg(m_mapObserverStructProtector);//¶ÁËø
	auto & InstrumentNode = m_mapInsid2Strategys[pDepthMarketData->m_strInstrumentID];
	auto & tick = InstrumentNode.first;
	InstrumentNode.first = *pDepthMarketData;

	for(auto & node: InstrumentNode.second)
	{
		boost::unique_lock<boost::shared_mutex> lk(*get<2>(node), boost::try_to_lock);
		if (lk.owns_lock())
		{
			tick.m_uDataID = get<1>(node);
			get<0>(node)->OnTick(get<1>(node), &tick);
			if (0 == get<1>(node))
			{
				ptime t = microsec_clock::local_time();
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
				pDepthMarketData->m_strInstrumentID);
	}

}

#pragma endregion