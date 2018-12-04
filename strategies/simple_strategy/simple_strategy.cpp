#include "Order.h"
#include "StrategyDefine.h"
#include "StrategyData.h"
#include "Tick.h"
#include "FutureTick.h"
#include "StockTick.h"
#include "FutureTick.h"
#include "OptionTick.h"
#include <cmath>
#include <list>
#include <string>
#include <fstream>
using namespace std;

class MyStrategy : public MStrategy
{
public:
    
    STRATEGY_TEMPLATE_DECLARE(MyStrategy)

    BEGIN_SERIALIZATION
        SERIALIZATION(a)
        SERIALIZATION(b)
        SERIALIZATION(c)
    END_SERIALIZATION



    int a = 5;
    double b = 0.2;
    int c = 2;
    int aa = 5;
    double bb = 0.2;
    int cc = 2;

    
    BEGIN_PARAMETER_BIND
        PARAMETER_INT(a)
        PARAMETER_DOUBLE(b)
        PARAMETER_INT(c)
        PARAMETER_INT(aa)
        PARAMETER_DOUBLE(bb)
        PARAMETER_INT(cc)
    END_PARAMETER_BIND

    bool IsSupport(TStrategyTickType ticktype)
    {
        return true;
    }
    
    BEGIN_PROBE_BIND
        BEGIN_GRAPH
            PROBE(&out_AskPrice1, "out_AskPrice1", TProbe_Color_Green)
        END_GRAPH("Line")
        BEGIN_GRAPH
            PROBE(&out_BidPrice1, "out_BidPrice1", TProbe_Color_Blue)
        END_GRAPH("Line")
    END_PROBE_BIND

    bool OnInquiry(MStrategyInquiryDataInterface*);
    TLastErrorIdType OnInit(ptime);
    void OnTick(TMarketDataIdType, const CTick *);
    void OnTrade(
        TOrderRefIdType,
        TOrderSysIdType,
        TVolumeType,
        TPriceType,
        TOrderDirectionType,
        TOrderOffsetType);
    void OnOrder(
        TOrderRefIdType,
        TOrderSysIdType,
        TOrderDirectionType,
        TOrderStatusType,
        TPriceType,
        TTradedVolumeType,
        TRemainVolumeType
        );
    void OnEndup();
    
    TLastErrorIdType OnInit_FromArchive(ptime)
    {
        return TLastErrorIdType::LB1_NO_ERROR;
    };
    TOrderRefIdType m_intIncreaseRef;
    TOrderRefIdType m_intDecreaseRef;
    enum {
        NoPosition,
        Increase_ing_Position,
        HasPosition,
        Descrease_ing_Position
    } m_enumPosition = NoPosition;
    bool OnGetPositionInfo(int *)
    {
        return true;
    }

    bool OnGetCustomInfo(char *, size_t)
    {
        return true;
    }

    bool OnGetFloatingProfit(double *)
    {
        return true;
    }

    bool OnGetStatus(char *, size_t)
    {
        return true;
    }
    unsigned int m_uCounter = 0;
private:
    atomic<double> out_AskPrice1;
    atomic<double> out_BidPrice1;
};

STRATEGY_TEMPLATE_DEFINITION(MyStrategy)

bool MyStrategy::OnInquiry(MStrategyInquiryDataInterface * data)
{
    return true;
}

TLastErrorIdType MyStrategy::OnInit(ptime)
{
    m_enumPosition = NoPosition;
    m_uCounter = 0;
    return LB1_NO_ERROR;
}

void MyStrategy::OnTick(TMarketDataIdType dataid, const CTick *pDepthMarketData)
{
    out_AskPrice1.store(pDepthMarketData->m_dbAskPrice[0]);
    out_BidPrice1.store(pDepthMarketData->m_dbBidPrice[0]);
    m_uCounter++;
    switch (m_enumPosition)
    {
    case NoPosition: {
        if (m_uCounter % 111 == 0)
        {
            m_intIncreaseRef = LIMITORDER(LB1_Buy, LB1_Increase, 1, pDepthMarketData->m_dbAskPrice[0] + 10, 0);
            m_enumPosition = Increase_ing_Position;
        }
    };break;
    case Increase_ing_Position: {

    };break;
    case HasPosition: {
        if (m_uCounter % 222 == 0)
        {
            m_intDecreaseRef = LIMITORDER(LB1_Sell, LB1_Decrease, 1, pDepthMarketData->m_dbBidPrice[0] - 10, 0);
            m_enumPosition = Descrease_ing_Position;
        }
    };break;
    case Descrease_ing_Position: {

    };break;
    }
    
}


void MyStrategy::OnEndup()
{
}


void MyStrategy::OnTrade(
    TOrderRefIdType ref,
    TOrderSysIdType sys,
    TVolumeType volume,
    TPriceType price,
    TOrderDirectionType dir,
    TOrderOffsetType offset)
{
    switch (m_enumPosition)
    {
        case NoPosition: 
        {
            
        };break;
        case Increase_ing_Position: 
        {
            if (ref == m_intIncreaseRef)
            {
                m_enumPosition = HasPosition;
            }
        };break;
        case HasPosition: 
        {
            
        };break;
        case Descrease_ing_Position: 
        {
            if (ref == m_intDecreaseRef)
            {
                m_enumPosition = NoPosition;
            }
        };break;
    }

}

void MyStrategy::OnOrder(
    TOrderRefIdType ref,
    TOrderSysIdType sysId,
    TOrderDirectionType direction,
    TOrderStatusType Status,
    TPriceType LimitPrice,
    TTradedVolumeType VolumeTraded,
    TRemainVolumeType VolumeRemain)
{

}
