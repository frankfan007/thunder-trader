#pragma once
#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include "StrategyData.h"

using namespace boost::posix_time;
using namespace boost::gregorian;
namespace HFTReportNamespace
{
	using namespace StrategyData;
	enum _charge_type
	{
		_charge_type_Fix,
		_charge_type_Float
	};
	struct CInstrumentInfoForReport
	{
		unsigned int m_uMultiNumber;
		_charge_type open_type;
		double open_value;
		_charge_type close_type;
		double close_value;
	};
	struct CReportForDay
	{
		CReportForDay():
		m_uOrderCount(0),
		m_dbCoveredProfit(0.0),
		m_dbUnCoveredProfit(0.0),
		m_dbFee(0.0){}
		

		unsigned int m_uOrderCount;
		TPriceType m_dbCoveredProfit;
		TPriceType m_dbUnCoveredProfit;
		TPriceType m_dbFee;

	};

	struct CForEveryOrder
	{
		CForEveryOrder(
		ptime ActionTime,
		TOrderRefIdType orderid,
		double Profit,
		double AccumulatedProfit)
		:m_timeActionTime(ActionTime),
		m_OrderId(orderid),
		m_dbProfit(Profit),
		m_dbAccumulatedProfit(AccumulatedProfit){}

		ptime m_timeActionTime;
		TOrderRefIdType m_OrderId;
		double m_dbProfit;
		double m_dbAccumulatedProfit;
	};
}
