#ifndef _QFCOMPRETRADESYSTEM_ATMTRADEPLUGINS_TWS_TDPLUGIN_H_
#define _QFCOMPRETRADESYSTEM_ATMTRADEPLUGINS_TWS_TDPLUGIN_H_
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


#include "TwsApi/TwsApi.h"
#include "TwsApi/TwsDataStructDef.h"
#include "TwsApi/TwsDataTypeDef.h"
#include "TwsApi/TwsSpi.h"

#include "SeverityLevel.h"

using namespace boost::posix_time;
using namespace boost::gregorian;
using namespace boost::asio;
using namespace std;

class CTWS_TDPlugin :
	public MAtmTradePluginInterface,
	public CTwsSpi
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
	MTwsApi * m_pUserApi = nullptr;//Init at Start()
#pragma endregion

#pragma region 账号线程关键属性
	string m_strServerAddress;//Init at Start()
	unsigned int m_uPort;//Init at Start()
	unsigned int m_uClientID;//Init at Start()
	bool m_boolIsOnline = false;//Init at Start()
	unsigned int m_uAccountNumber = 0;//Init at CTWS_TDPlugin()
	unsigned int m_uRequestID = 0;//Init at Start()
	unsigned int m_uIncreasePart = 0;//Init at OnRspUserLogin()
	TTwsOrderIdType m_intNextValidId;//Init at OnRspUserLogin()
	TTwsTimeType m_LongServerTime;//Init at OnRspUserLogin()
	char m_strManagedAccounts[64];//Init at OnRspUserLogin()
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
	CTWS_TDPlugin();
	~CTWS_TDPlugin();
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
	virtual void OnRspUserLogin(CTwsRspUserLoginField * loginField, bool IsSucceed);
	virtual void OnRspError(int ErrID, int ErrCode, const char * ErrMsg);
	virtual void OnDisconnected();
	virtual void OnRtnOrder(CTwsOrderField *);
	virtual void OnRtnTrade(CTwsTradeField *);
#pragma endregion
};
#endif

