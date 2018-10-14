//#include "stdafx.h"
#include <fstream>
#include <string>
#include <iostream>
#include <memory>
#include <exception>
#include <sstream>
#include <algorithm>
#include <regex>
#include "public.h"
#include "OrderRefResolve.h"

#include <boost/date_time/posix_time/posix_time.hpp>

using namespace boost::posix_time;
using namespace boost::gregorian;
#include <boost/log/common.hpp>
#ifndef WIN32
#include <unistd.h>
#endif

#ifndef BOOST_SPIRIT_THREADSAFE
#define BOOST_SPIRIT_THREADSAFE
#endif
#include <boost/property_tree/ptree.hpp>  
#include <boost/property_tree/json_parser.hpp> 
using namespace boost::property_tree;



namespace logging = boost::log;
namespace attrs = boost::log::attributes;
namespace src = boost::log::sources;
namespace sinks = boost::log::sinks;
namespace expr = boost::log::expressions;
namespace keywords = boost::log::keywords;

using namespace boost::property_tree;
using namespace std;

#include "trade_service.h"
#include "OrderRefResolve.h"

string GetNodeData(string name, const ptree & root)
{
    auto Node = root.find(name);
    if (Node == root.not_found())
    {
        char buf[128];
        snprintf(buf, sizeof(buf), "can not find <%s>", name.c_str());
        throw std::runtime_error(buf);
    }
    else
        return Node->second.data();
}

extern char ProcessName[256];
class CPauseMdSourceRAII
{
    vector<PluginPtrType> * m_P;
public:
    CPauseMdSourceRAII(vector<PluginPtrType> * p) :m_P(p) {};
    void Pause() 
    {
        for(auto & ptr: *m_P)
            dynamic_cast<MAtmMarketDataPluginInterface*>(ptr.get())->Pause();
    }
    ~CPauseMdSourceRAII()
    {
        for (auto & ptr : *m_P)
            dynamic_cast<MAtmMarketDataPluginInterface*>(ptr.get())->Continue();
    };
};

class CDynamicLinkLibraryRAII
{
    StrategyHandleType m_hHandle;
public:
    StrategyHandleType GetHandle()
    { 
        return m_hHandle;
    };

    CDynamicLinkLibraryRAII(const char * name)
    {
        m_hHandle = LoadStrategyBin(name);
    }

    ~CDynamicLinkLibraryRAII()
    {
        if(m_hHandle)
            UnLoadStrategyBin(m_hHandle);
    }

};

CTradeService::CTradeService(std::string configFile, unsigned int sysnum):m_strConfigFile(configFile)
{
    m_uSystemNumber = sysnum;
    m_vecAllTradeSource.first.resize(_MaxAccountNumber + 1);
}

CTradeService::~CTradeService()
{
    if (m_pApi)
    {
        m_pApi->Release();
        m_pApi = nullptr;
    }
}

void CTradeService::Start()
{
    auto temp=MCommuModForServInterface::CreateApi(GetAddress().c_str(),GetListenPort(), this, GetNetHandlerThreadCount());
    m_pApi = temp;
    if (m_pApi)
        m_pApi->StartListen();
}

void CTradeService::Join()
{
#ifdef WIN32
    Sleep(INFINITE);
#else
    sleep(0);
#endif
}

unsigned short CTradeService::GetListenPort()
{
    ptree g_Config;
    boost::property_tree::read_json(m_strConfigFile, g_Config);
    

    if (g_Config.find("basic") != g_Config.not_found())
    {
        auto Basic = g_Config.find("basic");
        if (Basic->second.find("listenport") != Basic->second.not_found())
        {
            unsigned short _ListenPort = atoi(Basic->second.find("listenport")->second.data().c_str());
            if (0 == _ListenPort)
                throw std::runtime_error("[error]invalid 'basic.listenport' value.");
            else
                return _ListenPort;
        }
        else
            throw std::runtime_error("[error]could not find 'basic.listenport' node.");
    }
    else
        throw std::runtime_error("[error]could not find 'basic' node.");

}

size_t CTradeService::GetNetHandlerThreadCount()
{
    ptree g_Config;
    boost::property_tree::read_json(m_strConfigFile, g_Config);


    if (g_Config.find("basic") != g_Config.not_found())
    {
        auto Basic = g_Config.find("basic");
        if (Basic->second.find("nethandlerthreadcount") != Basic->second.not_found())
        {
            unsigned short _NetHandlerThreadCount = atoi(Basic->second.find("nethandlerthreadcount")->second.data().c_str());
            if (0 == _NetHandlerThreadCount)
                throw std::runtime_error("[error]invalid 'basic.nethandlerthreadcount' value.");
            else
                return _NetHandlerThreadCount;
        }
        else
            throw std::runtime_error("[error]could not find 'basic.nethandlerthreadcount' node.");
    }
    else
        throw std::runtime_error("[error]could not find 'basic' node.");

}


