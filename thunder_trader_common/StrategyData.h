#pragma once
#include <string>
#include <map>
#include <vector>
#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <atomic>

using namespace boost::posix_time;
using namespace std;
class CTick;
class MStrategyContext;
class MStrategy;
namespace StrategyData
{
	typedef int TSharedIndexType;
	typedef long long TOrderRefIdType;
#define LB1_NullOrderRef -1
	typedef unsigned int TCustomRefPartType;
	typedef char * TOrderSysIdType;
	typedef long long TActionIdType;
	typedef int TVolumeType;
	typedef int TTradedVolumeType;
	typedef int TRemainVolumeType;
	typedef double TPriceType;
	typedef int TStrategyIdType;
	typedef char TInstrumentIDType[32];
	enum TLastErrorIdType{ LB1_NO_ERROR, LB1_NULL_PTR, LB1_INVALID_VAL };
	typedef unsigned int TMarketDataIdType;
	enum TOrderPriceType{
		OPLastPrice,
		OPAskPrice1, OPAskPrice2, OPAskPrice3, OPAskPrice4, OPAskPrice5,
		OPBidPrice1, OPBidPrice2, OPBidPrice3, OPBidPrice4, OPBidPrice5,
	};
	enum TStrategyTickType { FutureTick, OptionTick, ForexTick,StockTick,TwsTick };
	enum TPositionType{ LB1_Long, LB1_Short };
	enum TSoundType{ LB1_Warming, LB1_Error };
	enum THedgeFlagType{ LB1_Speculation, LB1_Arbitrage, LB1_Hedge };
	enum TOrderDirectionType{ LB1_Buy=0, LB1_Sell=1, LB1_UnknownDirection=2 };
	enum TOrderOffsetType{ LB1_Increase, LB1_Decrease, LB1_DecreaseYesterday, LB1_DecreaseToday, LB1_UnknownOffset };
	enum TOrderType{ 
		LB1_NormalLimitOrderType=0, 
		LB1_MarketOrderType=1,
		LB1_FOKLimitOrderType=2,
		LB1_FAKLimitOrderType=3
	};
	enum TOrderStatusType{
		LB1_StatusNoTradeQueueing,
		LB1_StatusPartTradedQueueing,
		LB1_StatusAllTraded,
		LB1_StatusCanceled,
		LB1_StatusUnknown
	};
#define MAXPARNAMELENGTH 64
	struct CParNode{
		char m_arrayParname[MAXPARNAMELENGTH];
		int32_t m_intOption;
		int * m_pIntAddress;
		double * m_pDoubleAddress;
		time_duration * m_pTimeDuraAddress;
		char * m_pStringAddress;
		
	};
#define MAXPARCOUNT 128
	typedef MStrategy * (*TFNCreateStrategyObject)(MStrategyContext*, TStrategyIdType);
#define BEGIN_PARAMETER_BIND CParNode m_array2ParameterStruct[MAXPARCOUNT+1] = {
#define END_PARAMETER_BIND { "",0,nullptr,nullptr,nullptr,nullptr}};\
						public:\
						CParNode * GetParamStruct(){return m_array2ParameterStruct;};
#define NO_PARAMETER public:CParNode * GetParamStruct(){return NULL;};

#define PARAMETER_INT(ParVar)			{#ParVar,0,				&ParVar,nullptr,nullptr,nullptr},		
#define PARAMETER_DOUBLE(ParVar)		{#ParVar,0,				nullptr,&ParVar,nullptr,nullptr},	
#define PARAMETER_TIMEDURATION(ParVar)  {#ParVar,0,				nullptr,nullptr,&ParVar,nullptr},
#define PARAMETER_STRING(ParVar)		{#ParVar,sizeof(ParVar),nullptr,nullptr,nullptr,ParVar },		





#define CANCEL(OrderRef,sysid,dataid) g_pStrategyContext->CancelOrder(g_StrategyId,(OrderRef),(sysid),(dataid))
#define REMAINCANCELAMOUNT(dataid) g_pStrategyContext->GetRemainCancelAmount(g_StrategyId,(dataid))
#define ORDER(id) g_pStrategyContext->GetOrder((id))

#define LIMITORDER(Direction,Offset,Volume,Price,DataId) g_pStrategyContext->MakeOrder(g_StrategyId,(LB1_NormalLimitOrderType),(Direction),(Offset),(Volume),(Price),(DataId),0)
#define LIMITORDER_Ex(Direction,Offset,Volume,Price,DataId,Custom) g_pStrategyContext->MakeOrder(g_StrategyId,(LB1_NormalLimitOrderType),(Direction),(Offset),(Volume),(Price),(DataId),(Custom))
#define MARKETORDER(Direction,Offset,Volume,Price,DataId) g_pStrategyContext->MakeOrder(g_StrategyId,(LB1_MarketOrderType),(Direction),(Offset),(Volume),(Price),(DataId),0)
#define FAKLIMITORDER(b,c,d,e,f) g_pStrategyContext->MakeOrder(g_StrategyId,(LB1_FAKLimitOrderType),(Direction),(Offset),(Volume),(Price),(DataId),0)
#define FOKLIMITORDER(b,c,d,e,f) g_pStrategyContext->MakeOrder(g_StrategyId,(LB1_FOKLimitOrderType),(Direction),(Offset),(Volume),(Price),(DataId),0)

#define SHAREDVALUE_GET(index,ret) g_pStrategyContext->GetSharedValue(index,ret)
#define SHAREDVALUE_INC(index,dt,discriminate) g_pStrategyContext->IncreaseSharedValue(index,dt,discriminate)
#define SHAREDVALUE_DEC(index,dt,discriminate) g_pStrategyContext->DecreaseSharedValue(index,dt,discriminate)
#define SHAREDVALUE_SET(index,nv,discriminate) g_pStrategyContext->SetSharedValue(index,nv,discriminate)

#define MEDDLE(outbuf,len) g_pStrategyContext->GetNextMeddle(g_StrategyId,(outbuf),(len))
#define INQUIRY(data) g_pStrategyContext->Inquery(g_StrategyId,data)
#ifdef WIN32
#define MEDDLERESPONSE(a,...) g_pStrategyContext->MeddleResponse(g_StrategyId,(a),__VA_ARGS__)
#define LOG(a,...) g_pStrategyContext->ShowMessage(g_StrategyId,(a),__VA_ARGS__)
#else
#define MEDDLERESPONSE(a,args...) g_pStrategyContext->MeddleResponse(g_StrategyId,(a),##args)
#define LOG(a,args...) g_pStrategyContext->ShowMessage(g_StrategyId,(a),##args)
#endif
#define INCREASE_COUNTER(CounterName) g_pStrategyContext->IncreaseCounter(g_StrategyId,CounterName)
#define DESCREASE_COUNTER(CounterName) g_pStrategyContext->DescreaseCounter(g_StrategyId,CounterName)
#define SerialonVice(name,value) g_pStrategyContext->SerialOnVice(g_StrategyId,(name),(value))
	

