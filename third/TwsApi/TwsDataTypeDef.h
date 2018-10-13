#ifndef _TWS_DATA_TYPE_DEF_H_
#define _TWS_DATA_TYPE_DEF_H_

typedef unsigned long TTwsOrderIdType;
typedef long TTwsTimeType;
typedef double TTwsPriceType;
typedef long TTwsVolumeType;
typedef unsigned int TTwsClientIDType;
enum TTwsCurrencyType {USD,JPY,GBP,CHF,};
enum TTwsActionType { Tws_BUY, Tws_SELL, Tws_SSHORT };
enum TTwsTimeInForceType { Tws_DAY, Tws_GTC, Tws_IOC, Tws_GTD };
enum TTwsSecTypeType { Tws_STK, Tws_OPT, Tws_FUT, Tws_IND, Tws_FOP, Tws_CASH, Tws_BAG};
enum TTwsOrderSideType { Tws_BOT, Tws_SLD};
enum TTwsOrderStatusType { Tws_PendingSubmit , Tws_PendingCancel, Tws_PreSubmitted, Tws_Submitted, Tws_Cancelled, Tws_Filled, Tws_Inactive};
#endif