#ifndef _QFCOMPRETRADESYSTEM_ATMMARKETDATAPLUGINS_CTP_MDPLUGIN_CTP_FUTURE_MDPlugin_H_
#define _QFCOMPRETRADESYSTEM_ATMMARKETDATAPLUGINS_CTP_MDPLUGIN_CTP_FUTURE_MDPlugin_H_
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

#ifdef WIN32
#include "ThostFtdcMdApi.h"
#include "ThostFtdcTraderApi.h"
#include "ThostFtdcUserApiDataType.h"
#include "ThostFtdcUserApiStruct.h"
#else
#include "ThostFtdcMdApi.h"
#include "ThostFtdcTraderApi.h"
#include "ThostFtdcUserApiDataType.h"
#include "ThostFtdcUserApiStruct.h"
#endif

#include "SeverityLevel.h"
#include <vector>
#include "FutureTick.h"
#include "AtmPluginInterface.h"
#include "AtmMarketDataPluginInterface.h"
using namespace boost::posix_time;
using namespace boost::gregorian;
using namespace boost::asio;
using namespace std;
class CCTP_FUTURE_MDPlugin :
	public MAtmMarketDataPluginInterface,
	public CThostFtdcMdSpi
{


	boost::log::sources::severity_logger< severity_levels > m_Logger;



	io_service  m_IOservice;
	deadline_timer m_StartAndStopCtrlTimer;
	std::future<bool> m_futTimerThreadFuture;


	string m_strServerAddress;
	string m_strBrokerID;
	string m_strUsername;
	string m_strPassword;


	std::shared_ptr<CThostFtdcMdApi> m_pUserApi;



	unsigned int m_uRequestID = 0;
	bool m_boolIsOnline = false;



	std::mutex m_mtxLoginSignal;
	condition_variable m_cvLoginSignalCV;



	boost::shared_mutex m_mapObserverStructProtector;
	unordered_map<string, pair<CFutureTick,list< tuple < MStrategy*, TMarketDataIdType, boost::shared_mutex*,atomic_uint_least64_t *> > > > m_mapInsid2Strategys;
	unordered_map< MStrategy*, list<string> > m_mapStrategy2Insids;


public:
	static const string s_strAccountKeyword;
	CCTP_FUTURE_MDPlugin();
	~CCTP_FUTURE_MDPlugin();
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
	bool Start();
	void Stop();
	void ShowMessage(severity_levels,const char * fmt, ...);
	void TimerHandler(boost::asio::deadline_timer* timer, const boost::system::error_code& err);

	virtual void OnFrontConnected();
	virtual void OnFrontDisconnected(int nReason);
	virtual void OnHeartBeatWarning(int nTimeLapse);
	virtual void OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	virtual void OnRspUserLogout(CThostFtdcUserLogoutField *pUserLogout, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	virtual void OnRspError(CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	virtual void OnRspSubMarketData(CThostFtdcSpecificInstrumentField *pSpecificInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	virtual void OnRspUnSubMarketData(CThostFtdcSpecificInstrumentField *pSpecificInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	virtual void OnRspSubForQuoteRsp(CThostFtdcSpecificInstrumentField *pSpecificInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	virtual void OnRspUnSubForQuoteRsp(CThostFtdcSpecificInstrumentField *pSpecificInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	virtual void OnRtnDepthMarketData(CThostFtdcDepthMarketDataField *pDepthMarketData);
	virtual void OnRtnForQuoteRsp(CThostFtdcForQuoteRspField *pForQuoteRsp);

};
#endif

