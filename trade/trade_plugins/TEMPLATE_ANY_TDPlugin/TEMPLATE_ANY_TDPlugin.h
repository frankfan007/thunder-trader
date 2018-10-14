#ifndef QFCOMPRETRADESYSTEM_ATMTRADEPLUGINS_TEMPLATE_TDPLUGIN_TEMPLATE_ANY_TDPlugin_H_
#define QFCOMPRETRADESYSTEM_ATMTRADEPLUGINS_TEMPLATE_TDPLUGIN_TEMPLATE_ANY_TDPlugin_H_
#include <boost/thread.hpp>
#include <thread>                // std::thread
#include <mutex>                // std::mutex, std::unique_lock
#include <condition_variable>    // std::condition_variable
#include <atomic>
#include <boost/asio.hpp>
#include <memory>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <thread>
#include <future>
#include <tuple>
#include <boost/log/common.hpp>
#include "AtmTradePluginInterface.h"
#include "TradePluginContextInterface.h"

#include "SeverityLevel.h"

using namespace boost::posix_time;
using namespace boost::gregorian;
using namespace boost::asio;
using namespace std;

class CTEMPLATE_ANY_TDPlugin :
	public MAtmTradePluginInterface
{


	boost::log::sources::severity_logger< severity_levels > m_Logger;



	date GetTradeday(ptime _Current);
	date m_dateTradeDay;
	boost::shared_mutex m_mtxProtectCancelAmount;
	map<string, int> m_mapCancelAmount;
	int m_intInitAmountOfCancelChancesPerDay;

	
public:
	static const string s_strAccountKeyword;
	CTEMPLATE_ANY_TDPlugin();
	~CTEMPLATE_ANY_TDPlugin();
	int m_intRefCount = 0;
	atomic_bool m_abIsPending = false;
	bool IsPedding();
	virtual bool IsOnline();
	virtual void IncreaseRefCount();
	virtual void DescreaseRefCount();
	virtual int GetRefCount();
	virtual void CheckSymbolValidity(const unordered_map<string, string> &);
	virtual string GetCurrentKeyword();
	virtual string GetProspectiveKeyword(const ptree &);
	virtual void GetState(ptree & out);
	virtual void TDInit(const ptree &, MTradePluginContextInterface*, unsigned int AccountNumber);
	virtual void TDHotUpdate(const ptree &);
	virtual void TDUnload();


	virtual TOrderRefIdType TDBasicMakeOrder(
		TOrderType ordertype,
		unordered_map<string, string> & instrument,
		TOrderDirectionType direction,
		TOrderOffsetType offset,
		TVolumeType volume,
		TPriceType LimitPrice,
		TOrderRefIdType orderRefBase
		);
	
	virtual TLastErrorIdType TDBasicCancelOrder(TOrderRefIdType, unordered_map<string, string> &, TOrderSysIdType);
	virtual int TDGetRemainAmountOfCancelChances(const char *);
private:
	bool Start();
	bool Stop();
	void ShowMessage(severity_levels, const char * fmt, ...);
	void TimerHandler(boost::asio::deadline_timer* timer, const boost::system::error_code& err);


};
#endif