void CTradeService::DeployStrategy(const ptree & in,unsigned int & strategyid)
{
    //策略数组互斥:        写互斥
    //行情源数组互斥:        读互斥
    //交易源数组互斥:        读互斥
    string Bin, Archive, ParFile;
    unordered_map<TMarketDataIdType, pair<vector<PluginPtrType>::iterator, unordered_map<string, string> > > _MD_DataChannelConfig;
    unordered_map<TMarketDataIdType, pair<vector<PluginPtrType>::iterator, unordered_map<string, string> > > _TD_DataChannelConfig;
    unsigned int StrategyID = 0;
    StrategyHandleType _pBinHandle = nullptr;
    MStrategy * _pStrategy = nullptr;
    unordered_map<string, string> _paramMap;
    unsigned int _maxIncreaseOrderCountPerDay = 10;


    auto MiocpdNode= in.find("maxincreaseordercountperday");
    if (MiocpdNode != in.not_found())
        _maxIncreaseOrderCountPerDay = atoi(MiocpdNode->second.data().c_str());

    auto BinNode = in.find("bin");
    if (BinNode == in.not_found() || BinNode->second.data().size() == 0)
        throw std::runtime_error("Invalid <bin>");
    else
        Bin = string(".") + FILE_PATH_SEPARATOR + BinNode->second.data() + STRATEGY_SUFFIX;

    auto ArchiveNode = in.find("archive");
    if (ArchiveNode == in.not_found())
        Archive = "";
    else
    {
        if (false == ArchiveNode->second.data().empty())
            Archive = ArchiveNode->second.data();
    }

    auto ParamNode = in.find("param");
    if (ParamNode != in.not_found())
    {
        for (auto & par : ParamNode->second)
            _paramMap[par.first] = par.second.data();
    }

    auto DataIDNode = in.find("dataid");
    if (DataIDNode == in.not_found())
        throw std::runtime_error("Can not find <dataid>.");

    boost::shared_lock<boost::shared_mutex> rlock_MD(m_vecAllMarketDataSource.second, boost::defer_lock);
    boost::shared_lock<boost::shared_mutex> rlock_TD(m_vecAllTradeSource.second, boost::defer_lock);
    boost::unique_lock<boost::shared_mutex> wlock_ST(m_mtxAllStrategys, boost::defer_lock);
    std::lock(rlock_MD, rlock_TD, wlock_ST);

    for (auto & PerDataIdNode : DataIDNode->second)
    {
        TMarketDataIdType dataid = atoi(PerDataIdNode.first.c_str());
        vector<PluginPtrType>::iterator mdsresult;
        vector<PluginPtrType>::iterator tdsresult;
        unordered_map<string, string> instrumentid;


        auto MarketDataSourceNode = PerDataIdNode.second.find("marketdatasource");
        if (PerDataIdNode.second.not_found() != MarketDataSourceNode)
        {
            mdsresult = find_if(
                m_vecAllMarketDataSource.first.begin(),
                m_vecAllMarketDataSource.first.end(),
                [MarketDataSourceNode](PluginPtrType ptr) {
                if (ptr
                    &&
                    ptr->GetCurrentKeyword()
                    ==
                    MarketDataSourceNode->second.data())
                    return true;
                else return false;
            }
            );

            if (mdsresult == m_vecAllMarketDataSource.first.end())
            {
                string exp = "This marketdatasource(" + MarketDataSourceNode->second.data() + ") does not exist.";
                throw std::runtime_error(exp.c_str());
            }
        }
        else
            throw std::runtime_error("Can not find <marketdatasource>.");

        auto TradeSourceNode = PerDataIdNode.second.find("tradesource");
        if (PerDataIdNode.second.not_found() != TradeSourceNode)
        {
            tdsresult = find_if(
                m_vecAllTradeSource.first.begin(),
                m_vecAllTradeSource.first.end(),
                [TradeSourceNode](PluginPtrType ptr) {
                if (
                    ptr
                    &&
                    ptr->GetCurrentKeyword()
                    ==
                    TradeSourceNode->second.data())
                    return true;
                else return false;
            }
            );

            if (tdsresult == m_vecAllTradeSource.first.end())
                throw std::runtime_error("This tradesource does not exist.");
        }
        else
            throw std::runtime_error("Can not find <tradesource>.");

        auto SymbolDefineNode = PerDataIdNode.second.find("symboldefine");
        if (PerDataIdNode.second.not_found() != SymbolDefineNode)
        {
            for (auto & attr : SymbolDefineNode->second)
                instrumentid[attr.first] = attr.second.data();
        }
        else
            throw std::runtime_error("Can not find <symboldefine>.");

        _MD_DataChannelConfig[dataid] = make_pair(mdsresult, instrumentid);
        _TD_DataChannelConfig[dataid] = make_pair(tdsresult, instrumentid);
    }


    

    for (auto & cfg : _MD_DataChannelConfig)
        (*cfg.second.first)->CheckSymbolValidity(cfg.second.second);

    

    for (auto & cfg : _TD_DataChannelConfig)
        (*cfg.second.first)->CheckSymbolValidity(cfg.second.second);

    

    for (;(StrategyID <= _MaxStrategyID) 
        && (nullptr != m_arrayAllStrategys[StrategyID].m_pStrategy);
        StrategyID++);
        if (StrategyID > _MaxStrategyID)
            throw std::runtime_error("Too many strategys MaxStrategyID.");

    

    _pBinHandle = LoadStrategyBin(Bin.c_str());
    if (nullptr == _pBinHandle)
        throw std::runtime_error("loadstrategybin failed.");
    typedef  MStrategy * (*TFNCreateStrategyObject)(MStrategyContext*, TStrategyIdType);
    auto Creator = (TFNCreateStrategyObject)GetProcessAddressByName(_pBinHandle, "CreateStrategyObject");
    if (nullptr == Creator)
    {
        UnLoadStrategyBin(_pBinHandle);
        throw std::runtime_error("can not export creator.");
    }

    _pStrategy = (*Creator)(this, StrategyID);
    if (nullptr == _pStrategy)
    {
        UnLoadStrategyBin(_pBinHandle);
        throw std::runtime_error("can not create strategy.");
    }

    m_arrayAllStrategys[StrategyID].clear();
    m_arrayAllStrategys[StrategyID].m_pBinHandle = _pBinHandle;
    m_arrayAllStrategys[StrategyID].m_pStrategy = _pStrategy;
    m_arrayAllStrategys[StrategyID].m_uStrategyID = StrategyID;
    m_arrayAllStrategys[StrategyID].m_pathStrategyPath = boost::filesystem::path(Bin);
    m_arrayAllStrategys[StrategyID].m_treeConfig = in;
    m_arrayAllStrategys[StrategyID].m_uMaxIncreaseOrderCountPerDay = _maxIncreaseOrderCountPerDay;
    m_arrayAllStrategys[StrategyID].m_uRemainIncreaseOrderCountPerDay = _maxIncreaseOrderCountPerDay;
    m_arrayAllStrategys[StrategyID].m_dateActionDate = second_clock::universal_time().date();
    string error = "";
    if (!Archive.empty())
    {
        try {
            m_arrayAllStrategys[StrategyID].m_pStrategy->OnLoad(Archive.c_str());
        }
        catch (std::exception & err)
        {
            error = string("Strategy load archive exception.") + err.what();
        }
    }
    if (error.empty())
    {
        CParNode * ppar = m_arrayAllStrategys[StrategyID].m_pStrategy->GetParamStruct();
        if (ppar && (false == _paramMap.empty()))
        {
            for (auto & par : _paramMap)
            {
                bool Found = false;
                for (unsigned int index = 0;(index < MAXPARNAMELENGTH) && (strlen(ppar[index].m_arrayParname) != 0);index++)
                {
                    if (strcmp(ppar[index].m_arrayParname, par.first.c_str()) == 0)
                    {
                        Found = true;
                        if (nullptr != ppar[index].m_pIntAddress)
                            *ppar[index].m_pIntAddress = atoi(par.second.c_str());
                        else if (nullptr != ppar[index].m_pDoubleAddress)
                            *ppar[index].m_pDoubleAddress = atof(par.second.c_str());
                        else if (nullptr != ppar[index].m_pTimeDuraAddress)
                        {
                            time_duration temptd;
                            try {
                                temptd = duration_from_string(par.second);
                            }
                            catch (...)
                            {
                                break;
                            }
                            *ppar[index].m_pTimeDuraAddress = temptd;
                        }
                        else if (nullptr != ppar[index].m_pStringAddress)
                            snprintf(ppar[index].m_pStringAddress, ppar[index].m_intOption, "%s",par.second.c_str());
                        break;
                    }
                }
                if (false == Found)
                {
                    error = "can not find param {" + par.first + "}";
                    break;
                }
            }
        }
    }

    if (error.empty())
    {
        if (Archive.empty())
        {
            if (m_arrayAllStrategys[StrategyID].m_pStrategy->OnInit(microsec_clock::universal_time()) != TLastErrorIdType::LB1_NO_ERROR)
                error = "Strategy OnInit failed.";
        }
        else if (m_arrayAllStrategys[StrategyID].m_pStrategy->OnInit_FromArchive(microsec_clock::universal_time()) != TLastErrorIdType::LB1_NO_ERROR)
        {
            error = "Strategy OnInit failed.";
        }

    }

    if (error.empty() == false)
    {
        m_arrayAllStrategys[StrategyID].m_pStrategy->OnRelease();
        UnLoadStrategyBin(m_arrayAllStrategys[StrategyID].m_pBinHandle);
        m_arrayAllStrategys[StrategyID].clear();
        throw std::runtime_error(error.c_str());
    }

    auto & ProbeInfo = m_arrayAllStrategys[StrategyID].m_vecProbeInfo;
    TProbeStructType ProbeMatrix = m_arrayAllStrategys[StrategyID].m_pStrategy->GetProbeStruct();
    if (nullptr != ProbeMatrix)
    {
        for (auto y = 0;;y++)
        {
            if (nullptr == ProbeMatrix[y][0].m_AtomicDoublePointer)
                break;
            else
            {

                ProbeInfo.push_back(make_pair("", vector< std::tuple<string, TProbeColorType, atomic<double>*, atomic<unsigned >*> >()));
                for (auto x = 0;;x++)
                {
                    if (nullptr == ProbeMatrix[y][x].m_AtomicDoublePointer)
                    {
                        ProbeInfo[y].first = ProbeMatrix[y][x].m_strProbeName;
                        break;
                    }
                    else
                    {
                        ProbeInfo[y].second.push_back(
                            make_tuple(
                                ProbeMatrix[y][x].m_strProbeName,
                                ProbeMatrix[y][x].m_enumColor,
                                ProbeMatrix[y][x].m_AtomicDoublePointer,
                                ProbeMatrix[y][x].AtomicCounterOverallPointer
                                )
                            );
                    }
                }
            }
        }
    }

    for (auto & cfg : _TD_DataChannelConfig)
    {
        m_arrayAllStrategys[StrategyID].m_mapDataid2TradeApi[cfg.first]
            = make_pair(
                dynamic_cast<MAtmTradePluginInterface*>(cfg.second.first->get()),
                cfg.second.second);
        (*cfg.second.first)->IncreaseRefCount();
    }

    for (auto & cfg : _MD_DataChannelConfig)
    {
        auto Source = dynamic_cast<MAtmMarketDataPluginInterface*>(cfg.second.first->get());
        m_arrayAllStrategys[StrategyID].m_mapDataid2MarketDataApi[cfg.first] = Source;
        Source->MDAttachStrategy(
            _pStrategy,
            cfg.first,
            cfg.second.second,
            m_arrayAllStrategys[StrategyID].m_mtxPropectStrategy,
            &m_arrayAllStrategys[StrategyID].m_auProbeUpdateDatetime);
        (*cfg.second.first)->IncreaseRefCount();
    }
    
    strategyid = StrategyID;
}

