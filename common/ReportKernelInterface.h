#pragma once
//#include "stdafx.h"
#include "StrategyContext.h"
#include "StrategyData.h"
#include "Order.h"
#include <map>
#include <vector>
#include "ReportStructs.h"
#include <unordered_map>
#pragma region ptree
#define BOOST_SPIRIT_THREADSAFE
#include <boost/property_tree/ptree.hpp>  
#include <boost/property_tree/json_parser.hpp> 
using namespace boost::property_tree;
#pragma endregion
#include "BackTestResult.h"
using namespace std::tr1;
using namespace std;
using namespace HFTReportNamespace;
typedef unordered_map<TOrderRefIdType, COrder> TOrderMapType;
typedef unordered_map<TMarketDataIdType, CInstrumentInfoForReport> TInsInfoMapType;
typedef map<date, CReportForDay> TEachDayReportType;

class AFX_EXT_CLASS MReportKernelInterface
{
public:
	static MReportKernelInterface* CreateReportKernel();
	virtual bool Calculate(CBackTestResult * inandout, const ptree config) = 0;
	virtual void Release() = 0;

};