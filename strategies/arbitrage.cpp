// StrategyDemo.cpp : 定义 DLL 应用程序的导出函数。
//
#define _EXPORTS
#define EXPORT_STRATEGY
#include "StrategyDefine.h"
#include "StrategyData.h"
#include "Tick.h"
#include <cmath>
#include <list>
#include <string>
#include "Order.h"
#include <fstream>
#include "ArbitrageLibInterface.h"
using namespace std;


#define MyLIMITORDER LIMITORDER


#pragma region 自定义数据结构
enum E_TradeState{
	T_INI,//初始化
	T_OI_W,//进场等待
	T_OI_F,//进场成功
	T_OO_W,//出场等待
	T_OO_F,//出场成功
	T_OI_L,//进场止损
};

struct TradeOrder{
	E_TradeState tradeState;//成交驱动状态
	int inRef;//进场报单引用
	int stopProfit;//止盈跳
	int stopLoss;//止损跳
	double inDealPrice;//进场成交价
	double inRealDealPrice;//进场成交价
	bool isFirstIn;//是否先进场
	bool isFirstOut;//是否先出场
	string inSysID;//进场报单编号
	TOrderDirectionType direction_In;//0买，1卖
	TOrderDirectionType direction_Out;//0买，1卖
	int vol;//报单量	
	int dealVol;//成交量
	int outRef;//出场报单引用
	int outStopLossRef;//止损出场报单引用

	string outSysID;//出场报单编号
	double outDealPrice;//出场成交价
	double outRealDealPrice;//出场成交价
	int orderInWaiteCount;//进场单等待计时
	int orderOutWaiteCount;//出场单等待计时
};
class CRapidMoveAverage
{
	unsigned int m_uPeriodLength;
	vector<double> m_vecContainer;
	unsigned int m_uCurrentIndex;
	double m_dbSum;
	unsigned int m_uCountOfValue;
public:
	CRapidMoveAverage(unsigned int L) :m_uPeriodLength(L)
	{
		Reset();
	}

	void Reset()
	{
		m_uCountOfValue = 0;
		m_uCurrentIndex = 0;
		m_dbSum = 0;
		m_vecContainer.clear();
		m_vecContainer.resize(m_uPeriodLength);
		for (unsigned int i = 0; i<m_vecContainer.size(); i++)
			m_vecContainer[i] = 0.0f;
	}

	void AppendNewValue(double newvalue)
	{
		m_uCountOfValue++;
		m_dbSum -= m_vecContainer[m_uCurrentIndex];
		m_dbSum += (m_vecContainer[m_uCurrentIndex] = newvalue);
		m_uCurrentIndex = (m_uCurrentIndex + 1) % m_uPeriodLength;
	}

	double GetAverage()
	{
		if (m_uCountOfValue <= m_uPeriodLength)
			return m_dbSum / m_uCountOfValue;
		else
			return m_dbSum / m_uPeriodLength;
	}

	unsigned int GetValueCount()
	{
		return m_uCountOfValue;
	}
};
class CRapidMoveVariance
{
	unsigned int m_uPeriodLength;
	CRapidMoveAverage m_rma;
	vector<double> m_vecContainerXPow2;
	unsigned int m_uCurrentIndex;
	double m_dbSumOfXPow2;
	unsigned int m_uCountOfValue;
public:
	CRapidMoveVariance(unsigned int _PeriodLength) : m_uPeriodLength(_PeriodLength), m_rma(_PeriodLength)
	{
		Reset();
	}

	void AppendNewValue(double newvalue)
	{
		m_uCountOfValue++;
		m_rma.AppendNewValue(newvalue);
		m_dbSumOfXPow2 -= m_vecContainerXPow2[m_uCurrentIndex];
		m_dbSumOfXPow2 += m_vecContainerXPow2[m_uCurrentIndex] = pow(newvalue, 2);
		m_uCurrentIndex = (m_uCurrentIndex + 1) % m_uPeriodLength;
	}

	double GetAverage()
	{
		return m_rma.GetAverage();
	}

	double GetVariance()
	{
		//计算方差D(X)=E(X^2)-[E(X)]^2
		if (m_uCountOfValue <= m_uPeriodLength)
			return m_dbSumOfXPow2 / m_uCountOfValue - pow(m_rma.GetAverage(), 2);
		else
			return m_dbSumOfXPow2 / m_uPeriodLength - pow(m_rma.GetAverage(), 2);
	}

	unsigned int GetValueCount()
	{
		return m_uCountOfValue;
	}

	void Reset()
	{
		m_uCountOfValue = 0;
		m_uCurrentIndex = 0;
		m_dbSumOfXPow2 = 0;
		m_vecContainerXPow2.clear();
		m_vecContainerXPow2.resize(m_uPeriodLength);
		for (unsigned int i = 0; i<m_vecContainerXPow2.size(); i++)
			m_vecContainerXPow2[i] = 0.0f;
	}
};
#pragma endregion




class MyStrategy : public MStrategy 
{
public:
	MStrategyContext * g_pStrategyContext;
	TStrategyIdType g_StrategyId;
	CParNode m_Pars[64];
	CParNode * GetParamStruct()
	{

 
		PARAMETER(0, "forward_Mutli", LB1_IntType, &forward_Mutli);
		PARAMETER(1, "guard_Mutli", LB1_IntType, &guard_Mutli);
		PARAMETER(2, "forward_priceTick", LB1_DoubleType, &forward_priceTick);
		PARAMETER(3, "guard_priceTick", LB1_DoubleType, &guard_priceTick);
		PARAMETER(4, "countLock_Long", LB1_IntType, &countLock_Long);
		PARAMETER(5, "countLock_Short", LB1_IntType, &countLock_Short);
		PARAMETER(6, "bollingerLength", LB1_IntType, &bollingerLength);
		PARAMETER(7, "PriceTickMutli", LB1_IntType, &PriceTickMutli);
		PARAMETER(8, "askBidVolGap", LB1_IntType, &askBidVolGap);
		PARAMETER(9, "priceGap", LB1_IntType, &priceGap);
		PARAMETER(10, "forward_safeUpperWidth", LB1_IntType, &forward_safeUpperWidth);
		PARAMETER(11, "guard_safeUpperWidth", LB1_IntType, &guard_safeUpperWidth);
		PARAMETER(12, "forward_safeLowerWidth", LB1_IntType, &forward_safeLowerWidth);
		PARAMETER(13, "guard_safeLowerWidth", LB1_IntType, &guard_safeLowerWidth);
		PARAMETER(14, "firstTimeIn", LB1_TimeDurationType, &firstTimeIn);
		PARAMETER(15, "lastTimeIn", LB1_TimeDurationType, &lastTimeIn);
		PARAMETER(16, "clearTime", LB1_TimeDurationType, &clearTime);
		PARAMETER(17, "forward_FeeMutli", LB1_IntType, &forward_FeeMutli);
		PARAMETER(18, "guard_FeeMutli", LB1_IntType, &guard_FeeMutli);
		PARAMETER(19, "", LB1_IntType, NULL);
		return m_Pars;
	}
	bool IsSupport(TStrategyTickType ticktype);
	TLastErrorIdType OnInit();
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

