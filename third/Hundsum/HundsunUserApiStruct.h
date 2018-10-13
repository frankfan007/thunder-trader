#if !defined(HUNDSUNUSERAPISTRUCT_H)
#define HUNDSUNUSERAPISTRUCT_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define PackerVersion 0x20
#define PANICHEARTBEATTIME (20*1000)

#define FuncSvrbankHisBktransferQry     339204  ///历史转账流水查询
#define FuncClientEntrustwayQry         331153  ///客户委托方式查询
#define FuncBankaccQry                  332296  ///银行账户查询
#define FuncBankTransfer                332200  ///银行转账
#define FuncBanktransferQry             332250  ///存管资金账户转账日志查询
#define FuncFundamtQry                  332253  ///银行余额查询
#define FuncUfxbankTransfer             332297  ///银行转账
#define FuncUfxbankBkbalanceQry         332298  ///银行余额查询
#define FuncClientBankacctQry           331157  ///客户银行账户查询
#define FuncClientFundFastQry           332254  ///客户资金快速查询
#define FuncClientAllQry                331150  ///客户信息查询
#define FuncClientLogin                 331100  ///客户登录
#define FuncClientPwdMod                331101  ///客户密码更改
#define FuncForquoteinsert              338218  ///期权询价
#define FuncQuoteinsert                 338219  ///期权报价
#define FuncQuoteaction                 338220  ///报价撤单
#define FuncQuoteQry                    338310  ///查询客户报价信息
#define FuncForquoteQry                 338311  ///查询询价信息
#define FuncOptioninstrtradecostQry     338312  ///查询期权交易成本
#define FuncExecorderinsert             338205  ///期权行权申请
#define FuncExecorderaction             338206  ///期权行权撤销申请
#define FuncExpireinvestorpositionQry   338309  ///到期执行持仓查询
#define FuncArginstrumentQry            330853  ///组合代码查询
#define FuncArgquoteQry                 330854  ///组合行情查询
#define FuncArgorderinsert              338204  ///组合委托确认
#define FuncFutuCombmarginactionInsert  338224  ///期货中金套利组合确认
#define FuncInstrumentcommissionrateQry 338304  ///UFT单客户查询费用属性
#define FuncInstrumentmarginrateQry     338305  ///UFT保证金属性查询
#define FuncExchangestatusQry           338313  ///交易所状态查询
#define FuncExchangeQry                 338314  ///交易所查询
#define FuncFutuComholeinfoQry          338323  ///期货中金组合持仓查询
#define FuncFutuComentrustinfoQry       338324  ///期货中金组合委托查询
#define FuncFutuContracthqQry           330850  ///期货合约行情查询
#define FuncFutuContractbailQry         330851  ///期货合约保证金查询
#define FuncExchangetimeQry             330852  ///取交易所时间差
#define FuncCfmmctradingaccountkeyQry   330855  ///监控中心查询密钥获取
#define FundFutuChkpassword             331124  ///密码校验
#define FuncTradingcodeQry              331850  ///期货交易账号查询
#define FuncSettlementInfoConfirm       331851  ///期货客户账单确认
#define FundFutuBankinfoQry             332213  ///取期货登记银行信息
#define FuncFutuCheckFutuCode           338200  ///检查期货合约代码
#define FuncMaxordervolume              338201  ///期货可买卖数量获取
#define FuncOrderinsert                 338202  ///期货委托确认
#define FuncFutuCheckCombineFutuCode    338203  ///检查组合合约代码
#define FuncOrderaction                 338217  ///期货委托撤单
#define FuncTradingaccountQry           338300  ///期货客户资金查询
#define FuncOrderQry                    338301  ///期货当日委托查询
#define FuncTradeQry                    338302  ///期货当日成交查询
#define FuncInvestorpositionQry         338303  ///期货持仓查询
#define FuncFutuFundjourQry             338306  ///当日资金流水查询
#define FuncInvestorpositiondetailQry   338307  ///持仓明细查询
#define FuncFutuSysinfoQry              338308  ///取系统信息
#define FuncSettlementinfoQry           339851  ///取客户账单
#define FuncHisorderQry                 339852  ///期货历史委托查询
#define FuncHistradeQry                 339853  ///期货历史成交查询
#define FuncHistradingaccountQry        339854  ///取客户历史资金
#define FuncOptOptcodeQry               338000  ///期权代码信息查询
#define FuncOptOptobjectcodeQry         338001  ///期权标的信息查询
#define FuncOptOptcodeEnter             338002  ///期权代码输入确认
#define FuncOptEnTrade                  338010  ///期权可交易数量获取
#define FuncOptEntrust                  338011  ///期权委托
#define FuncOptWithdraw                 338012  ///期权撤单
#define FuncOptEntrustQry               338020  ///期权委托查询
#define FuncOptRealtimeQry              338021  ///期权成交查询
#define FuncOptAssetQry                 338022  ///期权资产查询
#define FuncOptHoldQry                  338023  ///期权持仓查询
#define FuncOptExeassignQry             338024  ///期权行权指派查询
#define FuncAssetOptexedeliverQry       338025  ///期权行权交割信息查询
#define FuncAssetDebtinfoQry            338026  ///期权行权指派欠资欠券查询
#define FuncOptUnderlyamountPrompt      338027  ///期权标的证券数量提示
#define FuncOptOptholdrealPrompt        338028  ///客户期权持仓合约信息提示
#define FuncAssetAssetcoverstockQry     338029  ///客户备兑证券不足查询
#define FuncOptCoveredQry               338030  ///备兑证券可划转数量获取
#define FuncOptCoveredTrans             338031  ///备兑证券划转
#define FuncHisOptentrustQry            339800  ///历史期权委托查询
#define FuncHisOptbusinessQry           339801  ///历史期权成交查询
#define FuncHisOptdeliverQry            339803  ///历史期权交割信息查询
#define FuncHisExeassignQry             339804  ///历史期权行权指派查询
#define FuncHisOptexedeliverQry         339805  ///历史期权行权交割信息查询
#define FuncHisOptstatementQry          339806  ///历史期权对账单查询
#define FuncOptSettlementinfoQry        339807  ///取客户账单
#define FuncOptPriceQry                 395  ///期权行情查询

//错误信息
struct CHundsunRspInfoField
{
    int       ErrorNo;                  //错误代码
    char      ErrorInfo[255];           //错误提示
};

//用户信息
struct CHundsunUserInfoField
{
    int       OpBranchNo;               //操作分支机构
    char      OpEntrustWay;             //委托方式
    char      OpStation[255];           //站点地址
    int       BranchNo;                 //分支机构
    char      ClientId[18];             //客户编号
    char      Password[50];             //密码
    char      PasswordType;             //密码类别
    char      UserToken[512];           //用户口令
    char      FundAccount[18];          //资产账户
};

//339204 历史转账流水查询请求
struct CHundsunReqFuncSvrbankHisBktransferQryField
{
    int       StartDate;                //开始日期
    int       EndDate;                  //到期日期
    char      BankNo[4];                //银行代码
    int       ActionIn;                 //操作控制值
    char      PositionStr[100];         //定位串
    int       RequestNum;               //请求行数
};

//339204 历史转账流水查询应答
struct CHundsunRspFuncSvrbankHisBktransferQryField
{
    int       BranchNo;                 //分支机构
    int       InitDate;                 //交易日期
    int       CurrDate;                 //当前日期
    int       EntrustTime;              //委托时间
    char      BankNo[4];                //银行代码
    char      BankName[64];             //银行名称
    int       EntrustNo;                //委托编号
    char      BusinessType;             //业务类型
    char      SourceFlag;               //发起方
    char      MoneyType[3];             //币种类别
    double    OccurBalance;             //发生金额
    char      EntrustStatus;            //委托状态
    int       ErrorNo;                  //错误代码
    char      BankErrorInfo[2000];      //银行错误信息
    char      PositionStr[100];         //定位串
};

//331153 客户委托方式查询请求
struct CHundsunReqFuncClientEntrustwayQryField
{
};

//331153 客户委托方式查询应答
struct CHundsunRspFuncClientEntrustwayQryField
{
    int       BranchNo;                 //分支机构
    char      EnEntrustWay[64];         //允许委托方式
    char      EnEntrustWayName[255];    //允许委托方式名称
};

//332296 银行账户查询请求
struct CHundsunReqFuncBankaccQryField
{
};

//332296 银行账户查询应答
struct CHundsunRspFuncBankaccQryField
{
    char      MoneyType[3];             //币种类别
    char      BankNo[4];                //银行代码
    char      BankName[64];             //银行名称
    char      BankAccount[32];          //银行账号
};

//332200 银行转账请求
struct CHundsunReqFuncBankTransferField
{
    char      MoneyType[3];             //币种类别
    char      BankNo[4];                //银行代码
    char      TransferDirection;        //交易方向
    double    OccurBalance;             //发生金额
    char      FundPassword[15];         //资金密码
    char      BankPassword[15];         //银行密码
};

//332200 银行转账应答
struct CHundsunRspFuncBankTransferField
{
    int       EntrustNo;                //委托编号
};

//332250 存管资金账户转账日志查询请求
struct CHundsunReqFuncBanktransferQryField
{
    char      BankNo[4];                //银行代码
    int       EntrustNo;                //委托编号
    int       ActionIn;                 //操作控制值
    char      PositionStr[100];         //定位串
    int       RequestNum;               //请求行数
};

//332250 存管资金账户转账日志查询应答
struct CHundsunRspFuncBanktransferQryField
{
    int       BranchNo;                 //分支机构
    char      FundAccount[18];          //资产账户
    char      BankNo[4];                //银行代码
    char      BankName[64];             //银行名称
    char      TransName[64];            //转换机名字
    int       EntrustNo;                //委托编号
    char      SourceFlag;               //发起方
    char      MoneyType[3];             //币种类别
    double    OccurBalance;             //发生金额
    double    ClearBalance;             //清算金额
    int       EntrustTime;              //委托时间
    char      EntrustStatus;            //委托状态
    int       ErrorNo;                  //错误代码
    char      CancelInfo[20];           //废单原因
    char      BankErrorInfo[2000];      //银行错误信息
    char      Remark[2000];             //备注
    char      AssetProp;                //资产属性
    char      PositionStr[100];         //定位串
};

//332253 银行余额查询请求
struct CHundsunReqFuncFundamtQryField
{
    char      FundPassword[15];         //资金密码
    char      BankPassword[15];         //银行密码
    char      BankNo[4];                //银行代码
    char      MoneyType[3];             //币种类别
};

//332253 银行余额查询应答
struct CHundsunRspFuncFundamtQryField
{
    int       SerialNo;                 //流水序号
};

//332297 银行转账请求
struct CHundsunReqFuncUfxbankTransferField
{
    char      BankNo[4];                //银行代码
    char      MoneyType[3];             //币种类别
    char      TransferDirection;        //交易方向
    double    OccurBalance;             //发生金额
    char      FundPassword[15];         //资金密码
    char      BankPassword[15];         //银行密码
};

//332297 银行转账应答
struct CHundsunRspFuncUfxbankTransferField
{
    int       ErrorNo;                  //错误代码
    char      ErrorInfo[20];            //错误提示
};

//332298 银行余额查询请求
struct CHundsunReqFuncUfxbankBkbalanceQryField
{
    char      MoneyType[3];             //币种类别
    char      BankNo[4];                //银行代码
    char      FundPassword[15];         //资金密码
    char      BankPassword[15];         //银行密码
};

//332298 银行余额查询应答
struct CHundsunRspFuncUfxbankBkbalanceQryField
{
    double    OccurBalance;             //发生金额
};

//331157 客户银行账户查询请求
struct CHundsunReqFuncClientBankacctQryField
{
    char      MoneyType[3];             //币种类别
    char      BankNo[4];                //银行代码
};

//331157 客户银行账户查询应答
struct CHundsunRspFuncClientBankacctQryField
{
    int       OpenDate;                 //开户日期
    char      FundAccount[18];          //资产账户
    char      MoneyType[3];             //币种类别
    char      BankNo[4];                //银行代码
    char      BankName[64];             //银行名称
    char      BankAccount[32];          //银行账号
    char      BkaccountStatus;          //银行账户状态
};

//332254 客户资金快速查询请求
struct CHundsunReqFuncClientFundFastQryField
{
    char      MoneyType[3];             //币种类别
};

//332254 客户资金快速查询应答
struct CHundsunRspFuncClientFundFastQryField
{
    char      MoneyType[3];             //币种类别
    double    CurrentBalance;           //当前余额
    double    EnableBalance;            //可用资金
    double    FetchBalance;             //可取金额
    double    FrozenBalance;            //冻结资金
    double    UnfrozenBalance;          //解冻资金
    double    FundBalance;              //总资金余额
};

//331150 客户信息查询请求
struct CHundsunReqFuncClientAllQryField
{
    char      QueryMode;                //查询模式
};

