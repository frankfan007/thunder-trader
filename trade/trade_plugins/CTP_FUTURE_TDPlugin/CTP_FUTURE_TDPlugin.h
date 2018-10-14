#ifndef _QFCOMPRETRADESYSTEM_ATMTRADEPLUGINS_CTP_TDPLUGIN_CTP_FUTURE_TDPlugin_H_
#define _QFCOMPRETRADESYSTEM_ATMTRADEPLUGINS_CTP_TDPLUGIN_CTP_FUTURE_TDPlugin_H_
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

using namespace boost::posix_time;
using namespace boost::gregorian;
using namespace boost::asio;
using namespace std;

class CCTP_FUTURE_TDPlugin :
	public MAtmTradePluginInterface,
	public CThostFtdcTraderSpi
{


	boost::log::sources::severity_logger< severity_levels > m_Logger;



	io_service  m_IOservice;
	deadline_timer m_StartAndStopCtrlTimer;
	std::future<bool> m_futTimerThreadFuture;



	MTradePluginContextInterface * m_pTradePluginContext = nullptr;
	CThostFtdcTraderApi * m_pUserApi = nullptr;//Init at Start()



	string m_strServerAddress;//Init at TDInit
	string m_strBrokerID;//Init at TDInit
	string m_strUsername;//Init at TDInit
	string m_strPassword;//Init at TDInit
	bool m_boolIsOnline = false;//Init at Start()
	unsigned int m_uAccountNumber = 0;//Init at CCTP_FUTURE_TDPlugin()
	unsigned int m_uRequestID = 0;//Init at Start()
	unsigned int m_uIncreasePart = 0;//Init at OnRspUserLogin()
	TThostFtdcFrontIDType m_intFrontID = 0;//Init at OnRspUserLogin()
	TThostFtdcSessionIDType m_intSessionID = 0;//Init at OnRspUserLogin()



	std::mutex m_mtxLoginSignal;
	condition_variable m_cvLoginSignalCV;
	std::mutex m_mtxLogoutSignal;
	condition_variable m_cvLogoutSignalCV;



	date GetTradeday(ptime _Current);
	date m_dateTradeDay;
	boost::shared_mutex m_mtxProtectCancelAmount;
	map<string, int> m_mapCancelAmount;
	int m_intInitAmountOfCancelChancesPerDay;

	
public:
	static const string s_strAccountKeyword;
	CCTP_FUTURE_TDPlugin();
	~CCTP_FUTURE_TDPlugin();
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
	virtual void TDInit(const ptree &, MTradePluginContextInterface*, unsigned int AccountNumber);
	virtual void TDHotUpdate(const ptree &);
	virtual void TDUnload();

	void UpdateAccountInfo(const ptree & in);

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


	virtual void OnFrontConnected();
	virtual void OnFrontDisconnected(int nReason);
	virtual void OnHeartBeatWarning(int nTimeLapse);
	virtual void OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	virtual void OnRspUserLogout(CThostFtdcUserLogoutField *pUserLogout, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	virtual void OnRtnOrder(CThostFtdcOrderField *pOrder);
	virtual void OnRtnTrade(CThostFtdcTradeField *pTrade);
	virtual void OnRspOrderInsert(CThostFtdcInputOrderField *pInputOrder, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	virtual void OnRspOrderAction(CThostFtdcInputOrderActionField *pInputOrderAction, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	virtual void OnRspSettlementInfoConfirm(CThostFtdcSettlementInfoConfirmField *pSettlementInfoConfirm, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	virtual void OnRspError(CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	virtual void OnErrRtnOrderInsert(CThostFtdcInputOrderField *pInputOrder, CThostFtdcRspInfoField *pRspInfo);
	virtual void OnErrRtnOrderAction(CThostFtdcOrderActionField *pOrderAction, CThostFtdcRspInfoField *pRspInfo);

};
#endif

