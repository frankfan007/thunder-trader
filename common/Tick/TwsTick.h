#ifndef _TWS_TICK_H_
#define _TWS_TICK_H_
#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include "Tick.h"
using namespace boost::posix_time;
using namespace boost::gregorian;
using namespace boost::posix_time;
class CTwsTick :
	public CTick
{
public:
	bool	m_boolValid;
	long	m_longConId;
	char	m_strSecType[8];
	char	m_strExpiry[8];
	//double	m_dbStrike;
	//char	m_strRight[64];
	//char	m_strMultiplier[64];
	char	m_strExchange[10];
	char	m_strPrimaryExchange[10];
	char	m_strCurrency[6];
	//char	m_strLocalSymbol[10];
	//char	m_strTradingClass[10];
	//bool	m_boolIncludeExpired;
	//char	m_strSecIdType[10];
	//char	m_strSecId[10];
	//double	m_dbOpenPrice;
	//double	m_dbHighPrice;
	//double	m_dbLowPrice;
	//double	m_dbClosePrice;
	int		m_intLastSize;
	
	template<class Archive>
	void serialize(Archive& ar, const unsigned int version)
	{
		ar & boost::serialization::base_object<CTick>(*this);
		ar &	m_boolValid;
		ar &	m_longConId;
		ar &	m_strSecType;
		ar &	m_strExpiry;
		//ar &	m_dbStrike;
		//ar &	m_strRight;
		//ar &	m_strMultiplier;
		ar &	m_strExchange;
		ar &	m_strPrimaryExchange;
		ar &	m_strCurrency;
		//ar &	m_strLocalSymbol;
		//ar &	m_strTradingClass;
		//ar &	m_boolIncludeExpired;
		//ar &	m_strSecIdType;
		//ar &	m_strSecId;
		//ar &	m_dbOpenPrice;
		//ar &	m_dbHighPrice;
		//ar &	m_dbLowPrice;
		//ar &	m_dbClosePrice;
		ar &	m_intLastSize;
		

	}

};
#endif