//331150 客户信息查询应答
struct CHundsunRspFuncClientAllQryField
{
    int       BranchNo;                 //分支机构
    char      ClientName[14];           //客户姓名
    char      ClientStatus;             //客户状态
    char      FundCard[32];             //资金卡号
    char      IdKind;                   //证件类别
    char      IdNo[32];                 //证件号码
    char      IdAddress[120];           //身份证地址
    char      Mobiletelephone[32];      //手机号码
    char      Fax[32];                  //传真号码
    char      Zipcode[6];               //邮政编码
    char      EMail[64];                //电子信箱
    int       OpenDate;                 //开户日期
    char      Nationality[3];           //国籍地区
    char      Address[120];             //联系地址
    char      MailName[64];             //联系人
    char      RiskInfo[64];             //风险要素信息
    char      AccountData[64];          //开户规范信息
    char      OrganProp;                //机构标志
    char      OrganName[60];            //机构名称
    int       ClientGroup;              //客户分类
    char      GroupName[12];            //组别名称
    char      FullName[64];             //账户全称
    char      RiskName[64];             //风险要素信息名称
    char      AccountDataName[64];      //开户规范信息名称
    char      Phonecode[20];            //联系电话
    char      ClientId[18];             //客户编号
    char      FundAccount[18];          //资产账户
    char      ClientRights[64];         //客户权限
    int       CorpClientGroup;          //公司客户类型
    int       CorpRiskLevel;            //客户风险等级
    int       CorpBeginDate;            //客户风险测评日
    int       CorpEndDate;              //客户风险到期日
    int       AmlRiskLevel;             //反洗钱风险等级
    int       PaperScore;               //试卷得分
    char      ClientGender;             //客户性别
    char      InvestAdvice[2000];       //投资建议
    int       IdBegindate;              //证件开始日期
    int       IdEnddate;                //证件有效截止日期
    char      ProfitFlag;               //盈亏计算方式
    char      EnContactType[16];        //允许联络方式
    char      ControlPerson[60];        //操作控制人
    char      SecRelationName[60];      //第二联系人姓名
    char      SecRelationPhone[18];     //第二联系人联系电话
    char      SocialralType;            //社会关系类型
};

//331100 客户登录请求
struct CHundsunReqFuncClientLoginField
{
    int       OpBranchNo;               //操作分支机构
    char      OpEntrustWay;             //委托方式
    char      OpStation[255];           //站点地址
    int       BranchNo;                 //分支机构
    char      Password[50];             //密码
    char      PasswordType;             //密码类别
    char      InputContent;             //客户标志类别
    char      AccountContent[30];       //输入内容
    char      ContentType[6];           //银行号、市场类别
    char      AssetProp;                //资产属性
};

//331100 客户登录应答
struct CHundsunRspFuncClientLoginField
{
    int       InitDate;                 //交易日期
    char      SysStatus;                //系统状态
    char      CompanyName[64];          //公司名称
    char      ContentType[6];           //银行号、市场类别
    char      AccountContent[30];       //输入内容
    int       BranchNo;                 //分支机构
    char      ClientId[18];             //客户编号
    char      ClientName[14];           //客户姓名
    int       CorpClientGroup;          //公司客户类型
    int       CorpRiskLevel;            //客户风险等级
    int       CorpBeginDate;            //客户风险测评日
    int       CorpEndDate;              //客户风险到期日
    char      ValidFlag;                //有效标志
    int       FundaccountCount;         //资产账号个数
    char      FundAccount[18];          //资产账户
    char      ClientRights[64];         //客户权限
    char      LastOpEntrustWay;         //上次登陆委托方式
    char      LastOpStation[255];       //上次登陆站点/电话
    int       SysnodeId;                //系统节点编号
    char      UserToken[512];           //用户口令
    int       ErrorNo;                  //错误代码
    char      ErrorInfo[20];            //错误提示
    char      AssetProp;                //资产属性
    char      ProductFlag;              //产品标志
    char      MessageFlag;              //消息标志
    char      TabconfirmFlag;           //强制账单确认
    int       LastDate;                 //上次提成日期
    int       SessionNo;                //会话编号
};

//331101 客户密码更改请求
struct CHundsunReqFuncClientPwdModField
{
    char      NewPassword[50];          //新密码
};

//331101 客户密码更改应答
struct CHundsunRspFuncClientPwdModField
{
    int       ErrorNo;                  //错误代码
    char      ErrorInfo[20];            //错误提示
};

//338218 期权询价请求
struct CHundsunReqFuncForquoteinsertField
{
    char      FutuExchType[4];          //交易类别
    char      FuturesAccount[12];       //交易编码
    char      FutuCode[30];             //合约代码
    char      ForquoteReference[32];    //询价委托引用
};

//338218 期权询价应答
struct CHundsunRspFuncForquoteinsertField
{
    int       EntrustNo;                //委托编号
    char      ForquoteReference[32];    //询价委托引用
    int       SessionNo;                //会话编号
};

//338219 期权报价请求
struct CHundsunReqFuncQuoteinsertField
{
    char      FutuExchType[4];          //交易类别
    char      FuturesAccount[12];       //交易编码
    char      FutuCode[30];             //合约代码
    char      BuyFuturesDirection;      //买方开平方向
    char      BuyHedgeType;             //买方套保标志
    double    BuyQuotePrice;            //买方报价价格
    int       BuyQuoteAmount;           //买方报价数量
    char      SellFuturesDirection;     //卖方开平方向
    char      SellHedgeType;            //卖方套保标志
    double    SellQuotePrice;           //卖方报价价格
    int       SellQuoteAmount;          //卖方报价数量
    char      InquiryId[20];            //询价编号
    char      EntrustOccasion[32];      //委托场景
    char      EntrustReference[32];     //委托引用
};

//338219 期权报价应答
struct CHundsunRspFuncQuoteinsertField
{
    int       QuoteNo;                  //报价编号
    char      EntrustReference[32];     //委托引用
    int       SessionNo;                //会话编号
};

//338220 报价撤单请求
struct CHundsunReqFuncQuoteactionField
{
    int       QuoteNo;                  //报价编号
    char      ConfirmId[20];            //主场单号
    int       SessionNo;                //会话编号
    char      QuoteReference[32];       //报价委托引用
};

//338220 报价撤单应答
struct CHundsunRspFuncQuoteactionField
{
    int       QuoteNo;                  //报价编号
    char      QuoteReference[32];       //报价委托引用
    int       SessionNo;                //会话编号
};

//338310 查询客户报价信息请求
struct CHundsunReqFuncQuoteQryField
{
    char      QueryDirection;           //查询方向
    int       QuoteNo;                  //报价编号
    char      FuturesAccount[12];       //交易编码
    char      FutuExchType[4];          //交易类别
    char      FutuCode[30];             //合约代码
    char      EnQuoteStatus[10];        //报价状态
    int       RequestNum;               //请求行数
    char      PositionStr[100];         //定位串
};

//338310 查询客户报价信息应答
struct CHundsunRspFuncQuoteQryField
{
    char      PositionStr[100];         //定位串
    int       QuoteNo;                  //报价编号
    int       BranchNo;                 //分支机构
    char      FundAccount[18];          //资产账户
    char      FuturesAccount[12];       //交易编码
    char      FutuExchType[4];          //交易类别
    char      FutuCode[30];             //合约代码
    char      QuoteStatus;              //报价状态
    char      BuyFuturesDirection;      //买方开平方向
    char      BuyHedgeType;             //买方套保标志
    double    BuyQuotePrice;            //买方报价价格
    int       BuyQuoteAmount;           //买方报价数量
    char      SellFuturesDirection;     //卖方开平方向
    char      SellHedgeType;            //卖方套保标志
    double    SellQuotePrice;           //卖方报价价格
    int       SellQuoteAmount;          //卖方报价数量
    char      QuoteReference[32];       //报价委托引用
    char      InquiryId[20];            //询价编号
    char      ErrorMessage[255];        //提示信息
};

//338311 查询询价信息请求
struct CHundsunReqFuncForquoteQryField
{
    char      FutuExchType[4];          //交易类别
    char      FutuCode[30];             //合约代码
    int       RequestNum;               //请求行数
    char      PositionStr[100];         //定位串
};

//338311 查询询价信息应答
struct CHundsunRspFuncForquoteQryField
{
    char      PositionStr[100];         //定位串
    int       BranchNo;                 //分支机构
    char      FundAccount[18];          //资产账户
    char      FuturesAccount[12];       //交易编码
    char      FutuExchType[4];          //交易类别
    char      FutuCode[30];             //合约代码
    int       EntrustNo;                //委托编号
    char      ForquoteReference[32];    //询价委托引用
    char      EntrustStatus;            //委托状态
    int       EntrustTime;              //委托时间
};

//338312 查询期权交易成本请求
struct CHundsunReqFuncOptioninstrtradecostQryField
{
    char      FutuExchType[4];          //交易类别
    char      FutuCode[30];             //合约代码
    char      HedgeType;                //投机/套保类型
    double    FutuEntrustPrice;         //委托价格
};

//338312 查询期权交易成本应答
struct CHundsunRspFuncOptioninstrtradecostQryField
{
    double    HoldMargin;               //持仓保证金
    double    PreHoldMargin;            //昨仓保证金
    double    FoptBalance;              //权利金
};

//338205 期权行权申请请求
struct CHundsunReqFuncExecorderinsertField
{
    char      FuturesAccount[12];       //交易编码
    char      FutuCode[30];             //合约代码
    char      ApplyFlag;                //申请标志
    int       ApplyAmount;              //行权数量
    char      EntrustOccasion[32];      //委托场景
    char      EntrustReference[32];     //委托引用
    char      AutoDropFlag;             //自动平仓标志
    char      ApplyReserveFlag;         //行权保留期货头寸标志
    char      FutuExchType[4];          //交易类别
    char      HedgeType;                //投机/套保类型
};

//338205 期权行权申请应答
struct CHundsunRspFuncExecorderinsertField
{
    int       ExerciseNo;               //行权编号
    char      EntrustReference[32];     //委托引用
    int       SessionNo;                //会话编号
};

//338206 期权行权撤销申请请求
struct CHundsunReqFuncExecorderactionField
{
    int       ExerciseNo;               //行权编号
    char      EntrustOccasion[32];      //委托场景
    char      EntrustReference[32];     //委托引用
    char      FutuExchType[4];          //交易类别
    char      ConfirmId[20];            //主场单号
};

//338206 期权行权撤销申请应答
struct CHundsunRspFuncExecorderactionField
{
    int       ExerciseNo;               //行权编号
    char      EntrustReference[32];     //委托引用
    int       SessionNo;                //会话编号
};

//338309 到期执行持仓查询请求
struct CHundsunReqFuncExpireinvestorpositionQryField
{
    char      FutuExchType[4];          //交易类别
    char      FuturesAccount[12];       //交易编码
    char      FutuCode[30];             //合约代码
    double    UnderlyingPrice;          //标的指数价格
    char      GiveupType;               //放弃执行类型(1：不放弃2：最大对冲3：已申请放弃4：任意数量)
    int       GiveupAmount;             //申请放弃执行量
    int       RequestNum;               //请求行数
    char      PositionStr[100];         //定位串
};

//338309 到期执行持仓查询应答
struct CHundsunRspFuncExpireinvestorpositionQryField
{
    char      FundAccount[18];          //资产账户
    char      FutuExchType[4];          //交易类别
    char      FuturesAccount[12];       //交易编码
    char      FutuCode[30];             //合约代码
    double    UnderlyingPrice;          //标的指数价格
    char      GiveupType;               //放弃执行类型(1：不放弃2：最大对冲3：已申请放弃4：任意数量)
    double    ExercisePrice;            //行权价格
    char      FoptType;                 //期权合约类型
    char      ExecuteType;              //执行方式
    int       ExpireDate;               //到期日
    int       BuyUndropAmount;          //买未平仓量
    int       GiveupAmount;             //申请放弃执行量
    int       SellUndropAmount;         //卖未平仓量
    int       BuyExecableAmount;        //买可执行量
    int       BuyExecAmount;            //买执行量
    double    BuyExecIncome;            //买执行盈亏
    double    ExchBuyExecFare;          //买执行交易所手续费
    double    BuyExecFare;              //买执行客户手续费
    int       SellExecAmount;           //卖执行量
    int       SellExecIncome;           //卖执行盈亏
    int       ExchSellExecFare;         //卖执行交易所手续费
    int       SellExecFare;             //卖执行客户手续费
    double    DeliverPrice;             //交割结算价
    char      HedgeType;                //投机/套保类型
    char      PositionStr[100];         //定位串
};

//330853 组合代码查询请求
struct CHundsunReqFuncArginstrumentQryField
{
    char      FutuExchType[4];          //交易类别
};

//330853 组合代码查询应答
struct CHundsunRspFuncArginstrumentQryField
{
    char      ArbitCode[30];            //套利合约号
    char      FutuExchType[4];          //交易类别
};

//330854 组合行情查询请求
struct CHundsunReqFuncArgquoteQryField
{
    char      FutuExchType[4];          //交易类别
    char      ArbitCode[30];            //套利合约号
};

//330854 组合行情查询应答
struct CHundsunRspFuncArgquoteQryField
{
    char      ArbitCode[30];            //套利合约号
    char      FutuExchType[4];          //交易类别
    double    HighBuyPrice;             //最高买入价
    int       HighBuyAmount;            //最高买入价买量
    int       TotalBuyAmount;           //全部买量
    double    LowSellPrice;             //最低卖价价格
    int       LowSellAmount;            //最低卖出数量
    int       TotalSellAmount;          //总卖出数量
    double    FutuHighPrice;            //最高价
    double    FutuLowPrice;             //最低价
    double    UplimitedPrice;           //涨停板价格
    double    DownlimitedPrice;         //跌停板价格
};

