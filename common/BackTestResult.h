#pragma once
#include "QuantFundHFTBackTestTypedefine.h"
#include <vector>
#include <unordered_map>
#include <map>
#include "ReportStructs.h"
#include <string>
#include "Order.h"
#include "Tick.h"
#pragma region boost::ptime
#include <boost/date_time/posix_time/posix_time.hpp>
using namespace boost::posix_time;
using namespace boost::gregorian;
#pragma endregion
using namespace boost::posix_time;
using namespace HFTReportNamespace;
class CBackTestResult
{
public:
	unsigned int m_uSerialLength;
	TNormalGraphProbeInfoType m_vecNGraphProbeInfo;
	TCandlesticksGraphProbeInfoType m_vecKGraphProbeInfo;
	vector<ptime> m_vecTimeSerial;
	vector<pair<ptime, wstring> >  m_vecMessages;
	unordered_map<TOrderRefIdType, COrder>  m_mapOrders;
	unordered_map<string, string>  m_mapCriterions;
	vector<pair<ptime, MStrategyInquiryDataInterface*>> m_vecInqueries;
	map<date, CReportForDay>  m_mapReportForDay;

	unordered_map<string, CInstrumentInfoForReport> m_mapInsInfo;
	vector<TOrderRefIdType> m_vecOrderedOrderRefs;

	string m_strInsidInfoFile;
	vector<string> m_vecInstruments;
	vector<CTick> m_vecLastTick;


	vector<float> m_vecOrderProfit;
	vector<float> m_vecCumProfit;
	vector<float> m_vecCumFee;
	vector<float> m_vecCumNet;
	vector<ptime> m_vecTimeLable;

	vector<float> m_vecCumProfit2;
	vector<float> m_vecCumFee2;
	vector<float> m_vecCumNet2;
	vector<ptime> m_vecTimeLable2;
	vector<ptime> m_vecMarks;

	double m_dbMaximumDrawdown = -1;
	unsigned int m_intWinCnt;
	unsigned int m_intLoseCnt;
	unsigned int m_intEqueCnt;
	double m_dbWinMoney;
	double m_dbLoseMoney;

	float m_dbCumProfit_Closed = 0.0f;
	float m_dbCumProfit_UnClosed = 0.0f;
	float m_dbCumFee = 0.0f;

	unordered_map<TSharedIndexType, double> m_mapSharedValue;
	void Init()
	{
		m_uSerialLength = 0;
		m_vecNGraphProbeInfo.clear();
		m_vecKGraphProbeInfo.clear();
		m_vecTimeSerial.clear();
		m_vecMessages.clear();
		m_mapOrders.clear();
		m_mapCriterions.clear();
		m_vecInqueries.clear();
		m_mapReportForDay.clear();

		m_mapInsInfo.clear();
		m_vecOrderedOrderRefs.clear();

		m_strInsidInfoFile.clear();
		m_vecInstruments.clear();
		m_vecLastTick.clear();


		m_vecOrderProfit.clear();
		m_vecCumProfit.clear();
		m_vecCumFee.clear();
		m_vecCumNet.clear();
		m_vecTimeLable.clear();

		m_vecCumProfit2.clear();
		m_vecCumFee2.clear();
		m_vecCumNet2.clear();
		m_vecTimeLable2.clear();
		m_vecMarks.clear();

		m_dbMaximumDrawdown = -1;
		m_intWinCnt = 0;
		m_intLoseCnt = 0;
		m_intEqueCnt = 0;
		m_dbWinMoney = 0;
		m_dbLoseMoney = 0;

		m_dbCumProfit_Closed = 0.0f;
		m_dbCumProfit_UnClosed = 0.0f;
		m_dbCumFee = 0.0f;

		m_mapSharedValue.clear();
	}
};
