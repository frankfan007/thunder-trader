//原子事务下单，该类能保证一篮子报单能最终全部成交，返回成交的结果
//Code by 胡逸峰 20160729
#ifndef _COMMONFILES_LIMITORDERTRANSACTIONBASE_H_
#define _COMMONFILES_LIMITORDERTRANSACTIONBASE_H_
#include "StrategyDefine.h"
#include "StrategyData.h"
#include "Tick.h"
#include <string>
#include <tuple>
#include <set>
using namespace std;

typedef int TCustomOrderRef;


typedef tuple<
	TOrderDirectionType,/*DirectionI*/
	TOrderOffsetType,/*OffsetI*/
	TVolumeType,/*VolumeI*/
	TMarketDataIdType,/*DataidI*/
	TPriceType,/*PriceTickI*/
	int,/*PriceLevel*/
	bool/*NeedSysId*/
> TTransactionElementType;
#define DirectionI 0
#define OffsetI 1
#define VolumeI 2
#define DataidI 3
#define PriceTickI 4
#define PriceLevel 5
#define NeedSysId 6

typedef pair<double/*TradedTurnoverI*/, int/*TradedVolumeI*/> CElementTradeResult;
#define TradedTurnoverI 0
#define TradedVolumeI 1

class CBasketElement
{
public:
	class CCurrent
	{
	public:
		ptime m_ptimeLimitOrderTime;
		TOrderRefIdType m_intCurrentRefid;
		string m_strOrderSysid;
		TVolumeType m_intTradedVol;
		TPriceType m_dbTradedTurnover;
	};

	TCustomOrderRef m_intCustomRef;
	TMarketDataIdType m_uTargetDataid;
	TOrderDirectionType m_enumDirection;
	TOrderOffsetType m_enumOffset;
	TVolumeType m_intTargetVol;
	TPriceType m_dbPriceTick;
	int m_intPriceLevel;
	bool m_bIsNeedSysid;
	
	CCurrent m_Current;

	bool IsAlltraded() { return m_Current.m_intTradedVol == m_intTargetVol; }
	CBasketElement(){}
	CBasketElement(
		TCustomOrderRef CustomRef,
		TMarketDataIdType Dataid,
		TOrderDirectionType Dir,
		TOrderOffsetType Offset,
		TVolumeType TargetV,
		TPriceType PriceTick,
		int PriceLev,
		bool NeedSysid,
		const ptime & OrderTime,
		TOrderRefIdType Ref)
	{
		m_intCustomRef = CustomRef;
		m_uTargetDataid = Dataid;
		m_enumDirection = Dir;
		m_enumOffset = Offset;
		m_intTargetVol = TargetV;
		m_dbPriceTick = PriceTick;
		m_intPriceLevel = PriceLev;
		m_bIsNeedSysid = NeedSysid;

		m_Current.m_ptimeLimitOrderTime = OrderTime;
		m_Current.m_intCurrentRefid = Ref;
		m_Current.m_strOrderSysid = "";
		m_Current.m_intTradedVol = 0;
		m_Current.m_dbTradedTurnover = 0.0;
		
	}
};

typedef map<TOrderRefIdType, CBasketElement> TBasketType;

class CLimitOrderTransactionBase;

class MTransactionCallBackInterface
{
public:
	virtual void OnTransactionTraded(CLimitOrderTransactionBase * _this, map<TCustomOrderRef, CElementTradeResult>&) = 0;
};

class CLimitOrderTransactionBase
{
	MStrategyContext * g_pStrategyContext;				//下单接口句柄
	TStrategyIdType g_StrategyId;						//策略ID
	MTransactionCallBackInterface * g_TradedCallBack;	//成交回调句柄

	int m_intWaitMillisecond = 1500;					//撤单等待时间
	bool m_bCanExcute = true;							//当前是否可执行
	TBasketType m_bskOrderBasket;						//篮子报单信息
	ptime m_ptimeGlobalUTCTime;							//最新时间
	map<TMarketDataIdType, CTick> m_mapTicks;			//记录所有tick
	

	bool IsBasketAllTraded(TBasketType & basket)
	{
		for (auto & Ele : basket)
			if (Ele.second.IsAlltraded() == false)
				return false;
		return true;
	}

public:
	
	TLastErrorIdType OnInit(ptime time,MStrategyContext* context, TStrategyIdType strategyid, MTransactionCallBackInterface * callback)
	{
		g_pStrategyContext = context;
		g_StrategyId = strategyid;
		g_TradedCallBack = callback;

		m_bCanExcute = true;

		m_bskOrderBasket.clear();
		m_ptimeGlobalUTCTime = time;
		m_mapTicks.clear();
		return LB1_NO_ERROR;
	}