void CTradeService::CancelStrategy(unsigned int StrategyID, string & sarchive, ptree & config)
{
    boost::shared_lock<boost::shared_mutex> rlock_MD(m_vecAllMarketDataSource.second, boost::defer_lock);
    boost::shared_lock<boost::shared_mutex> rlock_TD(m_vecAllTradeSource.second, boost::defer_lock);
    boost::shared_lock<boost::shared_mutex> rlock_ST(m_mtxAllStrategys, boost::defer_lock);
    std::lock(rlock_MD, rlock_TD, rlock_ST);
    auto & Strategy = m_arrayAllStrategys[StrategyID];
    if (nullptr == Strategy.m_pStrategy)
        throw std::runtime_error("this strategyid does not exists.");
    config = Strategy.m_treeConfig;
    for (auto & dataid : Strategy.m_mapDataid2MarketDataApi)
    {
        dataid.second->MDDetachStrategy(Strategy.m_pStrategy);
        dynamic_cast<MAtmPluginInterface*>(dataid.second)->DescreaseRefCount();
    }
    for (auto & dataid : Strategy.m_mapDataid2TradeApi)
        dynamic_cast<MAtmPluginInterface*>(dataid.second.first)->DescreaseRefCount();
    
    auto SArchiveFilePath = Strategy.m_pathStrategyPath;
    string timePart = to_iso_string(microsec_clock::local_time());

    string SArchiveFileName = SArchiveFilePath.filename().string() + "." + timePart + ARCHIVE_FILE_SUFFIX;
    
    try {
        Strategy.m_pStrategy->OnSave(SArchiveFileName.c_str());
    }
    catch (std::exception & err)
    {
        BOOST_LOG_SEV(m_Logger, severity_levels::normal) << ProcessName << ": "<<err.what();
        SArchiveFileName = "";
    }
        
    Strategy.m_pStrategy->OnEndup();
    Strategy.m_pStrategy->OnRelease();
    if (Strategy.m_pBinHandle)
        UnLoadStrategyBin(Strategy.m_pBinHandle);
    Strategy.clear();
    sarchive = SArchiveFileName;
}


void CTradeService::OnCommunicate(const ptree & in, ptree & out)
{
    if (in.find("type") != in.not_found())
    {
        auto Type = in.find("type")->second.data();
        auto PackageHandler = m_mapString2PackageHandlerType.find(Type);
        if(PackageHandler == m_mapString2PackageHandlerType.end())
            MakeError(out, "invalid <type>");
        else
        {
            try {
                PackageHandler->second(in, out);    
            }
            catch (std::exception & err)
            {
                out.clear();
                MakeError(out, err.what());
            }
        }
    }
    else
        MakeError(out, "Can not find <type>.");
    
}

void CTradeService::MakeError(ptree & out, const char * fmt, ...)
{
    out.put("type", "error");
    char buf[STRATEGY_MESSAGE_MAXLEN];
    va_list arg;
    va_start(arg, fmt);
    auto n=vsnprintf(buf, STRATEGY_MESSAGE_MAXLEN, fmt, arg);
    va_end(arg);
    if (n > -1 && n < STRATEGY_MESSAGE_MAXLEN)
        out.put("errormsg", buf);
    else
        out.put("errormsg", "!<buffer is too short to put this message>");
}

string CTradeService::GetAddress()
{
    ptree g_Config;
    boost::property_tree::read_json(m_strConfigFile, g_Config);


    if (g_Config.find("basic") != g_Config.not_found())
    {
        auto Basic = g_Config.find("basic");
        if (Basic->second.find("address") != Basic->second.not_found())
        {
            string _Address = Basic->second.find("address")->second.data().c_str();
            if (_Address.empty())
                throw std::runtime_error("[error]invalid 'basic.address' value.");
            else
                return _Address;
        }
        else
            throw std::runtime_error("[error]could not find 'basic.address' node.");
    }
    else
        throw std::runtime_error("[error]could not find 'basic' node.");

}

void CTradeService::ReqGetSupportedTypes(PackageHandlerParamType param, const ptree & in, ptree & out)
{
    //只访问常量数据结构，不需要互斥
    const unordered_map<string, pair<TPluginFactory, string> > * target=nullptr;
    if (PackageHandlerParamType::MarketData == param)
    {
        out.put("type", "rspgetsupportedmdtypes");
        target = &m_mapAMarketDataPFactories;
    }
    else
    {
        out.put("type", "rspgetsupportedtdtypes");
        target = &m_mapATradePFactories;
    }

    for (auto & sup : *target)
        out.put(sup.first, sup.second.second);
}

void CTradeService::ReqGetAllSource(PackageHandlerParamType param, const ptree & in, ptree & out)
{
    //策略数组互斥:        不需要
    //行情源数组互斥:        需要(只读)
    //交易源数组互斥:        需要(只读)
    pair<vector<PluginPtrType>, boost::shared_mutex> * target=nullptr;
    if (PackageHandlerParamType::MarketData == param)
        target = &m_vecAllMarketDataSource;
    else
        target = &m_vecAllTradeSource;
    boost::shared_lock<boost::shared_mutex> lock(target->second);
    for (auto mds : target->first)
    {
        if (mds)
        {
            ptree StateTree;
            mds->GetState(StateTree);
            out.add_child(mds->GetCurrentKeyword(), StateTree);
        }
    }
}

void CTradeService::ReqAddSource(PackageHandlerParamType param, const ptree & in, ptree &out)
{
    //策略数组互斥:        不需要
    //行情源数组互斥:        需要(写)
    //交易源数组互斥:        需要(写)
    const unordered_map<string, pair<TPluginFactory, string> > * tarFactoryMap = nullptr;
    pair<vector<PluginPtrType>, boost::shared_mutex> * tarContainer = nullptr;
    if (PackageHandlerParamType::MarketData == param)
    {
        tarFactoryMap = &m_mapAMarketDataPFactories;
        tarContainer = &m_vecAllMarketDataSource;
    }
    else
    {
        tarFactoryMap = &m_mapATradePFactories;
        tarContainer = &m_vecAllTradeSource;
    }

    if (in.find("sourcetype") != in.not_found())
    {
        auto SourceType = in.find("sourcetype")->second.data();
        auto SourceTypeItr = tarFactoryMap->find(SourceType);
        if (SourceTypeItr != tarFactoryMap->end())
        {
            auto ObjectPlugin = SourceTypeItr->second.first();
            boost::unique_lock<boost::shared_mutex> lock(tarContainer->second);
            auto FindResult= find_if(
                tarContainer->first.begin(),
                tarContainer->first.end(),
                [ObjectPlugin, in](PluginPtrType CurrentPlugin) {
                return CurrentPlugin
                    &&
                    (CurrentPlugin->GetCurrentKeyword() == ObjectPlugin->GetProspectiveKeyword(in));}
            );
            if (FindResult!=tarContainer->first.end())
            {
                if (PackageHandlerParamType::MarketData == param)
                {
                    MAtmMarketDataPluginInterface * mdObjectPlugin
                        = dynamic_cast<MAtmMarketDataPluginInterface*>(FindResult->get());
                    if (mdObjectPlugin->IsPedding())
                    {
                        out.put("type", "rspaddmarketdatasource");
                        out.put("result", "market data source is pedding.");
                    }
                    else
                    {
                        mdObjectPlugin->MDHotUpdate(in);
                        out.put("type", "rspaddmarketdatasource");
                        out.put("result", "market data source hotupdate succeed.");
                    }
                }
                else
                {
                    MAtmTradePluginInterface * tdObjectPlugin
                        = dynamic_cast<MAtmTradePluginInterface*>(FindResult->get());
                    //已经获取了交易源列表的锁
                    boost::shared_lock<boost::shared_mutex> rlock_MD(m_vecAllMarketDataSource.second, boost::defer_lock);
                    boost::unique_lock<boost::shared_mutex> wlock_ST(m_mtxAllStrategys, boost::defer_lock);
                    std::lock(rlock_MD, wlock_ST);
                    CPauseMdSourceRAII _PlugsPauseRaii(&m_vecAllMarketDataSource.first);
                    _PlugsPauseRaii.Pause();
                    if (tdObjectPlugin->IsPedding())
                    {
                        out.put("type", "rspaddtradesource");
                        out.put("result", "trade source is pedding.");
                    }
                    else
                    {
                        tdObjectPlugin->TDHotUpdate(in);
                        out.put("type", "rspaddtradesource");
                        out.put("result", "trade source hotupdate succeed.");
                    }
                }
            }
            else
            {
                if (PackageHandlerParamType::MarketData == param)
                {
                    MAtmMarketDataPluginInterface * mdObjectPlugin
                        = dynamic_cast<MAtmMarketDataPluginInterface*>(ObjectPlugin.get());
                    mdObjectPlugin->MDInit(in);
                    tarContainer->first.push_back(ObjectPlugin);
                    vector<PluginPtrType>(tarContainer->first).swap(tarContainer->first);
                    out.put("type", "rspaddmarketdatasource");
                    out.put("result", "market data source init succeed.");
                }
                else
                {
                    unsigned int NewAccountNumber = 0;
                    for (;NewAccountNumber < tarContainer->first.size();NewAccountNumber++)
                    {
                        if (tarContainer->first[NewAccountNumber] == nullptr)
                            break;
                    }
                    if (NewAccountNumber >= tarContainer->first.size())
                        throw std::runtime_error("too much account exists in this process,maxmun 32");
                    MAtmTradePluginInterface * tdObjectPlugin
                        = dynamic_cast<MAtmTradePluginInterface*>(ObjectPlugin.get());
                    tdObjectPlugin->TDInit(in, this, NewAccountNumber);//
                    tarContainer->first[NewAccountNumber] = ObjectPlugin;
                    out.put("type", "rspaddtradesource");
                    out.put("result", "trade source init succeed.");

                }

            }

        }
        else
        {
            string exp = "the sourcetype " + SourceType + " does not support";
            throw std::runtime_error(exp.c_str());
        }
    }
    else
        throw std::runtime_error("can not find <sourcetype>");
}