//338204 组合委托确认请求
struct CHundsunReqFuncArgorderinsertField
{
    char      FuturesAccount[12];       //交易编码
    char      FutuExchType[4];          //交易类别
    char      FutuCode[30];             //合约代码
    char      EntrustBs;                //买卖方向
    char      FuturesDirection;         //开平仓方向
    char      HedgeType;                //投机/套保类型
    double    FutuEntrustPrice;         //委托价格
    int       EntrustAmount;            //委托数量
    double    SpringPrice;              //止损价格
    char      TimeCondition;            //有效期类型
    int       ValidDate;                //有效日期
    char      VolumeCondition;          //成交量类型
    char      WeaveType;                //组合类型
    char      EntrustProp[3];           //委托属性
    char      SecondCode[30];           //第二腿合约代码
    char      ArbitCode[30];            //套利合约号
    char      EntrustOccasion[32];      //委托场景
    char      EntrustReference[32];     //委托引用
    int       FexMinVolume;             //最小成交量
};

//338204 组合委托确认应答
struct CHundsunRspFuncArgorderinsertField
{
    int       EntrustNo;                //委托编号
    char      EntrustReference[32];     //委托引用
    int       SessionNo;                //会话编号
    char      FutuExchType[4];          //交易类别
    char      FutuCode[30];             //合约代码
    char      EntrustBs;                //买卖方向
    char      FuturesDirection;         //开平仓方向
    char      HedgeType;                //投机/套保类型
    int       EntrustAmount;            //委托数量
    double    FutuEntrustPrice;         //委托价格
    char      EntrustProp[3];           //委托属性
    char      EntrustStatus;            //委托状态
    int       EntrustTime;              //委托时间
    char      FuturesAccount[12];       //交易编码
    char      EntrustType;              //委托类别
    char      TimeCondition;            //有效期类型
    char      VolumeCondition;          //成交量类型
    char      WeaveType;                //组合类型
    char      SecondCode[30];           //第二腿合约代码
    char      ArbitCode[30];            //套利合约号
    char      EntrustOccasion[32];      //委托场景
};

//338224 期货中金套利组合确认请求
struct CHundsunReqFuncFutuCombmarginactionInsertField
{
    char      FuturesAccount[12];       //交易编码
    char      FutuExchType[4];          //交易类别
    char      EntrustBs;                //买卖方向
    char      HedgeType;                //投机/套保类型
    char      Combdirection;            //组合动作
    int       EntrustAmount;            //委托数量
    char      ArbitCode[30];            //套利合约号
    char      EntrustOccasion[32];      //委托场景
    char      EntrustReference[32];     //委托引用
};

//338224 期货中金套利组合确认应答
struct CHundsunRspFuncFutuCombmarginactionInsertField
{
    int       EntrustNo;                //委托编号
    char      EntrustStatus;            //委托状态
    char      EntrustReference[32];     //委托引用
    int       SessionNo;                //会话编号
};

//338304 UFT单客户查询费用属性请求
struct CHundsunReqFuncInstrumentcommissionrateQryField
{
    char      FutufareType;             //费用类别
    char      FutucodeType[4];          //合约代码类别
    char      FutuCode[30];             //合约代码
    char      EntrustBs;                //买卖方向
    int       RequestNum;               //请求行数
    char      PositionStr[100];         //定位串
};

//338304 UFT单客户查询费用属性应答
struct CHundsunRspFuncInstrumentcommissionrateQryField
{
    char      FutufareType;             //费用类别
    char      FutucodeType[4];          //合约代码类别
    char      FutuCode[30];             //合约代码
    char      EntrustBs;                //买卖方向
    double    OpenDropRatio;            //开平仓金额比例
    double    DropTodayRatio;           //平今成交金额比例
    double    OpenDropFare;             //单位数量开平仓手续费金额
    double    DropTodayFare;            //平今仓单位数量手续费金额
    char      PositionStr[100];         //定位串
};

//338305 UFT保证金属性查询请求
struct CHundsunReqFuncInstrumentmarginrateQryField
{
    char      HedgeType;                //投机/套保类型
    char      FutucodeType[4];          //合约代码类别
    char      FutuCode[30];             //合约代码
    char      EntrustBs;                //买卖方向
    int       RequestNum;               //请求行数
    char      PositionStr[100];         //定位串
};

//338305 UFT保证金属性查询应答
struct CHundsunRspFuncInstrumentmarginrateQryField
{
    char      HedgeType;                //投机/套保类型
    char      FutucodeType[4];          //合约代码类别
    char      FutuCode[30];             //合约代码
    char      EntrustBs;                //买卖方向
    double    OpenRatio;                //开仓成交金额比例
    double    OpenMargin;               //开仓单位数量保证金金额
    char      PositionStr[100];         //定位串
};

//338313 交易所状态查询请求
struct CHundsunReqFuncExchangestatusQryField
{
    char      FutuExchType[4];          //交易类别
};

//338313 交易所状态查询应答
struct CHundsunRspFuncExchangestatusQryField
{
    char      FutuExchType[4];          //交易类别
    char      ExchStatus;               //交易所状态
    char      ExchangeStatus;           //交易状态
};

//338314 交易所查询请求
struct CHundsunReqFuncExchangeQryField
{
    char      FutuExchType[4];          //交易类别
};

//338314 交易所查询应答
struct CHundsunRspFuncExchangeQryField
{
    char      FutuExchType[4];          //交易类别
    char      ExchangeName[16];         //交易名称
    char      MoneyType[3];             //币种类别
    int       InitDate;                 //交易日期
    char      EntrustKind;              //委托种类
    char      ExchangeStatus;           //交易状态
};

//338323 期货中金组合持仓查询请求
struct CHundsunReqFuncFutuComholeinfoQryField
{
    char      FutuExchType[4];          //交易类别
    char      EntrustBs;                //买卖方向
    char      QueryDirection;           //查询方向
    int       RequestNum;               //请求行数
    char      PositionStr[100];         //定位串
};

//338323 期货中金组合持仓查询应答
struct CHundsunRspFuncFutuComholeinfoQryField
{
    char      FutuExchType[4];          //交易类别
    char      PositionStr[100];         //定位串
    char      EntrustBs;                //买卖方向
    char      FutuCode[30];             //合约代码
    char      SecondCode[30];           //第二腿合约代码
    int       EnableAmount;             //可用数量
    double    HoldBalance;              //持仓金额
    double    FrozenBalance;            //冻结资金
};

//338324 期货中金组合委托查询请求
struct CHundsunReqFuncFutuComentrustinfoQryField
{
    char      FutuExchType[4];          //交易类别
    char      EntrustBs;                //买卖方向
    char      FuturesDirection;         //开平仓方向
    char      EnEntrustStatus[10];      //允许委托状态
    int       EntrustNo;                //委托编号
    int       BatchNo;                  //委托批号
    int       RequestNum;               //请求行数
    char      QueryDirection;           //查询方向
    char      PositionStr[100];         //定位串
};

//338324 期货中金组合委托查询应答
struct CHundsunRspFuncFutuComentrustinfoQryField
{
    int       InitDate;                 //交易日期
    int       EntrustNo;                //委托编号
    int       EntrustTime;              //委托时间
    int       CurrDate;                 //当前日期
    int       CurrTime;                 //当前时间
    char      FundAccount[18];          //资产账户
    char      FutuExchType[4];          //交易类别
    char      MoneyType[3];             //币种类别
    char      FuturesAccount[12];       //交易编码
    char      FutuCode[30];             //合约代码
    char      SecondCode[30];           //第二腿合约代码
    char      ArbitCode[30];            //套利合约号
    char      WeaveType;                //组合类型
    char      EntrustBs;                //买卖方向
    int       EntrustAmount;            //委托数量
    int       AmountPerHand;            //合约乘数
    char      FutuEntrustType;          //委托类别
    char      EntrustStatus;            //委托状态
    char      HedgeType;                //投机/套保类型
    double    HoldBalance;              //持仓金额
    char      ErrorMessage[255];        //提示信息
    double    FrozenBalance;            //冻结资金
    char      Combdirection;            //组合动作
    char      PositionStr[100];         //定位串
};

//330850 期货合约行情查询请求
struct CHundsunReqFuncFutuContracthqQryField
{
    char      FutuExchType[4];          //交易类别
    char      FutuCode[30];             //合约代码
    int       RequestNum;               //请求行数
    char      PositionStr[100];         //定位串
};

//330850 期货合约行情查询应答
struct CHundsunRspFuncFutuContracthqQryField
{
    char      PositionStr[100];         //定位串
    char      FutuExchType[4];          //交易类别
    char      FutuCode[30];             //合约代码
    char      FutuName[12];             //品种简称
    char      MoneyType[3];             //币种类别
    char      NativeCode[30];           //原始代码
    double    UplimitedPrice;           //涨停板价格
    double    DownlimitedPrice;         //跌停板价格
    double    PreSquarePrice;           //昨日结算价
    double    SquarePrice;              //结算价
    double    FutuOpenPrice;            //开盘价
    double    FutuLastPrice;            //最新价格
    double    HighBuyPrice;             //最高买入价
    int       HighBuyAmount;            //最高买入价买量
    int       TotalBuyAmount;           //全部买量
    double    LowSellPrice;             //最低卖价价格
    int       LowSellAmount;            //最低卖出数量
    int       TotalSellAmount;          //总卖出数量
    double    FutuHighPrice;            //最高价
    double    FutuLowPrice;             //最低价
    double    AveragePrice;             //平均价
    double    ChangeDirection;          //趋势
    int       BusinessAmount;           //成交数量
    int       BearAmount;               //空盘量
    double    BusinessBalance;          //成交金额
    double    ArbitBuyPrice;            //组合买入价格
    double    ArbitSellPrice;           //组合卖出价格
    int       ArbitBuyAmount;           //组合买入数量
    int       ArbitSellAmount;          //组合卖出数量
    int       AmountPerHand;            //合约乘数
    double    PreClosePrice;            //昨收盘
    double    FutuClosePrice;           //期货收盘价
};

//330851 期货合约保证金查询请求
struct CHundsunReqFuncFutuContractbailQryField
{
    char      UserToken[512];           //用户口令
    char      FutuExchType[4];          //交易类别
    char      FutuCode[30];             //合约代码
    int       RequestNum;               //请求行数
    char      PositionStr[100];         //定位串
};

//330851 期货合约保证金查询应答
struct CHundsunRspFuncFutuContractbailQryField
{
    char      FutuExchType[4];          //交易类别
    char      FutuCode[30];             //合约代码
    char      FutuName[12];             //品种简称
    char      FutucodeType[4];          //合约代码类别
    char      MoneyType[3];             //币种类别
    char      StopFlag;                 //停止交易标志
    int       AmountPerHand;            //合约乘数
    char      FutuReportUnit;           //申报单位
    char      PriceUnit;                //价格单位
    double    FutuPriceStep;            //最小价差
    double    OpenUnit;                 //开仓单位
    double    DropUnit;                 //平仓单位
    int       MaxMarketAmount;          //市价委托每笔最大下单数量
    int       MaxLimitedAmount;         //限价委托每笔最大下单数量
    double    OpenRatio;                //开仓成交金额比例
    double    OpenMargin;               //开仓单位数量保证金金额
    char      FoptExpiry[30];           //期权系列
    double    FoptStrikePrice;          //期权执行价格
    double    PreDelta;                 //昨虚实度
    char      FutuProductType;          //产品类别
    char      FoptType;                 //期权合约类型
    char      UnderlyingCode[30];       //标的合约代码
    int       ExpireDate;               //到期日
    int       StrikeDate;               //执行日期(上海期权采用百慕大执行方式,由交易所指定T日)
    char      HighSidePreferFlag;       //是否单向大边优惠
    double    PreDeltaValue;            //昨虚实值
    char      PositionStr[100];         //定位串
};

//330852 取交易所时间差请求
struct CHundsunReqFuncExchangetimeQryField
{
};

//330852 取交易所时间差应答
struct CHundsunRspFuncExchangetimeQryField
{
    char      FutuExchType[4];          //交易类别
    int       CurrTime;                 //当前时间
    int       DiffTime;                 //时间差
};

//330855 监控中心查询密钥获取请求
struct CHundsunReqFuncCfmmctradingaccountkeyQryField
{
};

//330855 监控中心查询密钥获取应答
struct CHundsunRspFuncCfmmctradingaccountkeyQryField
{
    char      CompanyId[10];            //机构编码
    int       CfmmcKeyNo;               //密钥序号
    char      CfmmcKey[10];             //密钥
};

//331124 密码校验请求
struct CHundsunReqFundFutuChkpasswordField
{
};

//331124 密码校验应答
struct CHundsunRspFundFutuChkpasswordField
{
    char      FundAccount[18];          //资产账户
    int       BranchNo;                 //分支机构
};

//331850 期货交易账号查询请求
struct CHundsunReqFuncTradingcodeQryField
{
};

//331850 期货交易账号查询应答
struct CHundsunRspFuncTradingcodeQryField
{
    char      FutuExchType[4];          //交易类别
    char      FuturesAccount[12];       //交易编码
    char      FutuacctStatus;           //期货账号状态
    char      FutuacctType;             //交易编码类型
    char      FutuacctRestricts[32];    //期货账号限制
    char      MainFlag;                 //主账标志
};

//331851 期货客户账单确认请求
struct CHundsunReqFuncSettlementInfoConfirmField
{
};

//331851 期货客户账单确认应答
struct CHundsunRspFuncSettlementInfoConfirmField
{
    char      BillFlag;                 //确认标志
};

//332213 取期货登记银行信息请求
struct CHundsunReqFundFutuBankinfoQryField
{
    char      MoneyType[3];             //币种类别
};

//332213 取期货登记银行信息应答
struct CHundsunRspFundFutuBankinfoQryField
{
    char      BankNo[4];                //银行代码
    char      BankName[64];             //银行名称
    char      BankAccount[32];          //银行账号
};