	void OnRelease();
	MyStrategy(MStrategyContext* context,TStrategyIdType strategyid);
private:
	

#pragma region 全局变量区

	bool feeRatioType = true;
	double CodeMultiplier = 300;
	double feeRatio = 2.9e-5;
	int totalC = 0;
	
	double avgLastSpread;//最新价差均值
	double standDevLastSpread;//最新价差标准差
	vector<double> vecXpow2;
	CMyTick *pDpMarketData_forward;//前锋行情
	CMyTick *pDpMarketData_guard;



	bool isSpreadLong;
	double inProfit = 0;
	double inSpread = 0;
	vector <double> downSpreadList;
	vector <double> UpSpreadList;
	
	double inAvgPrice = 0;
	int stopLoss = 10;//几倍止损

	unsigned int uLpSpreadCount;
	double OverAllProfit = 0;
	CRapidMoveAverage * mavrc;

	bool isForwardFirst;
	
	TradeOrder forwardOrder;//前锋报单
	TradeOrder guardOrder;//后卫报单
	vector<CTick*> forwardDataList;		// 记录行情列表
	vector<CTick*> guardDataList;		// 记录行情列表
#pragma endregion

#pragma region 关键参数
	double forward_FeeRatio = 0.000029;
	double guard_FeeRatio = 0.000029;
	unsigned int forward_FeeMutli = 300;
	unsigned int guard_FeeMutli = 300;
	int forward_Mutli = 1;
	int guard_Mutli = 1;
	double forward_priceTick = 0.2;//最小变动价
	double guard_priceTick = 0.2;//最小变动价
	int countLock_Long = 50;
	int countLock_Short = 50;
	unsigned int bollingerLength = 1000;
	int PriceTickMutli = 10;
	double addPrice = 0;
	int priceGap = 20;
	int forward_safeUpperWidth = 80;
	int guard_safeUpperWidth = 80;
	int forward_safeLowerWidth = 80;
	int guard_safeLowerWidth = 80;
	time_duration firstTimeIn = time_duration(9, 25, 0, 0);
	time_duration lastTimeIn = time_duration(14, 50, 0, 0);
	time_duration clearTime = time_duration(14, 55, 0, 0);
	int askBidVolGap = 10;
#pragma endregion
	
	void InsertDownDevSpread(double inSpread);
	void InsertUpDevSpread(double inSpread);
	double CountListN(vector<double> inList, int N);
};


//class CustomArbitrageContext
//	:public MStrategyContext
//{
//public:
//	virtual bool MeddleResponse(TStrategyIdType, const char *, ...);
//	virtual bool ShowMessage(TStrategyIdType, const char *, ...);
//	virtual bool GetNextMeddle(TStrategyIdType, char * retbuffer, unsigned int maxlength);
//	virtual void IncreaseCounter(TStrategyIdType, const char * CounterName);
//	virtual void DescreaseCounter(TStrategyIdType, const char * CounterName);
//	virtual void PlayASound(TStrategyIdType, TSoundType sound);
//	virtual void MarkOnMain(TStrategyIdType, double value, const char * text, TMarkType plottype = LB1_MARK_Cross);
//	virtual void SerialOnMain(TStrategyIdType, const char * serialname, double value, TSerialType plottype = LB1_SERIAL_BAR);
//	virtual void SerialOnVice(TStrategyIdType, const char * serialname, double value, TSerialType plottype = LB1_SERIAL_BAR);
//};
//class CustomArbitrageInterface
//{
//	virtual CParNode * GetParamStruct() = 0;
//};
extern "C"
{
	STRATEGY_INTERFACE  MStrategy * CreateStrategyObject(MStrategyContext*context, TStrategyIdType stid)
	{
		MStrategy * ret;
		try{
			ret = new MyStrategy(context, stid);
		}
		catch (...)
		{
			return NULL;
		}
		return ret;
	}
}



bool MyStrategy::IsSupport(TStrategyTickType ticktype)
{
	if (ticktype == FutureTick || ticktype == OptionTick || ticktype == ForexTick)
		return true;
	else return false;
};

MyStrategy::MyStrategy(MStrategyContext* context, TStrategyIdType strategyid) :
g_pStrategyContext(context),
g_StrategyId(strategyid)
{
	

	if (context == NULL)
		throw 1;

	forward_Mutli = 1;
	guard_Mutli = 1;
	forward_priceTick = 0.2;//最小变动价
	guard_priceTick = 0.2;//最小变动价
	countLock_Long = 50;
	countLock_Short = 50;
	bollingerLength = 1000;
	PriceTickMutli = 10;
	addPrice = 0;
	priceGap = 20;
	forward_safeUpperWidth = 80;
	guard_safeUpperWidth = 80;
	forward_safeLowerWidth = 80;
	guard_safeLowerWidth = 80;
	firstTimeIn = time_duration(9, 25, 0, 0);
	lastTimeIn = time_duration(14, 50, 0, 0);
	clearTime = time_duration(14, 55, 0, 0);
	askBidVolGap = 10;

	
}

void MyStrategy::InsertDownDevSpread(double inSpread)
{
	downSpreadList.insert(downSpreadList.begin(), inSpread);
	if (downSpreadList.size() > bollingerLength){
		downSpreadList.pop_back();
	}
}