void CTradeService::ReqDelSource(PackageHandlerParamType param, const ptree & in, ptree & out)
{
    //策略数组互斥:        不需要
    //行情源数组互斥:        需要(写)
    //交易源数组互斥:        需要(写)
    pair<vector<PluginPtrType>, boost::shared_mutex> * tarContainer = nullptr;
    if (PackageHandlerParamType::MarketData == param)
        tarContainer = &m_vecAllMarketDataSource;
    else
        tarContainer = &m_vecAllTradeSource;
    auto Keyword = in.find("keyword");
    if (Keyword != in.not_found())
    {
        auto strKeyword = Keyword->second.data();
        boost::unique_lock<boost::shared_mutex> lock(tarContainer->second);
        auto findres = find_if(
            tarContainer->first.begin(),
            tarContainer->first.end(),
            [strKeyword](PluginPtrType ptr) {
            if (ptr&&ptr->GetCurrentKeyword() == strKeyword)
                return true;
            else return false;}
        );
        if (findres == tarContainer->first.end())
        {
            string exp = "the keyword " + strKeyword + " does not exists.";
            out.put("type", "rspdelmarketdatasource");
            out.put("result", exp);
        }
        else
        {
            if((*findres)->IsPedding())
                throw std::runtime_error("this source is pedding");
            if ((*findres)->GetRefCount()>0)
                throw std::runtime_error("some strategy is dependent on it");
            if (PackageHandlerParamType::MarketData == param)
            {
                MAtmMarketDataPluginInterface * tdObjectPlugin
                    = dynamic_cast<MAtmMarketDataPluginInterface*>((*findres).get());
                tdObjectPlugin->MDUnload();
                tarContainer->first.erase(findres);
                out.put("type", "rspdelmarketdatasource");
                
            }
            else
            {
                MAtmTradePluginInterface * tdObjectPlugin
                    = dynamic_cast<MAtmTradePluginInterface*>((*findres).get());
                tdObjectPlugin->TDUnload();
                findres->reset();
                out.put("type", "rspdeltradesource");
            }
            out.put("result", "succeed.");
        }

    }
    else
        throw std::runtime_error("can not find <keyword>");
}

void CTradeService::ReqAllStrategyBin(PackageHandlerParamType param, const ptree & in, ptree & out)
{
    //策略数组互斥:        不需要
    //行情源数组互斥:        不需要
    //交易源数组互斥:        不需要
    namespace fs = boost::filesystem;
    fs::path fullpath(".");
    fs::directory_iterator item_begin(fullpath);
    fs::directory_iterator item_end;
    unsigned int count = 0;
    for (;item_begin != item_end; item_begin++)
    {
        if (false == fs::is_directory(*item_begin)&& item_begin->path().extension().string()== STRATEGY_SUFFIX)
        {
            string BinName = item_begin->path().string();
            StrategyHandleType handle = LoadStrategyBin(BinName.c_str());
            if (nullptr == handle)
            {
#ifdef WIN32
#else
                BOOST_LOG_SEV(m_Logger, severity_levels::normal) << ProcessName  << ": " << dlerror() << " [" << to_iso_string(microsec_clock::universal_time()) << "]";
#endif
                continue;
            }
            else
            {
                if (nullptr != GetProcessAddressByName(handle, "CreateStrategyObject"))
                {
                    char buf[64];
                    sprintf(buf, "%u", count);
                    string filename = item_begin->path().filename().string();
                    string bin = filename.substr(0, filename.size() - strlen(STRATEGY_SUFFIX));
                    out.put(buf, bin);
                    count++;
                }
                UnLoadStrategyBin(handle);
            }
        }

    }
    
}

void CTradeService::ReqAllArchiveFile(PackageHandlerParamType param, const ptree & in, ptree & out)
{
    //策略数组互斥:        不需要
    //行情源数组互斥:        不需要
    //交易源数组互斥:        不需要
    auto StrategyNameNode = in.find("strategyname");
    if(in.not_found()==StrategyNameNode)
        throw std::runtime_error("can not find <strategyname>");
    string strategyName = StrategyNameNode->second.data();
    namespace fs = boost::filesystem;
    fs::path fullpath(".");
    fs::directory_iterator item_begin(fullpath);
    fs::directory_iterator item_end;
    unsigned int count = 0;
    for (;item_begin != item_end; item_begin++)
    {
        if (false == fs::is_directory(*item_begin) && item_begin->path().extension().string() == ARCHIVE_FILE_SUFFIX)
        {
            string filename = item_begin->path().filename().generic_string();
            if (filename.substr(0, strategyName.size()) == strategyName)
            {
                char buf[64];
                sprintf(buf, "%u", count);
                out.put(buf, filename);
                count++;
            }
        }

    }

}


void CTradeService::ReqDeployNewStrategy(PackageHandlerParamType param, const ptree & in, ptree & out)
{
    unsigned int StrategyID;
    DeployStrategy(in, StrategyID);

    out.put("result", "Deploy Strategy succeed.");
}

void CTradeService::ReqGetAllRunningStrategies(PackageHandlerParamType param, const ptree & in, ptree & out)
{
    //策略数组互斥:        读互斥
    //行情源数组互斥:        不需要
    //交易源数组互斥:        不需要
    boost::shared_lock<boost::shared_mutex> rlock_ST(m_mtxAllStrategys);
    for (unsigned int i = 0;i <= (_MaxStrategyID);i++)
    {
        auto & str = m_arrayAllStrategys[i];
        if (nullptr != str.m_pStrategy)
        {
            stringstream ss;
            ss << i;
            string StrategyID;
            ss >> StrategyID;
            ptree Context;
            auto CommentNode = str.m_treeConfig.find("comment");
            auto BinNode = str.m_treeConfig.find("bin");
            if (CommentNode != str.m_treeConfig.not_found())
                Context.put("comment", CommentNode->second.data());
            if (BinNode != str.m_treeConfig.not_found())
                Context.put("bin", BinNode->second.data());
            Context.put("maxtickets", m_arrayAllStrategys[i].m_uMaxIncreaseOrderCountPerDay);
            Context.put("remaintickets", m_arrayAllStrategys[i].m_uRemainIncreaseOrderCountPerDay);
            int position_info;
            char custom_info[512] = { "-" };
            {
                boost::unique_lock<boost::shared_mutex> wlock_ST(str.m_mtxPropectStrategy, boost::try_to_lock);
                if (wlock_ST.owns_lock())
                {
                    if(str.m_pStrategy->OnGetPositionInfo(&position_info))
                        Context.put("position", position_info);
                    
                    if (str.m_pStrategy->OnGetCustomInfo(custom_info, sizeof(custom_info)))
                    {
                        custom_info[sizeof(custom_info) - 1] = '\0';
                        Context.put("custom", custom_info);
                    }
                }
            }
            out.put_child(StrategyID, Context);
            
        }
    }
}

void CTradeService::ReqCancelRunningStrategies(PackageHandlerParamType param, const ptree & in, ptree & out)
{
    //策略数组互斥:        写互斥
    //行情源数组互斥:        读互斥
    //交易源数组互斥:        读互斥
    auto StrategyNode = in.find("strategyid");
    if (in.not_found() == StrategyNode)
        throw std::runtime_error("can not find <strategyid>");
    unsigned int StrategyID = atoi(StrategyNode->second.data().c_str());
    if(StrategyID>_MaxStrategyID)
        throw std::runtime_error("invalid <strategyid>");
    string SArchiveFileName;
    ptree Config;
    string Result = "";
    
    CancelStrategy(StrategyID, SArchiveFileName, Config);
    if (Config.find("param") != Config.not_found())
        Config.erase("param");
    auto archiveNode = Config.find("archive");
    if (archiveNode != Config.not_found())
        archiveNode->second.data() = SArchiveFileName;
    else
        Config.put("archive", SArchiveFileName);
    string redeploy = to_iso_string(microsec_clock::local_time()) + ".json";
    std::shared_ptr<ofstream> save(
        new ofstream(redeploy),
        [](ofstream * file) {
        if (file)
        {
            if (file->is_open())
                file->close();
            delete file;
        }
    });
    if (save->is_open())
    {
        try {
            write_json(*save.get(), Config);
            Result = string("cancel strategy succeed,the redeploy json file is ") + redeploy;
        }
        catch (std::exception & err)
        {
            Result = string("cancel strategy succeed,But could not write_json redeploy json file. ") + redeploy;
        }
    }
    else
        Result="cancel strategy succeed.But could not open redeploy json file.";
    
    out.put("result", Result);
    out.put_child("redeploy", Config);
}



