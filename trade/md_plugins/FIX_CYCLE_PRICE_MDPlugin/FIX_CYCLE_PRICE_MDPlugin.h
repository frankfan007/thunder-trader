#ifndef QFCOMPRETRADESYSTEM_ATMMARKETDATAPLUGINS_FIX_CYCLE_PRICE_MDPLUGIN_H
#define QFCOMPRETRADESYSTEM_ATMMARKETDATAPLUGINS_FIX_CYCLE_PRICE_MDPLUGIN_H
#include <string>
#include <boost/thread.hpp>
#include <thread>                // std::thread
#include <mutex>                // std::mutex, std::unique_lock
#include <condition_variable>    // std::condition_variable
#include <atomic>
#include <boost/asio.hpp>
#include <memory>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <future>
#include <tuple>
#include <boost/log/common.hpp>


#include "SeverityLevel.h"
#include "FutureTick.h"

#include "AtmPluginInterface.h"
#include "AtmMarketDataPluginInterface.h"
using namespace boost::posix_time;
using namespace boost::gregorian;
using namespace boost::asio;
using namespace std;
class CFixCyclePricePluginImp :
	public MAtmMarketDataPluginInterface
{

	boost::log::sources::severity_logger< severity_levels > m_Logger;



	io_service  m_IOservice;
	deadline_timer m_StartAndStopCtrlTimer;
	std::future<bool> m_futTimerThreadFuture;


	string m_strCycleLen;
	string m_strPrice;
	


	boost::shared_mutex m_mapObserverStructProtector;
	unordered_map<string, pair<CTick,list< tuple < MStrategy*, TMarketDataIdType, boost::shared_mutex*, atomic_uint_least64_t *> > > > m_mapInsid2Strategys;
	unordered_map< MStrategy*, list<string> > m_mapStrategy2Insids;


public:
	static const string s_strAccountKeyword;
	CFixCyclePricePluginImp();
	~CFixCyclePricePluginImp();
	int m_intRefCount = 0;
	atomic_bool m_abIsPending;
	bool IsPedding();
	virtual bool IsOnline();
	virtual void IncreaseRefCount();
	virtual void DescreaseRefCount();
	virtual int GetRefCount();
	virtual void CheckSymbolValidity(const unordered_map<string, string> &);
	virtual string GetCurrentKeyword();
	virtual string GetProspectiveKeyword(const ptree &);
	virtual void GetState(ptree & out);
	virtual void MDInit(const ptree &);
	virtual void MDHotUpdate(const ptree &);
	virtual void MDUnload();
	atomic<bool> m_adbIsPauseed;
	virtual void Pause();
	virtual void Continue();

	virtual void MDAttachStrategy(
		MStrategy *, 
		TMarketDataIdType,
		const unordered_map<string, string> &,
		boost::shared_mutex &,
		atomic_uint_least64_t *);
	virtual void MDDetachStrategy(MStrategy*/*IN*/);
private:
	void ShowMessage(severity_levels,const char * fmt, ...);
	void TimerHandler(boost::asio::deadline_timer* timer, const boost::system::error_code& err);

};
#endif

