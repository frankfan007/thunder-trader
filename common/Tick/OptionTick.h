#ifndef _OPTION_TICK
#define _OPTION_TICK
#include "Tick.h"
enum TTradingPhaseCodeType
{
	AuctionPhase,TradingPhase, CircuitBreakingPhase
};
class COptionTick :
	public CTick
{
public:
	ptime m_datetimeTradingDateTime;
	TPriceType m_dbOpenPrice;
	TPriceType m_dbHighestPrice;
	TPriceType m_dbLowestPrice;
	TPriceType m_dbClosePrice;
	TPriceType m_dbAuctionPrice;
	TTradingPhaseCodeType m_enumPhase;
	template<class Archive>
	void serialize(Archive& ar, const unsigned int version)
	{
		ar & boost::serialization::base_object<CTick>(*this);
		ar & m_datetimeTradingDateTime;
		ar & m_dbOpenPrice;
		ar & m_dbHighestPrice;
		ar & m_dbLowestPrice;
		ar & m_dbClosePrice;
		ar & m_dbAuctionPrice;
		ar & m_enumPhase;
		
	}
};



#endif
