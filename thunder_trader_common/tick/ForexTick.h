#ifndef _FOREX_TICK
#define _FOREX_TICK
#include "Tick.h"

class CForexTick :
	public CTick
{
public:
	template<class Archive>
	void serialize(Archive& ar, const unsigned int version)
	{
		ar & boost::serialization::base_object<CTick>(*this);
	}
};



#endif
