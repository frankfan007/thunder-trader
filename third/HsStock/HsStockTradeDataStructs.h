#ifndef _COMMONFILES_HSSTOCK_HSSTOCKTRADEDATASTRUCTS_H_
#define _COMMONFILES_HSSTOCK_HSSTOCKTRADEDATASTRUCTS_H_
struct CHsStockTradeRspInfoField
{
	///错误代码
	unsigned int ErrorID;
	///错误信息
	char ErrorMsg[256];
};

struct CHsStockTradeAddressField
{
	
};

struct CHsStockTradeReqUserLoginField
{
	char m_strAccount[64];
	char m_strPassword[64];
};

struct CHsStockTradeRspUserLoginField
{
	
	char	error_info[20 + 1];
	char	content_type[6 + 1];
	char	account_content[30 + 1];
	char	client_id[18 + 1];
	char	client_name[14 + 1];
	char	client_rights[64 + 1];
	char	bank_no[4 + 1];
	char	exchange_type[4 + 1];
	char	exchange_name[8 + 1];
	char	stock_account[11 + 1];
	char	last_op_station[12 + 1];
	char	last_op_ip[20 + 1];
	char	company_name[64 + 1];
	char	sys_status_name[64 + 1];
	char	remark[255 + 1];
	char	corp_risklevel_name[32 + 1];

	char	money_type;
	char	square_flag;
	char	last_op_entrust_way;
	char	bank_trans_flag;
	char	tabconfirm_flag;
	char	initpasswd_flag;
	char	message_flag;
	char	sys_status;
	char	valid_flag;

	int		error_no;//N4
	int		branch_no;//	N5
	int		fund_account;//	N9
	int		online_time;//	N8
	int		fundaccount_count;//	N2
	int		money_count;//	N2
	int		login_date;//	N8
	int		login_time;//	N8
	int		init_date;//	N8
	int		last_date;//	N8
	int		corp_client_group;//	N5
	int		corp_risk_level;//	N5
	int		corp_end_date;//	N8

	double	enable_balance;//	N16.2
	double	current_balance;//	N16.2

};

struct CHsStockTradeRspQueryMoneyField
{
	char money_type;			//	C1	币种类别
	double current_balance;		//	N16.2	当前余额
	double enable_balance;		//	N16.2	可用金额
	double fetch_balance;		//	N16.2	可取金额
	double interest;			//	N16.2	待入账利息
	double asset_balance;		//	N16.2	资产总值（不含基金市值）
	double fetch_cash;			//	N16.2	可取现金
	double fund_balance;		//	N16.2	资金（ = 资产总值 - 证券市值）
	double market_value;		//	N16.2	证券市值
	double opfund_market_value;	//	N16.2	基金市值
	double pre_interest;		//	N16.2	预计利息
};

#define RTN_ORDER_STATE_NO_PARTLY_TRADED '1'
#define RTN_ORDER_STATE_NO_ALL_TRADED '2'
#define RTN_ORDER_STATE_NO_CANCELED '3'
struct CHsStockTradeOrderField
{
	///报单编号
	unsigned int m_intEntrustNo;
	///报单状态
	char m_chOrderStatus;
	///今成交数量
	unsigned int m_uVolumeTraded;
	///剩余数量
	unsigned int m_uVolumeTotal;
};

struct CHsStockTradeTradeField
{
	
};


#define EXCHANGE_TYPE_UNKNOW "0"
#define EXCHANGE_TYPE_SH "1"
#define EXCHANGE_TYPE_SZ "2"
#define EXCHANGE_TYPE_X_A "9"
#define EXCHANGE_TYPE_X_B "A"
#define EXCHANGE_TYPE_SH_B "D"
#define EXCHANGE_TYPE_SZ_B "H"
#define EXCHANGE_TYPE_CZCE "F1"
#define EXCHANGE_TYPE_DCE "F2"
#define EXCHANGE_TYPE_SHFE "F3"
#define EXCHANGE_TYPE_CFFEX "F4"


#define ENTRUST_PROP_DEFAULT '0'

#define ENTRUST_BS_BUY '1'
#define ENTRUST_BS_SELL '2'

struct CHsStockTradeInputOrderField
{
	char m_strStockAccount[11+1];//  C11 股东代码
	int m_intFundAccount;			//	N9	资金账号，必须输入
	char m_strExchangeType[4+1];	//	C4	交易类别，必须输入确定的市场，不支持‘0‘或空格。
	char m_strStockCode[6 + 1];		//	C6	证券代码，必须输入确定的股票代码，不支持内码。
	double m_dbEntrustAmount;		//	N16.2	委托数量
	double m_dbEntrustPrice;		//	N9.3	委托价格
	char m_chEntrustProp;			//	C1	委托属性，默认送'0' - 买卖，对开放式基金申购赎回和权证行权等业务，需要传入特定的委托属性，以和普通买卖区分。（v3.1增加
	char m_chEntrustBs;			//	C1	买卖方向 1 买入 2 卖出
};