void MyStrategy::InsertUpDevSpread(double inSpread)
{
	UpSpreadList.insert(UpSpreadList.begin(), inSpread);
	if (UpSpreadList.size() > bollingerLength){
		UpSpreadList.pop_back();
	}
}

double MyStrategy::CountListN(vector<double> inList, int N)
{
	int length = inList.size();
	if (length <= N)
		return 0;
	nth_element(inList.begin(), inList.begin() + length - N - 1, inList.end());
	return inList[length - N - 1];
}

TLastErrorIdType MyStrategy::OnInit()
{
	
	pDpMarketData_forward = new CMyTick();
	pDpMarketData_guard = new CMyTick();
	forwardOrder.vol = forward_Mutli;//交易量设置
	forwardOrder.tradeState = T_INI;
	forwardOrder.inRef = -1;//进场报单引用
	forwardOrder.stopProfit = 2;//止盈跳
	forwardOrder.stopLoss = 2;//止损跳
	forwardOrder.inDealPrice = 0;//进场成交价
	forwardOrder.inSysID = "";//进场报单编号
	forwardOrder.direction_In = LB1_Buy;//0买，1卖
	forwardOrder.direction_Out = LB1_Buy;//0买，1卖
	forwardOrder.outRef = -1;//出场报单引用
	forwardOrder.outStopLossRef = 0;//止损出场报单引用
	forwardOrder.outSysID = "";//出场报单编号
	forwardOrder.outDealPrice = 0;//出场成交价
	forwardOrder.orderInWaiteCount = 0;
	forwardOrder.orderOutWaiteCount = 0;
	forwardOrder.dealVol = 0;

	guardOrder.vol = guard_Mutli;//交易量设置
	guardOrder.dealVol = 0;
	guardOrder.tradeState = T_INI;
	guardOrder.inRef = -1;//进场报单引用
	guardOrder.stopProfit = 2;//止盈跳
	guardOrder.stopLoss = 2;//止损跳
	guardOrder.inDealPrice = 0;//进场成交价
	guardOrder.inSysID = "";//进场报单编号
	guardOrder.direction_In = LB1_Buy;//0买，1卖
	guardOrder.direction_Out = LB1_Buy;//0买，1卖
	guardOrder.outRef = -1;//出场报单引用
	guardOrder.outStopLossRef = 0;//止损出场报单引用
	guardOrder.outSysID = "";//出场报单编号
	guardOrder.outDealPrice = 0;//出场成交价
	guardOrder.orderInWaiteCount = 0;
	guardOrder.orderOutWaiteCount = 0;

	//addPrice = guard_priceTick*10;
	addPrice = 0;
	OverAllProfit = 0;
	uLpSpreadCount = 0;
	downSpreadList.clear(); //downSpreadList.resize(bollingerLength);
	UpSpreadList.clear(); //UpSpreadList.resize(bollingerLength);
	isForwardFirst = true;
	mavrc = new CRapidMoveAverage(bollingerLength);
	return LB1_NO_ERROR;
}