	enum TProbeColorType {
		TProbe_Color_Red,
		TProbe_Color_Green,
		TProbe_Color_Blue,
		TProbe_Color_Yellow,
		TProbe_Color_Purple,
		TProbe_Color_Jacinth,
		TProbe_Color_Peachblow,
		TProbe_Color_Royalblue,
		TProbe_Color_Powderblue,
		TProbe_Color_Red_Weak,
		TProbe_Color_Green_Weak,
		TProbe_Color_Blue_Weak,
		TProbe_Color_Yellow_Weak,
		TProbe_Color_Purple_Weak,
		TProbe_Color_Jacinth_Weak,
		TProbe_Color_Peachblow_Weak,
		TProbe_Color_Royalblue_Weak,
		TProbe_Color_Powderblue_Weak
	};
	enum TProbeType { DoubleProbe, IntProbe, DateTimeProbe };
	struct CProbeNode
	{
		atomic<double> * m_AtomicDoublePointer;
		atomic<unsigned int> * AtomicCounterOverallPointer;
		char m_strProbeName[50];
		TProbeColorType m_enumColor;
	};
#define PROBE_NULL_VALUE (-999999.0F)
#define MAX_GRAPH_COUNT 8
#define MAX_SERIAL_PER_GRAPH 10
#define NO_PROBE public:TProbeStructType GetProbeStruct(){return nullptr;};
#define BEGIN_PROBE_BIND CProbeNode m_array2ProbeStruct[MAX_GRAPH_COUNT+1][MAX_SERIAL_PER_GRAPH+1] = {
#define END_PROBE_BIND {{ nullptr,nullptr,"",TProbe_Color_Powderblue}}};\
						public:\
						TProbeStructType GetProbeStruct(){return m_array2ProbeStruct;};
#define BEGIN_GRAPH {
#define END_GRAPH(style) { nullptr,nullptr,style,TProbe_Color_Red}},
#define PROBE(Pointer,Name,Color) {Pointer,nullptr,Name,Color},
#define PROBE_EX(Pointer,Index,Name,Color) {Pointer,Index,Name,Color},


#define BEGIN_CANDLESTICKS_GRAPH(open,high,low,close,begintime) {\
 {high,nullptr,"_high_",TProbe_Color_Blue},\
{low,nullptr,"_low_",TProbe_Color_Green},\
{open,begintime,"_open_",TProbe_Color_Blue},\
{close,nullptr,"_close_",TProbe_Color_Red},


#define END_CANDLESTICKS_GRAPH(volume) {volume,nullptr,"_volume_",TProbe_Color_Red},{ nullptr,nullptr,"Candlesticks",TProbe_Color_Red}},
	
