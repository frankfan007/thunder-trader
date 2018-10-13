#pragma once
#include "StrategyData.h"
#include "StrategyInquiryDataInterface.h"
#include <functional>
using namespace StrategyData;
using namespace std;

class COrder;
class MStrategyContext
{
public:
	virtual bool Inquery(TStrategyIdType stid, MStrategyInquiryDataInterface *) = 0;

    virtual bool MeddleResponse(TStrategyIdType, const char *, ...) = 0;

    virtual bool ShowMessage(TStrategyIdType, const char *, ...) = 0;
	
    virtual bool GetNextMeddle(TStrategyIdType, char * retbuffer,unsigned int maxlength) = 0;

    virtual TOrderRefIdType MakeOrder(
			TStrategyIdType,
			TOrderType,
            TOrderDirectionType,
            TOrderOffsetType,
            TVolumeType,
            TPriceType,
            TMarketDataIdType,
			TCustomRefPartType) = 0;
    virtual TLastErrorIdType CancelOrder(
			TStrategyIdType,
			TOrderRefIdType,
            TOrderSysIdType,
            TMarketDataIdType) = 0;
	
	virtual void UpdateChart() = 0;

	virtual bool GetSharedValue(TSharedIndexType i, double & ret) = 0;

	virtual bool IncreaseSharedValue(TSharedIndexType i,double dt, function<bool(double)>) = 0;

	virtual bool DecreaseSharedValue(TSharedIndexType i,double dt, function<bool(double)>) = 0;

	virtual bool SetSharedValue(TSharedIndexType i, double newvalue, function<bool(double)>) = 0;

	virtual int  GetRemainCancelAmount(TStrategyIdType, TMarketDataIdType) = 0;
};