//338200 检查期货合约代码请求
struct CHundsunReqFuncFutuCheckFutuCodeField
{
    char      FutuExchType[4];          //交易类别
    char      FutuCode[30];             //合约代码
    char      FuturesAccount[12];       //交易编码
};

//338200 检查期货合约代码应答
struct CHundsunRspFuncFutuCheckFutuCodeField
{
    char      FutuExchType[4];          //交易类别
    char      FutuCode[30];             //合约代码
    char      FuturesAccount[12];       //交易编码
    double    EnableBalance;            //可用资金
    double    FutuPriceStep;            //最小价差
    int       BuyAmount;                //买持仓量
    int       SaleAmount;               //卖持仓量
    int       RealBuyAmount;            //回报买入数量
    double    RealSaleAmount;           //今开仓卖出持仓量
    double    PreSquarePrice;           //昨日结算价
    double    UplimitedPrice;           //涨停板价格
    double    DownlimitedPrice;         //跌停板价格
    double    FutuLastPrice;            //最新价格
    double    BuyHighPrice;             //最高买价价格
    double    SaleLowPrice;             //最低卖出价
    char      HedgeType;                //投机/套保类型
};

//338201 期货可买卖数量获取请求
struct CHundsunReqFuncMaxordervolumeField
{
    char      FutuExchType[4];          //交易类别
    char      FuturesAccount[12];       //交易编码
    char      FutuCode[30];             //合约代码
    char      EntrustBs;                //买卖方向
    char      FuturesDirection;         //开平仓方向
    char      HedgeType;                //投机/套保类型
    double    FutuEntrustPrice;         //委托价格
};

//338201 期货可买卖数量获取应答
struct CHundsunRspFuncMaxordervolumeField
{
    int       EnableAmount;             //可用数量
    int       MaxEntrustAmount;         //最大委托数量
};

//338202 期货委托确认请求
struct CHundsunReqFuncOrderinsertField
{
    char      FutuExchType[4];          //交易类别
    char      FuturesAccount[12];       //交易编码
    char      FutuCode[30];             //合约代码
    char      EntrustBs;                //买卖方向
    char      FuturesDirection;         //开平仓方向
    char      HedgeType;                //投机/套保类型
    int       EntrustAmount;            //委托数量
    double    FutuEntrustPrice;         //委托价格
    char      EntrustProp[3];           //委托属性
    char      EntrustOccasion[32];      //委托场景
    char      EntrustReference[32];     //委托引用
};

//338202 期货委托确认应答
struct CHundsunRspFuncOrderinsertField
{
    int       EntrustNo;                //委托编号
    char      EntrustReference[32];     //委托引用
    int       SessionNo;                //会话编号
    char      FutuExchType[4];          //交易类别
    char      FutuCode[30];             //合约代码
    char      EntrustBs;                //买卖方向
    char      FuturesDirection;         //开平仓方向
    char      HedgeType;                //投机/套保类型
    int       EntrustAmount;            //委托数量
    double    FutuEntrustPrice;         //委托价格
    char      EntrustProp[3];           //委托属性
    char      EntrustStatus;            //委托状态
    int       EntrustTime;              //委托时间
    char      FuturesAccount[12];       //交易编码
    char      EntrustType;              //委托类别
    char      EntrustOccasion[32];      //委托场景
};

//338203 检查组合合约代码请求
struct CHundsunReqFuncFutuCheckCombineFutuCodeField
{
    char      FuturesAccount[12];       //交易编码
    char      FutuExchType[4];          //交易类别
    char      FutuCode[30];             //合约代码
    char      EntrustBs;                //买卖方向
    char      FuturesDirection;         //开平仓方向
    char      SecondCode[30];           //第二腿合约代码
    char      ArbitCode[30];            //套利合约号
};

//338203 检查组合合约代码应答
struct CHundsunRspFuncFutuCheckCombineFutuCodeField
{
    char      ArbitCode[30];            //套利合约号
    char      FutuExchType[4];          //交易类别
    char      Arbicode[30];             //组合策略
    int       LegNum;                   //腿数
    double    FutuPriceStep;            //最小价差
    double    HighLimitPrice;           //报价上限
    double    LowLimitPrice;            //报价下限
    int       MaxLimitAmount;           //最大下单数量
    int       MinLimitAmount;           //最小下单数量
    double    UplimitedPrice;           //涨停板价格
    double    DownlimitedPrice;         //跌停板价格
};

//338217 期货委托撤单请求
struct CHundsunReqFuncOrderactionField
{
    char      FutuExchType[4];          //交易类别
    int       EntrustNo;                //委托编号
    char      ConfirmId[20];            //主场单号
    int       SessionNo;                //会话编号
    char      EntrustOccasion[32];      //委托场景
    char      EntrustReference[32];     //委托引用
};

//338217 期货委托撤单应答
struct CHundsunRspFuncOrderactionField
{
    int       EntrustNo;                //委托编号
    char      EntrustReference[32];     //委托引用
    int       SessionNo;                //会话编号
    char      FutuExchType[4];          //交易类别
    char      FutuCode[30];             //合约代码
    int       WithdrawAmount;           //撤单数量
    char      EntrustStatus;            //委托状态
    int       EntrustTime;              //委托时间
};

//338300 期货客户资金查询请求
struct CHundsunReqFuncTradingaccountQryField
{
    char      MoneyType[3];             //币种类别
};

//338300 期货客户资金查询应答
struct CHundsunRspFuncTradingaccountQryField
{
    char      MoneyType[3];             //币种类别
    double    CurrentBalance;           //当前余额
    double    EnableBalance;            //可用资金
    double    FetchBalance;             //可取金额
    double    FrozenBalance;            //冻结资金
    double    PreEntrustBalance;        //当日开仓预冻结金额
    double    EntrustBalance;           //委托金额
    double    HoldIncome;               //期货盯市盈亏
    double    HoldIncomeFloat;          //持仓浮动盈亏
    double    BeginEquityBalance;       //期初客户权益
    double    EquityBalance;            //客户权益
    double    InterestBalance;          //利息发生额
    double    DropIncome;               //平仓盯市盈亏
    double    DropIncomeFloat;          //平仓浮动盈亏
    double    BusinessFare;             //成交手续费金额
    double    HoldMargin;               //持仓保证金
    double    ExchHoldMargin;           //交易所持仓保证金
    double    ClientRiskRate;           //客户风险率
    double    ExchRiskRate;             //交易所风险率
    double    OutPremium;               //支出权利金
    double    InPremium;                //收取权利金
    double    MarketValue;              //证券市值
    double    OutImpawnBalance;         //质出金额
    double    InImpawnBalance;          //质入金额
    double    ImpawnEnableBalance;      //货币质押余额
    double    InePreHoldBalance;        //特殊品种客户预冻结保证金
    double    IneBailBalance;           //特殊品种客户保证金
    double    IneFrozenFare;            //特殊品种冻结总费用
    double    InePreFrozenFare;         //特殊品种预冻结开仓费用
    double    IneDropProfit;            //特殊品种平仓盯市盈亏
    double    IneHoldProfit;            //特殊品种持仓盯市盈亏
};

//338301 期货当日委托查询请求
struct CHundsunReqFuncOrderQryField
{
    char      FutuExchType[4];          //交易类别
    char      FuturesAccount[12];       //交易编码
    char      FutuCode[30];             //合约代码
    char      EnEntrustStatus[10];      //允许委托状态
    char      QueryDirection;           //查询方向
    int       BatchNo;                  //委托批号
    int       EntrustNo;                //委托编号
    char      EntrustType;              //委托类别
    char      FutuProductType;          //产品类别
    int       RequestNum;               //请求行数
    char      PositionStr[100];         //定位串
};

//338301 期货当日委托查询应答
struct CHundsunRspFuncOrderQryField
{
    int       BatchNo;                  //委托批号
    int       EntrustNo;                //委托编号
    char      FundAccount[18];          //资产账户
    char      FutuExchType[4];          //交易类别
    char      FuturesAccount[12];       //交易编码
    char      FutuCode[30];             //合约代码
    char      MoneyType[3];             //币种类别
    char      FuturesDirection;         //开平仓方向
    double    FutuEntrustPrice;         //委托价格
    char      EntrustBs;                //买卖方向
    char      HedgeType;                //投机/套保类型
    char      EntrustStatus;            //委托状态
    int       EntrustTime;              //委托时间
    int       ReportTime;               //申报时间
    int       BusinessAmount;           //成交数量
    int       EntrustAmount;            //委托数量
    int       WithdrawAmount;           //撤单数量
    double    CurrEntrustMargin;        //当前预冻结保证金
    char      EntrustType;              //委托类别
    char      ConfirmId[20];            //主场单号
    double    CurrEntrustFare;          //当前预冻结总费用
    char      ForcecloseReason;         //强平原因
    char      ErrorMessage[255];        //提示信息
    double    BusinessBalance;          //成交金额
    char      EntrustProp[3];           //委托属性
    char      ArbitCode[30];            //套利合约号
    char      SecondCode[30];           //第二腿合约代码
    char      WeaveType;                //组合类型
    double    SpringPrice;              //止损价格
    char      TimeCondition;            //有效期类型
    int       ValidDate;                //有效日期
    char      VolumeCondition;          //成交量类型
    int       FexMinVolume;             //最小成交量
    char      ReportId[32];             //申报合同序号
    int       SessionNo;                //会话编号
    char      EntrustReference[32];     //委托引用
    char      EntrustOccasion[32];      //委托场景
    char      FutuProductType;          //产品类别
    char      FoptType;                 //期权合约类型
    double    ExercisePrice;            //行权价格
    char      UnderlyingCode[30];       //标的合约代码
    char      PositionStr[100];         //定位串
};

//338302 期货当日成交查询请求
struct CHundsunReqFuncTradeQryField
{
    char      FutuExchType[4];          //交易类别
    char      FuturesAccount[12];       //交易编码
    char      FutuCode[30];             //合约代码
    char      QueryMode;                //查询模式
    char      QueryDirection;           //查询方向
    int       RequestNum;               //请求行数
    char      PositionStr[100];         //定位串
    char      FutuProductType;          //产品类别
};

//338302 期货当日成交查询应答
struct CHundsunRspFuncTradeQryField
{
    int       EntrustNo;                //委托编号
    char      FutuExchType[4];          //交易类别
    char      FuturesAccount[12];       //交易编码
    char      FutuCode[30];             //合约代码
    char      MoneyType[3];             //币种类别
    char      FuturesDirection;         //开平仓方向
    char      EntrustBs;                //买卖方向
    char      HedgeType;                //投机/套保类型
    double    FutuBusinessPrice;        //成交价格
    char      EntrustType;              //委托类别
    int       BusinessTime;             //成交时间
    int       BusinessAmount;           //成交数量
    double    BusinessFare;             //成交手续费金额
    char      BusinessId[16];           //成交编号
    char      PositionStr[100];         //定位串
    char      ConfirmId[20];            //主场单号
    int       SessionNo;                //会话编号
    char      EntrustReference[32];     //委托引用
    char      EntrustOccasion[32];      //委托场景
    char      ArbitCode[30];            //套利合约号
    char      FutuProductType;          //产品类别
    char      FoptType;                 //期权合约类型
    double    ExercisePrice;            //行权价格
    char      UnderlyingCode[30];       //标的合约代码
};

//338303 期货持仓查询请求
struct CHundsunReqFuncInvestorpositionQryField
{
    char      FutuExchType[4];          //交易类别
    char      FuturesAccount[12];       //交易编码
    char      FutuCode[30];             //合约代码
    char      EntrustBs;                //买卖方向
    char      QueryDirection;           //查询方向
    char      QueryMode;                //查询模式
    int       RequestNum;               //请求行数
    char      HedgeFlag;                //区分投机套保标志
    char      FutuProductType;          //产品类别
    char      PositionStr[100];         //定位串
};

//338303 期货持仓查询应答
struct CHundsunRspFuncInvestorpositionQryField
{
    char      FutuExchType[4];          //交易类别
    char      FuturesAccount[12];       //交易编码
    char      FutuCode[30];             //合约代码
    char      MoneyType[3];             //币种类别
    char      EntrustBs;                //买卖方向
    int       BeginAmount;              //期初数量
    int       EnableAmount;             //可用数量
    int       RealEnableAmount;         //当日开仓可用数量
    double    HoldIncomeFloat;          //持仓浮动盈亏
    double    HoldIncome;               //期货盯市盈亏
    double    HoldMargin;               //持仓保证金
    double    AveragePrice;             //平均价
    double    FutuLastPrice;            //最新价格
    char      HedgeType;                //投机/套保类型
    int       RealAmount;               //成交数量
    double    RealOpenBalance;          //回报开仓金额
    double    OldOpenBalance;           //老仓持仓成交额
    double    RealCurrentAmount;        //今总持仓
    int       OldCurrentAmount;         //老仓持仓数量
    char      FutuProductType;          //产品类别
    char      PositionStr[100];         //定位串
    char      FoptType;                 //期权合约类型
    double    ExercisePrice;            //行权价格
    char      UnderlyingCode[30];       //标的合约代码
};

//338306 当日资金流水查询请求
struct CHundsunReqFuncFutuFundjourQryField
{
    char      MoneyType[3];             //币种类别
    int       RequestNum;               //请求行数
    char      PositionStr[100];         //定位串
};

//338306 当日资金流水查询应答
struct CHundsunRspFuncFutuFundjourQryField
{
    int       BusinessDate;             //成交日期
    int       SerialNo;                 //流水序号
    char      BusinessName[16];         //业务名称
    double    OccurBalance;             //发生金额
    double    PostBalance;              //后资金额
    char      MoneyType[3];             //币种类别
    char      Remark[2000];             //备注
    char      PositionStr[100];         //定位串
};

