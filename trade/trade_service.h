#ifndef _QFCOMPRETRADESYSTEM_ATM_TRADESERVICE_H_
#define _QFCOMPRETRADESYSTEM_ATM_TRADESERVICE_H_

//#define CTP_FUTURE_MDPlugin
//#define CTP_FUTURE_TDPlugin
//
//#define DFITC_SOP_MDPlugin
//#define DFITC_SOP_TDPlugin
//
//#define DFITC_STOCK_MDPlugin
//
//#define HSCICC_STOCK_TDPlugin
//
//#define QT_STOCK_MDPlugin
//
//#define TEMPLATE_ANY_TDPluginvim
//
//#define FIX_CYCLE_PRICE_MDPlugin

//#define FEMAS_FUTURE_MDPlugin
#include "public.h"
#pragma region 插件

#ifdef CTP_FUTURE_MDPlugin
#include "CTP_FUTURE_MDPlugin/CTP_FUTURE_MDPlugin.h"
#endif

#ifdef CTP_FUTURE_TDPlugin
#include "CTP_FUTURE_TDPlugin/CTP_FUTURE_TDPlugin.h"
#endif

#ifdef DFITC_SOP_MDPlugin
#include "DFITC_SOP_MDPlugin/DFITC_SOP_MDPlugin.h"
#endif

#ifdef DFITC_SOP_TDPlugin
#include "DFITC_SOP_TDPlugin/DFITC_SOP_TDPlugin.h"
#endif

#ifdef DFITC_STOCK_MDPlugin
#include "DFITC_STOCK_MDPlugin/DFITC_STOCK_MDPlugin.h"
#endif

#ifdef QT_STOCK_MDPlugin
#include "QT_STOCK_MDPlugin/QT_STOCK_MDPlugin.h"
#endif

#ifdef TEMPLATE_ANY_TDPlugin
#include "TEMPLATE_ANY_TDPlugin/TEMPLATE_ANY_TDPlugin.h"
#endif

#ifdef FIX_CYCLE_PRICE_MDPlugin
#include "FIX_CYCLE_PRICE_MDPlugin/FIX_CYCLE_PRICE_MDPlugin.h"
#endif

#ifdef FEMAS_FUTURE_MDPlugin
#include "FEMAS_FUTURE_MDPlugin/FEMAS_FUTURE_MDPlugin.h"
#endif

#ifdef FEMAS_FUTURE_TDPlugin
#include "FEMAS_FUTURE_TDPlugin/FEMAS_FUTURE_TDPlugin.h"
#endif

#ifdef TWS_MDPlugin
#include "TWS_MDPlugin/TWS_MDPlugin.h"
#endif

#ifdef TWS_TDPlugin
#include "TWS_TDPlugin/TWS_TDPlugin.h"
#endif
#pragma endregion
#include <functional>
#include <list>
#include <memory>
#include <mutex>
#include <string>
#include <sstream>
#include <queue>
#include <unordered_map>
#include <boost/thread/mutex.hpp>
#include <boost/thread/shared_mutex.hpp>
#include <boost/log/common.hpp>
#include "CommuModForServInterface.h"
#include "AtmMarketDataPluginInterface.h"
#include "AtmTradePluginInterface.h"
#include "TradePluginContextInterface.h"
#include "OrderRefResolve.h"
#include "StrategyContext.h"
#pragma region filesystem
#include <boost/filesystem/path.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem.hpp>
#pragma endregion


#include "SeverityLevel.h"