void CTradeService::ReqGetProbe(PackageHandlerParamType param, const ptree & in, ptree & out)
{
    //策略数组互斥:        读互斥
    //行情源数组互斥:        不需要
    //交易源数组互斥:        不需要
    auto StrategyIDnode = in.find("strategyid");
    if (in.not_found() == StrategyIDnode)
        throw std::runtime_error("Can not find <strategyid>");
    unsigned int StrategyID = atoi(StrategyIDnode->second.data().c_str());
    boost::shared_lock<boost::shared_mutex> rlock3(m_mtxAllStrategys);
    if(nullptr== m_arrayAllStrategys[StrategyID].m_pStrategy)
        throw std::runtime_error("This strategy does not exists");
    auto & ProbeInfo=m_arrayAllStrategys[StrategyID].m_vecProbeInfo;
    uint_least64_t datetime = m_arrayAllStrategys[StrategyID].m_auProbeUpdateDatetime.load();
    out.put("rawdatetime", datetime);
    if (ProbeInfo.empty())
        return;
    unsigned int i = 0;
    ptree allGraph;
    for (auto & subset : ProbeInfo)
    {
        ptree subsetTree;
        stringstream ss1;
        string id;
        ss1 << i;
        ss1 >> id;

        ptree serialTree;
        for (auto & serial : subset.second)
        {
            stringstream ss2, ss3;
            string _strcolor, _strvalue;
            ptree OneSerial;
            OneSerial.put("name", get<0>(serial));

            ss2 << static_cast<unsigned int>(get<1>(serial));
            ss2 >> _strcolor;
            OneSerial.put("color", _strcolor);

            double val = (*get<2>(serial)).load();
            if(abs(val-PROBE_NULL_VALUE)<10e-6)
                ss3 <<"NULL" ;
            else
                ss3 << val;
            ss3 >> _strvalue;
            OneSerial.put("value", _strvalue);

            if (get<3>(serial) != nullptr)
                OneSerial.put("count_overall", (*get<3>(serial)).load());

            serialTree.put_child(get<0>(serial), OneSerial);
        }
        subsetTree.put("style", subset.first);
        subsetTree.put_child("serials", serialTree);
        allGraph.put_child(id, subsetTree);
        i++;
    }
    out.put_child("graph", allGraph);

}


void CTradeService::ReqMeddle(PackageHandlerParamType param, const ptree & in, ptree & out)
{
    //策略数组互斥:        读互斥
    //行情源数组互斥:        不需要
    //交易源数组互斥:        不需要
    auto StrategyIDnode = in.find("strategyid");
    if (in.not_found() == StrategyIDnode)
        throw std::runtime_error("Can not find <strategyid>");
    auto Commandnode = in.find("command");
    if (in.not_found() == Commandnode)
        throw std::runtime_error("Can not find <command>");

    unsigned int StrategyID = atoi(StrategyIDnode->second.data().c_str());
    boost::shared_lock<boost::shared_mutex> rlock_ST(m_mtxAllStrategys);
    auto & strategy = m_arrayAllStrategys[StrategyID];
    if (nullptr == strategy.m_pStrategy)
        throw std::runtime_error("This strategy does not exists");
    else
    {
        boost::unique_lock<boost::shared_mutex> wlock(strategy.m_mtxPropectMeddleQueue);
        string command = Commandnode->second.data();
        strategy.m_queueMeddleQueue.push(command);
        out.put("result", "send meddle string \'"+ command +"\' succeed.");
    }
}

void CTradeService::ReqGetMeddleResponse(PackageHandlerParamType param, const ptree & in, ptree & out)
{
    //策略数组互斥:        读互斥
    //行情源数组互斥:        不需要
    //交易源数组互斥:        不需要
    auto StrategyIDnode = in.find("strategyid");
    if (in.not_found() == StrategyIDnode)
        throw std::runtime_error("Can not find <strategyid>");
    unsigned int StrategyID = atoi(StrategyIDnode->second.data().c_str());
    boost::shared_lock<boost::shared_mutex> rlock(m_mtxAllStrategys);
    auto & strategy = m_arrayAllStrategys[StrategyID];
    if (nullptr == strategy.m_pStrategy)
        throw std::runtime_error("This strategy does not exists");
    else
    {
        boost::unique_lock<boost::shared_mutex> wlock(strategy.m_mtxPropectMeddleResponseQueue);
        int count = 15;
        while (count >= 0 && (!strategy.m_queueMeddleResponseQueue.empty()))
        {
            char buf[16];
            sprintf(buf, "%d", count);
            out.put(
                buf,
                to_iso_string(strategy.m_queueMeddleResponseQueue.front().first)+":"+strategy.m_queueMeddleResponseQueue.front().second);
            strategy.m_queueMeddleResponseQueue.pop();
            count--;
        }
    }
}

void CTradeService::ReqStrategyParams(PackageHandlerParamType, const ptree & in, ptree & out)
{
    //策略数组互斥:        不需要
    //行情源数组互斥:        不需要
    //交易源数组互斥:        不需要
    auto StrategyBinNode = in.find("strategybin");
    if (in.not_found() == StrategyBinNode)
        throw std::runtime_error("Can not find <strategybin>");
    else
    {
        string strategypath = string(".") + FILE_PATH_SEPARATOR + StrategyBinNode->second.data()+ STRATEGY_SUFFIX;
        CDynamicLinkLibraryRAII DynamicLinkLib(strategypath.c_str());
        if (nullptr == DynamicLinkLib.GetHandle())
            throw std::runtime_error("Can not open this strategy bin.");
        else
        {
            auto Creator = (TFNCreateStrategyObject)GetProcessAddressByName(DynamicLinkLib.GetHandle(), "CreateStrategyObject");
            if (nullptr!= Creator)
            {
                boost::shared_ptr<MStrategy> pStrategy(
                    Creator(this, _MaxStrategyID + 1),
                    [](MStrategy*ptr) {
                    if (ptr) 
                        ptr->OnRelease();
                });
                if (nullptr != pStrategy)
                {
                    auto ArchiveFileNode = in.find("archivefile");
                    if (ArchiveFileNode != in.not_found())
                    {
                        try {
                            pStrategy->OnLoad(ArchiveFileNode->second.data().c_str());
                        }
                        catch (std::exception & err)
                        {
                            throw std::runtime_error((string("Can not create strategy.") + err.what()).c_str());
                        }
                    }


                    auto ppar=pStrategy->GetParamStruct();
                    if (nullptr != ppar)
                    {
                        for (unsigned int iCount = 0;(iCount < MAXPARNAMELENGTH) && (strlen(ppar[iCount].m_arrayParname) != 0);iCount++)
                        {
                            char buf[1024];
                            if (nullptr != ppar[iCount].m_pIntAddress)
                                snprintf(buf, sizeof(buf), "%d", *ppar[iCount].m_pIntAddress);
                            else if (nullptr != ppar[iCount].m_pDoubleAddress)
                                snprintf(buf, sizeof(buf), "%lf", *ppar[iCount].m_pDoubleAddress);
                            else if (nullptr != ppar[iCount].m_pTimeDuraAddress)
                                snprintf(buf, sizeof(buf), "%s", to_simple_string(*ppar[iCount].m_pTimeDuraAddress).c_str());
                            else if (nullptr != ppar[iCount].m_pStringAddress)
                                snprintf(buf, sizeof(buf), "%s", ppar[iCount].m_pStringAddress);
                            out.put(ppar[iCount].m_arrayParname, buf);
                        }
                    }
                }
                else
                    throw std::runtime_error("Can not create strategy.");
            }
            else
                throw std::runtime_error("Can not find object creator in this bin.");
        }
    }
}

void CTradeService::ReqStrategyConfigJson(PackageHandlerParamType, const ptree & in, ptree & out)
{
    auto StrategyIdNode = in.find("strategyid");
    if (in.not_found() == StrategyIdNode)
        throw std::runtime_error("Can not find <strategyid>");
    else
    {
        boost::shared_lock<boost::shared_mutex> rlock_ST(m_mtxAllStrategys);
        unsigned int Strategyid = atoi(StrategyIdNode->second.data().c_str());
        if(Strategyid>_MaxStrategyID
            ||
            Strategyid<0
            ||
            nullptr==m_arrayAllStrategys[Strategyid].m_pStrategy)
            throw std::runtime_error("Invalid strategyid");
        out.put_child("result", m_arrayAllStrategys[Strategyid].m_treeConfig);
    }
}