struct CHsStockInputOrderCancelField
{
	
	int m_intEntrust_no;//	N8	委托编号, 使用批量撤单标志batch_flag为‘1‘，则将理解为批号，其它为委托编号，如果为0，则如果批量标志为‘1’，那么批量撤单账号整个市场的。

};

struct CHsStockInputOrderTradeField
{
	char m_strStockAccount[11 + 1];//  C11 股东代码
	char m_strStockCode[6 + 1];		//	C6	证券代码，必须输入确定的股票代码，不支持内码。
};

struct CHsStockInputOrderEntrustField
{
	char m_strStockAccount[11 + 1];//  C11 股东代码
	unsigned int m_intEntrust_no;//  N8 指定委托号，查指定委托号的委托，且request_num为1，且前台在收到应答后要检查返回的委托的委托号是否与要求查询的一致，此时exchange_type和stock_code要为空串。
};
#define RSP_BUSINESS_STATUS_TRADED 0
#define RSP_BUSINESS_STATUS_DISABLED 2
#define RSP_BUSINESS_STATUS_CANCELED 4

#define RSP_BUSINESS_TYPE_BUY_SELL 0
#define RSP_BUSINESS_TYPE_CANCEL 2

#define RSP_ENTRUST_BS_BUY 0
#define RSP_ENTRUST_BS_SELL 1
struct CHsStockOutputOrderTradeField
{
	char m_strPosition_str[32+1];		//	C32	定位串
	int m_intSerial_no;					//	N8	流水号（暂不使用）
	int m_intDate;						//	N8	日期
	char m_strExchange_type[4 + 1];		//	C4	交易类别
	char m_strStock_account[11+1];		//	C11	证券账号
	char m_strStock_code[6+1];			//	C6	证券代码
	char m_strStock_name[8+1];			//	C8	证券名称
	int m_intEntrust_bs;				//	C1	买卖方向
	char m_strBs_name[60+1];			//	C60	买卖方向名称
	double m_dbBusiness_price;			//	N9.3	成交价格
	double m_dbBusiness_amount;			//	N16.2	成交数量
	int m_intBusiness_time;				//	N8	成交时间
	char m_chBusiness_status;			//	C1	成交状态0：‘成交’，2：‘废单’4：‘确认’
	char m_strStatus_name[60+1];		//	C60	成交状态名称
	int m_intBusiness_type;				//	C1	成交类别，0：‘买卖’2：‘撤单’
	char m_strType_name[64+1];			//	C64	成交类别名称
	int m_intBusiness_times;			//	N5	成交笔数
	int m_intEntrust_no;				//	N8	合同号
	int m_intReport_no;					//	N8	申报号
	double m_dbBusiness_balance;		//	N16.2	成交金额
	int m_intBusiness_no;				//	N8	成交编号

};
struct CHsStockOutputOrderEntrustField
{
	char m_strPosition_str[32 + 1];			//	C32	定位串
	int m_intEntrust_no;					//	N8	委托序号
	char m_strExchange_type[4 + 1];			//	C4	交易类别
	char m_strStock_account[11 + 1];		//	C11	证券账号
	char m_strStock_code[6+1];				//	C6	证券代码
	char m_strStock_name[8+1];				//	C8	证券名称
	char m_chEntrust_bs;					//	C1	买卖方向
	char m_strBs_name[60+1];				//	C60	买卖方向名称
	double m_dbEntrust_price;				//	N9.3	委托价格
	double m_dbEntrust_amount;				//	N16.2	委托数量
	double m_dbBusiness_amount;				//	N16.2	成交数量
	double m_dbBusiness_price;				//	N9.3	成交价格
	int m_intReport_no;						//	N8	申报号
	int m_intReport_time;					//	N8	申报时间
	char m_chEntrust_type;					//	C1	委托类别
	char m_strType_name[64+1];				//	C64	委托类别名称
	char m_chEntrust_status;				//	C1	委托状态
	char m_strStatus_name[60+1];			//	C60	状态名称
	char m_strCancel_info[20+1];			//	C20	废单原因
	char m_chEntrust_prop;					//	C1	委托属性
	char m_chEntrust_prop_name;				//	C1	委托属性
	char m_chEntrust_way;					//	C1	委托方式
	char m_strEntrust_way_name[255+1];		//	C255	委托方式名称
};


#endif
