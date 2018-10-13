#include "FIX_CYCLE_PRICE_MDPlugin.h"
#include <stdarg.h>
#include <thread>
#include "AutoPend.h"
const string CFixCyclePricePluginImp::s_strAccountKeyword="cyclelenmillsec;price;";
extern char ProcessName[256];
#define NAME "fix_cycle_price&"
#define INSTRUMENTID_NAME "FIX_CYCLE_PRICE"
CFixCyclePricePluginImp::CFixCyclePricePluginImp():m_StartAndStopCtrlTimer(m_IOservice), m_abIsPending(false), m_adbIsPauseed(false)
{
}

CFixCyclePricePluginImp::~CFixCyclePricePluginImp()
{
}

bool CFixCyclePricePluginImp::IsPedding()
{
	return m_abIsPending.load();
}

bool CFixCyclePricePluginImp::IsOnline()
{
	return true;
}

void CFixCyclePricePluginImp::IncreaseRefCount()
{
	m_intRefCount++;
}

void CFixCyclePricePluginImp::DescreaseRefCount()
{
	m_intRefCount--;
}

int CFixCyclePricePluginImp::GetRefCount()
{
	return m_intRefCount;
}

void CFixCyclePricePluginImp::CheckSymbolValidity(const unordered_map<string, string> & insConfig)
{
	
}

string CFixCyclePricePluginImp::GetCurrentKeyword()
{
	return NAME+m_strCycleLen+"&"+m_strPrice;
}

string CFixCyclePricePluginImp::GetProspectiveKeyword(const ptree & in)
{
	string retKey = NAME;
	auto temp = in.find("cyclelenmillsec");
	if (temp != in.not_found())
	{
		retKey += temp->second.data();
	}
	else
		throw std::runtime_error("can not find <cyclelenmillsec>");
	retKey += "&";

	temp = in.find("price");
	if (temp != in.not_found())
		retKey += temp->second.data();
	else
		throw std::runtime_error("can not find <price>");

	return retKey;
}

void CFixCyclePricePluginImp::GetState(ptree & out)
{
	
	out.put("online","true");
	out.put("cyclelenmillsec", m_strCycleLen);
	out.put("price", m_strPrice);
}

void CFixCyclePricePluginImp::MDInit(const ptree & in)
{
	auto temp = in.find("cyclelenmillsec");
	if (temp != in.not_found())
	{
		m_strCycleLen = temp->second.data();
		if (0==atoi(m_strCycleLen.c_str()))
			throw std::runtime_error("cyclelenmillsec can not be zero");
	}
	else
		throw std::runtime_error("Can not find <cyclelenmillsec>");

	temp = in.find("price");
	if (temp != in.not_found())
		m_strPrice = temp->second.data();
	else
		throw std::runtime_error("Can not find <price>");
	
	m_StartAndStopCtrlTimer.expires_from_now(boost::posix_time::seconds(3));
	m_StartAndStopCtrlTimer.async_wait(boost::bind(
		&CFixCyclePricePluginImp::TimerHandler,
		this,
		&m_StartAndStopCtrlTimer,
		boost::asio::placeholders::error));
	m_futTimerThreadFuture=std::async(std::launch::async, [this] {
		this->m_IOservice.run();
		return true;
	});

}

void CFixCyclePricePluginImp::MDHotUpdate(const ptree & NewConfig)
{
	MDUnload();
	m_IOservice.reset();
	MDInit(NewConfig);
	
}

void CFixCyclePricePluginImp::TimerHandler(boost::asio::deadline_timer* timer, const boost::system::error_code& err)
{
	if (m_adbIsPauseed)
		return;
	boost::shared_lock<boost::shared_mutex> lg(m_mapObserverStructProtector);//¶ÁËø
	for (auto & InstrumentNode : m_mapInsid2Strategys)
	{
		auto & tick = InstrumentNode.second.first;
		try {
			TPriceType Price = atof(m_strPrice.c_str());
			strncpy(tick.m_strInstrumentID, InstrumentNode.first.c_str(), sizeof(tick.m_strInstrumentID));
			tick.m_intVolume = 0;
			tick.m_dbLastPrice = Price;
			tick.m_datetimeUTCDateTime = microsec_clock::universal_time();
			for (unsigned int i = 0;i < MAX_QUOTATIONS_DEPTH;i++)
			{
				tick.m_dbBidPrice[i] = Price;
				tick.m_intBidVolume[i] = 0;
				tick.m_dbAskPrice[i] = Price;
				tick.m_intAskVolume[i] = 0;
			}
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

		for (auto & node : InstrumentNode.second.second)
		{
			boost::unique_lock<boost::shared_mutex> lk(*get<2>(node), boost::try_to_lock);
			if (lk.owns_lock())
			{
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
					INSTRUMENTID_NAME);
		}
	}
	m_StartAndStopCtrlTimer.expires_from_now(microseconds(atoi(m_strCycleLen.c_str())));
	m_StartAndStopCtrlTimer.async_wait(boost::bind(
		&CFixCyclePricePluginImp::TimerHandler,
		this,
		&m_StartAndStopCtrlTimer,
		boost::asio::placeholders::error));
	
}

void CFixCyclePricePluginImp::MDUnload()
{

	m_StartAndStopCtrlTimer.cancel();
	m_IOservice.stop();
	m_futTimerThreadFuture.get();
}

void CFixCyclePricePluginImp::Pause()
{
	m_adbIsPauseed.store(true);
}

void CFixCyclePricePluginImp::Continue()
{
	m_adbIsPauseed.store(false);
}

void CFixCyclePricePluginImp::ShowMessage(severity_levels lv, const char * fmt, ...)
{
	char buf[512];
	va_list arg;
	va_start(arg, fmt);
	vsnprintf(buf, 512, fmt, arg);
	va_end(arg);
	boost::log::sources::severity_logger< severity_levels > m_Logger;
	BOOST_LOG_SEV(m_Logger, lv) << ProcessName << ": " << buf << " [" << to_iso_string(microsec_clock::universal_time()) << "]";
}

void CFixCyclePricePluginImp::MDAttachStrategy(
	MStrategy * strategy,
	TMarketDataIdType dataid,
	const unordered_map<string, string> & insConfig,
	boost::shared_mutex & mtx,
	atomic_uint_least64_t * updatetime)
{
	boost::unique_lock<boost::shared_mutex> lg(m_mapObserverStructProtector);//Ð´Ëø

	auto InstrumentID = insConfig.find("instrumentid")->second;
	m_mapStrategy2Insids[strategy].push_back(InstrumentID);
	auto findres = m_mapInsid2Strategys.find(InstrumentID);
	if (findres != m_mapInsid2Strategys.end())
		findres->second.second.push_back(make_tuple(strategy, dataid, &mtx, updatetime));
	else
	{
		m_mapInsid2Strategys[InstrumentID].second.push_back(make_tuple(strategy, dataid, &mtx, updatetime));
	}
	

}

void CFixCyclePricePluginImp::MDDetachStrategy(MStrategy * strategy)
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
	}
	m_mapStrategy2Insids.erase(strategy);

}