using namespace std;
using namespace std::placeholders;
#define MEDDLE_RESPONSE_MAXLEN 1024
#define STRATEGY_MESSAGE_MAXLEN 1024
class CStrategyNode
{
public:
	CStrategyNode() :m_pStrategy(nullptr) {}
	void clear() { 
		m_dateActionDate = second_clock::universal_time().date();
		m_uMaxIncreaseOrderCountPerDay = 10;
		m_uRemainIncreaseOrderCountPerDay = 10;
		m_treeConfig.clear();
		m_pathStrategyPath.clear();
		m_pBinHandle = nullptr;
		m_pStrategy = nullptr;
		unordered_map<TMarketDataIdType, pair< MAtmTradePluginInterface*, unordered_map<string, string> > >().swap(m_mapDataid2TradeApi);
		vector<pair<string, vector< std::tuple<string, TProbeColorType, atomic<double>*, atomic<unsigned int>*> > > >().swap(m_vecProbeInfo);
		unordered_map<TMarketDataIdType, MAtmMarketDataPluginInterface*>().swap(m_mapDataid2MarketDataApi);
		m_auProbeUpdateDatetime.store(0);
		while (m_queueMeddleQueue.empty() == false)
			m_queueMeddleQueue.pop();
		while (m_queueMeddleResponseQueue.empty() == false)
			m_queueMeddleResponseQueue.pop();
	}
	date m_dateActionDate;
	atomic_uint64_t m_uMaxIncreaseOrderCountPerDay;
	atomic_uint64_t m_uRemainIncreaseOrderCountPerDay;
	unsigned int m_uStrategyID;
	ptree m_treeConfig;
	boost::filesystem::path m_pathStrategyPath;
	StrategyHandleType m_pBinHandle;
	MStrategy * m_pStrategy;
	boost::shared_mutex m_mtxPropectStrategy;
	unordered_map<TMarketDataIdType, pair< MAtmTradePluginInterface*,unordered_map<string,string> > > m_mapDataid2TradeApi;
	vector<pair<string, vector< std::tuple<string, TProbeColorType,atomic<double>*, atomic<unsigned int>*> > > > m_vecProbeInfo;
	unordered_map<TMarketDataIdType, MAtmMarketDataPluginInterface*> m_mapDataid2MarketDataApi;
	atomic_uint_least64_t m_auProbeUpdateDatetime;

	boost::shared_mutex m_mtxPropectMeddleQueue;
	queue<string> m_queueMeddleQueue;

	boost::shared_mutex m_mtxPropectMeddleResponseQueue;
	queue<pair<ptime,string>> m_queueMeddleResponseQueue;
};

typedef boost::log::sources::severity_logger< severity_levels >& LoggerType;
typedef std::shared_ptr<MAtmPluginInterface> PluginPtrType;
typedef std::shared_ptr<MAtmMarketDataPluginInterface> MDPluginPtrType;
typedef std::shared_ptr<MAtmTradePluginInterface> TDPluginPtrType;
typedef PluginPtrType(*TPluginFactory)();