//338307 持仓明细查询请求
struct CHundsunReqFuncInvestorpositiondetailQryField
{
    char      FutuExchType[4];          //交易类别
    char      FuturesAccount[12];       //交易编码
    char      FutuCode[30];             //合约代码
    char      EntrustBs;                //买卖方向
    int       RequestNum;               //请求行数
    char      FutuProductType;          //产品类别
    char      PositionStr[100];         //定位串
};

//338307 持仓明细查询应答
struct CHundsunRspFuncInvestorpositiondetailQryField
{
    int       OpenDate;                 //开户日期
    int       SerialNo;                 //流水序号
    int       BranchNo;                 //分支机构
    char      FundAccount[18];          //资产账户
    char      FutuExchType[4];          //交易类别
    char      FuturesAccount[12];       //交易编码
    char      FutucodeType[4];          //合约代码类别
    char      FutuCode[30];             //合约代码
    char      EntrustBs;                //买卖方向
    char      HedgeType;                //投机/套保类型
    char      MoneyType[3];             //币种类别
    int       BeginAmount;              //期初数量
    int       CurrentAmount;            //当前数量
    int       BusinessAmount;           //成交数量
    double    FutuOpenPrice;            //开盘价
    double    SquarePrice;              //结算价
    double    BeginHoldMargin;          //上日持仓保证金
    double    HoldMargin;               //持仓保证金
    double    BeginExchHoldMargin;      //期初交易所保证金
    double    ExchHoldMargin;           //交易所持仓保证金
    double    DropIncome;               //平仓盯市盈亏
    double    HoldIncome;               //期货盯市盈亏
    double    HoldIncomeFloat;          //持仓浮动盈亏
    double    RealHoldIncome;           //当日持仓盈亏
    double    RealHoldIncomeFloat;      //当日持仓浮动盈亏
    double    PreDropAmount;            //当日预平仓数量
    int       DropAmount;               //平仓数量
    double    OldUnfrozenMargin;        //当日平老仓解冻保证金
    double    RealUnfrozenMargin;       //当日平新仓解冻保证金
    double    RealOpenMargin;           //当日开仓冻结保证金
    double    OldUnfrozenExchMargin;    //当日平老仓交易所解冻保证金
    double    RealUnfrozenExchMargin;   //当日平新交易所应解冻保证金
    double    RealExchOpenMargin;       //当日开仓交易所冻结保证金
    char      ArbitholdId[20];          //套利持仓号
    int       FutulegNo;                //腿号
    char      BusinessId[16];           //成交编号
    int       AmountPerHand;            //合约乘数
    char      DeliverFlag;              //确认标志
    double    MarginPerHand;            //每手保证金
    double    ExchMarginPerHand;        //交易所每手保证金
    char      FutuProductType;          //产品类别
    char      FoptType;                 //期权合约类型
    double    ExercisePrice;            //行权价格
    char      UnderlyingCode[30];       //标的合约代码
    char      PositionStr[100];         //定位串
};

//338308 取系统信息请求
struct CHundsunReqFuncFutuSysinfoQryField
{
    int       OpBranchNo;               //操作分支机构
    char      OpEntrustWay;             //委托方式
    char      OpStation[255];           //站点地址
    int       BranchNo;                 //分支机构
};

//338308 取系统信息应答
struct CHundsunRspFuncFutuSysinfoQryField
{
    char      CurrDatetime[14];         //当前日期时间
};

//339851 取客户账单请求
struct CHundsunReqFuncSettlementinfoQryField
{
    int       BeginDate;                //起始日期
    int       EndDate;                  //到期日期
    char      MoneyType[3];             //币种类别
    char      TotalType;                //汇总模式
};

//339851 取客户账单应答
struct CHundsunRspFuncSettlementinfoQryField
{
    char      Content[2000];            //公告内容
    char      CheckTabData[2000];       //帐单信息
};

//339852 期货历史委托查询请求
struct CHundsunReqFuncHisorderQryField
{
    char      FutuExchType[4];          //交易类别
    char      FuturesAccount[12];       //交易编码
    char      FutuCode[30];             //合约代码
    int       BeginDate;                //起始日期
    int       EndDate;                  //到期日期
    char      EnEntrustStatus[10];      //允许委托状态
    char      QueryDirection;           //查询方向
    int       BatchNo;                  //委托批号
    int       EntrustNo;                //委托编号
    char      EntrustType;              //委托类别
    int       RequestNum;               //请求行数
    char      PositionStr[100];         //定位串
    char      FutuProductType;          //产品类别
};

//339852 期货历史委托查询应答
struct CHundsunRspFuncHisorderQryField
{
    int       InitDate;                 //交易日期
    int       EntrustNo;                //委托编号
    char      FundAccount[18];          //资产账户
    char      ClientName[14];           //客户姓名
    char      FutuExchType[4];          //交易类别
    char      FuturesAccount[12];       //交易编码
    char      FutuCode[30];             //合约代码
    char      MoneyType[3];             //币种类别
    double    FutuEntrustPrice;         //委托价格
    char      FuturesDirection;         //开平仓方向
    char      EntrustBs;                //买卖方向
    char      HedgeType;                //投机/套保类型
    char      EntrustStatus;            //委托状态
    int       EntrustTime;              //委托时间
    int       ReportTime;               //申报时间
    int       AmountPerHand;            //合约乘数
    int       BusinessAmount;           //成交数量
    int       EntrustAmount;            //委托数量
    int       WithdrawAmount;           //撤单数量
    double    CurrEntrustMargin;        //当前预冻结保证金
    char      EntrustType;              //委托类别
    char      ConfirmId[20];            //主场单号
    double    CurrEntrustFare;          //当前预冻结总费用
    char      ForcedropReason;          //强平原因
    int       BatchNo;                  //委托批号
    char      ErrorMessage[255];        //提示信息
    double    BusinessBalance;          //成交金额
    char      EntrustProp[3];           //委托属性
    char      ArbitCode[30];            //套利合约号
    char      SecondCode[30];           //第二腿合约代码
    char      WeaveType;                //组合类型
    double    SpringPrice;              //止损价格
    char      TimeCondition;            //有效期类型
    int       ValidDate;                //有效日期
    char      VolumeCondition;          //成交量类型
    char      FutuProductType;          //产品类别
    char      FoptType;                 //期权合约类型
    double    ExercisePrice;            //行权价格
    char      UnderlyingCode[30];       //标的合约代码
    char      PositionStr[100];         //定位串
};

//339853 期货历史成交查询请求
struct CHundsunReqFuncHistradeQryField
{
    int       BeginDate;                //起始日期
    int       EndDate;                  //到期日期
    char      FutuExchType[4];          //交易类别
    char      FuturesAccount[12];       //交易编码
    char      FutuCode[30];             //合约代码
    char      QueryDirection;           //查询方向
    int       RequestNum;               //请求行数
    char      FutuProductType;          //产品类别
    char      PositionStr[100];         //定位串
};

//339853 期货历史成交查询应答
struct CHundsunRspFuncHistradeQryField
{
    int       InitDate;                 //交易日期
    int       SerialNo;                 //流水序号
    char      FutuExchType[4];          //交易类别
    char      FuturesAccount[12];       //交易编码
    char      FutuCode[30];             //合约代码
    char      MoneyType[3];             //币种类别
    char      FuturesDirection;         //开平仓方向
    char      EntrustBs;                //买卖方向
    double    FutuBusinessPrice;        //成交价格
    int       AmountPerHand;            //合约乘数
    int       BusinessTime;             //成交时间
    int       BusinessAmount;           //成交数量
    double    BusinessFare;             //成交手续费金额
    double    DropIncome;               //平仓盯市盈亏
    double    DropIncomeFloat;          //平仓浮动盈亏
    char      BusinessId[16];           //成交编号
    char      FutuProductType;          //产品类别
    char      FoptType;                 //期权合约类型
    double    ExercisePrice;            //行权价格
    char      UnderlyingCode[30];       //标的合约代码
    char      PositionStr[100];         //定位串
};

//339854 取客户历史资金请求
struct CHundsunReqFuncHistradingaccountQryField
{
    int       QueryDate;                //查询日期
    char      MoneyType[3];             //币种类别
};

//339854 取客户历史资金应答
struct CHundsunRspFuncHistradingaccountQryField
{
    char      FundAccount[18];          //资产账户
    char      ClientId[18];             //客户编号
    char      ClientName[14];           //客户姓名
    double    BeginBalance;             //期初余额
    double    CurrentBalance;           //当前余额
    int       SumIncome;                //盈亏总额
    int       SumIncomeFloat;           //浮动盈亏总额
    double    HoldIncome;               //期货盯市盈亏
    double    HoldIncomeFloat;          //持仓浮动盈亏
    double    DropIncome;               //平仓盯市盈亏
    double    DropIncomeFloat;          //平仓浮动盈亏
    double    ClientRiskRate;           //客户风险率
    double    ExchRiskRate;             //交易所风险率
    double    BeginHoldMargin;          //上日持仓保证金
    double    HoldMargin;               //持仓保证金
    double    BeginExchHoldMargin;      //期初交易所保证金
    double    ExchHoldMargin;           //交易所持仓保证金
    double    AdditionalMargin;         //追加保证金
    double    LateFee;                  //滞纳金
    double    LatefeeRate;              //滞纳金比例
    double    PaymentBalance;           //货款
    double    TotalHoldBalance;         //总持仓金额
    double    TotalFare;                //手续费
    double    InBalance;                //转入金额
    double    OutBalance;               //转出金额
    double    EnableBalance;            //可用资金
};

//338000 期权代码信息查询请求
struct CHundsunReqFuncOptOptcodeQryField
{
    char      StockCode[16];            //证券代码
    char      OptionCode[9];            //期权合约编码
    char      OptionType;               //期权种类
    int       ExerciseMonth;            //行权月份
    char      PositionStr[100];         //定位串
    int       RequestNum;               //请求行数
};

//338000 期权代码信息查询应答
struct CHundsunRspFuncOptOptcodeQryField
{
    char      ExchangeType[4];          //交易类别
    char      OptionCode[9];            //期权合约编码
    char      OptcontractId[32];        //合约交易代码
    char      OptionName[32];           //期权合约简称
    char      OptionType;               //期权种类
    char      StockType[4];             //证券类别
    char      StockCode[16];            //证券代码
    char      MoneyType[3];             //币种类别
    int       AmountPerHand;            //合约乘数
    char      OptionMode;               //期权执行方式
    double    OptClosePrice;            //合约前收盘价
    double    ClosePrice;               //昨收盘
    double    OptUpPrice;               //上限价
    double    OptDownPrice;             //下限价
    double    ExercisePrice;            //行权价格
    double    InitperBalance;           //单位保证金
    int       LimitHighAmount;          //单笔限价申报最高数量
    int       LimitLowAmount;           //单笔限价申报最低数量
    int       MktHighAmount;            //单笔市价申报最高数量
    int       MktLowAmount;             //单笔市价申报最低数量
    int       BeginDate;                //起始日期
    int       EndDate;                  //到期日期
    int       ExeBeginDate;             //行权开始日期
    int       ExeEndDate;               //行权截至日期
    char      OptcodeStatus;            //期权代码状态
    char      OptUpdatedStatus;         //期权合约调整标志
    int       OptionVersion;            //期权合约版本号
    char      OptOpenStatus;            //期权开仓状态
    char      OptionFlag;               //期权合约挂牌标志
    char      OptFinalStatus;           //期权合约临近到期标志
    double    OptPriceStep;             //最小价差（元）
    char      PositionStr[100];         //定位串
};

//338001 期权标的信息查询请求
struct CHundsunReqFuncOptOptobjectcodeQryField
{
    char      ExchangeType[4];          //交易类别
    char      StockCode[16];            //证券代码
    char      PositionStr[100];         //定位串
    int       RequestNum;               //请求行数
};

//338001 期权标的信息查询应答
struct CHundsunRspFuncOptOptobjectcodeQryField
{
    char      ExchangeType[4];          //交易类别
    char      StockCode[16];            //证券代码
    char      StockType[4];             //证券类别
    char      StockName[32];            //证券名称
    int       AmountPerHand;            //合约乘数
    char      UnderlyStatus;            //标的状态
    char      PositionStr[100];         //定位串
};

//338002 期权代码输入确认请求
struct CHundsunReqFuncOptOptcodeEnterField
{
    char      ExchangeType[4];          //交易类别
    char      OptionCode[9];            //期权合约编码
};

//338002 期权代码输入确认应答
struct CHundsunRspFuncOptOptcodeEnterField
{
    char      NoticeInfo[255];          //提示信息
};

//338010 期权可交易数量获取请求
struct CHundsunReqFuncOptEnTradeField
{
    char      AssetProp;                //资产属性
    char      ExchangeType[4];          //交易类别
    char      OptionAccount[13];        //衍生品合约账户
    char      OptionCode[9];            //期权合约编码
    double    OptEntrustPrice;          //委托价格
    char      EntrustProp[3];           //委托属性
    char      EntrustBs;                //买卖方向
    char      EntrustOc;                //开平仓方向
    char      CoveredFlag;              //备兑标志
};

//338010 期权可交易数量获取应答
struct CHundsunRspFuncOptEnTradeField
{
    int       EnableAmount;             //可用数量
};