void MyStrategy::OnTick(TMarketDataIdType dataid, const CTick *pDepthMarketData)
{
	totalC++;
	if (LB1_Data0 == dataid)
	{
		pDpMarketData_forward->m_dbLastPrice = pDepthMarketData->m_dbLastPrice;
		pDpMarketData_forward->m_u64UTCDateTime = pDepthMarketData->m_datetimeUTCDateTime;
		pDpMarketData_forward->m_dbBidPrice1 = pDepthMarketData->m_dbBidPrice1;
		pDpMarketData_forward->m_intBidVolume1 = pDepthMarketData->m_intBidVolume1;
		pDpMarketData_forward->m_dbAskPrice1 = pDepthMarketData->m_dbAskPrice1;
		pDpMarketData_forward->m_intAskVolume1 = pDepthMarketData->m_intAskVolume1;
		pDpMarketData_forward->m_boolValid = true;
	}
	else
	{
		pDpMarketData_guard->m_dbLastPrice = pDepthMarketData->m_dbLastPrice;
		pDpMarketData_guard->m_u64UTCDateTime = pDepthMarketData->m_datetimeUTCDateTime;
		pDpMarketData_guard->m_dbBidPrice1 = pDepthMarketData->m_dbBidPrice1;
		pDpMarketData_guard->m_intBidVolume1 = pDepthMarketData->m_intBidVolume1;
		pDpMarketData_guard->m_dbAskPrice1 = pDepthMarketData->m_dbAskPrice1;
		pDpMarketData_guard->m_intAskVolume1 = pDepthMarketData->m_intAskVolume1;
		pDpMarketData_guard->m_boolValid = true;
	}

	if (false == pDpMarketData_forward->m_boolValid || false == pDpMarketData_guard->m_boolValid)
		return;
	
	
#pragma region 计算数据
	double lastSpread = pDpMarketData_guard->m_dbLastPrice*guard_Mutli - pDpMarketData_forward->m_dbLastPrice*forward_Mutli;
	double upSpread = pDpMarketData_guard->m_dbAskPrice1*guard_Mutli - pDpMarketData_forward->m_dbBidPrice1*forward_Mutli;
	double downSpread = pDpMarketData_guard->m_dbBidPrice1*guard_Mutli - pDpMarketData_forward->m_dbAskPrice1*forward_Mutli;
	uLpSpreadCount++;
	mavrc->AppendNewValue(lastSpread);
	avgLastSpread = mavrc->GetAverage();
	if (uLpSpreadCount <  bollingerLength+1)
		return;


	double UpLevel_IN = 0;
	double DownLevel_IN = 0;

	InsertUpDevSpread(avgLastSpread - upSpread);
	InsertDownDevSpread(downSpread - avgLastSpread);

	UpLevel_IN = CountListN(UpSpreadList, countLock_Long);
	DownLevel_IN = CountListN(downSpreadList, countLock_Short);

	double PriceTick = (forward_priceTick + guard_priceTick) / 2;

	double shortCheck_IN = downSpread - avgLastSpread - DownLevel_IN;
	double longCheck_IN = avgLastSpread - upSpread- UpLevel_IN ;
	double shortCheck_OUT = downSpread - avgLastSpread;
	double longCheck_OUT = avgLastSpread - upSpread;

#pragma endregion


	
	StrategyState CurrentState;
	if (
		forwardOrder.tradeState == T_INI
		&&
		guardOrder.tradeState == T_INI
		)
		CurrentState = Outside;
	else if (
		forwardOrder.tradeState == T_OI_F
		&&
		guardOrder.tradeState == T_OI_F
		)
		CurrentState = Inside;
	else if (
		forwardOrder.tradeState == T_OI_W
		||
		guardOrder.tradeState == T_OI_W
		)
	{
		CurrentState = Entering;
	}
	else if (
		forwardOrder.tradeState == T_OO_W
		||
		guardOrder.tradeState == T_OO_W
		)
	{
		CurrentState = Exitting;
	}

	TTradeSignalType signal = GetTradeSignal(CMyTick *lega, CMyTick* legb, CurrentState);

#pragma region 入场信号生成
	MkShortIn = 
			shortCheck_IN > 0
		&& CurrentState== Outside
		;
	MkLongIn=
			longCheck_IN > 0
		&&	forwardOrder.tradeState == T_INI 
		&&	guardOrder.tradeState == T_INI 
		;
#pragma endregion

#pragma region 出场信号生成
	MkShortOut =(
					guardOrder.direction_In == LB1_Sell 
				&& 
					longCheck_OUT > 0
				);

	MkLongOut = (
					guardOrder.direction_In == LB1_Buy 
				&& 
					shortCheck_OUT > 0
				);
#pragma endregion

#pragma region 出场!!
	isForwardFirst = false;
	if (forwardOrder.tradeState == T_OI_F&&guardOrder.tradeState == T_OI_F)
	{
#pragma region 前锋与后卫均入场成功，准备出场

		if (MakeShortExit == signal)
		{
#pragma region MkShortOut
            //isTest = true;
			guardOrder.outDealPrice = pDpMarketData_guard->m_dbAskPrice1;
			forwardOrder.outDealPrice = pDpMarketData_forward->m_dbBidPrice1;
			if (!isForwardFirst)
			{//先挂远月单，做不利挂单				
				guardOrder.isFirstOut = true;
				guardOrder.dealVol = 0;
				guardOrder.outRef = MyLIMITORDER(
					guardOrder.direction_Out,
					LB1_Descrease,
					guardOrder.vol, 
                    pDpMarketData_guard->m_dbAskPrice1 + addPrice,
                    LB1_Data1);//以最新价挂单下单
				forwardOrder.isFirstOut = false;
				guardOrder.tradeState = T_OO_W;
			}
			else
			{
				forwardOrder.isFirstOut = true;
				forwardOrder.dealVol = 0;
				forwardOrder.outRef = MyLIMITORDER(
					forwardOrder.direction_Out,
					LB1_Descrease,
					forwardOrder.vol, 
                    pDpMarketData_forward->m_dbBidPrice1,
                    LB1_Data0);//以最新价挂单下单
				guardOrder.isFirstOut = false;
				forwardOrder.tradeState = T_OO_W;
			}
				LOG(
                "Time=%s,tick=%d,MakeShortOut upSpread=%f,avgLastSpread=%f,standDevLastSpread=%f",
				to_simple_string(pDpMarketData_forward->m_u64UTCDateTime).c_str(),
				totalC,
				upSpread,
				avgLastSpread,
				standDevLastSpread);

			forwardOrder.orderOutWaiteCount = 0;
			guardOrder.orderOutWaiteCount = 0;
#pragma endregion
		}
		else if (MakeLongExit == signal)
		{
#pragma region MkLongOut

				guardOrder.outDealPrice = pDpMarketData_guard->m_dbBidPrice1;
				forwardOrder.outDealPrice = pDpMarketData_forward->m_dbAskPrice1;
				if (!isForwardFirst)
				{
					guardOrder.isFirstOut = true;
					guardOrder.dealVol = 0;
					guardOrder.outRef = MyLIMITORDER(
						guardOrder.direction_Out,
						LB1_Descrease,
						guardOrder.vol, 
                        pDpMarketData_guard->m_dbBidPrice1 - addPrice,
                        LB1_Data1);//以最新价挂单下单
					forwardOrder.isFirstOut = false;
					guardOrder.tradeState = T_OO_W;
				}
				else
				{
					forwardOrder.isFirstOut = true;
					forwardOrder.dealVol = 0;
					forwardOrder.outRef = MyLIMITORDER(
						forwardOrder.direction_Out,
						LB1_Descrease,
						forwardOrder.vol, 
                        pDpMarketData_forward->m_dbAskPrice1,
                        LB1_Data0);//以最新价挂单下单
					guardOrder.isFirstOut = false;
					forwardOrder.tradeState = T_OO_W;
				}

				LOG_FOR_OnTick(
                    "Time=%s,tick=%d,MakeLongOut downSpread=%f,avgLastSpread=%f,standDevLastSpread=%f",
					to_simple_string(pDpMarketData_forward->m_u64UTCDateTime).c_str(),
					totalC,
					upSpread,
					avgLastSpread,
					standDevLastSpread);

				forwardOrder.orderOutWaiteCount = 0;
				guardOrder.orderOutWaiteCount = 0;
#pragma endregion
		}
#pragma endregion
	}
#pragma endregion

#pragma region 入场!!
	if (MakeShortEnter == signal)
	{	
#pragma region 前锋和后卫均在场外，已经找到机会，启动做空价差,赌价差会变小，guard做空，forward做多
		guardOrder.inDealPrice = pDpMarketData_guard->m_dbBidPrice1;
		forwardOrder.inDealPrice = pDpMarketData_forward->m_dbAskPrice1;
		//先挂远月单，
		if (!isForwardFirst)//价格空头趋势
		{
			guardOrder.isFirstIn = true;
			guardOrder.dealVol = 0;
			guardOrder.inRef = MyLIMITORDER(
				LB1_Sell,
				LB1_Increase,
				guardOrder.vol, 
                pDpMarketData_guard->m_dbBidPrice1 - addPrice,
                LB1_Data1);//不利挂单
			forwardOrder.isFirstIn = false;
			guardOrder.tradeState = T_OI_W;
		}
		else
		{//价格多头趋势，先挂近月单
			forwardOrder.isFirstIn = true;
			forwardOrder.dealVol = 0;
			forwardOrder.inRef = MyLIMITORDER(
				LB1_Buy, 
				LB1_Increase, 
				forwardOrder.vol, 
                pDpMarketData_forward->m_dbAskPrice1,
                LB1_Data0);//以最新价挂单下单
			guardOrder.isFirstIn = false;
			forwardOrder.tradeState = T_OI_W;
		}
		inAvgPrice = avgLastSpread;
		inProfit = abs(downSpread - avgLastSpread);
		forwardOrder.direction_In = LB1_Buy;
		guardOrder.direction_In = LB1_Sell;
		forwardOrder.direction_Out = LB1_Sell;
		guardOrder.direction_Out = LB1_Buy;
		forwardOrder.orderInWaiteCount = 0;
		guardOrder.orderInWaiteCount = 0;	
		LOG_FOR_OnTick("Time=%s,tick=%d,MakeShortIn downSpread=%f,avgLastSpread=%f,standDevLastSpread=%f",
			to_simple_string(pDpMarketData_forward->m_u64UTCDateTime).c_str(), totalC, downSpread, avgLastSpread, standDevLastSpread);
#pragma endregion
	}
	else if (MakeLongEnter == signal)
	{					
#pragma region 前锋和后卫均在场外，已经找到机会，启动做多价差，赌价差会变大，guard做多，forward做空
		guardOrder.inDealPrice = pDpMarketData_guard->m_dbAskPrice1;
		forwardOrder.inDealPrice = pDpMarketData_forward->m_dbBidPrice1;
		if (!isForwardFirst)
		{//先挂远月单
			guardOrder.isFirstIn = true;
			guardOrder.dealVol = 0;
			guardOrder.inRef = MyLIMITORDER(
				LB1_Buy,
				LB1_Increase, 
				guardOrder.vol, 
                pDpMarketData_guard->m_dbAskPrice1 + addPrice,
                LB1_Data1);//以最新价挂单下单
			forwardOrder.isFirstIn = false;
			guardOrder.tradeState = T_OI_W;
		}
		else
		{//先挂近月单					
			forwardOrder.isFirstIn = true;
			forwardOrder.dealVol = 0;
			forwardOrder.inRef = MyLIMITORDER(
				LB1_Sell,
				LB1_Increase, 
				forwardOrder.vol, 
                pDpMarketData_forward->m_dbBidPrice1,
                LB1_Data0);//以最新价挂单下单
			guardOrder.isFirstIn = false;
			forwardOrder.tradeState = T_OI_W;
		}
		inProfit = abs(upSpread - avgLastSpread);
		inAvgPrice = avgLastSpread;
		forwardOrder.direction_In = LB1_Sell;
		guardOrder.direction_In = LB1_Buy;
		forwardOrder.direction_Out = LB1_Buy;
		guardOrder.direction_Out = LB1_Sell;
		forwardOrder.orderInWaiteCount = 0;
		guardOrder.orderInWaiteCount = 0;
		LOG_FOR_OnTick("Time=%s,tick=%d,MakeLongIn upSpread=%f,avgLastSpread=%f,standDevLastSpread=%f",
			to_simple_string(pDpMarketData_forward->m_u64UTCDateTime).c_str(), totalC, upSpread, avgLastSpread, standDevLastSpread);
#pragma endregion
	}
#pragma endregion

#pragma region 撤单!!
	if (forwardOrder.tradeState == T_OI_W)
	{
#pragma region 前锋进场等待中
		if (forwardOrder.isFirstIn)
		{//前锋先进场
            if (forwardOrder.orderInWaiteCount > 1 && forwardOrder.inSysID.size()>0)
				_CANCEL((char*)forwardOrder.inSysID.c_str(), LB1_Data0);
		}
		else
		{//后卫进场后，前锋跟着进场
			if (forwardOrder.orderInWaiteCount > 1 && forwardOrder.inSysID.size()>0)
				_CANCEL((char*)forwardOrder.inSysID.c_str(), LB1_Data0);
		}
		forwardOrder.orderInWaiteCount++;
#pragma endregion
	}

	if (forwardOrder.tradeState == T_OO_W)
	{  
#pragma region 前锋出场等待中
		if (forwardOrder.isFirstOut)
		{//前锋先出场
        if (forwardOrder.orderOutWaiteCount > 1	&&
            forwardOrder.outSysID.size()>0)
			{
                _CANCEL((char*)forwardOrder.outSysID.c_str(),LB1_Data0);
				
			}
		}
		else
		{//后卫出场后，前锋跟着出场
            if (forwardOrder.orderOutWaiteCount > 1
                &&  forwardOrder.outSysID.size()>0)
			{
                _CANCEL((char*)forwardOrder.outSysID.c_str(),LB1_Data0);
				
			}
		}
		forwardOrder.orderOutWaiteCount++;
#pragma endregion
	}

	if (guardOrder.tradeState == T_OI_W)
	{  
#pragma region 后卫入场等待中
		if (guardOrder.isFirstIn)
        {//先进场
            if (guardOrder.orderInWaiteCount > 1
                && guardOrder.inSysID.size()>0)
			{
				_CANCEL((char*)guardOrder.inSysID.c_str(), LB1_Data1);
				
			}
		}
		else
		{
           if (guardOrder.orderInWaiteCount > 1
                && guardOrder.inSysID.size()>0)
			{
			   _CANCEL((char*)guardOrder.inSysID.c_str(), LB1_Data1);
				
			}
		}
		guardOrder.orderInWaiteCount++;
#pragma endregion
	}

	if (guardOrder.tradeState == T_OO_W)
	{   
#pragma region 后卫出场等待中
		if (guardOrder.isFirstOut)
        {//ltq20150513
            if (
				guardOrder.orderOutWaiteCount > 1 
				&& 
				guardOrder.outSysID.size()>0)
			{
				_CANCEL((char*)guardOrder.outSysID.c_str(), LB1_Data1);
				
			}
		}
		else
		{
            if (guardOrder.orderOutWaiteCount > 1
                && guardOrder.outSysID.size()>0)
			{
				_CANCEL((char*)guardOrder.outSysID.c_str(), LB1_Data1);
				
			}
		}
		guardOrder.orderOutWaiteCount++;
#pragma endregion 
	}
#pragma endregion


  
	//201507161006新增 改正交易状态更新问题
	if ((forwardOrder.tradeState == E_TradeState::T_OO_F && guardOrder.tradeState == E_TradeState::T_OO_F) 
		||
		(forwardOrder.tradeState == E_TradeState::T_OO_F && guardOrder.tradeState == E_TradeState::T_INI) 
		||
		(forwardOrder.tradeState == E_TradeState::T_INI && guardOrder.tradeState == E_TradeState::T_OO_F)
		)
	{
		forwardOrder.tradeState = T_INI;
		forwardOrder.inSysID = "";
		forwardOrder.outSysID = "";
		forwardOrder.orderInWaiteCount = 0;
		forwardOrder.orderOutWaiteCount = 0;

		guardOrder.tradeState = T_INI;
		guardOrder.inSysID = "";
		guardOrder.outSysID = "";
		guardOrder.orderInWaiteCount = 0;
		guardOrder.orderOutWaiteCount = 0;
	}


#pragma region 干预策略
	char cmd[1024];
	if (false == MEDDLE(cmd, 1024))
		return;
	else
	{
		if (strcmp("show", cmd) == 0)
        {
			MEDDLERESPONSE(
                        "lastSpread:%lf upSpread:%lf downSpread:%lf",
                        lastSpread,upSpread,downSpread
                        );
			MEDDLERESPONSE(
                        "UpLevel_IN:%lf DownLevel_IN:%lf",
                        UpLevel_IN,DownLevel_IN
                        );
			MEDDLERESPONSE(
                        "shortCheck_IN:%lf longCheck_IN:%lf",
                        shortCheck_IN,longCheck_IN
                        );
			MEDDLERESPONSE(
                        "shortCheck_OUT:%lf longCheck_OUT:%lf",
                        shortCheck_OUT,longCheck_OUT
                        );
			MEDDLERESPONSE(
						"forwardOrder.tradeState:%d",
						forwardOrder.tradeState
						);
			MEDDLERESPONSE(
						"guardOrder.tradeState:%d",
						guardOrder.tradeState
						);
        }
    }
#pragma endregion

}

