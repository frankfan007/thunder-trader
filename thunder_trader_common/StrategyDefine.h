#ifndef _STRATEGY_DEFINE_H_
#define _STRATEGY_DEFINE_H_
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

        //Get address of strategy graph prob struct
	virtual TProbeStructType GetProbeStruct() = 0;

        //Get the strategy which store the param
	virtual CParNode * GetParamStruct() = 0;

        //Response when received a customed inquery
	virtual bool OnInquiry(MStrategyInquiryDataInterface*) = 0;

	virtual bool IsSupport(TStrategyTickType) = 0;

	virtual TLastErrorIdType OnInit(ptime CurrentTime) = 0;

	virtual TLastErrorIdType OnInit_FromArchive(ptime CurrentTime) = 0;

        //Market data callback
	virtual void OnTick(TMarketDataIdType, const CTick *) = 0;

        //Order has traded callback
	virtual void OnTrade(
		TOrderRefIdType,
		TOrderSysIdType,
		TVolumeType,
		TPriceType,
		TOrderDirectionType,
		TOrderOffsetType
		) = 0;	

        //Order status has changed callback
	virtual void OnOrder(
		TOrderRefIdType,
		TOrderSysIdType,
		TOrderDirectionType,
		TOrderStatusType,
		TPriceType,
		TTradedVolumeType,
		TRemainVolumeType
		) = 0;

	virtual void OnEndup() = 0;

	virtual void OnRelease() = 0;

	virtual void OnSave(const char * ) = 0;

	virtual void OnLoad(const char * ) = 0;
	
	virtual bool OnGetPositionInfo(int *) = 0;

	virtual bool OnGetCustomInfo(char *, size_t) = 0;

	virtual bool OnGetFloatingProfit(double *) = 0;	

	virtual bool OnGetStatus(char *, size_t) = 0;
};



#endif





