#include "TEMPLATE_ANY_TDPlugin.h"
#include <stdarg.h>
#include <thread>
#include "OrderRefResolve.h"
#include "AutoPend.h"
extern char ProcessName[256];
#define NOTIFY_LOGIN_SUCCEED {m_boolIsOnline = true; std::unique_lock<std::mutex> lk(m_mtxLoginSignal);m_cvLoginSignalCV.notify_all();}
#define NOTIFY_LOGIN_FAILED  {m_boolIsOnline = false;std::unique_lock<std::mutex> lk(m_mtxLoginSignal);m_cvLoginSignalCV.notify_all();}

const string CTEMPLATE_ANY_TDPlugin::s_strAccountKeyword = "maxcancelperday;";
#define NAME ("template_td")
date CTEMPLATE_ANY_TDPlugin::GetTradeday(ptime _Current)
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
CTEMPLATE_ANY_TDPlugin::CTEMPLATE_ANY_TDPlugin():m_abIsPending(false)
{

}

CTEMPLATE_ANY_TDPlugin::~CTEMPLATE_ANY_TDPlugin()
{

}

bool CTEMPLATE_ANY_TDPlugin::IsPedding()
{
	return m_abIsPending.load();
}

bool CTEMPLATE_ANY_TDPlugin::IsOnline()
{
	return true;
}

void CTEMPLATE_ANY_TDPlugin::IncreaseRefCount()
{
	m_intRefCount++;
}

void CTEMPLATE_ANY_TDPlugin::DescreaseRefCount()
{
	m_intRefCount--;
}

int CTEMPLATE_ANY_TDPlugin::GetRefCount()
{
	return m_intRefCount;
}

void CTEMPLATE_ANY_TDPlugin::CheckSymbolValidity(const unordered_map<string, string> & insConfig)
{
	
}

string CTEMPLATE_ANY_TDPlugin::GetCurrentKeyword()
{
	return NAME;
}

string CTEMPLATE_ANY_TDPlugin::GetProspectiveKeyword(const ptree & in)
{
	string retKey = NAME;
	return retKey;
}

void CTEMPLATE_ANY_TDPlugin::GetState(ptree & out)
{
	out.put("online", "true");
}

void CTEMPLATE_ANY_TDPlugin::TDInit(const ptree & in, MTradePluginContextInterface * pTradePluginContext, unsigned int AccountNumber)
{
	auto MaxCancelPerDayNode = in.find("maxcancelperday");
	if (MaxCancelPerDayNode != in.not_found())
		m_intInitAmountOfCancelChancesPerDay = atoi(MaxCancelPerDayNode->second.data().c_str());
	else
		m_intInitAmountOfCancelChancesPerDay = 400;

	m_dateTradeDay = GetTradeday(second_clock::universal_time());
	map<string, int>().swap(m_mapCancelAmount);
}

void CTEMPLATE_ANY_TDPlugin::TDHotUpdate(const ptree &)
{

}

void CTEMPLATE_ANY_TDPlugin::TimerHandler(boost::asio::deadline_timer* timer, const boost::system::error_code& err)
{
	
}

bool CTEMPLATE_ANY_TDPlugin::Start()
{
	CAutoPend pend(m_abIsPending);
	return true;
}

bool CTEMPLATE_ANY_TDPlugin::Stop()
{

	CAutoPend pend(m_abIsPending);
	return true;
}

void CTEMPLATE_ANY_TDPlugin::TDUnload()
{
	
	Stop();
}

void CTEMPLATE_ANY_TDPlugin::ShowMessage(severity_levels lv, const char * fmt, ...)
{
	char buf[512];
	va_list arg;
	va_start(arg, fmt);
	vsnprintf(buf, 512, fmt, arg);
	va_end(arg);
	boost::log::sources::severity_logger< severity_levels > m_Logger;
	BOOST_LOG_SEV(m_Logger, lv) << ProcessName << ": " << buf << " [" << to_iso_string(microsec_clock::universal_time()) << "]";
}

TOrderRefIdType CTEMPLATE_ANY_TDPlugin::TDBasicMakeOrder(
	TOrderType ordertype,
	unordered_map<string, string> & instrument,
	TOrderDirectionType direction,
	TOrderOffsetType offset,
	TVolumeType volume,
	TPriceType LimitPrice,
	TOrderRefIdType orderRefBase)
{
	
	return orderRefBase;
}

TLastErrorIdType CTEMPLATE_ANY_TDPlugin::TDBasicCancelOrder(TOrderRefIdType Ref, unordered_map<string, string> & instrument, TOrderSysIdType orderSysId)
{
	return LB1_NO_ERROR;
}

int CTEMPLATE_ANY_TDPlugin::TDGetRemainAmountOfCancelChances(const char * ins)
{
	return std::numeric_limits<int>::max();
}

