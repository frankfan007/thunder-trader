#ifndef _QFCOMPRETRADESYSTEM_ATMTRADEPLUGINS_DFITC_TDPLUGIN_DFITCTDPLUGINIMP_H_
#define _QFCOMPRETRADESYSTEM_ATMTRADEPLUGINS_DFITC_TDPLUGIN_DFITCTDPLUGINIMP_H_
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
#include "AtmTradePluginInterface.h"
#include "TradePluginContextInterface.h"
#include <boost/log/common.hpp>
#include "DFITCSECTraderApi.h"
#include "SeverityLevel.h"

using namespace boost::posix_time;
using namespace boost::gregorian;
using namespace boost::asio;
using namespace std;
class CDFITC_SOP_TDPlugin :
	public MAtmTradePluginInterface,
	public DFITCSECTraderSpi
{
	unordered_map<int, const char *> m_mapDeclareResult =
	{
		{ DFITCSEC_DR_UnDeclare,		"DFITCSEC_DR_UnDeclare"			},///未申报	
		{ DFITCSEC_DR_Declaring,		"DFITCSEC_DR_Declaring"			},///正在申报
		{ DFITCSEC_DR_UnTrade,			"DFITCSEC_DR_UnTrade"			},///已申报未成交
		{ DFITCSEC_DR_EntrustFail,		"DFITCSEC_DR_EntrustFail"		},///非法委托
		{ DFITCSEC_DR_AppFundsAuthing,	"DFITCSEC_DR_AppFundsAuthing"	},///申请资金授权中
		{ DFITCSEC_DR_PartTrade,		"DFITCSEC_DR_PartTrade"			},///部分成交
		{ DFITCSEC_DR_TotalTrade,		"DFITCSEC_DR_TotalTrade"		},///全部成交
		{ DFITCSEC_DR_TradeAWithdraw,	"DFITCSEC_DR_TradeAWithdraw"	},///部成部撤
		{ DFITCSEC_DR_TotalWithdraw,	"DFITCSEC_DR_TotalWithdraw"		},///全部撤单
		{ DFITCSEC_DR_WithdrawFail,		"DFITCSEC_DR_WithdrawFail"		},///撤单未成
		{ DFITCSEC_DR_ManualDeclare,	"DFITCSEC_DR_ManualDeclare"		},///等待人工申报
	};

	boost::log::sources::severity_logger< severity_levels > m_Logger;



	io_service  m_IOservice;
	deadline_timer m_StartAndStopCtrlTimer;
	std::future<bool> m_futTimerThreadFuture;



	MTradePluginContextInterface * m_pTradePluginContext = nullptr;
	DFITCSECTraderApi * m_pUserApi = nullptr;//Init at Start()



	string m_strServerAddress;//Init at TDInit
	string m_strAccountid;//Init at TDInit
	string m_strPassword;//Init at TDInit
	bool m_boolIsOnline = false;//Init at Start()
	unsigned int m_uAccountNumber = 0;//Init at CDFITC_SOP_TDPlugin()
	unsigned int m_uRequestID = 0;//Init at Start()
	unsigned int m_uIncreasePart = 0;//Init at OnRspUserLogin()
	DFITCSECFrontIDType m_intFrontID = 0;//Init at OnRspUserLogin()
	DFITCSECSessionIDType m_intSessionID = 0;//Init at OnRspUserLogin()



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
	CDFITC_SOP_TDPlugin();
	~CDFITC_SOP_TDPlugin();
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
	void OnFrontConnected();
	void OnFrontDisconnected(int nReason);
private:
	bool Start();
	bool Stop();
	void ShowMessage(severity_levels, const char * fmt, ...);
	void TimerHandler(boost::asio::deadline_timer* timer, const boost::system::error_code& err);


	virtual void OnRspSOPUserLogin(DFITCSECRspUserLoginField *pData, DFITCSECRspInfoField *pRspInfo);
	virtual void OnRspSOPUserLogout(DFITCSECRspUserLogoutField *pData, DFITCSECRspInfoField *pRspInfo);
	virtual void OnRspSOPEntrustOrder(DFITCSOPRspEntrustOrderField *pData, DFITCSECRspInfoField *pRspInfo);
	virtual void OnSOPTradeRtn(DFITCSOPTradeRtnField * pData);
	virtual void OnRspSOPWithdrawOrder(DFITCSECRspWithdrawOrderField *pData, DFITCSECRspInfoField *pRspInfo);
	virtual void OnSOPWithdrawOrderRtn(DFITCSOPWithdrawOrderRtnField * pData);
	virtual void OnRspError(DFITCSECRspInfoField *pRspInfo);


};
#endif