void CTradeService::ReqUpdateStrategyBin(PackageHandlerParamType, const ptree & in, ptree & out)
{
    auto StrategyIdNode = in.find("strategyid");
    auto NewStrategyBin = in.find("newbin");
    if (StrategyIdNode != in.not_found() && NewStrategyBin != in.not_found())
    {
        unsigned int Strategyid = atoi(StrategyIdNode->second.data().c_str());
        if(Strategyid>_MaxStrategyID)
            throw std::runtime_error("valid strategyid");

        ptree config;
        string archiveFilename;
        string OldStrategyBin;
        CancelStrategy(Strategyid, archiveFilename, config);
        if(archiveFilename.empty())
            throw std::runtime_error("save archive failed,the strategy is canceled!!");
        if (config.find("param") != config.not_found())
            config.erase("param");

        auto binNode = config.find("bin");
        if (binNode != config.not_found())
        {
            OldStrategyBin = binNode->second.data();
            binNode->second.data() = NewStrategyBin->second.data();
        }
        else
            config.put("bin", NewStrategyBin->second.data());

        auto archiveNode = config.find("archive");
        if (archiveNode != config.not_found())
            archiveNode->second.data() = archiveFilename;
        else
            config.put("archive", archiveFilename);

        unsigned int NewStrategyID = 0;
        string Result = "succeed";
        try {
            DeployStrategy(config, NewStrategyID);
        }
        catch (std::exception & err)
        {
            Result = string("failed ")+err.what();
            config.find("bin")->second.data() = OldStrategyBin;
            try {
                DeployStrategy(config, NewStrategyID);
            }
            catch (std::exception & e)
            {
                Result = string("error ") + err.what();
            }
        }
        out.put("result", Result);
    }
    else
        throw std::runtime_error("Can not find <strategyid>");
}

void CTradeService::ReqModifySharedValue(PackageHandlerParamType, const ptree & in, ptree & out)
{
    //写锁
    unsigned int ValueId = 0;
    double NewValue = 0.0;
    auto ValueIdNode = in.find("valueid");
    if (ValueIdNode != in.not_found())
        ValueId = atoi(ValueIdNode->second.data().c_str());
    else
        throw std::runtime_error("can not find <valueid>");
    auto NewValueNode = in.find("newvalue");
    if (NewValueNode != in.not_found())
        NewValue = atof(NewValueNode->second.data().c_str());
    else
        throw std::runtime_error("can not find <newvalue>");

    SetSharedValue(ValueId, NewValue, [](double) {return true;});
    out.put("type", "rspmodifysharedvalue");
    out.put("newvalue", NewValue);
}

void CTradeService::ReqAllSharedValue(PackageHandlerParamType, const ptree & in, ptree & out)
{
    //读锁
    boost::shared_lock<boost::shared_mutex> rlock(m_mtxSharedValue,boost::try_to_lock);
    if (rlock.owns_lock())
    {
        out.put("type", "rspallsharedvalue");
        ptree values;
        char buf[64];
        for (auto & p : m_mapSharedValue)
        {
            sprintf(buf, "%u", p.first);
            values.put(buf, p.second);
        }
        out.add_child("values", values);
    }
}

void CTradeService::ReqSetOrderTickets(PackageHandlerParamType, const ptree & in, ptree & out)
{
    auto StrategyID = atoi(GetNodeData("strategyid", in).c_str());
    auto MaxTicket = atoi(GetNodeData("maxticket", in).c_str());
    auto RemainTicket = atoi(GetNodeData("remainticket", in).c_str());
    boost::shared_lock<boost::shared_mutex> rlock1(m_mtxAllStrategys);
    if(StrategyID>_MaxStrategyID|| StrategyID<0)
        throw std::runtime_error("strategyid error.");
    if(nullptr == m_arrayAllStrategys[StrategyID].m_pStrategy)
        throw std::runtime_error("strategyid does not exist.");
    if(RemainTicket>MaxTicket)
        throw std::runtime_error("currentticket need be equal or smaller than maxticket.");
    m_arrayAllStrategys[StrategyID].m_uMaxIncreaseOrderCountPerDay = MaxTicket;
    m_arrayAllStrategys[StrategyID].m_uRemainIncreaseOrderCountPerDay = RemainTicket;

}

void CTradeService::ReqGetPositionInfo(PackageHandlerParamType, const ptree & in, ptree & out)
{
    auto StrategyID = atoi(GetNodeData("strategyid", in).c_str());
    boost::shared_lock<boost::shared_mutex> rlock1(m_mtxAllStrategys, boost::try_to_lock);
    if (rlock1.owns_lock())
    {
        if (StrategyID > _MaxStrategyID || StrategyID < 0)
            throw std::runtime_error("strategyid error.");
        if (nullptr == m_arrayAllStrategys[StrategyID].m_pStrategy)
            throw std::runtime_error("strategyid does not exist.");
        boost::unique_lock<boost::shared_mutex> wlock_strategy(m_arrayAllStrategys[StrategyID].m_mtxPropectStrategy, boost::try_to_lock);
        if (wlock_strategy.owns_lock())
        {
            int out_position;
            if (m_arrayAllStrategys[StrategyID].m_pStrategy->OnGetPositionInfo(&out_position))
            {
                
                out.put("result", "valid");
                out.put("positioninfo", out_position);
            }
            else
                out.put("result", "undefined");
        }
        else
            out.put("result", "unknown");
    }
    else
        out.put("result", "unknown");
}

void CTradeService::ReqGetCustomInfo(PackageHandlerParamType, const ptree & in, ptree & out)
{
    auto StrategyID = atoi(GetNodeData("strategyid", in).c_str());
    boost::shared_lock<boost::shared_mutex> rlock1(m_mtxAllStrategys,boost::try_to_lock);
    if (rlock1.owns_lock())
    {
        if (StrategyID > _MaxStrategyID || StrategyID < 0)
            throw std::runtime_error("strategyid error.");
        if (nullptr == m_arrayAllStrategys[StrategyID].m_pStrategy)
            throw std::runtime_error("strategyid does not exist.");
        boost::unique_lock<boost::shared_mutex> wlock_strategy(m_arrayAllStrategys[StrategyID].m_mtxPropectStrategy, boost::try_to_lock);
        if (wlock_strategy.owns_lock())
        {
            char CustomBuf[1024] = {"-"};
            if (m_arrayAllStrategys[StrategyID].m_pStrategy->OnGetCustomInfo(CustomBuf, sizeof(CustomBuf)))
            {
                CustomBuf[sizeof(CustomBuf) - 1] = '\0';
                out.put("result", "valid");
                out.put("custominfo", CustomBuf);
            }
            else 
                out.put("result", "undefined");
        }
        else
            out.put("result", "unknown");
    }
    else
        out.put("result", "unknown");
}

void CTradeService::ReqGetFloatingProfit(PackageHandlerParamType, const ptree & in, ptree & out)
{
    string StrategiesList = GetNodeData("strategyid", in).c_str();
    vector<int> strategids_vec;
    const std::regex pattern_match(R"((([0-9]+),)+)");
    
    std::match_results<std::string::const_iterator> result;

    if (std::regex_search(StrategiesList, result, pattern_match))
    {
        const std::regex pattern_search(R"(([0-9]+),)");
        for (std::sregex_iterator p(StrategiesList.cbegin(), StrategiesList.cend(), pattern_search), q; p != q; ++p)
            strategids_vec.push_back(atoi(p->format("$1").c_str()));
    }
    else
        throw std::runtime_error("strategyid list error.");
    boost::shared_lock<boost::shared_mutex> rlock1(m_mtxAllStrategys, boost::try_to_lock);
    if (rlock1.owns_lock())
    {
        for (auto StrategyID : strategids_vec)
        {
            if (StrategyID > _MaxStrategyID || StrategyID < 0)
                throw std::runtime_error("strategyid error.");
            if (nullptr == m_arrayAllStrategys[StrategyID].m_pStrategy)
                throw std::runtime_error("strategyid does not exist.");
        }
        double sum_floating_profit = 0;
        for (auto StrategyID : strategids_vec)
        {
            boost::unique_lock<boost::shared_mutex> wlock_strategy(m_arrayAllStrategys[StrategyID].m_mtxPropectStrategy);
            double profit = 0;
            if (m_arrayAllStrategys[StrategyID].m_pStrategy->OnGetFloatingProfit(&profit))
                sum_floating_profit += profit;
            else
                std::runtime_error("some strategy does not support floating profit.");
        }
        out.put("result", sum_floating_profit);
    }
    else
        out.put("result", "unknown");
}