//338011 期权委托请求
struct CHundsunReqFuncOptEntrustField
{
    char      AssetProp;                //资产属性
    char      ExchangeType[4];          //交易类别
    char      OptionAccount[13];        //衍生品合约账户
    char      OptionCode[9];            //期权合约编码
    int       EntrustAmount;            //委托数量
    double    OptEntrustPrice;          //委托价格
    char      EntrustBs;                //买卖方向
    char      EntrustOc;                //开平仓方向
    char      CoveredFlag;              //备兑标志
    char      EntrustProp[3];           //委托属性
    int       BatchNo;                  //委托批号
};

//338011 期权委托应答
struct CHundsunRspFuncOptEntrustField
{
    int       InitDate;                 //交易日期
    int       EntrustNo;                //委托编号
    int       ReportNo;                 //申请编号
    int       BatchNo;                  //委托批号
    int       EntrustTime;              //委托时间
};

//338012 期权撤单请求
struct CHundsunReqFuncOptWithdrawField
{
    char      AssetProp;                //资产属性
    char      ExchangeType[4];          //交易类别
    int       EntrustNo;                //委托编号
};

//338012 期权撤单应答
struct CHundsunRspFuncOptWithdrawField
{
    int       InitDate;                 //交易日期
    int       EntrustNo;                //委托编号
    int       EntrustNoOld;             //原委托编号
    int       ReportNoOld;              //原申请编号
    char      ExchangeType[4];          //交易类别
    char      OptionAccount[13];        //衍生品合约账户
    char      StockCode[16];            //证券代码
    char      OptionCode[9];            //期权合约编码
    char      EntrustStatusOld;         //原委托的委托状态
};

//338020 期权委托查询请求
struct CHundsunReqFuncOptEntrustQryField
{
    char      AssetProp;                //资产属性
    char      ExchangeType[4];          //交易类别
    char      OptionAccount[13];        //衍生品合约账户
    char      OptionCode[9];            //期权合约编码
    char      StockCode[16];            //证券代码
    int       ReportNo;                 //申请编号
    char      SortDirection;            //返回排序方式
    char      QueryKind;                //查询控制值
    int       LocateEntrustNo;          //指定委托号
    char      QueryType;                //查询类别
    char      PositionStr[100];         //定位串
    int       RequestNum;               //请求行数
};

//338020 期权委托查询应答
struct CHundsunRspFuncOptEntrustQryField
{
    int       InitDate;                 //交易日期
    int       BatchNo;                  //委托批号
    int       EntrustNo;                //委托编号
    char      ExchangeType[4];          //交易类别
    char      FundAccount[18];          //资产账户
    char      OptionAccount[13];        //衍生品合约账户
    char      OptionCode[9];            //期权合约编码
    char      OptcontractId[32];        //合约交易代码
    char      StockCode[16];            //证券代码
    char      EntrustBs;                //买卖方向
    char      EntrustOc;                //开平仓方向
    char      CoveredFlag;              //备兑标志
    double    OptEntrustPrice;          //委托价格
    int       EntrustAmount;            //委托数量
    int       BusinessAmount;           //成交数量
    double    OptBusinessPrice;         //成交价格
    int       ReportNo;                 //申请编号
    int       ReportTime;               //申报时间
    char      EntrustType;              //委托类别
    char      EntrustStatus;            //委托状态
    int       EntrustTime;              //委托时间
    int       EntrustDate;              //委托日期
    char      EntrustProp[3];           //委托属性
    char      EntrustSrc;               //委托来源
    char      TradeName[64];            //订单名称
    char      OptionName[32];           //期权合约简称
    char      CancelInfo[20];           //废单原因
    int       WithdrawAmount;           //撤单数量
    char      WithdrawFlag;             //撤单允许标志
    char      PositionStr[100];         //定位串
};

//338021 期权成交查询请求
struct CHundsunReqFuncOptRealtimeQryField
{
    char      AssetProp;                //资产属性
    char      ExchangeType[4];          //交易类别
    char      OptionAccount[13];        //衍生品合约账户
    char      OptionCode[9];            //期权合约编码
    char      StockCode[16];            //证券代码
    int       SerialNo;                 //流水序号
    char      SortDirection;            //返回排序方式
    char      QueryDirection;           //查询方向
    int       ReportNo;                 //申请编号
    char      QueryMode;                //查询模式
    char      PositionStr[100];         //定位串
    int       RequestNum;               //请求行数
};

//338021 期权成交查询应答
struct CHundsunRspFuncOptRealtimeQryField
{
    int       InitDate;                 //交易日期
    int       SerialNo;                 //流水序号
    char      ExchangeType[4];          //交易类别
    char      FundAccount[18];          //资产账户
    char      OptionAccount[13];        //衍生品合约账户
    char      OptionCode[9];            //期权合约编码
    char      OptcontractId[32];        //合约交易代码
    char      StockCode[16];            //证券代码
    char      EntrustBs;                //买卖方向
    char      EntrustOc;                //开平仓方向
    char      CoveredFlag;              //备兑标志
    double    OptBusinessPrice;         //成交价格
    int       BusinessAmount;           //成交数量
    int       BusinessTime;             //成交时间
    char      RealType;                 //成交类型
    char      RealStatus;               //处理标志
    int       BusinessTimes;            //分笔成交笔数
    int       EntrustNo;                //委托编号
    double    BusinessBalance;          //成交金额
    char      OptionName[32];           //期权合约简称
    char      TradeName[64];            //订单名称
    int       ReportNo;                 //申请编号
    char      EntrustProp[3];           //委托属性
    char      BusinessId[16];           //成交编号
    char      PositionStr[100];         //定位串
};

//338022 期权资产查询请求
struct CHundsunReqFuncOptAssetQryField
{
    char      AssetProp;                //资产属性
    char      MoneyType[3];             //币种类别
};

//338022 期权资产查询应答
struct CHundsunRspFuncOptAssetQryField
{
    double    TotalAsset;               //总资产
    double    FundAsset;                //现金资产
    double    CurrentBalance;           //当前余额
    double    EnableBalance;            //可用资金
    double    EnableBailBalance;        //可用保证金
    double    UsedBailBalance;          //已用保证金
    double    UsedPurBalance;           //已用限购额度
    double    EnablePurBalance;         //可用限购额度
    double    PurQuota;                 //限购额度
    double    IncomeBalance;            //盈亏金额
    double    RiskDegree;               //风险度
    double    RealRiskDegree;           //实时风险度
    double    DynaMarketValue;          //期权持仓动态市值
    double    RealUsedBail;             //实时已用保证金
    char      OptriskType;              //风险监控类别
    double    ShortBalance;             //缺口资金
    double    HedgeRiskDegree;          //对冲风险度
};

//338023 期权持仓查询请求
struct CHundsunReqFuncOptHoldQryField
{
    char      AssetProp;                //资产属性
    char      ExchangeType[4];          //交易类别
    char      OptionAccount[13];        //衍生品合约账户
    char      OptholdType;              //期权持仓类别
    char      OptionCode[9];            //期权合约编码
    char      PositionStr[100];         //定位串
    int       RequestNum;               //请求行数
};

//338023 期权持仓查询应答
struct CHundsunRspFuncOptHoldQryField
{
    char      FundAccount[18];          //资产账户
    char      ExchangeType[4];          //交易类别
    char      OptionAccount[13];        //衍生品合约账户
    char      OptholdType;              //期权持仓类别
    char      OptionCode[9];            //期权合约编码
    char      StockCode[16];            //证券代码
    char      OptcontractId[32];        //合约交易代码
    char      OptionName[32];           //期权合约简称
    char      OptionType;               //期权种类
    int       CurrentAmount;            //当前数量
    int       HoldAmount;               //持有数量
    int       EnableAmount;             //可用数量
    int       RealOpenAmount;           //回报开仓数量
    int       RealDropAmount;           //回报平仓数量
    int       EntrustDropAmount;        //委托平仓数量
    double    LastPrice;                //最新价
    double    OptLastPrice;             //最新价
    double    OptCostPrice;             //开仓均价
    double    ExercisePrice;            //行权价格
    double    MarketValue;              //证券市值
    double    CostBalance;              //持仓成本
    double    IncomeBalance;            //盈亏金额
    double    ExerciseIncome;           //行权盈亏
    double    DutyUsedBail;             //义务仓占用保证金
    int       ExerciseDate;             //行权日期
    int       AmountPerHand;            //合约乘数
    char      PositionStr[100];         //定位串
};

//338024 期权行权指派查询请求
struct CHundsunReqFuncOptExeassignQryField
{
    char      ExchangeType[4];          //交易类别
    char      OptionAccount[13];        //衍生品合约账户
    char      OptholdType;              //期权持仓类别
    char      OptionCode[9];            //期权合约编码
    char      PositionStr[100];         //定位串
    int       RequestNum;               //请求行数
};

//338024 期权行权指派查询应答
struct CHundsunRspFuncOptExeassignQryField
{
    char      FundAccount[18];          //资产账户
    char      ExchangeType[4];          //交易类别
    char      OptionAccount[13];        //衍生品合约账户
    char      OptholdType;              //期权持仓类别
    char      OptionCode[9];            //期权合约编码
    char      OptionName[32];           //期权合约简称
    char      OptionType;               //期权种类
    char      StockCode[16];            //证券代码
    double    ExercisePrice;            //行权价格
    int       ExerciseAmount;           //行权数量
    double    ExefrozenBalance;         //行权冻结资金
    int       SettleAmount;             //交收数量
    double    SettleBalance;            //结算金额
    char      PositionStr[100];         //定位串
};

//338025 期权行权交割信息查询请求
struct CHundsunReqFuncAssetOptexedeliverQryField
{
    char      ExchangeType[4];          //交易类别
    char      StockCode[16];            //证券代码
    char      PositionStr[100];         //定位串
    int       RequestNum;               //请求行数
};

//338025 期权行权交割信息查询应答
struct CHundsunRspFuncAssetOptexedeliverQryField
{
    int       InitDate;                 //交易日期
    int       BranchNo;                 //分支机构
    char      ExchangeType[4];          //交易类别
    char      ClientId[18];             //客户编号
    char      FundAccount[18];          //资产账户
    char      OptionAccount[13];        //衍生品合约账户
    char      StockCode[16];            //证券代码
    char      StockType[4];             //证券类别
    int       ClearAmount;              //清算数量
    int       SettleAmount;             //交收数量
    int       ShortAmount;              //缺口数量
    int       TreatAmount;              //处置数量
    int       ReturnAmount;             //返还数量
    double    ClearBalance;             //清算金额
    double    SettleBalance;            //结算金额
    double    ShortBalance;             //缺口资金
    double    ReturnBalance;            //返还金额
    char      PositionStr[100];         //定位串
};

//338026 期权行权指派欠资欠券查询请求
struct CHundsunReqFuncAssetDebtinfoQryField
{
    char      StockCode[16];            //证券代码
    char      FundAccountOpt[18];       //期权资产账户
};

//338026 期权行权指派欠资欠券查询应答
struct CHundsunRspFuncAssetDebtinfoQryField
{
    int       InitDate;                 //交易日期
    int       BranchNo;                 //分支机构
    char      ExchangeType[4];          //交易类别
    char      ClientId[18];             //客户编号
    char      FundAccount[18];          //资产账户
    char      FundAccountOpt[18];       //期权资产账户
    char      OptionAccount[13];        //衍生品合约账户
    char      DebtType;                 //负债类型
    char      StockCode[16];            //证券代码
    char      StockType[4];             //证券类别
    double    ShortBalance;             //缺口资金
    int       SettleAmount;             //交收数量
    int       ShortAmount;              //缺口数量
    char      StockName[32];            //证券名称
    double    SettleBalance;            //结算金额
};

//338027 期权标的证券数量提示请求
struct CHundsunReqFuncOptUnderlyamountPromptField
{
    char      OptionCode[9];            //期权合约编码
    int       EntrustAmount;            //委托数量
    char      EntrustBs;                //买卖方向
    char      EntrustOc;                //开平仓方向
    char      ExchangeType[4];          //交易类别
    char      FundAccountOpt[18];       //期权资产账户
    char      OptionAccount[13];        //衍生品合约账户
};

//338027 期权标的证券数量提示应答
struct CHundsunRspFuncOptUnderlyamountPromptField
{
    char      NoticeInfo[255];          //提示信息
};

//338028 客户期权持仓合约信息提示请求
struct CHundsunReqFuncOptOptholdrealPromptField
{
    char      AssetProp;                //资产属性
    char      MoneyType[3];             //币种类别
};

//338028 客户期权持仓合约信息提示应答
struct CHundsunRspFuncOptOptholdrealPromptField
{
    char      OptionCode[9];            //期权合约编码
    char      OptionName[32];           //期权合约简称
    char      NoticeInfo[255];          //提示信息
};

//338029 客户备兑证券不足查询请求
struct CHundsunReqFuncAssetAssetcoverstockQryField
{
    char      StockCode[16];            //证券代码
    char      PositionStr[100];         //定位串
    int       RequestNum;               //请求行数
};

//338029 客户备兑证券不足查询应答
struct CHundsunRspFuncAssetAssetcoverstockQryField
{
    int       BranchNo;                 //分支机构
    char      ExchangeType[4];          //交易类别
    char      ClientId[18];             //客户编号
    char      FundAccount[18];          //资产账户
    char      OptionAccount[13];        //衍生品合约账户
    char      StockCode[16];            //证券代码
    char      StockType[4];             //证券类别
    int       CoveredLockAmount;        //备兑锁定数量
    int       CoveredShortAmount;       //备兑缺口数量
    int       CoveredPreshortAmount;    //备兑预估缺口数量
    char      PositionStr[100];         //定位串
};

