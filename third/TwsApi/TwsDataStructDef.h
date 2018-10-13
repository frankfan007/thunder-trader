#ifndef _TWS_DATA_STRUCT_DEF_H_
#define _TWS_DATA_STRUCT_DEF_H_

#include "TwsDataTypeDef.h"
#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
using namespace boost::posix_time;
using namespace boost::gregorian;
struct CTwsRspUserLoginField
{
	TTwsOrderIdType m_intNextValidId;
	TTwsTimeType m_LongServerTime;
	char m_strManagedAccounts[64];
};

struct CTwsDepthMarketDataField
{
		
	bool	m_boolValid;
	long	m_longConId;
	char	m_strSymbol[64];
	char	m_strSecType[64];
	char	m_strExpiry[64];
	double	m_dbStrike;
	char	m_strRight[64];
	char	m_strMultiplier[64];
	char	m_strExchange[64];
	char	m_strPrimaryExchange[64];
	char	m_strCurrency[64];
	char	m_strLocalSymbol[64];
	char	m_strTradingClass[64];
	bool	m_boolIncludeExpired;
	char	m_strSecIdType[64];
	char	m_strSecId[64];
	ptime	m_datetimeUTCDateTime;
	TTwsPriceType	m_dbLastPrice;
	TTwsVolumeType	m_intVolume;
	TTwsPriceType	m_dbBidPrice1;
	TTwsVolumeType	m_intBidVolume1;
	TTwsPriceType	m_dbAskPrice1;
	TTwsVolumeType	m_intAskVolume1;
		
	TTwsPriceType	m_dbOpenPrice;
	TTwsPriceType	m_dbHighPrice;
	TTwsPriceType	m_dbLowPrice;
	TTwsPriceType	m_dbClosePrice;
	TTwsVolumeType	m_intLastSize;
};

struct CTwsInputOrderField
{
	TTwsOrderIdType m_longOrderId;
	TTwsActionType m_enumAction;//BUY， SELL， SSHORT
	TTwsVolumeType m_longTotalQuantity;//定单数量。
	TTwsTimeInForceType m_enumTimeInForceType;//确认定单类型。取消前有效GTC
	TTwsPriceType   m_dbLmtPrice;//限价价格，用于限价单、止损限价单和相对定单。 所有其它情形下为零。 对没有限价价格的相对定单，也为零。
	TTwsPriceType   m_dbAuxPrice;
	TTwsVolumeType  m_longMinQty;
};

struct CTwsContractField
{
	char			m_strSymbol[32];
	TTwsSecTypeType	m_enumSecType;
	char			m_strExpiry[32];
	double			m_dbStrike;
	char			m_strMultiplier[32];
	char			m_strExchange[32];
	char			m_strPrimaryExchange[32];
	char			m_strCurrency[32];
	char			m_strLocalSymbol[32];
	char			m_strTradingClass[32];
};

struct CTwsOrderField
{
	TTwsClientIDType m_uClientID;
	TTwsOrderIdType m_longOrderId;
	TTwsOrderStatusType m_enumStatus;
	TTwsVolumeType m_longFilledVolume;
	TTwsVolumeType m_longRemainingVolume;
	TTwsPriceType m_dbAvgFillPrice;
	TTwsPriceType m_dbLastFillPrice;
};

struct CTwsTradeField
{
	TTwsClientIDType m_uClientID;
	TTwsOrderIdType m_longOrderId;
	TTwsOrderSideType m_enumSide;
	TTwsVolumeType m_longFilledVolume;
	TTwsPriceType m_dbFilledPrice;
	TTwsPriceType m_dbFilledAvgPrice;
};

struct CTwsAccountValue
{
	TTwsPriceType m_dbCashBalance;//账户现金余额
	TTwsCurrencyType m_enumCurrency;//货币字符串
	unsigned int m_uDayTradesRemaining;//剩余日交易次
	TTwsPriceType m_dbEquityWithLoanValue;//含贷款价值权益
	TTwsPriceType m_dbInitMarginReq;//当前初始保证金要求
	TTwsPriceType m_dbLongOptionValue;//做多期权头寸
	TTwsPriceType m_dbMaintMarginReq;//当前维持保证金
	TTwsPriceType m_dbNetLiquidation;//净清算值
	TTwsPriceType m_dbOptionMarketValue;//期权市值
	TTwsPriceType m_dbShortOptionValue;//做空期权值
	TTwsPriceType m_dbStockMarketValue;//股票市值
	TTwsPriceType m_dbUnalteredInitMarginReq;//隔夜初始保证金要求
	TTwsPriceType m_dbUnalteredMaintMarginReq;//隔夜维持保证金要求
};
#endif
