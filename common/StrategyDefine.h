#pragma once
#include "StrategyData.h"
#include "StrategyContext.h"
#include "StrategyInquiryDataInterface.h"
#include <map>
#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include "boost/serialization/serialization.hpp"  
#include "boost/archive/binary_oarchive.hpp"  
#include "boost/archive/binary_iarchive.hpp"  
#include "boost/serialization/export.hpp"  
#include "boost/serialization/vector.hpp"  
#include "boost/serialization/deque.hpp" 
#include "boost/serialization/map.hpp" 
#include "boost/serialization/split_member.hpp"  
#include "boost/serialization/utility.hpp"
#include "boost/serialization/list.hpp"

#include <istream>
#include <ostream>
using namespace boost::posix_time;
using namespace boost::gregorian;
using namespace StrategyData;
using namespace std;
class MStrategyContext;
class MTick;
#ifdef WIN32

#ifdef EXPORT_STRATEGY 
#define STRATEGY_INTERFACE __declspec(dllexport) 
#else
#define STRATEGY_INTERFACE __declspec(dllimport) 
#endif

#else 

#define STRATEGY_INTERFACE

#endif



class STRATEGY_INTERFACE MStrategy
{
public:

	virtual TProbeStructType GetProbeStruct() = 0;								//获取策略绘图探针结构体首地址

	virtual CParNode * GetParamStruct() = 0;									//获取策略参数结构体首地址

	virtual bool OnInquiry(MStrategyInquiryDataInterface*) = 0;					//策略自定义交互

	virtual bool IsSupport(TStrategyTickType) = 0;								//策略是否支持该种类型的tick

	virtual TLastErrorIdType OnInit(ptime CurrentTime) = 0;						//策略初始化

	virtual TLastErrorIdType OnInit_FromArchive(ptime CurrentTime) = 0;			//从存档文件载入后的OnInit

	virtual void OnTick(TMarketDataIdType, const CTick *) = 0;					//行情推送

	virtual void OnTrade(
		TOrderRefIdType,
		TOrderSysIdType,
		TVolumeType,
		TPriceType,
		TOrderDirectionType,
		TOrderOffsetType
		) = 0;																	//报单有成交

	virtual void OnOrder(
		TOrderRefIdType,
		TOrderSysIdType,
		TOrderDirectionType,
		TOrderStatusType,
		TPriceType,
		TTradedVolumeType,
		TRemainVolumeType
		) = 0;																	//报单状态发生变化

	virtual void OnEndup() = 0;													//卸载策略时调用

	virtual void OnRelease() = 0;												//释放策略

	virtual void OnSave(const char * ) = 0;										//保存策略

	virtual void OnLoad(const char * ) = 0;										//加载策略
	
	virtual bool OnGetPositionInfo(int *) = 0;									//获取策略当前持仓，如果策略不支持此功能，返回false即可

	virtual bool OnGetCustomInfo(char *, size_t) = 0;							//获取用户自定义信息，如果策略不支持此功能，返回false即可

	virtual bool OnGetFloatingProfit(double *) = 0;								//获取当前持仓浮动盈亏，如果策略不支持此功能，返回false即可

	virtual bool OnGetStatus(char *, size_t) = 0;								//获取策略状态，如果策略不支持此功能，返回false即可
};









