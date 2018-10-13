#ifndef _COMPRETRADESYSTEMHEADERS_ATMMARKETDATAPLUGININTERFACE_H_
#define _COMPRETRADESYSTEMHEADERS_ATMMARKETDATAPLUGININTERFACE_H_
#include "StrategyData.h"
#include "StrategyDefine.h"
#include <unordered_map>
#include <atomic>
#ifndef BOOST_SPIRIT_THREADSAFE
#define BOOST_SPIRIT_THREADSAFE
#endif
#include <boost/property_tree/ptree.hpp>  
#include <sstream>
#include <string>
#include <boost/thread.hpp>
#include <unordered_map>
#include "AtmPluginInterface.h"

using namespace std;
using namespace boost::property_tree;

class MAtmMarketDataPluginInterface:
	public MAtmPluginInterface
{
public:
	virtual bool IsPedding() = 0;
	virtual void MDInit(const ptree &) = 0;
	virtual void MDHotUpdate(const ptree &) = 0;
	virtual void Pause() = 0;
	virtual void Continue() = 0;
	virtual void MDUnload() = 0;
	virtual void MDAttachStrategy(
		MStrategy *,
		TMarketDataIdType,
		const unordered_map<string, string> &,
		boost::shared_mutex &,
		atomic_uint_least64_t *) = 0;
	virtual void MDDetachStrategy(MStrategy*) = 0;
};
#endif