#define PLUGIN(name,classname) {name,make_pair([]()->PluginPtrType {return PluginPtrType(new classname());},classname::s_strAccountKeyword)}
enum class PackageHandlerParamType
{
	MarketData,
	Trade,
	Nothing
};
class CTradeService :
	public CCommuModForServSpi,
	public MStrategyContext,
	public MTradePluginContextInterface
{
	const unordered_map<string, pair<TPluginFactory, string> > m_mapAMarketDataPFactories =
	{

#ifdef CTP_FUTURE_MDPlugin
		PLUGIN("ctp",CCTP_FUTURE_MDPlugin),
#endif

#ifdef DFITC_SOP_MDPlugin
		PLUGIN("dfitc_sop",CDFITC_SOP_MDPlugin),
#endif

#ifdef DFITC_STOCK_MDPlugin
		PLUGIN("dfitc_stock",CDFITC_STOCK_MDPlugin),
#endif

#ifdef QT_STOCK_MDPlugin
		PLUGIN("quant_tech_stock",CQT_STOCK_MDPlugin),
#endif
#ifdef FEMAS_FUTURE_MDPlugin
		PLUGIN("femas_future",CFEMAS_FUTURE_MDPlugin),
#endif

#ifdef TWS_MDPlugin
		PLUGIN("tws",CTWS_MDPlugin),
#endif

#ifdef FIX_CYCLE_PRICE_MDPlugin
		PLUGIN("fix_cycle_price",CFixCyclePricePluginImp)
#endif

	};

	const unordered_map<string, pair<TPluginFactory, string> > m_mapATradePFactories =
	{
#ifdef CTP_FUTURE_TDPlugin
		PLUGIN("ctp",CCTP_FUTURE_TDPlugin),
#endif
#ifdef DFITC_SOP_TDPlugin
		PLUGIN("dfitc_sop",CDFITC_SOP_TDPlugin),
#endif
#ifdef FEMAS_FUTURE_TDPlugin
		PLUGIN("femas_future",CFEMAS_FUTURE_TDPlugin),
#endif
#ifdef TWS_TDPlugin
		PLUGIN("tws",CTWS_TDPlugin),
#endif
#ifdef TEMPLATE_ANY_TDPlugin
		PLUGIN("template",CTEMPLATE_ANY_TDPlugin)
#endif
	};
	typedef void (CTradeService::*TPackageHandlerFuncType)(PackageHandlerParamType, const ptree & in, ptree &out);
#define HANDLER(fn,param) (std::bind(&CTradeService::fn,this,PackageHandlerParamType::param,std::placeholders::_1,std::placeholders::_2))


	const unordered_map<string, function<void(const ptree & in, ptree &out)>> m_mapString2PackageHandlerType = {
		//获取源类型
		{ "reqgetsupportedmdtypes",		HANDLER(ReqGetSupportedTypes,		MarketData)},
		{ "reqgetsupportedtdtypes",		HANDLER(ReqGetSupportedTypes,		Trade) },
		//获取现有的源
		{ "reqgetallmarketdatasource",	HANDLER(ReqGetAllSource,			MarketData)},
		{ "reqgetalltradesource",		HANDLER(ReqGetAllSource,			Trade) },
		//增加源
		{ "reqaddmarketdatasource",		HANDLER(ReqAddSource,				MarketData) },
		{ "reqaddtradesource",			HANDLER(ReqAddSource,				Trade) },
		//删除源
		{ "reqdelmarketdatasource",		HANDLER(ReqDelSource,				MarketData) },
		{ "reqdeltradesource",			HANDLER(ReqDelSource,				Trade) },
		
		{ "reqallstrategybin",			HANDLER(ReqAllStrategyBin,			Nothing) },
		{ "reqallarchivefile",			HANDLER(ReqAllArchiveFile,			Nothing) },

		{ "reqdeploynewstrategy",		HANDLER(ReqDeployNewStrategy,		Nothing) },
		{ "reqgetallrunningstrategies",	HANDLER(ReqGetAllRunningStrategies,	Nothing) },
		{ "reqcancelrunningstrategies",	HANDLER(ReqCancelRunningStrategies,	Nothing) },
		{ "reqgetprobe",				HANDLER(ReqGetProbe,				Nothing) },

		{ "reqmeddle",					HANDLER(ReqMeddle,					Nothing) },
		{ "reqgetmeddleresponse",		HANDLER(ReqGetMeddleResponse,		Nothing) },
		{ "reqstrategyparams",			HANDLER(ReqStrategyParams,			Nothing) },
		{ "reqstrategyconfigjson",		HANDLER(ReqStrategyConfigJson,		Nothing) },
		{ "requpdatestrategybin",		HANDLER(ReqUpdateStrategyBin,		Nothing) },
		{ "reqmodifysharedvalue",		HANDLER(ReqModifySharedValue,		Nothing) },
		{ "reqallsharedvalue",			HANDLER(ReqAllSharedValue,			Nothing) },
		{ "reqsetordertickets",			HANDLER(ReqSetOrderTickets,			Nothing) },
		{ "getpositioninfo",			HANDLER(ReqGetPositionInfo,			Nothing) },
		{ "getcustominfo",				HANDLER(ReqGetCustomInfo,			Nothing) },
		{ "getfloatingprofit",			HANDLER(ReqGetFloatingProfit,		Nothing) },
		{ "getstatus",					HANDLER(ReqStatus,					Nothing) },
	};

	boost::log::sources::severity_logger< severity_levels > m_Logger;
	std::string m_strConfigFile;
	unsigned int m_uSystemNumber = 0;
	//仅一下三个数据结构需要互斥:策略数组\行情源数组\交易源数组
#pragma region 管理所有策略
	//策略数组:保存所有策略，该数据结构需要互斥保护
	boost::shared_mutex m_mtxAllStrategys;
	CStrategyNode m_arrayAllStrategys[(_MaxStrategyID+1)];
#pragma endregion
	
#pragma region 管理所有的行情源与交易源
	//管理所有行情源与交易源，这两个数据结构需要互斥保护
	pair<vector<PluginPtrType>, boost::shared_mutex> m_vecAllMarketDataSource;//行情源数组
	pair<vector<PluginPtrType>, boost::shared_mutex> m_vecAllTradeSource;//交易源数组
#pragma endregion

#pragma region 系统环境共享变量
	boost::shared_mutex m_mtxSharedValue;
	unordered_map<TSharedIndexType, double> m_mapSharedValue;
#pragma endregion
public:
	CTradeService(std::string configFile,unsigned int sysnum);
	~CTradeService();
	void Start();
	void Join();
private:
	void MakeError(ptree & out, const char * fmt,...);
	string GetAddress();
	unsigned short GetListenPort();
	size_t GetNetHandlerThreadCount();
	
	MCommuModForServInterface * m_pApi = nullptr;

#pragma region 功能函数
	void DeployStrategy(const ptree &, unsigned int & strategyid);
	void CancelStrategy(unsigned int strategyid, string & sarchive,ptree & config);
#pragma endregion

#pragma region CCommuModForServSpi
	virtual void OnCommunicate(const ptree & in, ptree & out);
#pragma endregion

#pragma region PackageHandler
#define PACKAGE_HANDLER(fun_name) void fun_name (PackageHandlerParamType,const ptree & in, ptree &out);
	PACKAGE_HANDLER(ReqGetSupportedTypes)
	PACKAGE_HANDLER(ReqGetAllSource)
	PACKAGE_HANDLER(ReqAddSource)
	PACKAGE_HANDLER(ReqDelSource)
	PACKAGE_HANDLER(ReqAllStrategyBin)
	PACKAGE_HANDLER(ReqAllArchiveFile)
	PACKAGE_HANDLER(ReqDeployNewStrategy)
	PACKAGE_HANDLER(ReqGetAllRunningStrategies)
	PACKAGE_HANDLER(ReqCancelRunningStrategies)
	PACKAGE_HANDLER(ReqGetProbe)
	PACKAGE_HANDLER(ReqMeddle)
	PACKAGE_HANDLER(ReqGetMeddleResponse)
	PACKAGE_HANDLER(ReqStrategyParams)
	PACKAGE_HANDLER(ReqStrategyConfigJson)
	PACKAGE_HANDLER(ReqUpdateStrategyBin)
	PACKAGE_HANDLER(ReqModifySharedValue)
	PACKAGE_HANDLER(ReqAllSharedValue)
	PACKAGE_HANDLER(ReqSetOrderTickets)
	PACKAGE_HANDLER(ReqGetPositionInfo)
	PACKAGE_HANDLER(ReqGetCustomInfo)
	PACKAGE_HANDLER(ReqGetFloatingProfit)
	PACKAGE_HANDLER(ReqStatus)
#pragma endregion

#pragma region MStrategyContext
	virtual bool Inquery(TStrategyIdType stid, MStrategyInquiryDataInterface *);

	virtual bool MeddleResponse(TStrategyIdType, const char *, ...);

	virtual bool ShowMessage(TStrategyIdType, const char *, ...);

	virtual bool GetNextMeddle(TStrategyIdType, char * retbuffer, unsigned int maxlength);

	virtual TOrderRefIdType MakeOrder(
		TStrategyIdType,
		TOrderType,
		TOrderDirectionType,
		TOrderOffsetType,
		TVolumeType,
		TPriceType,
		TMarketDataIdType,
		TCustomRefPartType);

	virtual TLastErrorIdType CancelOrder(
		TStrategyIdType,
		TOrderRefIdType,
		TOrderSysIdType,
		TMarketDataIdType);

	virtual void UpdateChart();

	virtual bool GetSharedValue(TSharedIndexType i, double & ret);

	virtual bool IncreaseSharedValue(TSharedIndexType i, double dt, function<bool(double)>);

	virtual bool DecreaseSharedValue(TSharedIndexType i, double dt, function<bool(double)>);

	virtual bool SetSharedValue(TSharedIndexType i, double newvalue, function<bool(double)>);

	virtual int  GetRemainCancelAmount(TStrategyIdType, TMarketDataIdType);
#pragma endregion

#pragma region MTradePluginContextInterface
	virtual void OnTrade(
		TOrderRefIdType,
		TOrderSysIdType,
		TPriceType,
		TVolumeType);
	virtual void OnOrder(
		TOrderRefIdType,
		TOrderSysIdType,
		TOrderStatusType,
		TPriceType,
		TTradedVolumeType,
		TRemainVolumeType
		);
#pragma endregion
};
#endif
