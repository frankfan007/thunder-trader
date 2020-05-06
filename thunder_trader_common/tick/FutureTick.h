#ifndef _FUTURE_TICK
#define _FUTURE_TICK
#include "Tick.h"

class CFutureTick:
	public CTick
{
public:
	TPriceType m_dbLowerLimitPrice;
	TPriceType m_dbUpperLimitPrice;
	TPriceType m_dbAveragePrice;
	TPriceType m_dbPreSettlementPrice;
	TPriceType m_dbPreClosePrice;
	double m_dbTurnover;
	double m_dbOpenInterest;
	TPriceType m_dbOpenPrice;
	TPriceType m_dbHighestPrice;
	TPriceType m_dbLowestPrice;
	TPriceType m_dbClosePrice;

	template<class Archive>
	void serialize(Archive& ar, const unsigned int version)
	{
		ar & boost::serialization::base_object<CTick>(*this);
		ar &  m_dbLowerLimitPrice;
		ar &  m_dbUpperLimitPrice;
		ar &  m_dbAveragePrice;
		ar &  m_dbPreSettlementPrice;
		ar &  m_dbPreClosePrice;
		ar &  m_dbTurnover;
		ar &  m_dbOpenInterest;
		ar &  m_dbOpenPrice;
		ar &  m_dbHighestPrice;
		ar &  m_dbLowestPrice;
		ar &  m_dbClosePrice;
	}
};



#endif