//338030 备兑证券可划转数量获取请求
struct CHundsunReqFuncOptCoveredQryField
{
    char      FundAccountOpt[18];       //期权资产账户
    char      OptionAccount[13];        //衍生品合约账户
    char      StockAccount[11];         //证券账号
    char      SeatNo[6];                //席位编号
    char      ExchangeType[4];          //交易类别
    char      StockCode[16];            //证券代码
    char      LockDirection;            //锁定方向
};

//338030 备兑证券可划转数量获取应答
struct CHundsunRspFuncOptCoveredQryField
{
    char      ExchangeType[4];          //交易类别
    char      StockCode[16];            //证券代码
    char      StockName[32];            //证券名称
    char      StockType[4];             //证券类别
    int       EnableAmount;             //可用数量
};

//338031 备兑证券划转请求
struct CHundsunReqFuncOptCoveredTransField
{
    char      FundAccountOpt[18];       //期权资产账户
    char      OptionAccount[13];        //衍生品合约账户
    char      StockAccount[11];         //证券账号
    char      SeatNo[6];                //席位编号
    char      ExchangeType[4];          //交易类别
    char      StockCode[16];            //证券代码
    int       EntrustAmount;            //委托数量
    char      LockDirection;            //锁定方向
};

//338031 备兑证券划转应答
struct CHundsunRspFuncOptCoveredTransField
{
    int       InitDate;                 //交易日期
    int       EntrustNo;                //委托编号
    int       EntrustTime;              //委托时间
};

//339800 历史期权委托查询请求
struct CHundsunReqFuncHisOptentrustQryField
{
    int       StartDate;                //开始日期
    int       EndDate;                  //到期日期
    char      ExchangeType[4];          //交易类别
    char      OptionAccount[13];        //衍生品合约账户
    char      OptionCode[9];            //期权合约编码
    char      StockCode[16];            //证券代码
    char      PositionStr[100];         //定位串
    int       RequestNum;               //请求行数
};

//339800 历史期权委托查询应答
struct CHundsunRspFuncHisOptentrustQryField
{
    int       EntrustNo;                //委托编号
    char      ExchangeType[4];          //交易类别
    char      OptionAccount[13];        //衍生品合约账户
    char      OptionCode[9];            //期权合约编码
    char      OptcontractId[32];        //合约交易代码
    char      StockCode[16];            //证券代码
    char      OptionName[32];           //期权合约简称
    char      EntrustBs;                //买卖方向
    char      EntrustOc;                //开平仓方向
    char      CoveredFlag;              //备兑标志
    double    OptEntrustPrice;          //委托价格
    int       EntrustAmount;            //委托数量
    int       BusinessAmount;           //成交数量
    double    OptBusinessPrice;         //成交价格
    int       ReportNo;                 //申请编号
    int       EntrustDate;              //委托日期
    int       EntrustTime;              //委托时间
    int       ReportTime;               //申报时间
    char      EntrustType;              //委托类别
    char      EntrustSrc;               //委托来源
    char      EntrustProp[3];           //委托属性
    char      EntrustStatus;            //委托状态
    int       ErrorNo;                  //错误代码
    char      PositionStr[100];         //定位串
};

//339801 历史期权成交查询请求
struct CHundsunReqFuncHisOptbusinessQryField
{
    int       StartDate;                //开始日期
    int       EndDate;                  //到期日期
    char      ExchangeType[4];          //交易类别
    char      OptionAccount[13];        //衍生品合约账户
    char      OptionCode[9];            //期权合约编码
    char      PositionStr[100];         //定位串
    int       RequestNum;               //请求行数
};

//339801 历史期权成交查询应答
struct CHundsunRspFuncHisOptbusinessQryField
{
    int       InitDate;                 //交易日期
    int       SerialNo;                 //流水序号
    char      ExchangeType[4];          //交易类别
    char      OptionAccount[13];        //衍生品合约账户
    char      OptholdType;              //期权持仓类别
    char      OptionCode[9];            //期权合约编码
    char      OptcontractId[32];        //合约交易代码
    char      OptionName[32];           //期权合约简称
    char      StockCode[16];            //证券代码
    char      StockName[32];            //证券名称
    char      EntrustBs;                //买卖方向
    char      EntrustOc;                //开平仓方向
    char      CoveredFlag;              //备兑标志
    double    OptBusinessPrice;         //成交价格
    int       BusinessTime;             //成交时间
    char      BusinessStatus;           //业务状态
    int       BusinessTimes;            //分笔成交笔数
    int       EntrustNo;                //委托编号
    int       ReportNo;                 //申请编号
    int       OccurAmount;              //发生数量
    double    PostBalance;              //后资金额
    double    BusinessBalance;          //成交金额
    double    OccurBalance;             //发生金额
    int       PostAmount;               //后证券额
    double    Fare0;                    //佣金
    double    Fare1;                    //印花税
    double    Fare2;                    //过户费
    double    Fare3;                    //费用3
    double    Farex;                    //费用x
    char      Remark[2000];             //备注
    char      PositionStr[100];         //定位串
};

//339803 历史期权交割信息查询请求
struct CHundsunReqFuncHisOptdeliverQryField
{
    int       StartDate;                //开始日期
    int       EndDate;                  //到期日期
    char      DeliverType;              //交割标志
    char      ExchangeType[4];          //交易类别
    char      OptionAccount[13];        //衍生品合约账户
    char      OptionCode[9];            //期权合约编码
    char      MoneyType[3];             //币种类别
    char      PositionStr[100];         //定位串
    int       RequestNum;               //请求行数
};

//339803 历史期权交割信息查询应答
struct CHundsunRspFuncHisOptdeliverQryField
{
    int       InitDate;                 //交易日期
    int       EntrustDate;              //委托日期
    int       BusinessFlag;             //业务标志
    char      BusinessType;             //业务类型
    char      ExchangeType[4];          //交易类别
    char      OptionAccount[13];        //衍生品合约账户
    char      SeatNo[6];                //席位编号
    char      OptholdType;              //期权持仓类别
    char      OptionCode[9];            //期权合约编码
    char      OptcontractId[32];        //合约交易代码
    char      OptionName[32];           //期权合约简称
    char      StockCode[16];            //证券代码
    char      StockName[32];            //证券名称
    char      EntrustBs;                //买卖方向
    char      EntrustOc;                //开平仓方向
    char      CoveredFlag;              //备兑标志
    double    OptBusinessPrice;         //成交价格
    int       OccurAmount;              //发生数量
    double    BusinessBalance;          //成交金额
    double    OccurBalance;             //发生金额
    double    PostBalance;              //后资金额
    int       PostAmount;               //后证券额
    int       EntrustNo;                //委托编号
    char      BusinessId[16];           //成交编号
    int       ReportTime;               //申报时间
    int       BusinessTime;             //成交时间
    char      BusinessName[16];         //业务名称
    double    Fare0;                    //佣金
    double    Fare1;                    //印花税
    double    Fare2;                    //过户费
    double    Fare3;                    //费用3
    double    Farex;                    //费用x
    char      Remark[2000];             //备注
    int       ReportNo;                 //申请编号
    char      PositionStr[100];         //定位串
};

//339804 历史期权行权指派查询请求
struct CHundsunReqFuncHisExeassignQryField
{
    int       StartDate;                //开始日期
    int       EndDate;                  //到期日期
    char      ExchangeType[4];          //交易类别
    char      OptionAccount[13];        //衍生品合约账户
    char      OptholdType;              //期权持仓类别
    char      OptionCode[9];            //期权合约编码
    char      PositionStr[100];         //定位串
    int       RequestNum;               //请求行数
};

//339804 历史期权行权指派查询应答
struct CHundsunRspFuncHisExeassignQryField
{
    int       InitDate;                 //交易日期
    char      FundAccount[18];          //资产账户
    char      ExchangeType[4];          //交易类别
    char      OptionAccount[13];        //衍生品合约账户
    char      OptholdType;              //期权持仓类别
    char      OptionCode[9];            //期权合约编码
    char      OptionName[32];           //期权合约简称
    char      OptionType;               //期权种类
    char      StockCode[16];            //证券代码
    double    ExercisePrice;            //行权价格
    int       ExerciseAmount;           //行权数量
    double    ExefrozenBalance;         //行权冻结资金
    int       SettleAmount;             //交收数量
    double    SettleBalance;            //结算金额
    char      PositionStr[100];         //定位串
};

//339805 历史期权行权交割信息查询请求
struct CHundsunReqFuncHisOptexedeliverQryField
{
    int       StartDate;                //开始日期
    int       EndDate;                  //到期日期
    char      StockCode[16];            //证券代码
    char      PositionStr[100];         //定位串
    int       RequestNum;               //请求行数
};

//339805 历史期权行权交割信息查询应答
struct CHundsunRspFuncHisOptexedeliverQryField
{
    int       InitDate;                 //交易日期
    int       BranchNo;                 //分支机构
    char      ExchangeType[4];          //交易类别
    char      ClientId[18];             //客户编号
    char      FundAccount[18];          //资产账户
    char      OptionAccount[13];        //衍生品合约账户
    char      StockCode[16];            //证券代码
    char      StockType[4];             //证券类别
    int       ClearAmount;              //清算数量
    int       SettleAmount;             //交收数量
    int       ShortAmount;              //缺口数量
    int       TreatAmount;              //处置数量
    int       ReturnAmount;             //返还数量
    double    ClearBalance;             //清算金额
    double    SettleBalance;            //结算金额
    double    ShortBalance;             //缺口资金
    double    ReturnBalance;            //返还金额
    char      PositionStr[100];         //定位串
};

//339806 历史期权对账单查询请求
struct CHundsunReqFuncHisOptstatementQryField
{
    int       BeginDate;                //起始日期
    int       EndDate;                  //到期日期
    char      FundAccount[18];          //资产账户
    char      MoneyType[3];             //币种类别
    char      QueryMode;                //查询模式
    char      PositionStrLong[100];     //定位串（长）
    int       RequestNum;               //请求行数
};

//339806 历史期权对账单查询应答
struct CHundsunRspFuncHisOptstatementQryField
{
    int       InitDate;                 //交易日期
    int       BusinessFlag;             //业务标志
    char      BusinessType;             //业务类型
    char      BusinessName[16];         //业务名称
    char      FundAccount[18];          //资产账户
    char      BankNo[4];                //银行代码
    char      BankName[64];             //银行名称
    char      MoneyType[3];             //币种类别
    double    BusinessBalance;          //成交金额
    double    ClearBalance;             //清算金额
    char      ExchangeType[4];          //交易类别
    char      OptionAccount[13];        //衍生品合约账户
    char      OptionCode[9];            //期权合约编码
    char      OptionName[32];           //期权合约简称
    int       BusinessAmount;           //成交数量
    double    StandardFare0;            //标准佣金
    double    Fare0;                    //佣金
    double    Fare1;                    //印花税
    double    Fare2;                    //过户费
    double    Fare3;                    //费用3
    double    Farex;                    //费用x
    double    ExchangeFare;             //一级总费用
    double    ExchangeFare0;            //一级经手费
    double    ExchangeFare1;            //一级印花税
    double    ExchangeFare2;            //一级过户费
    double    ExchangeFare3;            //一级证管费
    double    ExchangeFare4;            //一级规费
    double    ExchangeFare5;            //一级手续费
    double    ExchangeFare6;            //其他费(结算费)
    double    ExchangeFarex;            //风险金
    char      PositionStrLong[100];     //定位串（长）
};

//339807 取客户账单请求
struct CHundsunReqFuncOptSettlementinfoQryField
{
    int       BeginDate;                //起始日期
    int       EndDate;                  //到期日期
    char      MoneyType[3];             //币种类别
};

//339807 取客户账单应答
struct CHundsunRspFuncOptSettlementinfoQryField
{
    char      CheckTabData[2000];       //帐单信息
};

//395 期权行情查询请求
struct CHundsunReqFuncOptPriceQryField
{
    char      ExchangeType[4];          //交易类别
    char      OptionCode[9];            //期权合约编码
};

//395 期权行情查询应答
struct CHundsunRspFuncOptPriceQryField
{
    double    OptLastPrice;             //最新价
    double    OptOpenPrice;             //开盘价
    double    OptHighPrice;             //最高价
    double    OptLowPrice;              //最低价
    double    BusinessBalance;          //成交金额
    int       BusinessAmount;           //成交数量
    double    OptBuyPrice1;             //申买价一
    double    OptBuyPrice2;             //申买价二
    double    OptBuyPrice3;             //申买价三
    double    OptBuyPrice4;             //申买价四
    double    OptBuyPrice5;             //申买价五
    double    OptSalePrice1;            //申卖价一
    double    OptSalePrice2;            //申卖价二
    double    OptSalePrice3;            //申卖价三
    double    OptSalePrice4;            //申卖价四
    double    OptSalePrice5;            //申卖价五
    int       BuyAmount1;               //申买量一
    int       BuyAmount2;               //申买量二
    int       BuyAmount3;               //申买量三
    int       BuyAmount4;               //申买量四
    int       BuyAmount5;               //申买量五
    int       SaleAmount1;              //申卖量一
    int       SaleAmount2;              //申卖量二
    int       SaleAmount3;              //申卖量三
    int       SaleAmount4;              //申卖量四
    int       SaleAmount5;              //申卖量五
};

