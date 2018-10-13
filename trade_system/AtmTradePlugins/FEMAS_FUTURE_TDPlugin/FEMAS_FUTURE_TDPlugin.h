#ifndef _QFCOMPRETRADESYSTEM_ATMTRADEPLUGINS_FEMAS_TDPLUGIN_FEMAS_FUTURE_TDPLUGIN_H_
#define _QFCOMPRETRADESYSTEM_ATMTRADEPLUGINS_FEMAS_TDPLUGIN_FEMAS_FUTURE_TDPLUGIN_H_
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


#include "Femas/USTPFtdcTraderApi.h"
#include "Femas/USTPFtdcUserApiDataType.h"
#include "Femas/USTPFtdcUserApiStruct.h"


#include "SeverityLevel.h"

using namespace boost::posix_time;
using namespace boost::gregorian;
using namespace boost::asio;
using namespace std;

class CFEMAS_FUTURE_TDPlugin :
	public MAtmTradePluginInterface,
	public CUstpFtdcTraderSpi
{

#pragma region 日志属性
	boost::log::sources::severity_logger< severity_levels > m_Logger;
#pragma endregion

#pragma region 定时器属性
	io_service  m_IOservice;
	deadline_timer m_StartAndStopCtrlTimer;
	std::future<bool> m_futTimerThreadFuture;
#pragma endregion

#pragma region 交易接口属性
	MTradePluginContextInterface * m_pTradePluginContext = nullptr;
	CUstpFtdcTraderApi * m_pUserApi = nullptr;//Init at Start()
#pragma endregion

#pragma region 账号线程关键属性
	//在飞马中 Username与InvestorID不一样 investorid是“投资者帐号”，userid是“投资者帐号a”
	string m_strServerAddress;//Init at TDInit
	string m_strBrokerID;//Init at TDInit
	string m_strUsername;//Init at TDInit
	string m_strInvestorID;//Init at TDInit
	string m_strPassword;//Init at TDInit
	bool m_boolIsOnline = false;//Init at Start()
	unsigned int m_uAccountNumber = 0;//Init at CFEMAS_FUTURE_TDPlugin()
	unsigned int m_uRequestID = 0;//Init at Start()
	unsigned int m_uIncreasePart = 0;//Init at OnRspUserLogin()
	string m_strUserOrderLocalIDFixHead;//Init at OnRspUserLogin()
	char m_bufOrderRefFormat[32];
#pragma endregion

#pragma region 登录登出同步
	std::mutex m_mtxLoginSignal;
	condition_variable m_cvLoginSignalCV;
	std::mutex m_mtxLogoutSignal;
	condition_variable m_cvLogoutSignalCV;
#pragma endregion

#pragma region 撤单次数控制
	date GetTradeday(ptime _Current);
	date m_dateTradeDay;
	boost::shared_mutex m_mtxProtectCancelAmount;
	map<string, int> m_mapCancelAmount;
	int m_intInitAmountOfCancelChancesPerDay;
#pragma endregion
	
public:
	static const string s_strAccountKeyword;
	CFEMAS_FUTURE_TDPlugin();
	~CFEMAS_FUTURE_TDPlugin();
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

#pragma region CThostFtdcTraderSpi
	virtual void OnFrontConnected();
	virtual void OnFrontDisconnected(int nReason);
	virtual void OnHeartBeatWarning(int nTimeLapse);
	virtual void OnPackageStart(int nTopicID, int nSequenceNo);
	virtual void OnPackageEnd(int nTopicID, int nSequenceNo);
	virtual void OnRspError(CUstpFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	virtual void OnRspUserLogin(CUstpFtdcRspUserLoginField *pRspUserLogin, CUstpFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	virtual void OnRspUserLogout(CUstpFtdcRspUserLogoutField *pRspUserLogout, CUstpFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	virtual void OnRspOrderInsert(CUstpFtdcInputOrderField *pInputOrder, CUstpFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	virtual void OnRspOrderAction(CUstpFtdcOrderActionField *pOrderAction, CUstpFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	virtual void OnRtnTrade(CUstpFtdcTradeField *pTrade);
	virtual void OnRtnOrder(CUstpFtdcOrderField *pOrder);
	virtual void OnErrRtnOrderInsert(CUstpFtdcInputOrderField *pInputOrder, CUstpFtdcRspInfoField *pRspInfo);
	virtual void OnErrRtnOrderAction(CUstpFtdcOrderActionField *pOrderAction, CUstpFtdcRspInfoField *pRspInfo);
	virtual void OnRspSubscribeTopic(CUstpFtdcDisseminationField *pDissemination, CUstpFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
#pragma endregion
};
#endif

