#ifndef _MSTRATEGYINQUIRYRESPONSEINTERFACE
#define _MSTRATEGYINQUIRYRESPONSEINTERFACE
class MStrategyInquiryDataInterface
{
public:
	virtual bool ValueToString(char * buf, size_t len) = 0;
	virtual void Release() = 0;
};
#endif