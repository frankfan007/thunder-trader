#if !defined(HUNDSUNTRADERAPI_H)
#define HUNDSUNTRADERAPI_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <string.h>
#include "HundsunUserApiStruct.h"

#if defined(ISLIB)

#ifdef WIN32
#ifdef LIB_TRADER_API_EXPORT
#define TRADER_API_EXPORT __declspec(dllexport)
#else
#define TRADER_API_EXPORT __declspec(dllimport)
#endif
#endif

#ifdef LINUX
#ifdef LIB_TRADER_API_EXPORT
#define TRADER_API_EXPORT __attribute__((visibility("default")))
#else
#define TRADER_API_EXPORT
#endif
#endif

#else
#define TRADER_API_EXPORT
#endif

//回调虚类
class CHundsunTraderSpi
{
public:
    ///当客户端与交易后台建立起通信连接时（还未登录前），该方法被调用。
    virtual void OnFrontConnected(){};

    ///当客户端与交易后台通信连接断开时，该方法被调用。当发生这个情况后，API会自动重新连接，客户端可不做处理。
    virtual void OnFrontDisconnected(int nReason){};

    ///339204 历史转账流水查询应答
    virtual void OnRspFuncSvrbankHisBktransferQry(CHundsunRspFuncSvrbankHisBktransferQryField *pRspFuncSvrbankHisBktransferQry, CHundsunRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {};

    ///331153 客户委托方式查询应答
    virtual void OnRspFuncClientEntrustwayQry(CHundsunRspFuncClientEntrustwayQryField *pRspFuncClientEntrustwayQry, CHundsunRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {};

    ///332296 银行账户查询应答
    virtual void OnRspFuncBankaccQry(CHundsunRspFuncBankaccQryField *pRspFuncBankaccQry, CHundsunRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {};

    ///332200 银行转账应答
    virtual void OnRspFuncBankTransfer(CHundsunRspFuncBankTransferField *pRspFuncBankTransfer, CHundsunRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {};

    ///332250 存管资金账户转账日志查询应答
    virtual void OnRspFuncBanktransferQry(CHundsunRspFuncBanktransferQryField *pRspFuncBanktransferQry, CHundsunRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {};

    ///332253 银行余额查询应答
    virtual void OnRspFuncFundamtQry(CHundsunRspFuncFundamtQryField *pRspFuncFundamtQry, CHundsunRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {};

    ///332297 银行转账应答
    virtual void OnRspFuncUfxbankTransfer(CHundsunRspFuncUfxbankTransferField *pRspFuncUfxbankTransfer, CHundsunRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {};

    ///332298 银行余额查询应答
    virtual void OnRspFuncUfxbankBkbalanceQry(CHundsunRspFuncUfxbankBkbalanceQryField *pRspFuncUfxbankBkbalanceQry, CHundsunRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {};

    ///331157 客户银行账户查询应答
    virtual void OnRspFuncClientBankacctQry(CHundsunRspFuncClientBankacctQryField *pRspFuncClientBankacctQry, CHundsunRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {};

    ///332254 客户资金快速查询应答
    virtual void OnRspFuncClientFundFastQry(CHundsunRspFuncClientFundFastQryField *pRspFuncClientFundFastQry, CHundsunRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {};

    ///331150 客户信息查询应答
    virtual void OnRspFuncClientAllQry(CHundsunRspFuncClientAllQryField *pRspFuncClientAllQry, CHundsunRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {};

    ///331100 客户登录应答
    virtual void OnRspFuncClientLogin(CHundsunRspFuncClientLoginField *pRspFuncClientLogin, CHundsunRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {};

    ///331101 客户密码更改应答
    virtual void OnRspFuncClientPwdMod(CHundsunRspFuncClientPwdModField *pRspFuncClientPwdMod, CHundsunRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {};

    ///338218 期权询价应答
    virtual void OnRspFuncForquoteinsert(CHundsunRspFuncForquoteinsertField *pRspFuncForquoteinsert, CHundsunRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {};

    ///338219 期权报价应答
    virtual void OnRspFuncQuoteinsert(CHundsunRspFuncQuoteinsertField *pRspFuncQuoteinsert, CHundsunRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {};

    ///338220 报价撤单应答
    virtual void OnRspFuncQuoteaction(CHundsunRspFuncQuoteactionField *pRspFuncQuoteaction, CHundsunRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {};

    ///338310 查询客户报价信息应答
    virtual void OnRspFuncQuoteQry(CHundsunRspFuncQuoteQryField *pRspFuncQuoteQry, CHundsunRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {};

    ///338311 查询询价信息应答
    virtual void OnRspFuncForquoteQry(CHundsunRspFuncForquoteQryField *pRspFuncForquoteQry, CHundsunRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {};

    ///338312 查询期权交易成本应答
    virtual void OnRspFuncOptioninstrtradecostQry(CHundsunRspFuncOptioninstrtradecostQryField *pRspFuncOptioninstrtradecostQry, CHundsunRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {};

    ///338205 期权行权申请应答
    virtual void OnRspFuncExecorderinsert(CHundsunRspFuncExecorderinsertField *pRspFuncExecorderinsert, CHundsunRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {};

    ///338206 期权行权撤销申请应答
    virtual void OnRspFuncExecorderaction(CHundsunRspFuncExecorderactionField *pRspFuncExecorderaction, CHundsunRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {};

    ///338309 到期执行持仓查询应答
    virtual void OnRspFuncExpireinvestorpositionQry(CHundsunRspFuncExpireinvestorpositionQryField *pRspFuncExpireinvestorpositionQry, CHundsunRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {};

    ///330853 组合代码查询应答
    virtual void OnRspFuncArginstrumentQry(CHundsunRspFuncArginstrumentQryField *pRspFuncArginstrumentQry, CHundsunRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {};

    ///330854 组合行情查询应答
    virtual void OnRspFuncArgquoteQry(CHundsunRspFuncArgquoteQryField *pRspFuncArgquoteQry, CHundsunRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {};

    ///338204 组合委托确认应答
    virtual void OnRspFuncArgorderinsert(CHundsunRspFuncArgorderinsertField *pRspFuncArgorderinsert, CHundsunRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {};

    ///338224 期货中金套利组合确认应答
    virtual void OnRspFuncFutuCombmarginactionInsert(CHundsunRspFuncFutuCombmarginactionInsertField *pRspFuncFutuCombmarginactionInsert, CHundsunRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {};

    ///338304 UFT单客户查询费用属性应答
    virtual void OnRspFuncInstrumentcommissionrateQry(CHundsunRspFuncInstrumentcommissionrateQryField *pRspFuncInstrumentcommissionrateQry, CHundsunRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {};

    ///338305 UFT保证金属性查询应答
    virtual void OnRspFuncInstrumentmarginrateQry(CHundsunRspFuncInstrumentmarginrateQryField *pRspFuncInstrumentmarginrateQry, CHundsunRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {};

    ///338313 交易所状态查询应答
    virtual void OnRspFuncExchangestatusQry(CHundsunRspFuncExchangestatusQryField *pRspFuncExchangestatusQry, CHundsunRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {};

    ///338314 交易所查询应答
    virtual void OnRspFuncExchangeQry(CHundsunRspFuncExchangeQryField *pRspFuncExchangeQry, CHundsunRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {};

    ///338323 期货中金组合持仓查询应答
    virtual void OnRspFuncFutuComholeinfoQry(CHundsunRspFuncFutuComholeinfoQryField *pRspFuncFutuComholeinfoQry, CHundsunRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {};

    ///338324 期货中金组合委托查询应答
    virtual void OnRspFuncFutuComentrustinfoQry(CHundsunRspFuncFutuComentrustinfoQryField *pRspFuncFutuComentrustinfoQry, CHundsunRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {};

    ///330850 期货合约行情查询应答
    virtual void OnRspFuncFutuContracthqQry(CHundsunRspFuncFutuContracthqQryField *pRspFuncFutuContracthqQry, CHundsunRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {};

    ///330851 期货合约保证金查询应答
    virtual void OnRspFuncFutuContractbailQry(CHundsunRspFuncFutuContractbailQryField *pRspFuncFutuContractbailQry, CHundsunRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {};

    ///330852 取交易所时间差应答
    virtual void OnRspFuncExchangetimeQry(CHundsunRspFuncExchangetimeQryField *pRspFuncExchangetimeQry, CHundsunRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {};

    ///330855 监控中心查询密钥获取应答
    virtual void OnRspFuncCfmmctradingaccountkeyQry(CHundsunRspFuncCfmmctradingaccountkeyQryField *pRspFuncCfmmctradingaccountkeyQry, CHundsunRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {};

    ///331124 密码校验应答
    virtual void OnRspFundFutuChkpassword(CHundsunRspFundFutuChkpasswordField *pRspFundFutuChkpassword, CHundsunRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {};

    ///331850 期货交易账号查询应答
    virtual void OnRspFuncTradingcodeQry(CHundsunRspFuncTradingcodeQryField *pRspFuncTradingcodeQry, CHundsunRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {};

    ///331851 期货客户账单确认应答
    virtual void OnRspFuncSettlementInfoConfirm(CHundsunRspFuncSettlementInfoConfirmField *pRspFuncSettlementInfoConfirm, CHundsunRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {};

    ///332213 取期货登记银行信息应答
    virtual void OnRspFundFutuBankinfoQry(CHundsunRspFundFutuBankinfoQryField *pRspFundFutuBankinfoQry, CHundsunRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {};

    ///338200 检查期货合约代码应答
    virtual void OnRspFuncFutuCheckFutuCode(CHundsunRspFuncFutuCheckFutuCodeField *pRspFuncFutuCheckFutuCode, CHundsunRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {};

    ///338201 期货可买卖数量获取应答
    virtual void OnRspFuncMaxordervolume(CHundsunRspFuncMaxordervolumeField *pRspFuncMaxordervolume, CHundsunRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {};

    ///338202 期货委托确认应答
    virtual void OnRspFuncOrderinsert(CHundsunRspFuncOrderinsertField *pRspFuncOrderinsert, CHundsunRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {};

    ///338203 检查组合合约代码应答
    virtual void OnRspFuncFutuCheckCombineFutuCode(CHundsunRspFuncFutuCheckCombineFutuCodeField *pRspFuncFutuCheckCombineFutuCode, CHundsunRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {};

    ///338217 期货委托撤单应答
    virtual void OnRspFuncOrderaction(CHundsunRspFuncOrderactionField *pRspFuncOrderaction, CHundsunRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {};

    ///338300 期货客户资金查询应答
    virtual void OnRspFuncTradingaccountQry(CHundsunRspFuncTradingaccountQryField *pRspFuncTradingaccountQry, CHundsunRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {};

    ///338301 期货当日委托查询应答
    virtual void OnRspFuncOrderQry(CHundsunRspFuncOrderQryField *pRspFuncOrderQry, CHundsunRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {};

    ///338302 期货当日成交查询应答
    virtual void OnRspFuncTradeQry(CHundsunRspFuncTradeQryField *pRspFuncTradeQry, CHundsunRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {};

    ///338303 期货持仓查询应答
    virtual void OnRspFuncInvestorpositionQry(CHundsunRspFuncInvestorpositionQryField *pRspFuncInvestorpositionQry, CHundsunRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {};

    ///338306 当日资金流水查询应答
    virtual void OnRspFuncFutuFundjourQry(CHundsunRspFuncFutuFundjourQryField *pRspFuncFutuFundjourQry, CHundsunRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {};

    ///338307 持仓明细查询应答
    virtual void OnRspFuncInvestorpositiondetailQry(CHundsunRspFuncInvestorpositiondetailQryField *pRspFuncInvestorpositiondetailQry, CHundsunRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {};

    ///338308 取系统信息应答
    virtual void OnRspFuncFutuSysinfoQry(CHundsunRspFuncFutuSysinfoQryField *pRspFuncFutuSysinfoQry, CHundsunRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {};

    ///339851 取客户账单应答
    virtual void OnRspFuncSettlementinfoQry(CHundsunRspFuncSettlementinfoQryField *pRspFuncSettlementinfoQry, CHundsunRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {};

    ///339852 期货历史委托查询应答
    virtual void OnRspFuncHisorderQry(CHundsunRspFuncHisorderQryField *pRspFuncHisorderQry, CHundsunRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {};

    ///339853 期货历史成交查询应答
    virtual void OnRspFuncHistradeQry(CHundsunRspFuncHistradeQryField *pRspFuncHistradeQry, CHundsunRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {};

    ///339854 取客户历史资金应答
    virtual void OnRspFuncHistradingaccountQry(CHundsunRspFuncHistradingaccountQryField *pRspFuncHistradingaccountQry, CHundsunRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {};

    ///338000 期权代码信息查询应答
    virtual void OnRspFuncOptOptcodeQry(CHundsunRspFuncOptOptcodeQryField *pRspFuncOptOptcodeQry, CHundsunRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {};

    ///338001 期权标的信息查询应答
    virtual void OnRspFuncOptOptobjectcodeQry(CHundsunRspFuncOptOptobjectcodeQryField *pRspFuncOptOptobjectcodeQry, CHundsunRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {};

    ///338002 期权代码输入确认应答
    virtual void OnRspFuncOptOptcodeEnter(CHundsunRspFuncOptOptcodeEnterField *pRspFuncOptOptcodeEnter, CHundsunRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {};

    ///338010 期权可交易数量获取应答
    virtual void OnRspFuncOptEnTrade(CHundsunRspFuncOptEnTradeField *pRspFuncOptEnTrade, CHundsunRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {};

    ///338011 期权委托应答
    virtual void OnRspFuncOptEntrust(CHundsunRspFuncOptEntrustField *pRspFuncOptEntrust, CHundsunRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {};

    ///338012 期权撤单应答
    virtual void OnRspFuncOptWithdraw(CHundsunRspFuncOptWithdrawField *pRspFuncOptWithdraw, CHundsunRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {};

    ///338020 期权委托查询应答
    virtual void OnRspFuncOptEntrustQry(CHundsunRspFuncOptEntrustQryField *pRspFuncOptEntrustQry, CHundsunRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {};

    ///338021 期权成交查询应答
    virtual void OnRspFuncOptRealtimeQry(CHundsunRspFuncOptRealtimeQryField *pRspFuncOptRealtimeQry, CHundsunRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {};

    ///338022 期权资产查询应答
    virtual void OnRspFuncOptAssetQry(CHundsunRspFuncOptAssetQryField *pRspFuncOptAssetQry, CHundsunRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {};

    ///338023 期权持仓查询应答
    virtual void OnRspFuncOptHoldQry(CHundsunRspFuncOptHoldQryField *pRspFuncOptHoldQry, CHundsunRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {};

    ///338024 期权行权指派查询应答
    virtual void OnRspFuncOptExeassignQry(CHundsunRspFuncOptExeassignQryField *pRspFuncOptExeassignQry, CHundsunRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {};

    ///338025 期权行权交割信息查询应答
    virtual void OnRspFuncAssetOptexedeliverQry(CHundsunRspFuncAssetOptexedeliverQryField *pRspFuncAssetOptexedeliverQry, CHundsunRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {};

    ///338026 期权行权指派欠资欠券查询应答
    virtual void OnRspFuncAssetDebtinfoQry(CHundsunRspFuncAssetDebtinfoQryField *pRspFuncAssetDebtinfoQry, CHundsunRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {};

    ///338027 期权标的证券数量提示应答
    virtual void OnRspFuncOptUnderlyamountPrompt(CHundsunRspFuncOptUnderlyamountPromptField *pRspFuncOptUnderlyamountPrompt, CHundsunRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {};

    ///338028 客户期权持仓合约信息提示应答
    virtual void OnRspFuncOptOptholdrealPrompt(CHundsunRspFuncOptOptholdrealPromptField *pRspFuncOptOptholdrealPrompt, CHundsunRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {};

    ///338029 客户备兑证券不足查询应答
    virtual void OnRspFuncAssetAssetcoverstockQry(CHundsunRspFuncAssetAssetcoverstockQryField *pRspFuncAssetAssetcoverstockQry, CHundsunRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {};

    ///338030 备兑证券可划转数量获取应答
    virtual void OnRspFuncOptCoveredQry(CHundsunRspFuncOptCoveredQryField *pRspFuncOptCoveredQry, CHundsunRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {};

    ///338031 备兑证券划转应答
    virtual void OnRspFuncOptCoveredTrans(CHundsunRspFuncOptCoveredTransField *pRspFuncOptCoveredTrans, CHundsunRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {};

    ///339800 历史期权委托查询应答
    virtual void OnRspFuncHisOptentrustQry(CHundsunRspFuncHisOptentrustQryField *pRspFuncHisOptentrustQry, CHundsunRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {};

    ///339801 历史期权成交查询应答
    virtual void OnRspFuncHisOptbusinessQry(CHundsunRspFuncHisOptbusinessQryField *pRspFuncHisOptbusinessQry, CHundsunRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {};

    ///339803 历史期权交割信息查询应答
    virtual void OnRspFuncHisOptdeliverQry(CHundsunRspFuncHisOptdeliverQryField *pRspFuncHisOptdeliverQry, CHundsunRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {};

    ///339804 历史期权行权指派查询应答
    virtual void OnRspFuncHisExeassignQry(CHundsunRspFuncHisExeassignQryField *pRspFuncHisExeassignQry, CHundsunRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {};

    ///339805 历史期权行权交割信息查询应答
    virtual void OnRspFuncHisOptexedeliverQry(CHundsunRspFuncHisOptexedeliverQryField *pRspFuncHisOptexedeliverQry, CHundsunRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {};

    ///339806 历史期权对账单查询应答
    virtual void OnRspFuncHisOptstatementQry(CHundsunRspFuncHisOptstatementQryField *pRspFuncHisOptstatementQry, CHundsunRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {};

    ///339807 取客户账单应答
    virtual void OnRspFuncOptSettlementinfoQry(CHundsunRspFuncOptSettlementinfoQryField *pRspFuncOptSettlementinfoQry, CHundsunRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {};

    ///395 期权行情查询应答
    virtual void OnRspFuncOptPriceQry(CHundsunRspFuncOptPriceQryField *pRspFuncOptPriceQry, CHundsunRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {};

    //620001_33101 订阅-期货委托成交回报应答
    virtual void OnRspFuncQhEntrustDealSubscribe(CHundsunRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {};

    //620002_33101 订阅取消-期货委托成交回报应答
    virtual void OnRspFuncQhEntrustDealCancel(CHundsunRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {};

    //620003_33101主推-期货委托回报应答
    virtual void OnRtnFuncQhEntrustPush(CHundsunRtnFuncQhEntrustPushField *pFuncQhEntrustPush) {};

    //620003_33101主推-期货成交回报应答
    virtual void OnRtnFuncQhRealPush(CHundsunRtnFuncQhRealPushField *pFuncQhRealPush) {};

    //620001_33105 订阅-期货交易所状态信息应答
    virtual void OnRspFuncQhExchStatusSubscribe(CHundsunRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {};

    //620002_33105 订阅取消-期货交易所状态信息应答
    virtual void OnRspFuncQhExchStatusCancel(CHundsunRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {};

    //620003_33105 主推-期货交易所状态信息应答
    virtual void OnRtnFuncQhExchStatusPush(CHundsunRtnFuncQhExchStatusPushField *pFuncQhExchStatusPush) {};

};

///交易
class TRADER_API_EXPORT CHundsunTraderApi
{
public:
    ///创建TraderApi
    static CHundsunTraderApi *CreateHundsunTraderApi(const char *pszFlowPath = "");

    ///删除接口对象本身
    virtual void ReleaseApi() = 0;

    ///初始连接
    virtual void Init(const char *pszLicFile, const char *pszPwd = "", const char *pszSslFile = "", const char *pszSslPwd = "") = 0;

    ///等待接口线程结束运行
    virtual int Join() = 0;

    ///注册前置机网络地址
    ///@param pszFrontAddress：前置机网络地址。
    ///@param nSubSystemNo：子系统号(根据具体期货公司部署来定，一般期货是115)
    ///@param nCompanyID：公司编号(对接ITN时需要，可以具体可以向ITN询问)
    virtual void RegisterFront(const char *pszFrontAddress, int nSubSystemNo = 0, int nCompanyID = 0) = 0;

    ///注册回调接口
    ///@param pSpi 派生自回调接口类的实例
    virtual void RegisterSpi(CHundsunTraderSpi *pSpi) = 0;

    ///返回用户信息
    virtual CHundsunUserInfoField* GetUserInfo() = 0;

    ///339204 历史转账流水查询请求
    virtual int ReqFuncSvrbankHisBktransferQry(CHundsunReqFuncSvrbankHisBktransferQryField *pReqFuncSvrbankHisBktransferQry, int nRequestID = 0, void** ppRetData = NULL, int* pnItems = NULL, CHundsunRspInfoField *pRspInfo = NULL) = 0;

    ///331153 客户委托方式查询请求
    virtual int ReqFuncClientEntrustwayQry(CHundsunReqFuncClientEntrustwayQryField *pReqFuncClientEntrustwayQry, int nRequestID = 0, void** ppRetData = NULL, int* pnItems = NULL, CHundsunRspInfoField *pRspInfo = NULL) = 0;

    ///332296 银行账户查询请求
    virtual int ReqFuncBankaccQry(CHundsunReqFuncBankaccQryField *pReqFuncBankaccQry, int nRequestID = 0, void** ppRetData = NULL, int* pnItems = NULL, CHundsunRspInfoField *pRspInfo = NULL) = 0;

    ///332200 银行转账请求
    virtual int ReqFuncBankTransfer(CHundsunReqFuncBankTransferField *pReqFuncBankTransfer, int nRequestID = 0, void** ppRetData = NULL, int* pnItems = NULL, CHundsunRspInfoField *pRspInfo = NULL) = 0;

    ///332250 存管资金账户转账日志查询请求
    virtual int ReqFuncBanktransferQry(CHundsunReqFuncBanktransferQryField *pReqFuncBanktransferQry, int nRequestID = 0, void** ppRetData = NULL, int* pnItems = NULL, CHundsunRspInfoField *pRspInfo = NULL) = 0;

    ///332253 银行余额查询请求
    virtual int ReqFuncFundamtQry(CHundsunReqFuncFundamtQryField *pReqFuncFundamtQry, int nRequestID = 0, void** ppRetData = NULL, int* pnItems = NULL, CHundsunRspInfoField *pRspInfo = NULL) = 0;

    ///332297 银行转账请求
    virtual int ReqFuncUfxbankTransfer(CHundsunReqFuncUfxbankTransferField *pReqFuncUfxbankTransfer, int nRequestID = 0, void** ppRetData = NULL, int* pnItems = NULL, CHundsunRspInfoField *pRspInfo = NULL) = 0;

    ///332298 银行余额查询请求
    virtual int ReqFuncUfxbankBkbalanceQry(CHundsunReqFuncUfxbankBkbalanceQryField *pReqFuncUfxbankBkbalanceQry, int nRequestID = 0, void** ppRetData = NULL, int* pnItems = NULL, CHundsunRspInfoField *pRspInfo = NULL) = 0;

    ///331157 客户银行账户查询请求
    virtual int ReqFuncClientBankacctQry(CHundsunReqFuncClientBankacctQryField *pReqFuncClientBankacctQry, int nRequestID = 0, void** ppRetData = NULL, int* pnItems = NULL, CHundsunRspInfoField *pRspInfo = NULL) = 0;

    ///332254 客户资金快速查询请求
    virtual int ReqFuncClientFundFastQry(CHundsunReqFuncClientFundFastQryField *pReqFuncClientFundFastQry, int nRequestID = 0, void** ppRetData = NULL, int* pnItems = NULL, CHundsunRspInfoField *pRspInfo = NULL) = 0;

    ///331150 客户信息查询请求
    virtual int ReqFuncClientAllQry(CHundsunReqFuncClientAllQryField *pReqFuncClientAllQry, int nRequestID = 0, void** ppRetData = NULL, int* pnItems = NULL, CHundsunRspInfoField *pRspInfo = NULL) = 0;

    ///331100 客户登录请求
    virtual int ReqFuncClientLogin(CHundsunReqFuncClientLoginField *pReqFuncClientLogin, int nRequestID = 0, void** ppRetData = NULL, int* pnItems = NULL, CHundsunRspInfoField *pRspInfo = NULL) = 0;

    ///331101 客户密码更改请求
    virtual int ReqFuncClientPwdMod(CHundsunReqFuncClientPwdModField *pReqFuncClientPwdMod, int nRequestID = 0, void** ppRetData = NULL, int* pnItems = NULL, CHundsunRspInfoField *pRspInfo = NULL) = 0;

    ///338218 期权询价请求
    virtual int ReqFuncForquoteinsert(CHundsunReqFuncForquoteinsertField *pReqFuncForquoteinsert, int nRequestID = 0, void** ppRetData = NULL, int* pnItems = NULL, CHundsunRspInfoField *pRspInfo = NULL) = 0;

    ///338219 期权报价请求
    virtual int ReqFuncQuoteinsert(CHundsunReqFuncQuoteinsertField *pReqFuncQuoteinsert, int nRequestID = 0, void** ppRetData = NULL, int* pnItems = NULL, CHundsunRspInfoField *pRspInfo = NULL) = 0;

    ///338220 报价撤单请求
    virtual int ReqFuncQuoteaction(CHundsunReqFuncQuoteactionField *pReqFuncQuoteaction, int nRequestID = 0, void** ppRetData = NULL, int* pnItems = NULL, CHundsunRspInfoField *pRspInfo = NULL) = 0;

    ///338310 查询客户报价信息请求
    virtual int ReqFuncQuoteQry(CHundsunReqFuncQuoteQryField *pReqFuncQuoteQry, int nRequestID = 0, void** ppRetData = NULL, int* pnItems = NULL, CHundsunRspInfoField *pRspInfo = NULL) = 0;

    ///338311 查询询价信息请求
    virtual int ReqFuncForquoteQry(CHundsunReqFuncForquoteQryField *pReqFuncForquoteQry, int nRequestID = 0, void** ppRetData = NULL, int* pnItems = NULL, CHundsunRspInfoField *pRspInfo = NULL) = 0;

    ///338312 查询期权交易成本请求
    virtual int ReqFuncOptioninstrtradecostQry(CHundsunReqFuncOptioninstrtradecostQryField *pReqFuncOptioninstrtradecostQry, int nRequestID = 0, void** ppRetData = NULL, int* pnItems = NULL, CHundsunRspInfoField *pRspInfo = NULL) = 0;

    ///338205 期权行权申请请求
    virtual int ReqFuncExecorderinsert(CHundsunReqFuncExecorderinsertField *pReqFuncExecorderinsert, int nRequestID = 0, void** ppRetData = NULL, int* pnItems = NULL, CHundsunRspInfoField *pRspInfo = NULL) = 0;

    ///338206 期权行权撤销申请请求
    virtual int ReqFuncExecorderaction(CHundsunReqFuncExecorderactionField *pReqFuncExecorderaction, int nRequestID = 0, void** ppRetData = NULL, int* pnItems = NULL, CHundsunRspInfoField *pRspInfo = NULL) = 0;

    ///338309 到期执行持仓查询请求
    virtual int ReqFuncExpireinvestorpositionQry(CHundsunReqFuncExpireinvestorpositionQryField *pReqFuncExpireinvestorpositionQry, int nRequestID = 0, void** ppRetData = NULL, int* pnItems = NULL, CHundsunRspInfoField *pRspInfo = NULL) = 0;

    ///330853 组合代码查询请求
    virtual int ReqFuncArginstrumentQry(CHundsunReqFuncArginstrumentQryField *pReqFuncArginstrumentQry, int nRequestID = 0, void** ppRetData = NULL, int* pnItems = NULL, CHundsunRspInfoField *pRspInfo = NULL) = 0;

    ///330854 组合行情查询请求
    virtual int ReqFuncArgquoteQry(CHundsunReqFuncArgquoteQryField *pReqFuncArgquoteQry, int nRequestID = 0, void** ppRetData = NULL, int* pnItems = NULL, CHundsunRspInfoField *pRspInfo = NULL) = 0;

    ///338204 组合委托确认请求
    virtual int ReqFuncArgorderinsert(CHundsunReqFuncArgorderinsertField *pReqFuncArgorderinsert, int nRequestID = 0, void** ppRetData = NULL, int* pnItems = NULL, CHundsunRspInfoField *pRspInfo = NULL) = 0;

    ///338224 期货中金套利组合确认请求
    virtual int ReqFuncFutuCombmarginactionInsert(CHundsunReqFuncFutuCombmarginactionInsertField *pReqFuncFutuCombmarginactionInsert, int nRequestID = 0, void** ppRetData = NULL, int* pnItems = NULL, CHundsunRspInfoField *pRspInfo = NULL) = 0;

    ///338304 UFT单客户查询费用属性请求
    virtual int ReqFuncInstrumentcommissionrateQry(CHundsunReqFuncInstrumentcommissionrateQryField *pReqFuncInstrumentcommissionrateQry, int nRequestID = 0, void** ppRetData = NULL, int* pnItems = NULL, CHundsunRspInfoField *pRspInfo = NULL) = 0;

    ///338305 UFT保证金属性查询请求
    virtual int ReqFuncInstrumentmarginrateQry(CHundsunReqFuncInstrumentmarginrateQryField *pReqFuncInstrumentmarginrateQry, int nRequestID = 0, void** ppRetData = NULL, int* pnItems = NULL, CHundsunRspInfoField *pRspInfo = NULL) = 0;

    ///338313 交易所状态查询请求
    virtual int ReqFuncExchangestatusQry(CHundsunReqFuncExchangestatusQryField *pReqFuncExchangestatusQry, int nRequestID = 0, void** ppRetData = NULL, int* pnItems = NULL, CHundsunRspInfoField *pRspInfo = NULL) = 0;

    ///338314 交易所查询请求
    virtual int ReqFuncExchangeQry(CHundsunReqFuncExchangeQryField *pReqFuncExchangeQry, int nRequestID = 0, void** ppRetData = NULL, int* pnItems = NULL, CHundsunRspInfoField *pRspInfo = NULL) = 0;

    ///338323 期货中金组合持仓查询请求
    virtual int ReqFuncFutuComholeinfoQry(CHundsunReqFuncFutuComholeinfoQryField *pReqFuncFutuComholeinfoQry, int nRequestID = 0, void** ppRetData = NULL, int* pnItems = NULL, CHundsunRspInfoField *pRspInfo = NULL) = 0;

    ///338324 期货中金组合委托查询请求
    virtual int ReqFuncFutuComentrustinfoQry(CHundsunReqFuncFutuComentrustinfoQryField *pReqFuncFutuComentrustinfoQry, int nRequestID = 0, void** ppRetData = NULL, int* pnItems = NULL, CHundsunRspInfoField *pRspInfo = NULL) = 0;

    ///330850 期货合约行情查询请求
    virtual int ReqFuncFutuContracthqQry(CHundsunReqFuncFutuContracthqQryField *pReqFuncFutuContracthqQry, int nRequestID = 0, void** ppRetData = NULL, int* pnItems = NULL, CHundsunRspInfoField *pRspInfo = NULL) = 0;

    ///330851 期货合约保证金查询请求
    virtual int ReqFuncFutuContractbailQry(CHundsunReqFuncFutuContractbailQryField *pReqFuncFutuContractbailQry, int nRequestID = 0, void** ppRetData = NULL, int* pnItems = NULL, CHundsunRspInfoField *pRspInfo = NULL) = 0;

    ///330852 取交易所时间差请求
    virtual int ReqFuncExchangetimeQry(CHundsunReqFuncExchangetimeQryField *pReqFuncExchangetimeQry, int nRequestID = 0, void** ppRetData = NULL, int* pnItems = NULL, CHundsunRspInfoField *pRspInfo = NULL) = 0;

    ///330855 监控中心查询密钥获取请求
    virtual int ReqFuncCfmmctradingaccountkeyQry(CHundsunReqFuncCfmmctradingaccountkeyQryField *pReqFuncCfmmctradingaccountkeyQry, int nRequestID = 0, void** ppRetData = NULL, int* pnItems = NULL, CHundsunRspInfoField *pRspInfo = NULL) = 0;

    ///331124 密码校验请求
    virtual int ReqFundFutuChkpassword(CHundsunReqFundFutuChkpasswordField *pReqFundFutuChkpassword, int nRequestID = 0, void** ppRetData = NULL, int* pnItems = NULL, CHundsunRspInfoField *pRspInfo = NULL) = 0;

    ///331850 期货交易账号查询请求
    virtual int ReqFuncTradingcodeQry(CHundsunReqFuncTradingcodeQryField *pReqFuncTradingcodeQry, int nRequestID = 0, void** ppRetData = NULL, int* pnItems = NULL, CHundsunRspInfoField *pRspInfo = NULL) = 0;

    ///331851 期货客户账单确认请求
    virtual int ReqFuncSettlementInfoConfirm(CHundsunReqFuncSettlementInfoConfirmField *pReqFuncSettlementInfoConfirm, int nRequestID = 0, void** ppRetData = NULL, int* pnItems = NULL, CHundsunRspInfoField *pRspInfo = NULL) = 0;

    ///332213 取期货登记银行信息请求
    virtual int ReqFundFutuBankinfoQry(CHundsunReqFundFutuBankinfoQryField *pReqFundFutuBankinfoQry, int nRequestID = 0, void** ppRetData = NULL, int* pnItems = NULL, CHundsunRspInfoField *pRspInfo = NULL) = 0;

    ///338200 检查期货合约代码请求
    virtual int ReqFuncFutuCheckFutuCode(CHundsunReqFuncFutuCheckFutuCodeField *pReqFuncFutuCheckFutuCode, int nRequestID = 0, void** ppRetData = NULL, int* pnItems = NULL, CHundsunRspInfoField *pRspInfo = NULL) = 0;

    ///338201 期货可买卖数量获取请求
    virtual int ReqFuncMaxordervolume(CHundsunReqFuncMaxordervolumeField *pReqFuncMaxordervolume, int nRequestID = 0, void** ppRetData = NULL, int* pnItems = NULL, CHundsunRspInfoField *pRspInfo = NULL) = 0;

    ///338202 期货委托确认请求
    virtual int ReqFuncOrderinsert(CHundsunReqFuncOrderinsertField *pReqFuncOrderinsert, int nRequestID = 0, void** ppRetData = NULL, int* pnItems = NULL, CHundsunRspInfoField *pRspInfo = NULL) = 0;

    ///338203 检查组合合约代码请求
    virtual int ReqFuncFutuCheckCombineFutuCode(CHundsunReqFuncFutuCheckCombineFutuCodeField *pReqFuncFutuCheckCombineFutuCode, int nRequestID = 0, void** ppRetData = NULL, int* pnItems = NULL, CHundsunRspInfoField *pRspInfo = NULL) = 0;

    ///338217 期货委托撤单请求
    virtual int ReqFuncOrderaction(CHundsunReqFuncOrderactionField *pReqFuncOrderaction, int nRequestID = 0, void** ppRetData = NULL, int* pnItems = NULL, CHundsunRspInfoField *pRspInfo = NULL) = 0;

    ///338300 期货客户资金查询请求
    virtual int ReqFuncTradingaccountQry(CHundsunReqFuncTradingaccountQryField *pReqFuncTradingaccountQry, int nRequestID = 0, void** ppRetData = NULL, int* pnItems = NULL, CHundsunRspInfoField *pRspInfo = NULL) = 0;

    ///338301 期货当日委托查询请求
    virtual int ReqFuncOrderQry(CHundsunReqFuncOrderQryField *pReqFuncOrderQry, int nRequestID = 0, void** ppRetData = NULL, int* pnItems = NULL, CHundsunRspInfoField *pRspInfo = NULL) = 0;

    ///338302 期货当日成交查询请求
    virtual int ReqFuncTradeQry(CHundsunReqFuncTradeQryField *pReqFuncTradeQry, int nRequestID = 0, void** ppRetData = NULL, int* pnItems = NULL, CHundsunRspInfoField *pRspInfo = NULL) = 0;

    ///338303 期货持仓查询请求
    virtual int ReqFuncInvestorpositionQry(CHundsunReqFuncInvestorpositionQryField *pReqFuncInvestorpositionQry, int nRequestID = 0, void** ppRetData = NULL, int* pnItems = NULL, CHundsunRspInfoField *pRspInfo = NULL) = 0;

    ///338306 当日资金流水查询请求
    virtual int ReqFuncFutuFundjourQry(CHundsunReqFuncFutuFundjourQryField *pReqFuncFutuFundjourQry, int nRequestID = 0, void** ppRetData = NULL, int* pnItems = NULL, CHundsunRspInfoField *pRspInfo = NULL) = 0;

    ///338307 持仓明细查询请求
    virtual int ReqFuncInvestorpositiondetailQry(CHundsunReqFuncInvestorpositiondetailQryField *pReqFuncInvestorpositiondetailQry, int nRequestID = 0, void** ppRetData = NULL, int* pnItems = NULL, CHundsunRspInfoField *pRspInfo = NULL) = 0;

    ///338308 取系统信息请求
    virtual int ReqFuncFutuSysinfoQry(CHundsunReqFuncFutuSysinfoQryField *pReqFuncFutuSysinfoQry, int nRequestID = 0, void** ppRetData = NULL, int* pnItems = NULL, CHundsunRspInfoField *pRspInfo = NULL) = 0;

    ///339851 取客户账单请求
    virtual int ReqFuncSettlementinfoQry(CHundsunReqFuncSettlementinfoQryField *pReqFuncSettlementinfoQry, int nRequestID = 0, void** ppRetData = NULL, int* pnItems = NULL, CHundsunRspInfoField *pRspInfo = NULL) = 0;

    ///339852 期货历史委托查询请求
    virtual int ReqFuncHisorderQry(CHundsunReqFuncHisorderQryField *pReqFuncHisorderQry, int nRequestID = 0, void** ppRetData = NULL, int* pnItems = NULL, CHundsunRspInfoField *pRspInfo = NULL) = 0;

    ///339853 期货历史成交查询请求
    virtual int ReqFuncHistradeQry(CHundsunReqFuncHistradeQryField *pReqFuncHistradeQry, int nRequestID = 0, void** ppRetData = NULL, int* pnItems = NULL, CHundsunRspInfoField *pRspInfo = NULL) = 0;

    ///339854 取客户历史资金请求
    virtual int ReqFuncHistradingaccountQry(CHundsunReqFuncHistradingaccountQryField *pReqFuncHistradingaccountQry, int nRequestID = 0, void** ppRetData = NULL, int* pnItems = NULL, CHundsunRspInfoField *pRspInfo = NULL) = 0;

    ///338000 期权代码信息查询请求
    virtual int ReqFuncOptOptcodeQry(CHundsunReqFuncOptOptcodeQryField *pReqFuncOptOptcodeQry, int nRequestID = 0, void** ppRetData = NULL, int* pnItems = NULL, CHundsunRspInfoField *pRspInfo = NULL) = 0;

    ///338001 期权标的信息查询请求
    virtual int ReqFuncOptOptobjectcodeQry(CHundsunReqFuncOptOptobjectcodeQryField *pReqFuncOptOptobjectcodeQry, int nRequestID = 0, void** ppRetData = NULL, int* pnItems = NULL, CHundsunRspInfoField *pRspInfo = NULL) = 0;

    ///338002 期权代码输入确认请求
    virtual int ReqFuncOptOptcodeEnter(CHundsunReqFuncOptOptcodeEnterField *pReqFuncOptOptcodeEnter, int nRequestID = 0, void** ppRetData = NULL, int* pnItems = NULL, CHundsunRspInfoField *pRspInfo = NULL) = 0;

    ///338010 期权可交易数量获取请求
    virtual int ReqFuncOptEnTrade(CHundsunReqFuncOptEnTradeField *pReqFuncOptEnTrade, int nRequestID = 0, void** ppRetData = NULL, int* pnItems = NULL, CHundsunRspInfoField *pRspInfo = NULL) = 0;

    ///338011 期权委托请求
    virtual int ReqFuncOptEntrust(CHundsunReqFuncOptEntrustField *pReqFuncOptEntrust, int nRequestID = 0, void** ppRetData = NULL, int* pnItems = NULL, CHundsunRspInfoField *pRspInfo = NULL) = 0;

    ///338012 期权撤单请求
    virtual int ReqFuncOptWithdraw(CHundsunReqFuncOptWithdrawField *pReqFuncOptWithdraw, int nRequestID = 0, void** ppRetData = NULL, int* pnItems = NULL, CHundsunRspInfoField *pRspInfo = NULL) = 0;

    ///338020 期权委托查询请求
    virtual int ReqFuncOptEntrustQry(CHundsunReqFuncOptEntrustQryField *pReqFuncOptEntrustQry, int nRequestID = 0, void** ppRetData = NULL, int* pnItems = NULL, CHundsunRspInfoField *pRspInfo = NULL) = 0;

    ///338021 期权成交查询请求
    virtual int ReqFuncOptRealtimeQry(CHundsunReqFuncOptRealtimeQryField *pReqFuncOptRealtimeQry, int nRequestID = 0, void** ppRetData = NULL, int* pnItems = NULL, CHundsunRspInfoField *pRspInfo = NULL) = 0;

    ///338022 期权资产查询请求
    virtual int ReqFuncOptAssetQry(CHundsunReqFuncOptAssetQryField *pReqFuncOptAssetQry, int nRequestID = 0, void** ppRetData = NULL, int* pnItems = NULL, CHundsunRspInfoField *pRspInfo = NULL) = 0;

    ///338023 期权持仓查询请求
    virtual int ReqFuncOptHoldQry(CHundsunReqFuncOptHoldQryField *pReqFuncOptHoldQry, int nRequestID = 0, void** ppRetData = NULL, int* pnItems = NULL, CHundsunRspInfoField *pRspInfo = NULL) = 0;

    ///338024 期权行权指派查询请求
    virtual int ReqFuncOptExeassignQry(CHundsunReqFuncOptExeassignQryField *pReqFuncOptExeassignQry, int nRequestID = 0, void** ppRetData = NULL, int* pnItems = NULL, CHundsunRspInfoField *pRspInfo = NULL) = 0;

    ///338025 期权行权交割信息查询请求
    virtual int ReqFuncAssetOptexedeliverQry(CHundsunReqFuncAssetOptexedeliverQryField *pReqFuncAssetOptexedeliverQry, int nRequestID = 0, void** ppRetData = NULL, int* pnItems = NULL, CHundsunRspInfoField *pRspInfo = NULL) = 0;

    ///338026 期权行权指派欠资欠券查询请求
    virtual int ReqFuncAssetDebtinfoQry(CHundsunReqFuncAssetDebtinfoQryField *pReqFuncAssetDebtinfoQry, int nRequestID = 0, void** ppRetData = NULL, int* pnItems = NULL, CHundsunRspInfoField *pRspInfo = NULL) = 0;

    ///338027 期权标的证券数量提示请求
    virtual int ReqFuncOptUnderlyamountPrompt(CHundsunReqFuncOptUnderlyamountPromptField *pReqFuncOptUnderlyamountPrompt, int nRequestID = 0, void** ppRetData = NULL, int* pnItems = NULL, CHundsunRspInfoField *pRspInfo = NULL) = 0;

    ///338028 客户期权持仓合约信息提示请求
    virtual int ReqFuncOptOptholdrealPrompt(CHundsunReqFuncOptOptholdrealPromptField *pReqFuncOptOptholdrealPrompt, int nRequestID = 0, void** ppRetData = NULL, int* pnItems = NULL, CHundsunRspInfoField *pRspInfo = NULL) = 0;

    ///338029 客户备兑证券不足查询请求
    virtual int ReqFuncAssetAssetcoverstockQry(CHundsunReqFuncAssetAssetcoverstockQryField *pReqFuncAssetAssetcoverstockQry, int nRequestID = 0, void** ppRetData = NULL, int* pnItems = NULL, CHundsunRspInfoField *pRspInfo = NULL) = 0;

    ///338030 备兑证券可划转数量获取请求
    virtual int ReqFuncOptCoveredQry(CHundsunReqFuncOptCoveredQryField *pReqFuncOptCoveredQry, int nRequestID = 0, void** ppRetData = NULL, int* pnItems = NULL, CHundsunRspInfoField *pRspInfo = NULL) = 0;

    ///338031 备兑证券划转请求
    virtual int ReqFuncOptCoveredTrans(CHundsunReqFuncOptCoveredTransField *pReqFuncOptCoveredTrans, int nRequestID = 0, void** ppRetData = NULL, int* pnItems = NULL, CHundsunRspInfoField *pRspInfo = NULL) = 0;

    ///339800 历史期权委托查询请求
    virtual int ReqFuncHisOptentrustQry(CHundsunReqFuncHisOptentrustQryField *pReqFuncHisOptentrustQry, int nRequestID = 0, void** ppRetData = NULL, int* pnItems = NULL, CHundsunRspInfoField *pRspInfo = NULL) = 0;

    ///339801 历史期权成交查询请求
    virtual int ReqFuncHisOptbusinessQry(CHundsunReqFuncHisOptbusinessQryField *pReqFuncHisOptbusinessQry, int nRequestID = 0, void** ppRetData = NULL, int* pnItems = NULL, CHundsunRspInfoField *pRspInfo = NULL) = 0;

    ///339803 历史期权交割信息查询请求
    virtual int ReqFuncHisOptdeliverQry(CHundsunReqFuncHisOptdeliverQryField *pReqFuncHisOptdeliverQry, int nRequestID = 0, void** ppRetData = NULL, int* pnItems = NULL, CHundsunRspInfoField *pRspInfo = NULL) = 0;

    ///339804 历史期权行权指派查询请求
    virtual int ReqFuncHisExeassignQry(CHundsunReqFuncHisExeassignQryField *pReqFuncHisExeassignQry, int nRequestID = 0, void** ppRetData = NULL, int* pnItems = NULL, CHundsunRspInfoField *pRspInfo = NULL) = 0;

    ///339805 历史期权行权交割信息查询请求
    virtual int ReqFuncHisOptexedeliverQry(CHundsunReqFuncHisOptexedeliverQryField *pReqFuncHisOptexedeliverQry, int nRequestID = 0, void** ppRetData = NULL, int* pnItems = NULL, CHundsunRspInfoField *pRspInfo = NULL) = 0;

    ///339806 历史期权对账单查询请求
    virtual int ReqFuncHisOptstatementQry(CHundsunReqFuncHisOptstatementQryField *pReqFuncHisOptstatementQry, int nRequestID = 0, void** ppRetData = NULL, int* pnItems = NULL, CHundsunRspInfoField *pRspInfo = NULL) = 0;

    ///339807 取客户账单请求
    virtual int ReqFuncOptSettlementinfoQry(CHundsunReqFuncOptSettlementinfoQryField *pReqFuncOptSettlementinfoQry, int nRequestID = 0, void** ppRetData = NULL, int* pnItems = NULL, CHundsunRspInfoField *pRspInfo = NULL) = 0;

    ///395 期权行情查询请求
    virtual int ReqFuncOptPriceQry(CHundsunReqFuncOptPriceQryField *pReqFuncOptPriceQry, int nRequestID = 0, void** ppRetData = NULL, int* pnItems = NULL, CHundsunRspInfoField *pRspInfo = NULL) = 0;

    ///620001_33101 订阅-期货委托成交回报请求
    virtual int ReqFuncQhEntrustDealSubscribe(int nRequestID = 0) = 0;

    ///620002_33101 订阅取消-期货委托成交回报请求
    virtual int ReqFuncQhEntrustDealCancel(int nRequestID = 0) = 0;

    ///620001_33105 订阅-期货交易所状态信息请求
    virtual int ReqFuncQhExchStatusSubscribe(int nRequestID = 0) = 0;

    ///620002_33105 订阅取消-期货交易所状态信息请求
    virtual int ReqFuncQhExchStatusCancel(int nRequestID = 0) = 0;

protected:
    ~CHundsunTraderApi(){};
};

#endif