	typedef CProbeNode(*TProbeStructType)[MAX_SERIAL_PER_GRAPH + 1];


#define  STRATEGY_TEMPLATE_DECLARE(CLASSNAME) MStrategyContext * g_pStrategyContext;\
TStrategyIdType g_StrategyId;\
CLASSNAME(MStrategyContext* context, TStrategyIdType strategyid);\
void OnSave(const char *);\
void OnLoad(const char *);\
void OnRelease();\
virtual ~CLASSNAME();


#define  STRATEGY_TEMPLATE_DEFINITION(CLASSNAME) \
extern "C" {STRATEGY_INTERFACE  MStrategy * CreateStrategyObject(MStrategyContext*context, TStrategyIdType stid)\
{MStrategy * ret;\
try { ret = new CLASSNAME(context, stid); }\
catch (...) { return NULL; }\
return ret;}}\
CLASSNAME::CLASSNAME(MStrategyContext* context, TStrategyIdType strategyid)\
:g_pStrategyContext(context),g_StrategyId(strategyid)\
{if (context == NULL) throw 1;}\
void CLASSNAME::OnRelease(){delete this;}\
void CLASSNAME::OnLoad(const char * fname) { \
	if(nullptr == fname)  throw runtime_error("fname is nullptr."); \
	ifstream is(fname,std::ios::binary); \
	if (is.is_open()){boost::archive::binary_iarchive ia(is);ia >> *this;is.close();} else throw runtime_error("can not open file.");} \
void CLASSNAME::OnSave(const char * fname) { \
	if(nullptr == fname)  throw runtime_error("fname is nullptr."); \
	ofstream os(fname,std::ios::binary); \
	if (os.is_open()){boost::archive::binary_oarchive oa(os);oa << *this;os.close();} else throw runtime_error("can not open file.");} \
CLASSNAME::~CLASSNAME(){};




#define SERIALIZATION(var) ar & var;
#define BEGIN_SERIALIZATION	friend class boost::serialization::access;\
	template<class Archive> void serialize(Archive& ar, const unsigned int version){
#define END_SERIALIZATION };

#define BEGIN_SHOW(FunctionName) void FunctionName(){LOG("Strategy[%d]: -----begin show------", g_StrategyId);
#define SHOW_INT(name) LOG("Strategy[%d]: "#name"=%d", g_StrategyId, (int)name);
#define SHOW_UINT(name) LOG("Strategy[%d]: "#name"=%u", g_StrategyId, (unsigned int)name);
#define SHOW_DOUBLE(name) LOG("Strategy[%d]: "#name"=%lf", g_StrategyId, (double)name);
#define SHOW_STRING(name) LOG("Strategy[%d]: "#name"=%s", g_StrategyId, (const char*)name);
#define END_SHOW LOG("Strategy[%d]: -----end show------", g_StrategyId);}
};


