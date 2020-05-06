#pragma once
//#include "stdafx.h"
#include "StrategyContext.h"
#include "StrategyData.h"
#include "Tick\Tick.h"
#include "SimulateKernelEnvironment.h"
#include <vector>
#include <unordered_map>
#include "TickDataContainer.h"
#include "BackTestResult.h"
#include <memory>
using namespace std;
using namespace std::tr1;
//AFX_EXT_CLASS 
#define Simulate_HasMessage (1<<0)
#define Simulate_HasProbes (1<<1)
#define Simulate_HasOnTickTimeConsuming (1<<2)
#define Simulate_HasOrders (1<<3)
class AFX_EXT_CLASS MSimulateKernelInterface
	: public MStrategyContext
{
public:
	static MSimulateKernelInterface* CreateSimulateKernel(MSimulateKernelEnvironment *);
	virtual void StartBackTest(
		MStrategy* pStrategy,/*IN*/
		vector<string> tickFiles,
		string strInArchiveFile,/*IN*/
		string strOutArchiveFile,/*IN*/
		const ptree config,/*IN*/
		unsigned int flags,/*IN*/
		CBackTestResult * /*OUT*/
		) = 0;
	virtual void Release() = 0;

#pragma region StrategyContext
	
	virtual bool ShowMessage(TStrategyIdType, const char *, ...) = 0;
	virtual bool GetNextMeddle(TStrategyIdType, char * retbuffer, unsigned int maxlength) = 0;
	virtual TOrderRefIdType MakeOrder(
		TStrategyIdType,
		TOrderType,
		TOrderDirectionType,
		TOrderOffsetType,
		TVolumeType,
		TPriceType,
		TMarketDataIdType,
		TCustomRefPartType) = 0;
	
	virtual TLastErrorIdType CancelOrder(
		TStrategyIdType,
		TOrderRefIdType,
		TOrderSysIdType,
		TMarketDataIdType) = 0;
	
	virtual void UpdateChart() = 0;
#pragma  endregion

};