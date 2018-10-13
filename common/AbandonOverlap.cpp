#include <vector>
#include "Tick\Tick.h"
using namespace std;


void AbandonOverlap(vector<CTick*>&result)
{
	auto OverAllSerialLength = result.size();
	unsigned int incment = 1;
	unsigned int Be = 0;
	while (Be + incment<OverAllSerialLength)
	{
		if (
			result[Be + incment]->m_datetimeUTCDateTime
			==
			result[Be]->m_datetimeUTCDateTime
			)
		{
			string temp = to_simple_string(result[Be + incment]->m_datetimeUTCDateTime);
			result[Be + incment]->m_datetimeUTCDateTime += microseconds(incment);
			temp = to_simple_string(result[Be + incment]->m_datetimeUTCDateTime);
			++incment;
		}
		else
		{
			Be += incment;
			incment = 1;
		}
	}
}