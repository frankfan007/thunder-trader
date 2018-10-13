#ifndef _QFCOMPRETRADESYSTEM_ATMMARKETDATAPLUGINS_CTP_MDPLUGIN_DFITC_SOP_MDPlugin_H_
#define _QFCOMPRETRADESYSTEM_ATMMARKETDATAPLUGINS_CTP_MDPLUGIN_DFITC_SOP_MDPlugin_H_
#include <string>
#include <boost/log/common.hpp>
#include <boost/thread.hpp>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <boost/asio.hpp>
#include <memory>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <thread>
#include <future>
#include <tuple>
#include "DFITCSECMdApi.h"
#include "SeverityLevel.h"
#include "OptionTick.h"
#include "AtmMarketDataPluginInterface.h"
using namespace boost::posix_time;
using namespace boost::gregorian;
using namespace boost::asio;
using namespace std;

class CDFITC_SOP_MDPlugin :
	public MAtmMarketDataPluginInterface,
	public DFITCSECMdSpi
{
#pragma region 日志属性
	boost::log::sources::severity_logger< severity_levels > m_Logger;
#pragma endregion

#pragma region 定时器属性
	io_service  m_IOservice;
	deadline_timer m_StartAndStopCtrlTimer;
	std::future<bool> m_futTimerThreadFuture;
#pragma endregion

	string m_strServerAddress;
	string m_strAccountID;
	string m_strPassword;

#pragma region 接口属性
	std::shared_ptr<DFITCSECMdApi> m_pUserApi;
#pragma endregion

#pragma region 账号线程关键属性
	unsigned int m_uRequestID = 0;
	bool m_boolIsOnline = false;
#pragma endregion

#pragma region 登录登出同步
	std::mutex m_mtxLoginSignal;
	condition_variable m_cvLoginSignalCV;
	std::mutex m_mtxLogoutSignal;
	condition_variable m_cvLogoutSignalCV;
#pragma endregion

#pragma region 观察者管理
	boost::shared_mutex m_mapObserverStructProtector;
	unordered_map<string, pair<COptionTick,list< tuple < MStrategy*, TMarketDataIdType, boost::shared_mutex*,atomic_uint_least64_t *> > > > m_mapInsid2Strategys;
	unordered_map< MStrategy*, list<string> > m_mapStrategy2Insids;
#pragma endregion

public:
	static const string s_strTypeword;
	static const string s_strAccountKeyword;
	CDFITC_SOP_MDPlugin();
	~CDFITC_SOP_MDPlugin();
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
#pragma region CThostFtdcMdSpi
	virtual void OnFrontConnected();
	virtual void OnFrontDisconnected(int nReason);
	virtual void OnRspSOPUserLogin(struct DFITCSECRspUserLoginField * pRspUserLogin, struct DFITCSECRspInfoField * pRspInfo);
	virtual void OnRspSOPUserLogout(struct DFITCSECRspUserLogoutField * pRspUsrLogout, struct DFITCSECRspInfoField * pRspInfo);
	virtual void OnRspSOPSubMarketData(struct DFITCSECSpecificInstrumentField * pSpecificInstrument, struct DFITCSECRspInfoField * pRspInfo);
	virtual void OnRspSOPUnSubMarketData(struct DFITCSECSpecificInstrumentField * pSpecificInstrument, struct DFITCSECRspInfoField * pRspInfo);
	virtual void OnRspError(struct DFITCSECRspInfoField *pRspInfo);
	virtual void OnSOPMarketData(struct DFITCSOPDepthMarketDataField * pMarketDataField);
#pragma endregion
};

#endif