//33100 主推-期货单腿行情主推
struct CHundsunRtnFuncQhSingleHqPushField
{
    char      QuoteType;                //行情类型(0-境内普通行情1-境外行情)
    int       QuoteLength;              //行情数据长度（从第3字段开始到最后一个字段计算长度）
    int       UpdateTime;               //时间戳(eg:144510500,14点45分10秒500毫秒)
    int       QuoteIndex;               //行情序号
    char      FutuExchType[4];          //交易类别
    char      CommodityType[6];         //商品代码
    char      FutuCode[30];             //合约代码
    double    PreSettlementPrice;       //昨日结算价
    double    PreClosePrice;            //昨日收盘价
    double    PreOpenInterest;          //昨日空盘量
    double    PreDelta;                 //昨日虚实度
    double    FutuOpenPrice;            //开盘价
    double    FutuLastPrice;            //最新价格
    double    BuyHighPrice;             //最高买入价
    double    BuyHighAmount;            //最高买价买入量
    double    LowSellPrice;             //最低卖价价格
    double    LowSellAmount;            //最低卖价卖量
    double    FutuHighPrice;            //最高价
    double    FutuLowPrice;             //最低价
    double    UplimitedPrice;           //涨停板
    double    DownlimitedPrice;         //跌停板
    double    AveragePrice;             //均价
    double    ChangeDirection;          //趋势
    double    BusinessAmount;           //成交量
    double    OpenInterest;             //总持量
    double    BusinessBalance;          //成交额
    double    FutuClosePrice;           //今日收盘价
    double    SquarePrice;              //结算价
    int       CurrDate;                 //当前日期(yyyymmdd)
};

//33101 委托回报主推
struct CHundsunRtnFuncQhEntrustPushField
{
    int       EntrustNo;                //委托号
    char      FuturesAccount[12];       //交易编码
    char      FutuExchType[3];          //交易所类别（F1-郑州F2大连-F3上海-F4中金）
    char      FutuCode[30];             //合约代码
    char      EntrustBs;                //买卖标识(1-买入 2-卖出)
    char      FuturesDirection;         //开平标识(1-开仓2-平仓4-平今仓，境外为空)
    char      HedgeType;                //套保标识(0-投机1-套保2-套利)
    char      FundAccount[18];          //资金账户
    char      ReportId[21];             //本地单号
    char      FirmId[21];               //会员号
    char      OperatorNo[18];           //操作员号
    int       ClientGroup;              //客户类别
    double    EntrustAmount;            //委托数量
    double    TotalBusinessAmount;      //成交总数量
    double    WithdrawAmount;           //撤单数量
    double    FutuEntrustPrice;         //委托价格
    char      EntrustStatus;            //委托状态
    int       BranchNo;                 //营业部号
    int       BatchNo;                  //委托批号/报价编号
    char      EntrustType;              //委托类型
    int       AmountPerHand;            //合约乘数
    char      ForcedropReason;          //强平原因
    int       InitDate;                 //交易日期
    int       EntrustTime;              //当前时间
    char      ConfirmId[20];            //主场单号
    char      EntrustOccasion[32];      //委托场景
    double    FutuEntrustPrice2;        //委托价格
    char      EntrustProp[3];           //期货委托属性(0：限价单；1：市价单 2：止损定单 3：止盈定单 4：限价止损定单 5：限价止盈定单 6：止损 7：组合定单 A：跨期套利确认；B：持仓套保确认；C：请求报价；D：期权权力行使；E：期权权力放弃；F：双边报价)
    char      ArbitCode[30];            //套利合约代码
    char      EntrustReference[32];     //委托引用
    char      ErrorMessage[32];         //错误说明
    char      PositionStr[32];          //定位串
    int       SessionNo;                //会话编号
    char      SecondCode[30];           //第二腿合约代码
    char      WeaveType;                //组合类型
    double    DeltaEntrustFare;         //委托费用变化
    double    DeltaEntrustMargin;       //委托保证金变化
    double    DeltaEntrustPremium;      //委托权利金变化
};

//33101 成交回报主推
struct CHundsunRtnFuncQhRealPushField
{
    int       EntrustNo;                //委托号
    char      FuturesAccount[12];       //交易编码
    char      FutuExchType[4];          //交易所类别
    int       BusinessNo;               //成交编号
    char      FutuCode[30];             //合约代码
    char      EntrustBs;                //买卖标识(1-买入 2-卖出)
    char      FuturesDirection;         //开平标识(1-开仓2-平仓3-平今仓  境外为空)
    double    FutuBusinessPrice;        //成交价格
    double    BusinessAmount;           //成交数量
    char      HedgeType;                //套保标识(0-投机1-套保2-套利)
    char      FundAccount[18];          //资金账户
    char      ReportId[21];             //本地单号
    char      FirmId[21];               //会员号
    char      OperatorNo[18];           //操作员号
    int       ClientGroup;              //客户类别
    double    EntrustAmount;            //委托数量
    double    TotalBusinessAmount;      //成交总数量
    double    WithdrawAmount;           //撤单数量
    double    FutuEntrustPrice;         //委托价格
    char      EntrustStatus;            //委托状态
    int       BranchNo;                 //营业部号
    int       BatchNo;                  //委托批号/报价编号
    char      EntrustType;              //委托类型
    int       AmountPerHand;            //合约乘数
    char      ForcedropReason;          //强平原因
    int       InitDate;                 //交易日期
    int       BusinessTime;             //成交时间
    char      ConfirmId[20];            //主场单号
    char      EntrustOccasion[32];      //委托场景
    double    FutuEntrustPrice2;        //委托数量(委托输入价格)
    char      EntrustProp[3];           //期货委托属性(0：限价单；1：市价单 2：止损定单 3：止盈定单 4：限价止损定单 5：限价止盈定单 6：止损 7：组合定单 A：跨期套利确认；B：持仓套保确认；C：请求报价；D：期权权力行使；E：期权权力放弃；F：双边报价)
    char      ArbitCode[30];            //套利合约代码
    char      EntrustReference[32];     //委托引用
    char      PositionStr[32];          //定位串
    int       SessionNo;                //会话编号
    double    DeltaEntrustFare;         //委托费用变化
    double    DeltaBusinessFare;        //成交费用变化
    double    DeltaEntrustMargin;       //委托保证金变化
    double    DeltaHoldMargin;          //持仓保证金变化
    double    DeltaEntrustPremium;      //委托权利金变化
    double    DeltaPremium;             //成交权利金变化
    double    DeltaDropIncome;          //平仓盯市盈亏
    double    DeltaDropIncomeFloat;     //平仓浮动盈亏
};

//33102 主推-期货组合行情主推
struct CHundsunRtnFuncQhCombinePushField
{
    char      ArbitCode[30];            //套利合约号
    char      FutuExchType[4];          //交易类别
    char      FirstCode[30];            //第一腿
    char      SecondCode[30];           //第二腿
    char      WeaveType;                //组合类型1-SPD, 2 -IPS
    double    FutuBuyPrice;             //最高买入价
    double    BuyAmount;                //最高买入价买量
    double    TotalBuyAmount;           //全部买量
    double    FutuSellPrice;            //最低卖价价格
    double    SellAmount;               //最低卖价卖量
    double    TotalSaleAmount;          //全部卖量
    double    FutuHighPrice;            //最高价
    double    FutuLowPrice;             //最低价
    double    UplimitedPrice;           //涨停板价格
    double    DownlimitedPrice;         //跌停板价格
};

//33105 主推-期货交易所状态信息主推
struct CHundsunRtnFuncQhExchStatusPushField
{
    char      FutuExchType[4];          //交易所类别（F1-郑州F2大连-F3上海-F4中金）
    char      ExchStatus;               //交易所状态（0连接断开,1开盘前, 2集合报价 3竞价平衡 4竞价撮合 5 连续交易 6暂停交易 7闭市 X未知）
    char      EntradeFlag;              //是否可交易（0不可交易，1可交易）
};

//33300 主推-期货询价通知主推
struct CHundsunRtnFuncQhInquiryPushField
{
    int       InquiryNo;                //询价编号
    char      FutuCode[30];             //合约代码
    int       InitDate;                 //交易日期
    int       UpdateTime;               //询价时间(eg:144510500,14点45分10秒500毫秒)
};

//33301 主推-期货报价回报主推
struct CHundsunRtnFuncQhQuotePushField
{
    char      FutuExchType[3];          //交易所类别（F1-郑州F2大连-F3上海-F4中金）
    char      FutuCode[30];             //合约代码
    char      BuyFuturesDirection;      //买方开平标志
    char      BuyHedgeType;             //买方套保标志
    double    BuyQuotePrice;            //买方价格
    double    BuyQuoteAmount;           //买方量
    char      SellFuturesDirection;     //卖方开平标志
    char      SellHedgeType;            //卖方套保标志
    double    SellQuotePrice;           //卖方价格
    double    SellQuoteAmount;          //卖方量
    int       QuoteNo;                  //报价编号
    char      FundAccount[18];          //资金账户
    char      ReportId[21];             //本地单号
    char      FirmId[21];               //会员号
    char      OperatorNo[18];           //操作员号
    char      QuoteStatus;              //委托状态
    int       BranchNo;                 //营业部号
    int       InitDate;                 //交易日期
    int       CurrTime;                 //当前时间
    char      ConfirmId[20];            //主场单号
    char      BuyConfirmId[20];         //买方主场单号
    char      SellConfirmId[20];        //卖方主场单号
    char      EntrustOccasion[32];      //委托场景
    char      EntrustReference[32];     //报价引用
    char      RequestId[32];            //请求编号
    char      PositionStr[32];          //定位串
};
//33011 主推-个股期权成交回报主推
struct CHundsunRtnFuncOptDealPushField
{
    int       InitDate;                 //交易日期
    int       BranchNo;                 //分支机构
    char      ClientId[18];             //客户编号
    char      FundAccount[18];          //资产账户
    char      RealType;                 //成交类型
    char      RealStatus;               //处理标志
    char      ExchangeType[4];          //交易类别
    char      ReportSeat[8];            //申报席位
    int       ReportNo;                 //申请编号
    char      ReportAccount[11];        //申报账号
    char      StockAccount[11];         //证券账号
    char      OptholdType;              //期权持仓类别
    char      OptionCode[8];            //期权合约编码
    char      OptionType;               //期权种类
    int       RecordNo;                 //委托库记录号
    char      ReportBs;                 //申报方向
    char      EntrustBs;                //买卖方向
    char      EntrustOc;                //开平仓方向
    char      CoveredFlag;              //备兑标志
    char      EntrustStatus;            //委托状态
    int       EntrustNo;                //委托编号
    char      BusinessId[16];           //成交编号
    double    BusinessAmount;           //成交数量
    double    OptBusinessPrice;         //成交价格
    int       BusinessTime;             //成交时间
    char      ExternCode[8];            //外部错误代码
    int       ReportTime;               //申报时间
    double    EntrustAmount;            //委托数量
    int       BatchNo;                  //委托批号
    double    OptEntrustPrice;          //委托价格
    char      EntrustType;              //委托类别
};

//33012 主推-个股期权委托回写主推
struct CHundsunRtnFuncOptEntrustPushField
{
    int       BranchNo;                 //分支机构
    char      FundAccount[18];          //资产账户
    int       ReportNo;                 //申请编号
    char      OptionCode[8];            //期权合约编码
    char      EntrustBs;                //买卖方向
    char      EntrustOc;                //开平仓方向
    char      CoveredFlag;              //备兑标志
    char      EntrustStatus;            //委托状态
    int       EntrustNo;                //委托编号
    double    EntrustAmount;            //委托数量
    int       BatchNo;                  //委托批号
    double    OptEntrustPrice;          //委托价格
    char      EntrustType;              //委托类别
    int       ErrorNo;                  //错误代码
    char      ErrorInfo[20];            //错误提示
};

//33099 实时行情主推主推
struct CHundsunRtnFuncOptHqPushField
{
    char      Opthq[1024];              //行情信息内容（所有组成字段以字符串格式存放，中间以ASCII值1分割）
    char      HqsrcType;                //行情源类型
    char      ExchangeType[4];          //交易类别
    char      OptionCode[9];            //期权合约编码
    char      OptionName[32];           //期权合约简称
    double    ClosePrice;               //昨收盘
    double    OpenPrice;                //开盘价
    double    BusinessBalance;          //成交金额
    double    HighPrice;                //最高价
    double    LowPrice;                 //最低价
    double    LastPrice;                //最新价
    int       BusinessAmount;           //成交数量
    double    OptBuyPrice1;             //申买价一
    double    OptBuyPrice2;             //申买价二
    double    OptBuyPrice3;             //申买价三
    double    OptBuyPrice4;             //申买价四
    double    OptBuyPrice5;             //申买价五
    double    OptSalePrice1;            //申卖价一
    double    OptSalePrice2;            //申卖价二
    double    OptSalePrice3;            //申卖价三
    double    OptSalePrice4;            //申卖价四
    double    OptSalePrice5;            //申卖价五
    int       BuyAmount1;               //申买量一
    int       BuyAmount2;               //申买量二
    int       BuyAmount3;               //申买量三
    int       BuyAmount4;               //申买量四
    int       BuyAmount5;               //申买量五
    int       SaleAmount1;              //申卖量一
    int       SaleAmount2;              //申卖量二
    int       SaleAmount3;              //申卖量三
    int       SaleAmount4;              //申卖量四
    int       SaleAmount5;              //申卖量五
    char      ExchangeIndex[32];        //交易指数
    char      CloseExchangeIndex;       //闭市索引
    double    PreSquarePrice;           //昨日结算价
    double    SquarePrice;              //结算价
    double    AuctionPrice;             //动态参考价格
    double    AuctionAmount;            //虚拟匹配数量
    double    UndropAmount;             //未平仓数量
    char      OptexchStatus;            //期权交易状态
    char      OptOpenRestriction[64];   //期权实时开仓限制
};

#endif