	void OnTick(TMarketDataIdType dataid, const CTick * tick)
	{
		m_ptimeGlobalUTCTime = tick->m_datetimeUTCDateTime;
		m_mapTicks[dataid] = *tick;
		for (auto & order : m_bskOrderBasket)
		{
			if (
				false == order.second.IsAlltraded()
				&&
				m_ptimeGlobalUTCTime - order.second.m_Current.m_ptimeLimitOrderTime>milliseconds(m_intWaitMillisecond)
				&&
				(order.second.m_bIsNeedSysid? (false==order.second.m_Current.m_strOrderSysid.empty()):true)
				)
			{
				CANCEL(order.first,(char *)order.second.m_Current.m_strOrderSysid.c_str(),order.second.m_uTargetDataid);
			}
		}
	}

	void OnTrade(
		TOrderRefIdType ref,
		TOrderSysIdType,
		TVolumeType volume,
		TPriceType price,
		TOrderDirectionType dir,
		TOrderOffsetType offset)
	{
		auto FindResult = m_bskOrderBasket.find(ref);
		if (m_bskOrderBasket.end() != FindResult)
		{
			FindResult->second.m_Current.m_intTradedVol += volume;
			FindResult->second.m_Current.m_dbTradedTurnover += volume*price;
		}
		if (IsBasketAllTraded(m_bskOrderBasket))
		{
			map<TCustomOrderRef, CElementTradeResult> result;
			for (auto & ele : m_bskOrderBasket)
			{
				get<TradedTurnoverI>(result[ele.second.m_intCustomRef]) = ele.second.m_Current.m_dbTradedTurnover;
				get<TradedVolumeI>(result[ele.second.m_intCustomRef]) = ele.second.m_Current.m_intTradedVol;
			}
			if(g_TradedCallBack)
				g_TradedCallBack->OnTransactionTraded(this, result);
			m_bCanExcute = true;
		}
	}

	void OnOrder(
		TOrderRefIdType ref,
		TOrderSysIdType sys,
		TOrderDirectionType,
		TOrderStatusType status,
		TPriceType,
		TTradedVolumeType,
		TRemainVolumeType
		)
	{
		auto FindResult = m_bskOrderBasket.find(ref);
		if (FindResult != m_bskOrderBasket.end())
		{
			if (LB1_StatusCanceled == status)
			{
				auto LackedVolume = FindResult->second.m_intTargetVol - FindResult->second.m_Current.m_intTradedVol;
				auto Dataid = FindResult->second.m_uTargetDataid;

				auto NewRef = LIMITORDER(
					FindResult->second.m_enumDirection,
					FindResult->second.m_enumOffset,
					LackedVolume,
					FindResult->second.m_enumDirection == LB1_Buy ?
					m_mapTicks[Dataid].m_dbAskPrice[0] + FindResult->second.m_dbPriceTick *FindResult->second.m_intPriceLevel
					:
					max(m_mapTicks[Dataid].m_dbBidPrice[0] - FindResult->second.m_dbPriceTick *FindResult->second.m_intPriceLevel, FindResult->second.m_dbPriceTick),
					Dataid
					);

				m_bskOrderBasket[NewRef] = m_bskOrderBasket[ref];
				m_bskOrderBasket[NewRef].m_Current.m_ptimeLimitOrderTime = m_ptimeGlobalUTCTime;
				m_bskOrderBasket[NewRef].m_Current.m_intCurrentRefid = NewRef;
				m_bskOrderBasket[NewRef].m_Current.m_strOrderSysid = "";
				m_bskOrderBasket.erase(ref);
			}
			else
			{
				if (m_bskOrderBasket[ref].m_bIsNeedSysid)
					m_bskOrderBasket[ref].m_Current.m_strOrderSysid = sys;
			}
		}
	}

	void ExcuteTransaction(const map<TCustomOrderRef,TTransactionElementType> & elements)
	{
		if (false == m_bCanExcute)
			return;
		m_bCanExcute = false;
		m_bskOrderBasket.clear();
		for (auto & ele : elements)
		{
			auto CustomRef = ele.first;
			auto Element = ele.second;
			auto Ref = LIMITORDER(
				get<DirectionI>(Element),
				get<OffsetI>(Element),
				get<VolumeI>(Element),
				get<DirectionI>(Element) == LB1_Buy ?
				m_mapTicks[get<DataidI>(Element)].m_dbAskPrice[0] + get<PriceTickI>(Element)*get<PriceLevel>(Element)
				:
				max(m_mapTicks[get<DataidI>(Element)].m_dbBidPrice[0] - get<PriceTickI>(Element)*get<PriceLevel>(Element), get<PriceTickI>(Element)),
				get<DataidI>(Element));
			
			m_bskOrderBasket[Ref] = CBasketElement(CustomRef,
				get<DataidI>(Element), get<DirectionI>(Element), get<OffsetI>(Element), get<VolumeI>(Element),
				get<PriceTickI>(Element), get<PriceLevel>(Element), get<NeedSysId>(Element), m_ptimeGlobalUTCTime, Ref);
		}
	}

	bool CanExcute() { return m_bCanExcute; };

};
#endif