void MyStrategy::OnTrade(
    TOrderRefIdType ref,
    TOrderSysIdType sys,
	TVolumeType volume, 
	TPriceType price, 
	TOrderDirectionType dir, 
	TOrderOffsetType offset)
{
	if (ref	== guardOrder.outRef)
	{
		//后卫出场单成交
		if (guardOrder.tradeState != T_OI_L)
		{
#pragma region 前锋单子还未出场，后卫单率先出场
			if (guardOrder.isFirstOut)
			{
				if (dir == LB1_Buy)//后卫出场成功，立即挂前锋止盈单
				{
#pragma region 后卫单多头出场，立即挂前锋单止盈空头出场
					guardOrder.dealVol += volume;
					if (guardOrder.dealVol >= guardOrder.vol)
					{//全部成交
						forwardOrder.outRef = MyLIMITORDER(
							forwardOrder.direction_Out,
							LB1_Descrease,
							forwardOrder.vol, 
							pDpMarketData_forward->m_dbBidPrice1 - forward_priceTick,
							LB1_Data0);//以最新价挂单下单
						forwardOrder.tradeState = T_OO_W;
						guardOrder.outRealDealPrice = price;
						guardOrder.tradeState = T_OO_F;//后卫出场成功	
					}
#pragma endregion
				}
				else
				{
#pragma region 后卫单空头出场，立即挂前锋止盈多头出场
					guardOrder.dealVol += volume;
					if (guardOrder.dealVol >= guardOrder.vol)
					{//全部成交
						forwardOrder.outRef = MyLIMITORDER(
							forwardOrder.direction_Out,
							LB1_Descrease,
							forwardOrder.vol, 
							pDpMarketData_forward->m_dbAskPrice1 + forward_priceTick,
							LB1_Data0);//以最新价挂单下单	
						forwardOrder.tradeState = T_OO_W;
						guardOrder.outRealDealPrice = price;
						guardOrder.tradeState = T_OO_F;//后卫出场成功	
					}
#pragma endregion
				}
			}
			else
			{
				guardOrder.dealVol += volume;
				if (guardOrder.dealVol >= guardOrder.vol)
				{//全部成交
					guardOrder.outRealDealPrice = price;
					guardOrder.tradeState = T_OO_F;//后卫出场成功
				}
			}
#pragma endregion
		}
		else
			guardOrder.tradeState = T_INI;
		guardOrder.orderOutWaiteCount = 0;

		return;
	}
	else if (ref == guardOrder.inRef)
	{
#pragma region 后卫入场单成交
		if (guardOrder.isFirstIn)
		{
#pragma region 前锋单子还未入场成功，后卫单率先入场成功
			if (dir == LB1_Buy )
			{
				guardOrder.dealVol += volume;
				if (guardOrder.dealVol >= guardOrder.vol)
				{//全部成交
					forwardOrder.inRef = MyLIMITORDER(
						LB1_Sell,
						LB1_Increase,
						forwardOrder.vol, 
						pDpMarketData_forward->m_dbBidPrice1 - forward_priceTick,
						LB1_Data0);			
					forwardOrder.tradeState = T_OI_W;
					guardOrder.tradeState = T_OI_F;
					guardOrder.inRealDealPrice = price;
				}			
			}
			else
			{
				guardOrder.dealVol += volume;
				if (guardOrder.dealVol >= guardOrder.vol)
				{//全部成交
					forwardOrder.inRef = MyLIMITORDER(
						LB1_Buy,
						LB1_Increase,
						forwardOrder.vol, 
						pDpMarketData_forward->m_dbAskPrice1 + forward_priceTick,
						LB1_Data0);			
					forwardOrder.tradeState = T_OI_W;
					guardOrder.tradeState = T_OI_F;
					guardOrder.inRealDealPrice = price;
				}
				//}
				//else
				//{//价差不利，止损出场
				//	guardOrder.outRef = OrderInsert(1, pDpMarketData_guard->UpperLimitPrice, 0, 1, guardOrder.code);//不利挂单
				//	guardOrder.tradeState = T_OI_L;//进场止损
				//}
			}
#pragma endregion
		}
		else
		{
			guardOrder.dealVol += volume;
			if (guardOrder.dealVol >= guardOrder.vol)
			{//全部成交
				guardOrder.inRealDealPrice = price;
				guardOrder.tradeState = T_OI_F;//后卫进场成功		
			}
		}
		guardOrder.orderInWaiteCount = 0;
#pragma endregion
		return;
	}
	else if (ref	==	forwardOrder.outRef	)
	{
#pragma region 前锋出场单成交
		if (forwardOrder.tradeState != T_OI_L)
		{

			if (forwardOrder.isFirstOut)
			{
#pragma region 前锋单率先出场成功
				if (dir == LB1_Buy)//前锋出场成功，立即挂后卫止盈单
				{
					forwardOrder.dealVol += volume;
					if (forwardOrder.dealVol >= forwardOrder.vol)
					{//全部成交
						guardOrder.outRef = MyLIMITORDER(
							guardOrder.direction_Out,
							LB1_Descrease,
							guardOrder.vol, 
							pDpMarketData_guard->m_dbBidPrice1 - guard_priceTick,
							LB1_Data1);//以最新价挂单下单
						guardOrder.tradeState = T_OO_W;
						forwardOrder.outRealDealPrice = price;
						forwardOrder.tradeState = T_OO_F;//后卫出场成功	
					}
				}
				else
				{
					forwardOrder.dealVol += volume;
					if (forwardOrder.dealVol >= forwardOrder.vol)
					{//全部成交
						guardOrder.outRef = MyLIMITORDER(
							guardOrder.direction_Out,
							LB1_Descrease,
							guardOrder.vol, 
							pDpMarketData_guard->m_dbAskPrice1 + guard_priceTick,
							LB1_Data1);//以最新价挂单下单					
						guardOrder.tradeState = T_OO_W;
						forwardOrder.outRealDealPrice = price;
						forwardOrder.tradeState = T_OO_F;//后卫出场成功	
					}
				}
#pragma endregion
			}
			else
			{
				forwardOrder.dealVol += volume;
				if (forwardOrder.dealVol >= forwardOrder.vol)
				{//全部成交
					forwardOrder.outRealDealPrice = price;
					forwardOrder.tradeState = T_OO_F;//前锋出场成功
				}
			}
		}
		else
		{//止损出场
			forwardOrder.tradeState = T_INI;
		}
		forwardOrder.orderOutWaiteCount = 0;
#pragma endregion
		return;
	}
	else if (ref	==	forwardOrder.inRef	)
	{
#pragma region 前锋入场单成交
		if (forwardOrder.isFirstIn)
		{
#pragma region 后卫还未入场，前锋率先进场成功
			if (dir == LB1_Buy)
			{
#pragma region 前锋多头进场，后卫空头进场
				forwardOrder.dealVol += volume;
				if (forwardOrder.dealVol >= forwardOrder.vol)
				{//全部成交
					guardOrder.inRef = MyLIMITORDER(
						LB1_Sell,
						LB1_Increase,
						guardOrder.vol, 
						pDpMarketData_guard->m_dbBidPrice1 - guard_priceTick,
						LB1_Data1);//以最新价挂单下单				
					guardOrder.tradeState = T_OI_W;
					forwardOrder.tradeState = T_OI_F;
					forwardOrder.inRealDealPrice = price;
				}
#pragma endregion
			}
			else //(dir == LB1_Sell)
			{
#pragma region 前锋空头进场，后卫多头进场
				forwardOrder.dealVol += volume;
				if (forwardOrder.dealVol >= forwardOrder.vol)
				{//全部成交
					guardOrder.inRef = MyLIMITORDER(
						LB1_Buy,
						LB1_Increase,
						guardOrder.vol, 
						pDpMarketData_guard->m_dbAskPrice1 + guard_priceTick,
						LB1_Data1);//以最新价挂单下单				
					guardOrder.tradeState = T_OI_W;
					forwardOrder.tradeState = T_OI_F;
					forwardOrder.inRealDealPrice = price;
				}
#pragma endregion
			}
#pragma endregion
		}
		else
		{
#pragma region 后卫已经进场成功，前锋跟随进场成功
			forwardOrder.dealVol += volume;
			if (forwardOrder.dealVol >= forwardOrder.vol)
			{//全部成交
				forwardOrder.inRealDealPrice = price;
				forwardOrder.tradeState = T_OI_F;//前锋进场成功			
			}
#pragma endregion
		}
		forwardOrder.orderInWaiteCount = 0;
#pragma endregion
		return;
	}
	else
	{
		LOG("[!][OnTrade][!four]ThisRef=%d guardOrder.outRef=%d guardOrder.inRef=%d forwardOrder.outRef=%d forwardOrder.inRef=%d",
			ref,
			guardOrder.outRef,
			guardOrder.inRef,
			forwardOrder.outRef,
			forwardOrder.inRef
			);
		LOG("[!][OnTrade][!four]ThisRef=%d guardOrder.dealVol=%d guardOrder.dealVol=%d forwardOrder.dealVol=%d forwardOrder.dealVol=%d",
			ref,
			guardOrder.dealVol,
			guardOrder.dealVol,
			forwardOrder.dealVol,
			forwardOrder.dealVol
			);
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
    if (Status == LB1_StatusCanceled)
    {
        if (ref==guardOrder.inRef)//后卫入场撤单
        {

            if (guardOrder.isFirstIn)
            {
#pragma region 前锋单还未入场，后卫单率先尝试入场但是失败，现在已经撤单成功
                if (VolumeTraded == 0 && guardOrder.dealVol == 0)
                    guardOrder.tradeState = T_INI;
                else
                    guardOrder.inRef = MyLIMITORDER(
                        guardOrder.direction_In,
                        LB1_Increase,
                        VolumeRemaind,
							LB1_Buy == guardOrder.direction_In
						? 
							pDpMarketData_guard->m_dbAskPrice1 + 2 * guard_priceTick 
						: 
							pDpMarketData_guard->m_dbBidPrice1 - 2 * guard_priceTick,
                        LB1_Data1);
#pragma endregion
            }
			else
			{
#pragma region 前锋单已经入场，后卫单子必须无条件追单入场
				guardOrder.inRef = MyLIMITORDER(
					guardOrder.direction_In,
					LB1_Increase,
					VolumeRemaind,
					guardOrder.direction_In == LB1_Buy
					?
					pDpMarketData_guard->m_dbAskPrice1 + 2 * guard_priceTick
					:
					pDpMarketData_guard->m_dbBidPrice1 - 2 * guard_priceTick,
					LB1_Data1);
#pragma endregion
			}

            guardOrder.orderInWaiteCount = 0;

            return;
        }
        else if (ref==guardOrder.outRef)
        {
			//后卫出场撤单
            if (guardOrder.isFirstOut)
            {
#pragma region 前锋单子还未出场，后卫单子率先尝试出场但是失败，现在已经撤单成功
                if (VolumeTraded == 0 && guardOrder.dealVol == 0)
                    guardOrder.tradeState = T_OI_F;
                else
					guardOrder.outRef = MyLIMITORDER(//bug20150715
                        guardOrder.direction_Out,//bug20150715
                        LB1_Descrease,
                        VolumeRemaind,
						LB1_Buy == guardOrder.direction_Out
						? 
							pDpMarketData_guard->m_dbAskPrice1 + 2 * guard_priceTick 
						: 
							pDpMarketData_guard->m_dbBidPrice1 - 2 * guard_priceTick,
                        LB1_Data1);
#pragma endregion
            }
            else
            {
#pragma region 前锋单子已经成功出场，现在后卫单子尝试出场但是未成功，现在已经撤单成功，继续无条件追单
                guardOrder.outRef = MyLIMITORDER(
                    guardOrder.direction_Out,
                    LB1_Descrease,
                    VolumeRemaind,
						 LB1_Buy == guardOrder.direction_Out
					? 
						pDpMarketData_guard->m_dbAskPrice1 + 2 * guard_priceTick 
					: 
						pDpMarketData_guard->m_dbBidPrice1 - 2 * guard_priceTick,
                    LB1_Data1);
#pragma endregion
            }

            guardOrder.orderOutWaiteCount = 0;
            return;
        }
		else if (ref==forwardOrder.inRef)
        {
			//前锋入场撤单
            if (forwardOrder.isFirstIn)
            {
#pragma region 后卫还未入场，前锋率先尝试入场但是失败，现在已经撤单成功
                if (VolumeTraded == 0 && forwardOrder.dealVol == 0)
                    forwardOrder.tradeState = T_INI;
                else
                    forwardOrder.inRef = MyLIMITORDER(
                    forwardOrder.direction_In,
                    LB1_Increase,
                    VolumeRemaind,
						LB1_Buy == forwardOrder.direction_In 
					? 
						pDpMarketData_forward->m_dbAskPrice1 + 2 * forward_priceTick 
					: 
						pDpMarketData_forward->m_dbBidPrice1 - 2 * forward_priceTick,
                    LB1_Data0);
#pragma endregion
            }
            else
            {
#pragma region 后卫已经入场成功，前锋尝试入场但是失败，现在已经撤单成功
                forwardOrder.inRef = MyLIMITORDER(
                    forwardOrder.direction_In,
                    LB1_Increase,
                    VolumeRemaind,
						LB1_Buy == forwardOrder.direction_In
					? 
						pDpMarketData_forward->m_dbAskPrice1 + 2 * forward_priceTick 
					: 
						pDpMarketData_forward->m_dbBidPrice1 - 2 * forward_priceTick,
                    LB1_Data0);
#pragma endregion
            }
            forwardOrder.orderInWaiteCount = 0;
    #pragma endregion
            return;
        }
		else if (ref==forwardOrder.outRef)
        {
			//前锋出场撤单
            if (forwardOrder.isFirstOut)
            {
#pragma region 后卫还未出场，前锋率先尝试出场但是失败，现在已经撤单成功
                if (VolumeTraded == 0 && forwardOrder.dealVol == 0)
                   forwardOrder.tradeState = T_OI_F;
                else
                    forwardOrder.outRef = MyLIMITORDER(
                    forwardOrder.direction_Out,
                    LB1_Descrease,
                    VolumeRemaind,
						LB1_Buy == forwardOrder.direction_Out 
					? 
						pDpMarketData_forward->m_dbAskPrice1 + 2 * forward_priceTick 
					: 
						pDpMarketData_forward->m_dbBidPrice1 - 2 * forward_priceTick,
                    LB1_Data0);
#pragma endregion
            }
            else
            {
#pragma region 后卫已经成功出场，前锋尝试出场但是失败，现在 已经撤单成功
                forwardOrder.outRef = MyLIMITORDER(
                    forwardOrder.direction_Out,
                    LB1_Descrease,
                    VolumeRemaind,
						LB1_Buy == forwardOrder.direction_Out
					?
						pDpMarketData_forward->m_dbAskPrice1 + 2 * forward_priceTick 
					: 
						pDpMarketData_forward->m_dbBidPrice1 - 2 * forward_priceTick,
                    LB1_Data0);
            }
            forwardOrder.orderOutWaiteCount = 0;
#pragma endregion
            return;
        }
    }
	else
	{
		if (ref == forwardOrder.inRef)
			forwardOrder.inSysID = sysId;
		else if (ref == forwardOrder.outRef)
			forwardOrder.outSysID = sysId;
		else if (ref == guardOrder.inRef)
			guardOrder.inSysID = sysId;
		else if (ref == guardOrder.outRef)
			guardOrder.outSysID = sysId;
    }
}

void MyStrategy::OnEndup()
{
	if (mavrc)
	{
		delete mavrc;
		mavrc = NULL;
	}
	if (pDpMarketData_forward)
	{
		delete pDpMarketData_forward;
		pDpMarketData_forward = NULL;
	}
	if (pDpMarketData_guard)
	{
		delete pDpMarketData_guard;
		pDpMarketData_guard = NULL;
	}
}

void MyStrategy::OnRelease()
{
	delete this;
}