void CTradeService::ReqStatus(PackageHandlerParamType, const ptree & in, ptree & out)
{
    auto StrategyID = atoi(GetNodeData("strategyid", in).c_str());
    boost::shared_lock<boost::shared_mutex> rlock1(m_mtxAllStrategys, boost::try_to_lock);
    if (rlock1.owns_lock())
    {
        if (StrategyID > _MaxStrategyID || StrategyID < 0)
            throw std::runtime_error("strategyid error.");
        if (nullptr == m_arrayAllStrategys[StrategyID].m_pStrategy)
            throw std::runtime_error("strategyid does not exist.");
        boost::unique_lock<boost::shared_mutex> wlock_strategy(m_arrayAllStrategys[StrategyID].m_mtxPropectStrategy, boost::try_to_lock);
        if (wlock_strategy.owns_lock())
        {
            char StatusBuf[256] = { "-" };
            if (m_arrayAllStrategys[StrategyID].m_pStrategy->OnGetStatus(StatusBuf, sizeof(StatusBuf)))
            {
                StatusBuf[sizeof(StatusBuf) - 1] = '\0';
                out.put("result", "valid");
                out.put("status", StatusBuf);
            }
            else
                out.put("result", "undefined");
        }
        else
            out.put("result", "unknown");
    }
    else
        out.put("result", "unknown");
}



bool CTradeService::Inquery(TStrategyIdType stid, MStrategyInquiryDataInterface * inquery)
{
    inquery->Release();
    return false;
}

bool CTradeService::MeddleResponse(TStrategyIdType StrategyID, const char * fmt, ...)
{
    boost::shared_lock<boost::shared_mutex> rlock(m_mtxAllStrategys, boost::try_to_lock);
    if (rlock.owns_lock())
    {
        auto & strategy = m_arrayAllStrategys[StrategyID];
        if (nullptr == strategy.m_pStrategy)
            return false;
        else
        {
            char buf[MEDDLE_RESPONSE_MAXLEN];
            va_list arg;
            va_start(arg, fmt);
            vsnprintf(buf, MEDDLE_RESPONSE_MAXLEN, fmt, arg);
            va_end(arg);
            boost::unique_lock<boost::shared_mutex> wlock(strategy.m_mtxPropectMeddleResponseQueue, boost::try_to_lock);
            if (wlock.owns_lock())
            {
                strategy.m_queueMeddleResponseQueue.push(make_pair(microsec_clock::universal_time(), buf));
                return true;
            }
            else
                return false;
        }
    }
    else
        return false;
}

bool CTradeService::ShowMessage(TStrategyIdType stid, const char * fmt, ...)
{
    char buf[STRATEGY_MESSAGE_MAXLEN];
    va_list arg;
    va_start(arg, fmt);
    vsnprintf(buf, STRATEGY_MESSAGE_MAXLEN, fmt, arg);
    va_end(arg);
    BOOST_LOG_SEV(m_Logger, severity_levels::normal) << ProcessName << "|" << stid << ": " <<buf << " [" << to_iso_string(microsec_clock::universal_time()) << "]";
    return true;
}

bool CTradeService::GetNextMeddle(TStrategyIdType StrategyID, char * retbuffer, unsigned int maxlength)
{
    boost::shared_lock<boost::shared_mutex> rlock(m_mtxAllStrategys, boost::try_to_lock);
    if (rlock.owns_lock())
    {
        auto & strategy = m_arrayAllStrategys[StrategyID];
        if (nullptr == strategy.m_pStrategy)
            return false;
        else
        {
            boost::unique_lock<boost::shared_mutex> wlock(strategy.m_mtxPropectMeddleQueue, boost::try_to_lock);
            if (wlock.owns_lock())
            {
                if (strategy.m_queueMeddleQueue.empty())
                    return false;
                else
                {
                    string nextmeddle = strategy.m_queueMeddleQueue.front();
                    strategy.m_queueMeddleQueue.pop();
                    if (nextmeddle.size() > maxlength - 1)
                        return false;
                    else
                    {
                        strncpy(retbuffer, nextmeddle.c_str(), maxlength);
                        return true;
                    }
                }
            }
            else
                return false;
        }
    }
    else
        return false;
    
}

TOrderRefIdType CTradeService::MakeOrder(
    TStrategyIdType stid,
    TOrderType type,
    TOrderDirectionType dir,
    TOrderOffsetType offset,
    TVolumeType volume,
    TPriceType price,
    TMarketDataIdType dataid,
    TCustomRefPartType custom)
{
    boost::shared_lock<boost::shared_mutex> rlock1(m_mtxAllStrategys);
    if (nullptr == m_arrayAllStrategys[stid].m_pStrategy)
    {
        BOOST_LOG_SEV(m_Logger, severity_levels::error) << ProcessName <<"|"<< stid << ": "<<"Limitorder with invalid strategyid "<< stid
            << " [" << to_iso_string(microsec_clock::universal_time()) << "]";
        return LB1_NullOrderRef;
    }
    if (LB1_Increase == offset)
    {//风控
        if (second_clock::universal_time().date() != m_arrayAllStrategys[stid].m_dateActionDate)
        {
            m_arrayAllStrategys[stid].m_uRemainIncreaseOrderCountPerDay.store(m_arrayAllStrategys[stid].m_uMaxIncreaseOrderCountPerDay.load());
            m_arrayAllStrategys[stid].m_dateActionDate = second_clock::universal_time().date();
        }
        
        if (0 == m_arrayAllStrategys[stid].m_uRemainIncreaseOrderCountPerDay)
        {
            BOOST_LOG_SEV(m_Logger, severity_levels::error) << ProcessName << "|" << stid << ": "<<"There is no limitorder tickets today!";
            return LB1_NullOrderRef;
        }
        else
            --m_arrayAllStrategys[stid].m_uRemainIncreaseOrderCountPerDay;
    }
    auto API = m_arrayAllStrategys[stid].m_mapDataid2TradeApi[dataid];
    if (false == API.first->IsOnline())
    {
        BOOST_LOG_SEV(m_Logger, severity_levels::error) << ProcessName << "|" << stid << ": "<<"make order when API is offline"
            << " [" << to_iso_string(microsec_clock::universal_time()) << "]";;
        return LB1_NullOrderRef;
    }
    TOrderRefIdType orderRefBase =
        _StrategyCustom2OrderRefPart(custom) +
        _StrategyID2OrderRefPart(stid) +
        _OrderDirection2OrderRefPart(dir) +
        _OrderOffset2OrderRefPart(offset) +
        _SystemNumberPart2OrderRefPart(m_uSystemNumber);
    auto OrderRef= API.first->TDBasicMakeOrder(type,API.second, dir, offset, volume, price, orderRefBase);
    BOOST_LOG_SEV(m_Logger, severity_levels::normal) 
        << ProcessName << "|" << stid << ": "
        << "LimitOrder Volume:"<< volume
        << " Price:"<< price
        << " Ref:"<< OrderRef
        << " Custom:" << _OrderRef2StrategyCustomPart(OrderRef)
        << " SystemN:" << _OrderRef2SystemNumberPart(OrderRef)
        << " AccountN:" << _OrderRef2AccountNumberPart(OrderRef)
        << " OrderInc:" << _OrderRef2OrderIncreasePart(OrderRef)
        << " RemainTickets:"<< m_arrayAllStrategys[stid].m_uRemainIncreaseOrderCountPerDay
        << " [" << to_iso_string(microsec_clock::universal_time()) << "]";
    return OrderRef;
}

TLastErrorIdType CTradeService::CancelOrder(TStrategyIdType stid,TOrderRefIdType ref, TOrderSysIdType sys, TMarketDataIdType dataid)
{
    boost::shared_lock<boost::shared_mutex> rlock1(m_mtxAllStrategys);
    if (nullptr == m_arrayAllStrategys[stid].m_pStrategy)
    {
        BOOST_LOG_SEV(m_Logger, severity_levels::error) << ProcessName << "|" << stid << ": " <<"Cancel order with invalid strategyid " << stid
            << " [" << to_iso_string(microsec_clock::universal_time()) << "]";;
        return LB1_INVALID_VAL;
    }
    auto & API = m_arrayAllStrategys[stid].m_mapDataid2TradeApi[dataid];
    if (false == API.first->IsOnline())
    {
        BOOST_LOG_SEV(m_Logger, severity_levels::error) << ProcessName << "|" << stid << ": " << "cancel order when API is offline"
            << " [" << to_iso_string(microsec_clock::universal_time()) << "]";;
        return LB1_INVALID_VAL;
    }
    auto res = API.first->TDBasicCancelOrder(ref, API.second, sys);
    BOOST_LOG_SEV(m_Logger, severity_levels::normal) << ProcessName << "|" << stid << ": " << "Cancel order "<< ref<< ((TLastErrorIdType::LB1_NO_ERROR == res)?" Succeed":" Failed")
        << " [" << to_iso_string(microsec_clock::universal_time()) << "]";;
    return res;
}

