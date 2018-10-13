#ifndef _TICK
#define _TICK
#include "StrategyData.h"
#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/date_time/posix_time/time_serialize.hpp>
using namespace boost::posix_time;
using namespace StrategyData;
using namespace std;
#define MAX_QUOTATIONS_DEPTH 5



class CTick
{
public:
	TMarketDataIdType		m_uDataID;
	TInstrumentIDType		m_strInstrumentID;
	ptime					m_datetimeUTCDateTime;
	TPriceType				m_dbLastPrice;
	TVolumeType				m_intVolume;
	TPriceType				m_dbBidPrice[MAX_QUOTATIONS_DEPTH];
	TPriceType				m_dbAskPrice[MAX_QUOTATIONS_DEPTH];
	TVolumeType				m_intBidVolume[MAX_QUOTATIONS_DEPTH];
	TVolumeType				m_intAskVolume[MAX_QUOTATIONS_DEPTH];
	template<class Archive>
	void serialize(Archive& ar, const unsigned int version)
	{
		ar & m_uDataID;
		ar & m_strInstrumentID;
		ar & m_datetimeUTCDateTime;
		ar & m_dbLastPrice;
		ar & m_intVolume;
		ar & m_dbBidPrice;
		ar & m_dbAskPrice;
		ar & m_intBidVolume;
		ar & m_intAskVolume;
	}
};
#endif