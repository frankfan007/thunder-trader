// SimulateKernel.cpp : 定义 DLL 的初始化例程。
//

#include "StrategyData.h"
#include "StrategyContext.h"
#include "SimulateKernelInterface.h"
#include "SimulateKernelEnvironment.h"
#include "Order.h"
#include <queue>
#include <list>
#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include "resource.h"
#include "StrategyDefine.h"
#include <unordered_map>
//#include "TickLoader.h"
#include "FutureTick.h"
#include "StockTick.h"
#include "QuantFundHFTBackTestTypedefine.h"
#include <memory>
#include <tuple>
using namespace boost::posix_time;
using namespace boost::gregorian;
using namespace std;
enum TTradePriceType
{
	TradePrice_FixPrice,
	TradePrice_BestPrice,
};

#define Index_ResBuff 0
#define Value_ResBuff 1

#define IndexAddr 2
#define ValueAddr 3
typedef std::tuple<
	std::shared_ptr<vector<unsigned int>>/*Index*/,
	std::shared_ptr<vector<float>>/*Value*/,
	atomic<unsigned int>*/*IndexAddr*/,
	atomic<double>*/*ValueAddr*/> TProbeNodeType;

class CDataSerial;

class CMySimulateKernel :
	public MSimulateKernelInterface
{
public:
	TTradePriceType m_enumTradePriceType = TradePrice_BestPrice;
	bool m_boolNewOrder = false;
	bool m_boolNewCancel = false;
	CDataSerial * m_pCurrentTop;
	list<COrder*> m_listOrdersTobeHandle;
	map<string, TOrderRefIdType> mapSysId2OrderRef;
	vector<unsigned int> m_vecOnTickTimeConsuming;
	vector<TProbeNodeType> m_vecProbeUpdateList;
	bool m_boolAutoUpdateChart = true;
	unsigned int m_uFlags = Simulate_HasMessage | Simulate_HasProbes | Simulate_HasOnTickTimeConsuming;
// 保存输出信息指针
	CBackTestResult * m_Out;
	MSimulateKernelEnvironment * m_environment;
	CTick * m_pLastTick_Global;
	CMySimulateKernel(MSimulateKernelEnvironment *_env);
	virtual void Release();
	
	virtual void StartBackTest(
		MStrategy* pStrategy,/*IN*/
		vector<string> tickFiles,
		string strInArchiveFile,/*IN*/
		string strOutArchiveFile,/*IN*/
		const ptree config,/*IN*/
		unsigned int flags,/*IN*/
		CBackTestResult * /*OUT*/
		);

	// StrategyContext
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

};
