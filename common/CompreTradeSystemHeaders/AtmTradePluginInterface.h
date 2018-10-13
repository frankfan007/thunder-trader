#ifndef _COMPRETRADESYSTEMHEADERS_ATMTRADEPLUGININTERFACE_H_
#define _COMPRETRADESYSTEMHEADERS_ATMTRADEPLUGININTERFACE_H_
#include "StrategyData.h"
#include "StrategyDefine.h"
#ifndef BOOST_SPIRIT_THREADSAFE
#define BOOST_SPIRIT_THREADSAFE
#endif
#include <boost/property_tree/ptree.hpp>
#include "TradePluginContextInterface.h"
#include <unordered_map>
#include "AtmPluginInterface.h"

using namespace StrategyData;
using namespace std;
using namespace boost::property_tree;

class MAtmTradePluginInterface:
	public MAtmPluginInterface
{
public:
	virtual bool IsPedding() = 0;
	virtual void TDInit(const ptree &, MTradePluginContextInterface*, unsigned int AccountNumber) = 0;
	virtual void TDHotUpdate(const ptree &) = 0;
	virtual void TDUnload() = 0;
	virtual TOrderRefIdType TDBasicMakeOrder(
		TOrderType ordertype,
		unordered_map<string, string> & instrument,
		TOrderDirectionType direction,
		TOrderOffsetType offset,
		TVolumeType volume,
		TPriceType LimitPrice,
		TOrderRefIdType orderRefBase
		) = 0;
	virtual TLastErrorIdType TDBasicCancelOrder(TOrderRefIdType,unordered_map<string, string> &, TOrderSysIdType) = 0;	
	virtual int TDGetRemainAmountOfCancelChances(const char *) = 0;
};
#endif