void CTradeService::UpdateChart()
{

}

bool CTradeService::GetSharedValue(TSharedIndexType i,double & ret)
{
    boost::shared_lock<boost::shared_mutex> rlock(m_mtxSharedValue);
    if (m_mapSharedValue.find(i) != m_mapSharedValue.end())
    {
        ret = m_mapSharedValue[i];
        return true;
    }
    else
        return false;
}

bool CTradeService::IncreaseSharedValue(TSharedIndexType i, double dt, function<bool(double)> issatisfy)
{
    boost::unique_lock<boost::shared_mutex> wlock(m_mtxSharedValue);
    if (m_mapSharedValue.find(i) == m_mapSharedValue.end())
        return false;
    if (issatisfy(m_mapSharedValue[i]))
    {
        m_mapSharedValue[i] += dt;
        BOOST_LOG_SEV(m_Logger, severity_levels::normal) << ProcessName << ": " <<"SharedValue[" << i << "]=" << m_mapSharedValue[i]
            << " [" << to_iso_string(microsec_clock::universal_time()) << "]";
        return true;
    }
    else
        return false;
}

bool CTradeService::DecreaseSharedValue(TSharedIndexType i, double dt, function<bool(double)> issatisfy)
{
    boost::unique_lock<boost::shared_mutex> wlock(m_mtxSharedValue);
    if (m_mapSharedValue.find(i) == m_mapSharedValue.end())
        return false;
    if (issatisfy(m_mapSharedValue[i]))
    {
        m_mapSharedValue[i] -= dt;
        BOOST_LOG_SEV(m_Logger, severity_levels::normal) << ProcessName<< ": " << "SharedValue[" << i << "]=" << m_mapSharedValue[i]
            << " [" << to_iso_string(microsec_clock::universal_time()) << "]";
        return true;
    }
    else
        return false;
}

bool CTradeService::SetSharedValue(TSharedIndexType i, double newvalue, function<bool(double)> issatisfy)
{
    boost::unique_lock<boost::shared_mutex> wlock(m_mtxSharedValue);
    if (issatisfy(m_mapSharedValue[i]))
    {
        m_mapSharedValue[i] = newvalue;
        BOOST_LOG_SEV(m_Logger, severity_levels::normal) << ProcessName << ": " << "SharedValue[" << i << "]=" << m_mapSharedValue[i]
            << " [" << to_iso_string(microsec_clock::universal_time()) << "]";
        return true;
    }
    else
        return false;
}

int CTradeService::GetRemainCancelAmount(TStrategyIdType stid, TMarketDataIdType dataid)
{
    boost::shared_lock<boost::shared_mutex> rlock1(m_mtxAllStrategys);
    if (stid< 0 || stid>_MaxStrategyID)
        return 0;
    if (nullptr == m_arrayAllStrategys[stid].m_pStrategy)
    {
        BOOST_LOG_SEV(m_Logger, severity_levels::error) << ProcessName << "|" << stid << ": " << "Cancel order with invalid strategyid " << stid
            << " [" << to_iso_string(microsec_clock::universal_time()) << "]";;
        return 0;
    }
    auto & API = m_arrayAllStrategys[stid].m_mapDataid2TradeApi[dataid];
    return API.first->TDGetRemainAmountOfCancelChances(API.second["instrumentid"].c_str());
}



void CTradeService::OnTrade(
    TOrderRefIdType Ref, 
    TOrderSysIdType Sys, 
    TPriceType Price,
    TVolumeType Volume)
{
    unsigned int CustomPart = _OrderRef2StrategyCustomPart(Ref);
    unsigned int StrategyIDPart = _OrderRef2StrategyIDPart(Ref);
    unsigned int DirectionPart = _OrderRef2OrderDirectionPart(Ref);
    unsigned int OffsetPart = _OrderRef2OrderOffsetPart(Ref);
    unsigned int SystemNumberPart = _OrderRef2SystemNumberPart(Ref);
    unsigned int AccountNumberPart = _OrderRef2AccountNumberPart(Ref);
    unsigned int OrderIncreasePart = _OrderRef2OrderIncreasePart(Ref);
    if (SystemNumberPart != m_uSystemNumber)
        return;
    
    boost::shared_lock<boost::shared_mutex> rlock1(m_mtxAllStrategys/*,boost::defer_lock*/);
    auto & StrategyNode = m_arrayAllStrategys[StrategyIDPart];
    if (nullptr == StrategyNode.m_pStrategy)
        return;
    boost::unique_lock<boost::shared_mutex> wlock2(StrategyNode.m_mtxPropectStrategy/*, boost::defer_lock*/);
    //std::lock(rlock1, wlock2);
    StrategyNode.m_pStrategy->OnTrade(
        Ref,
        Sys,
        Volume,
        Price,
        static_cast<StrategyData::TOrderDirectionType>(DirectionPart),
        static_cast<StrategyData::TOrderOffsetType>(OffsetPart)
        );
    BOOST_LOG_SEV(m_Logger, severity_levels::normal)
        << ProcessName << "|" << StrategyIDPart << ": "
        << "Strategy" << StrategyIDPart
        << ": OnTrade"
        << " Ref:" << Ref
        << " Price:" << Price
        << " Volume:" << Volume
        << " Custom:" << CustomPart
        << " SystemN:" << SystemNumberPart
        << " AccountN:" << AccountNumberPart
        << " OrderInc:" << OrderIncreasePart
        << " [" << to_iso_string(microsec_clock::universal_time()) << "]";
        ;


}
void CTradeService::OnOrder(
    TOrderRefIdType Ref, 
    TOrderSysIdType Sys, 
    TOrderStatusType Status, 
    TPriceType Price, 
    TTradedVolumeType TradedV, 
    TRemainVolumeType RemainV)
{
    unsigned int CustomPart = _OrderRef2StrategyCustomPart(Ref);
    unsigned int StrategyIDPart = _OrderRef2StrategyIDPart(Ref);
    unsigned int DirectionPart = _OrderRef2OrderDirectionPart(Ref);
    unsigned int OffsetPart = _OrderRef2OrderOffsetPart(Ref);
    unsigned int SystemNumberPart = _OrderRef2SystemNumberPart(Ref);
    unsigned int AccountNumberPart = _OrderRef2AccountNumberPart(Ref);
    unsigned int OrderIncreasePart = _OrderRef2OrderIncreasePart(Ref );
    if (SystemNumberPart != m_uSystemNumber)
        return;
    
    boost::shared_lock<boost::shared_mutex> rlock1(m_mtxAllStrategys/*, boost::defer_lock*/);
    auto & StrategyNode = m_arrayAllStrategys[StrategyIDPart];
    if (nullptr == StrategyNode.m_pStrategy)
        return;
    boost::unique_lock<boost::shared_mutex> wlock2(StrategyNode.m_mtxPropectStrategy/*, boost::defer_lock*/);
    //std::lock(rlock1, wlock2);
    StrategyNode.m_pStrategy->OnOrder(
        Ref,
        Sys,
        static_cast<StrategyData::TOrderDirectionType>(DirectionPart),
        Status, Price, TradedV, RemainV
        );
    string _strStatus;
    switch (Status)
    {
        case LB1_StatusNoTradeQueueing:_strStatus = "NoTradeQueueing";break;
        case LB1_StatusPartTradedQueueing:_strStatus = "PartTradedQueueing";break;
        case LB1_StatusAllTraded:_strStatus = "AllTraded";break;
        case LB1_StatusCanceled:_strStatus = "Canceled";break;
        case LB1_StatusUnknown:_strStatus = "Unknown";break;
    }
    BOOST_LOG_SEV(m_Logger, severity_levels::normal) 
        << ProcessName << "|" << StrategyIDPart << ": "
        << "Strategy" << StrategyIDPart
        << ": OnOrder"
        << " Ref:"<< Ref
        << " Sys:" << Sys
        << " Dir:" << ((static_cast<StrategyData::TOrderDirectionType>(DirectionPart)==LB1_Buy) ? "Buy":"Sell")
        << " Offset:" << ((static_cast<StrategyData::TOrderOffsetType>(OffsetPart) == LB1_Increase) ? "Increase" : "Decrease")
        << " Status:" << _strStatus
        << " Price:" << Price
        << " TradedVolume:" << TradedV
        << " RemainVolume:" << RemainV
        << " Custom:"<< CustomPart
        << " SystemN:" << SystemNumberPart
        << " AccountN:" << AccountNumberPart
        << " OrderInc:" << OrderIncreasePart
        << " [" << to_iso_string(microsec_clock::universal_time()) << "]";
        ;
}

