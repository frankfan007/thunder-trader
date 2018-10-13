#pragma once
#include <set>
#include <map>
#include <queue>
#include <string>
#include <vector>
#include <list>
#include "StrategyData.h"
#include <tuple>
#include <unordered_map>
using namespace std;
using namespace StrategyData;

class COrder
{
public:
    string m_strOrderSysID;
    TOrderRefIdType m_OrderID;
    TOrderType m_enumOrderType;
    unsigned int m_uDataId;
    TOrderDirectionType m_enumDirection;
    TOrderOffsetType m_enumOffset;
    TOrderStatusType m_enumOrderStatus;
    ptime m_u64UTCDateTime;
    ptime m_u64TradeUTCDateTime;
    ptime m_u64TryCancelUTCDateTime;
    ptime m_u64CanceledUTCDateTime;
    TPriceType m_LimitPrice;
    TPriceType m_TradeLimitPrice;
    TVolumeType m_Volume;
    TVolumeType m_TradedVolume;
    TVolumeType m_RemainderPositionForReport;
    double m_dbProfit;
    double m_dbCommission;
    TOrderRefIdType m_uTargetPositionOrder;
    bool m_boolActived;
    unordered_map<
        TMarketDataIdType, 
        std::tuple<unsigned int, double, unsigned int, double> 
    > m_mapPositionInfo;
    
    bool operator<(COrder & ord)
    {
        return m_u64UTCDateTime < ord.m_u64UTCDateTime;
    }
};

