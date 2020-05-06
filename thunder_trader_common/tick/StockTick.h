  #ifndef _STOCK_TICK_H_
#define _STOCK_TICK_H_
#include "Tick.h"
class CStockTick :
	public CTick
{
public:
	TPriceType m_dbUpperLimitPrice;
	TPriceType m_dbLowerLimitPrice;

	TPriceType m_dbOpenPrice;
	TPriceType m_dbHighestPrice;
	TPriceType m_dbLowestPrice;
	TPriceType m_dbPreClosePrice;

	template<class Archive>
	void serialize(Archive& ar, const unsigned int version)
	{
		ar & boost::serialization::base_object<CTick>(*this);
		ar & m_dbUpperLimitPrice;
		ar & m_dbLowerLimitPrice;
		ar & m_dbOpenPrice;
		ar & m_dbHighestPrice;
		ar & m_dbLowestPrice;
		ar & m_dbPreClosePrice;

	}

};
#endif