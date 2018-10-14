
#include <string>
#include <memory>
#include "mysql_driver.h"
#include "cppconn/driver.h"
#include "cppconn/exception.h"
#include "cppconn/resultset.h"
#include "cppconn/prepared_statement.h"
#include "cppconn/prepared_statement.h"
#include "StockTick.h"
#include "TwsTick.h"
#include <algorithm>
#include "Tick.h"
#include <vector>
#include "TickLoader.h"
#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <unordered_map>
#include <fstream>
#include <set>
#include "oci.h"
#include "ocilib\ocilib.h"
#include "boost/serialization/serialization.hpp"  
#include "boost/archive/binary_oarchive.hpp"  
#include "boost/archive/binary_iarchive.hpp"  
#include <boost/serialization/export.hpp>  
#include <boost/property_tree/ptree.hpp>  
#include <boost/property_tree/json_parser.hpp> 
#include <boost/serialization/vector.hpp>  
#include <sstream>  
using namespace std::tr1;
using namespace boost::posix_time;
using namespace boost::gregorian;
using namespace std;

#define ShowLog
#ifdef ShowLog
void betaLog(char * fmt, ...);
#define RunTimeLog(fmt,...) betaLog(fmt,__VA_ARGS__)
#else
#define RunTimeLog(fmt,...)
#endif
class MTickDataContainerInterface
{
public:
	virtual CTick* operator[](unsigned int) = 0;
	virtual void Release() = 0;
	virtual void LoadFromDB(const char * HostAddress,
		const char * UserName,
		const char * PassWord,
		const char * DataBase,
		const char * TableName,
		const char * instrumentId,
		ptime begin,ptime end) = 0;
	virtual void LoadFromFile(std::tr1::shared_ptr<ifstream> filein) = 0;
	virtual void SaveToFile(std::tr1::shared_ptr<ifstream> filein) = 0;
};


template<class T>
void LoadFromDatabase(const char * jsonstring,vector<T> & m_vecTicks)
{

}

std::shared_ptr<sql::ResultSet> GetResult(string sqls, map<string, string> & config)
{
	if (config.find("HostAddress") == config.end())
		throw std::exception("Can not find <HostAddress> in the json string.");
	if (config.find("UserName") == config.end())
		throw std::exception("Can not find <UserName> in the json string.");
	if (config.find("PassWord") == config.end())
		throw std::exception("Can not find <PassWord> in the json string.");
	if (config.find("DataBase") == config.end())
		throw std::exception("Can not find <DataBase> in the json string.");
	sql::Driver *m_sqlDriver = NULL;
	sql::Connection *m_sqlConnector = NULL;
	sql::ResultSet * DataRes = NULL;
	try {
		m_sqlDriver = get_driver_instance();
		m_sqlConnector = m_sqlDriver->connect(
			sql::SQLString(config["HostAddress"].c_str()),
			sql::SQLString(config["UserName"].c_str()),
			sql::SQLString(config["PassWord"].c_str())
			);
		m_sqlConnector->setSchema(sql::SQLString(config["DataBase"].c_str()));
		auto _setgbk = m_sqlConnector->createStatement();
		_setgbk->execute("SET NAMES GBK");
		delete _setgbk;
	}
	catch (sql::SQLException &e) {
		return NULL;
	}



	try {
		if (m_sqlConnector == NULL)
		{
			cout << "无法连接数据库，请检查您的网络是否可用" << endl;
			return false;
		}
		auto stmtData = m_sqlConnector->createStatement();
		DataRes = stmtData->executeQuery(sql::SQLString(sqls.c_str()));
		delete stmtData;

	}
	catch (sql::SQLException &e) {
		cout << e.what() << endl;
	}
	return std::shared_ptr<sql::ResultSet>(DataRes);
}

template<>
void LoadFromDatabase<CFutureTick>(
	const char * jsonstring,
	vector<CFutureTick> & res)
{
	res.clear();
	boost::property_tree::ptree root;
	boost::property_tree::read_json<boost::property_tree::ptree>(jsonstring, root);
	map<string, string> config;
	for (auto & node : root)
		config[node.first] = node.second.data();

	
	if (config.find("TableName") == config.end())
		throw std::exception("Can not find <TableName> in the json string.");
	if (config.find("InstrumentID") == config.end())
		throw std::exception("Can not find <InstrumentID> in the json string.");
	if (config.find("BeginTime") == config.end())
		throw std::exception("Can not find <BeginTime> in the json string.");
	if (config.find("EndTime") == config.end())
		throw std::exception("Can not find <EndTime> in the json string.");
	ptime begin = from_iso_string(config["BeginTime"]);
	ptime end = from_iso_string(config["EndTime"]);
	char sqlBuf[512];
	sprintf_s(sqlBuf, "select * from InstrumentID=\'%s\' where %s and ActionDateTime between '%s' and '%s' order by ActionDateTime,Microseconds  asc",
		config["TableName"].c_str(),
		config["InstrumentID"].c_str(),
		(to_iso_extended_string(begin.date()) + ' ' + to_simple_string(begin.time_of_day())).c_str(),
		(to_iso_extended_string(end.date()) + ' ' + to_simple_string(end.time_of_day())).c_str()
		);
	unsigned int TickCounter = 0;
	auto DataRes = GetResult(sqlBuf, config);
	DataRes->beforeFirst();
	while (DataRes->next())
	{
		res.push_back(CFutureTick());
		CFutureTick &_tick = res[res.size() - 1];
		strcpy_s(_tick.m_strInstrumentID, DataRes->getString("InstrumentID").c_str());
		_tick.m_intVolume = DataRes->getUInt("Volume");
		_tick.m_dbLowerLimitPrice = 0;
		_tick.m_dbUpperLimitPrice = 0;
		_tick.m_dbAveragePrice = DataRes->getDouble("AveragePrice");
		_tick.m_dbTurnover = DataRes->getDouble("Turnover");
		_tick.m_dbOpenInterest = DataRes->getDouble("OpenInterest");
		_tick.m_dbLastPrice = DataRes->getDouble("LastPrice");
		_tick.m_dbBidPrice1 = DataRes->getDouble("BidPrice1");
		_tick.m_dbBidPrice2 = DataRes->getDouble("BidPrice2");
		_tick.m_dbBidPrice3 = DataRes->getDouble("BidPrice3");
		_tick.m_dbBidPrice4 = DataRes->getDouble("BidPrice4");
		_tick.m_dbBidPrice5 = DataRes->getDouble("BidPrice5");
		_tick.m_intBidVolume1 = DataRes->getInt("BidVolume1");
		_tick.m_intBidVolume2 = DataRes->getInt("BidVolume2");
		_tick.m_intBidVolume3 = DataRes->getInt("BidVolume3");
		_tick.m_intBidVolume4 = DataRes->getInt("BidVolume4");
		_tick.m_intBidVolume5 = DataRes->getInt("BidVolume5");
		_tick.m_dbAskPrice1 = DataRes->getDouble("AskPrice1");
		_tick.m_dbAskPrice2 = DataRes->getDouble("AskPrice2");
		_tick.m_dbAskPrice3 = DataRes->getDouble("AskPrice3");
		_tick.m_dbAskPrice4 = DataRes->getDouble("AskPrice4");
		_tick.m_dbAskPrice5 = DataRes->getDouble("AskPrice5");
		_tick.m_intAskVolume1 = DataRes->getInt("AskVolume1");
		_tick.m_intAskVolume2 = DataRes->getInt("AskVolume2");
		_tick.m_intAskVolume3 = DataRes->getInt("AskVolume3");
		_tick.m_intAskVolume4 = DataRes->getInt("AskVolume4");
		_tick.m_intAskVolume5 = DataRes->getInt("AskVolume5");
		string at = DataRes->getString("ActionDateTime").c_str();
		string tt = DataRes->getString("TradingDateTime").c_str();
		auto Microsecond = microseconds(DataRes->getInt("Microseconds"));
		_tick.m_datetimeUTCDateTime = ptime(
			from_simple_string(at.substr(0, 10)),
			duration_from_string(at.substr(11, 8)) + Microsecond);

	}
}

template<>
void LoadFromDatabase<COptionTick>(
	const char * jsonstring,
	vector<COptionTick> & res)
{
	res.clear();
	boost::property_tree::ptree root;
	boost::property_tree::read_json<boost::property_tree::ptree>(jsonstring, root);
	map<string, string> config;
	for (auto & node : root)
		config[node.first] = node.second.data();

	if (config.find("TableName") == config.end())
		throw std::exception("Can not find <TableName> in the json string.");
	if (config.find("InstrumentID") == config.end())
		throw std::exception("Can not find <InstrumentID> in the json string.");
	if (config.find("BeginTime") == config.end())
		throw std::exception("Can not find <BeginTime> in the json string.");
	if (config.find("EndTime") == config.end())
		throw std::exception("Can not find <EndTime> in the json string.");
	ptime begin = from_iso_string(config["BeginTime"]);
	ptime end = from_iso_string(config["EndTime"]);
	char sqlBuf[512];
	sprintf_s(sqlBuf, "select * from %s where OptionCode=\'%s\' and ActionDateTime between '%s' and '%s' order by ActionDateTime,Microseconds asc",
		config["TableName"].c_str(),
		config["InstrumentID"].c_str(),
		(to_iso_extended_string(begin.date()) + ' ' + to_simple_string(begin.time_of_day())).c_str(),
		(to_iso_extended_string(end.date()) + ' ' + to_simple_string(end.time_of_day())).c_str()
		);
	unsigned int TickCounter = 0;
	auto DataRes = GetResult(sqlBuf, config);
	DataRes->beforeFirst();
	while (DataRes->next())
	{
		res.push_back(COptionTick());
		COptionTick &_tick = res[res.size() - 1];
		strcpy_s(_tick.m_strInstrumentID, DataRes->getString("OptionCode").c_str());
		_tick.m_intVolume = DataRes->getUInt("BusinessAmount");
		_tick.m_dbLastPrice = DataRes->getDouble("LastPrice");
		_tick.m_dbBidPrice1 = DataRes->getDouble("OptBuyPrice1");
		_tick.m_dbBidPrice2 = DataRes->getDouble("OptBuyPrice2");
		_tick.m_dbBidPrice3 = DataRes->getDouble("OptBuyPrice3");
		_tick.m_dbBidPrice4 = DataRes->getDouble("OptBuyPrice4");
		_tick.m_dbBidPrice5 = DataRes->getDouble("OptBuyPrice5");
		_tick.m_intBidVolume1 = DataRes->getInt("BuyAmount1");
		_tick.m_intBidVolume2 = DataRes->getInt("BuyAmount2");
		_tick.m_intBidVolume3 = DataRes->getInt("BuyAmount3");
		_tick.m_intBidVolume4 = DataRes->getInt("BuyAmount4");
		_tick.m_intBidVolume5 = DataRes->getInt("BuyAmount5");
		_tick.m_dbAskPrice1 = DataRes->getDouble("OptSalePrice1");
		_tick.m_dbAskPrice2 = DataRes->getDouble("OptSalePrice2");
		_tick.m_dbAskPrice3 = DataRes->getDouble("OptSalePrice3");
		_tick.m_dbAskPrice4 = DataRes->getDouble("OptSalePrice4");
		_tick.m_dbAskPrice5 = DataRes->getDouble("OptSalePrice5");
		_tick.m_intAskVolume1 = DataRes->getInt("SaleAmount1");
		_tick.m_intAskVolume2 = DataRes->getInt("SaleAmount2");
		_tick.m_intAskVolume3 = DataRes->getInt("SaleAmount3");
		_tick.m_intAskVolume4 = DataRes->getInt("SaleAmount4");
		_tick.m_intAskVolume5 = DataRes->getInt("SaleAmount5");

		string at = DataRes->getString("ActionDateTime").c_str();
		auto Microsecond = microseconds(DataRes->getInt("Microseconds"));
		_tick.m_datetimeUTCDateTime = ptime(
			from_simple_string(at.substr(0, 10)),
			duration_from_string(at.substr(11, 8)) + Microsecond);
	}
}

template<>
void LoadFromDatabase<CForexTick>(
	const char * jsonstring,
	vector<CForexTick> & res)
{
	boost::property_tree::ptree root;
	boost::property_tree::read_json<boost::property_tree::ptree>(jsonstring, root);
	map<string, string> config;
	for (auto & node : root)
		config[node.first] = node.second.data();

	if (config.find("TableName") == config.end())
		throw std::exception("Can not find <TableName> in the json string.");
	if (config.find("InstrumentID") == config.end())
		throw std::exception("Can not find <InstrumentID> in the json string.");
	if (config.find("Currency") == config.end())
		throw std::exception("Can not find <Currency> in the json string.");
	if (config.find("BeginTime") == config.end())
		throw std::exception("Can not find <BeginTime> in the json string.");
	if (config.find("EndTime") == config.end())
		throw std::exception("Can not find <EndTime> in the json string.");
	ptime begin = from_iso_string(config["BeginTime"]);
	ptime end = from_iso_string(config["EndTime"]);
	char sqlBuf[512];
	sprintf_s(sqlBuf, "select * from %s where (InstrumentID=\'%s\' and Currency=\'%s\') and ActionDateTime between '%s' and '%s' order by TickID  asc",
		config["TableName"].c_str(),
		config["InstrumentID"].c_str(),
		config["Currency"].c_str(),
		(to_iso_extended_string(begin.date()) + ' ' + to_simple_string(begin.time_of_day())).c_str(),
		(to_iso_extended_string(end.date()) + ' ' + to_simple_string(end.time_of_day())).c_str()
		);
	unsigned int TickCounter = 0;
	auto DataRes = GetResult(sqlBuf, config);

	DataRes->beforeFirst();
	while (DataRes->next())
	{
		res.push_back(CForexTick());
		CForexTick &_tick = res[res.size() - 1];
		string Instrument = DataRes->getString("InstrumentID").c_str();
		string Currency = DataRes->getString("Currency").c_str();
		strcpy_s(
			_tick.m_strInstrumentID, (Instrument + "." + Currency).c_str()
			);
		_tick.m_intVolume = -1;

		_tick.m_dbBidPrice1 = DataRes->getDouble("BidPrice");
		_tick.m_intBidVolume1 = DataRes->getInt("BidSize");
		_tick.m_dbAskPrice1 = DataRes->getDouble("AskPrice");
		_tick.m_intAskVolume1 = DataRes->getInt("AskSize");
		_tick.m_dbLastPrice = _tick.m_dbBidPrice1;
		auto Microsecond = microseconds(DataRes->getInt("Microseconds"));
		string at = DataRes->getString("ActionDateTime").c_str();
		_tick.m_datetimeUTCDateTime = ptime(
			from_simple_string(at.substr(0, 10)),
			duration_from_string(at.substr(11, 8)) + Microsecond);

	}
}

template<>
void LoadFromDatabase<CStockTick>(
	const char * jsonstring,
	vector<CStockTick> & res
	)
{
	boost::property_tree::ptree root;
	boost::property_tree::read_json<boost::property_tree::ptree>(jsonstring, root);
	map<string, string> config;
	for (auto & node : root)
		config[node.first] = node.second.data();

	if (config.find("HostAddress") == config.end())
		throw std::exception("Can not find <HostAddress> in the json string.");
	if (config.find("UserName") == config.end())
		throw std::exception("Can not find <UserName> in the json string.");
	if (config.find("PassWord") == config.end())
		throw std::exception("Can not find <PassWord> in the json string.");
	if (config.find("DataBase") == config.end())
		throw std::exception("Can not find <DataBase> in the json string.");
	if (config.find("TableName") == config.end())
		throw std::exception("Can not find <TableName> in the json string.");
	if (config.find("InstrumentID") == config.end())
		throw std::exception("Can not find <InstrumentID> in the json string.");
	if (config.find("BeginTime") == config.end())
		throw std::exception("Can not find <BeginTime> in the json string.");
	if (config.find("EndTime") == config.end())
		throw std::exception("Can not find <EndTime> in the json string.");
	OCI_Connection* conn = NULL;
	OCI_Statement* stmt = NULL;
	OCI_Resultset * rs = NULL;
	ptime begin=from_iso_string(config["BeginTime"]);
	ptime end = from_iso_string(config["EndTime"]);
	string OverAllSQL;
	if (begin.date().year() == end.date().year()
		&&
		begin.date().month() == end.date().month()
		)//如果所选数据是一个月以内的数据
	{
		string TimeCondition;
		char sqlBuf[1024];

		sprintf_s(sqlBuf, "select secucode, tradingday,precloseprice, lastprice,turnvol,openprice, highprice, lowprice , "
			"sp1, sp2, sp3, sp4, sp5, bp1, bp2, bp3, bp4, bp5, sv1, sv2, sv3, sv4, sv5, bv1, bv2, bv3, bv4, bv5 "
			"from %s%.4d%.2d "
			"where secucode=\'%s\' "
			"and TRADINGDAY between "
			"to_date('%s', 'yyyy-mm-dd hh24:mi:ss') and "
			"to_date('%s', 'yyyy-mm-dd hh24:mi:ss') ",
			config["InstrumentID"].c_str(),
			(unsigned int)begin.date().year(),
			begin.date().month().as_number(),
			config["TableName"].c_str(),
			(to_iso_extended_string(begin.date()) + ' ' + to_simple_string(begin.time_of_day())).c_str(),
			(to_iso_extended_string(end.date()) + ' ' + to_simple_string(end.time_of_day())).c_str()
			);
		OverAllSQL = sqlBuf;

	}
	else//如果所选数据是跨两个月或者以上
	{
		ptime currentdate = begin;
		ptime lastday = begin - months(1);
		while (currentdate <= end)
		{
			if (currentdate.date().month() != lastday.date().month())
			{
				if (currentdate.date().year() == begin.date().year()
					&&
					currentdate.date().month() == begin.date().month()
					)//最开始的一个月
				{

					string TimeCondition;
					char sqlBuf[1024];

					sprintf_s(sqlBuf, "select secucode, tradingday,precloseprice, lastprice,turnvol,openprice, highprice, lowprice , "
						"sp1, sp2, sp3, sp4, sp5, bp1, bp2, bp3, bp4, bp5, sv1, sv2, sv3, sv4, sv5, bv1, bv2, bv3, bv4, bv5 "
						"from %s%.4d%.2d "
						"where secucode=\'%s\' "
						"and TRADINGDAY between "
						"to_date('%s', 'yyyy-mm-dd hh24:mi:ss') and "
						"to_date('%s', 'yyyy-mm-dd hh24:mi:ss') ",

						config["TableName"].c_str(),
						(unsigned int)currentdate.date().year(),
						currentdate.date().month().as_number(),
						config["InstrumentID"].c_str(),
						(to_iso_extended_string(begin.date()) + ' ' + to_simple_string(begin.time_of_day())).c_str(),
						(to_iso_extended_string(currentdate.date().end_of_month()) + " 23:59:59").c_str()
						);
					OverAllSQL += "(" + string(sqlBuf) + ") UNION ";
				}
				else if (
					currentdate.date().year() == end.date().year()
					&&
					currentdate.date().month() == end.date().month()
					)//最后的一个月
				{
					string TimeCondition;
					char sqlBuf[1024];

					sprintf_s(sqlBuf, "select secucode, tradingday,precloseprice, lastprice,turnvol,openprice, highprice, lowprice , "
						"sp1, sp2, sp3, sp4, sp5, bp1, bp2, bp3, bp4, bp5, sv1, sv2, sv3, sv4, sv5, bv1, bv2, bv3, bv4, bv5 "
						"from %s%.4d%.2d "
						"where secucode=\'%s\' "
						"and TRADINGDAY between "
						"to_date('%s', 'yyyy-mm-dd hh24:mi:ss') and "
						"to_date('%s', 'yyyy-mm-dd hh24:mi:ss') ",
						config["TableName"].c_str(), 
						(unsigned int)currentdate.date().year(), 
						currentdate.date().month().as_number(),
						config["InstrumentID"].c_str(),
						(to_iso_extended_string(currentdate.date()) + " 0:0:0").c_str(),
						(to_iso_extended_string(end.date()) + " " + to_simple_string(end.time_of_day())).c_str()
						);
					OverAllSQL += "(" + string(sqlBuf) + ") UNION ";
				}
				else//中间的月
				{
					string TimeCondition;
					char sqlBuf[1024];

					sprintf_s(sqlBuf, "select secucode, tradingday,precloseprice, lastprice,turnvol,openprice, highprice, lowprice , "
						"sp1, sp2, sp3, sp4, sp5, bp1, bp2, bp3, bp4, bp5, sv1, sv2, sv3, sv4, sv5, bv1, bv2, bv3, bv4, bv5 "
						"from %s%.4d%.2d "
						"where secucode=\'%s\' ",
						config["TableName"].c_str(), 
						(unsigned int)currentdate.date().year(),
						currentdate.date().month().as_number(),
						config["InstrumentID"].c_str()
						);
					OverAllSQL += "(" + string(sqlBuf) + ") UNION ";
				}
			}
			lastday = currentdate;
			currentdate += days(1);
		}
		OverAllSQL = OverAllSQL.substr(0, OverAllSQL.size() - 7);
	}


	OverAllSQL = "(" + OverAllSQL + ")" + " order by TRADINGDAY";
	int ret = OCI_Initialize(NULL, NULL, OCI_ENV_DEFAULT);
	if (!ret)
		throw std::exception("OCI_Initialize error.");

	conn = OCI_ConnectionCreate(
		config["HostAddress"].c_str(),
		config["UserName"].c_str(),
		config["PassWord"].c_str(), OCI_SESSION_DEFAULT);
	if (NULL == conn)
		throw std::exception("OCI_ConnectionCreate error.");;

	stmt = OCI_StatementCreate(conn);

	if (!OCI_ExecuteStmt(stmt, OverAllSQL.c_str()))
		throw std::exception("OCI_StatementCreate error.");;
	rs = OCI_GetResultset(stmt);
	enum _field {
		secucode = 1, tradingday, precloseprice, lastprice, turnvol, openprice, highprice, lowprice,
		sp1, sp2, sp3, sp4, sp5, bp1, bp2, bp3, bp4, bp5, sv1, sv2, sv3, sv4, sv5, bv1, bv2, bv3, bv4, bv5
	};
	while (OCI_FetchNext(rs))
	{
		res.push_back(CStockTick());
		auto & tick = res[res.size() - 1];

		strcpy_s(tick.m_strInstrumentID, OCI_GetString(rs, secucode));

		auto date = OCI_GetDate(rs, tradingday);
		char buf[20] = { 0 };
		OCI_DateToText(date, "YYYY/MM/DD HH24:MI:SS", sizeof(buf), buf);

		string at = buf;
		auto Microsecond = microseconds(0);
		tick.m_datetimeUTCDateTime = ptime(
			from_simple_string(at.substr(0, 10)),
			duration_from_string(at.substr(11, 8)) + Microsecond);


		tick.m_dbLastPrice = OCI_GetDouble(rs, lastprice);
		tick.m_intVolume = OCI_GetInt(rs, turnvol);
		tick.m_dbPreClosePrice = OCI_GetDouble(rs, precloseprice);



		tick.m_dbOpenPrice = OCI_GetDouble(rs, openprice);
		tick.m_dbHighestPrice = OCI_GetDouble(rs, highprice);
		tick.m_dbLowestPrice = OCI_GetDouble(rs, lowprice);

		tick.m_dbAskPrice1 = OCI_GetDouble(rs, sp1);
		tick.m_dbAskPrice2 = OCI_GetDouble(rs, sp2);
		tick.m_dbAskPrice3 = OCI_GetDouble(rs, sp3);
		tick.m_dbAskPrice4 = OCI_GetDouble(rs, sp4);
		tick.m_dbAskPrice5 = OCI_GetDouble(rs, sp5);

		tick.m_dbBidPrice1 = OCI_GetDouble(rs, bp1);
		tick.m_dbBidPrice2 = OCI_GetDouble(rs, bp2);
		tick.m_dbBidPrice3 = OCI_GetDouble(rs, bp3);
		tick.m_dbBidPrice4 = OCI_GetDouble(rs, bp4);
		tick.m_dbBidPrice5 = OCI_GetDouble(rs, bp5);


		tick.m_intAskVolume1 = OCI_GetInt(rs, sv1);
		tick.m_intAskVolume2 = OCI_GetInt(rs, sv2);
		tick.m_intAskVolume3 = OCI_GetInt(rs, sv3);
		tick.m_intAskVolume4 = OCI_GetInt(rs, sv4);
		tick.m_intAskVolume5 = OCI_GetInt(rs, sv5);

		tick.m_intBidVolume1 = OCI_GetInt(rs, bv1);
		tick.m_intBidVolume2 = OCI_GetInt(rs, bv2);
		tick.m_intBidVolume3 = OCI_GetInt(rs, bv3);
		tick.m_intBidVolume4 = OCI_GetInt(rs, bv4);
		tick.m_intBidVolume5 = OCI_GetInt(rs, bv5);

	}
	OCI_StatementFree(stmt);
	OCI_ConnectionFree(conn);
	OCI_Cleanup();
}

template<>
void LoadFromDatabase<CTwsTick>(
	const char * jsonstring,
	vector<CTwsTick> & res
	)
{
	res.clear();




	boost::property_tree::ptree root;
	boost::property_tree::read_json<boost::property_tree::ptree>(jsonstring, root);
	map<string, string> config;
	for (auto & node : root)
		config[node.first] = node.second.data();

	if (config.find("TableName") == config.end())
		throw std::exception("Can not find <TableName> in the json string.");
	if (config.find("InstrumentID") == config.end())
		throw std::exception("Can not find <InstrumentID> in the json string.");
	if (config.find("Expiry") == config.end())
		throw std::exception("Can not find <Expiry> in the json string.");
	if (config.find("BeginTime") == config.end())
		throw std::exception("Can not find <BeginTime> in the json string.");
	if (config.find("EndTime") == config.end())
		throw std::exception("Can not find <EndTime> in the json string.");
	char sqlBuf[512];
	sprintf_s(sqlBuf, "select Symbol,Expiry,AskPrice,BidPrice,LastPrice,AskSize,"
		"BidSize,Volume,UTCDateTime,Microseconds from %s where (Symbol=\'%s\' and  Expiry=\'%s\') and UTCDateTime between '%s' and '%s' order by TickID  asc",
		config["TableName"].c_str(),
		config["InstrumentID"].c_str(),
		config["Expiry"].c_str(),
		config["BeginTime"].c_str(),
		config["EndTime"].c_str()
		);
	unsigned int TickCounter = 0;
	auto DataRes = GetResult(sqlBuf, config);
	DataRes->beforeFirst();
	while (DataRes->next())
	{
		res.push_back(CTwsTick());
		CTwsTick &_tick = res[res.size() - 1];
		auto Microsecond = microseconds(DataRes->getInt("Microseconds"));
		string at = DataRes->getString("UTCDateTime").c_str();



		_tick.m_datetimeUTCDateTime = ptime(from_simple_string(at.substr(0, 10)), duration_from_string(at.substr(11, 8)) + Microsecond);
		strcpy_s(_tick.m_strExpiry, DataRes->getString("Expiry").c_str());
		strcpy_s(_tick.m_strInstrumentID, DataRes->getString("Symbol").c_str());
		strcat_s(_tick.m_strInstrumentID, _tick.m_strExpiry);
		_tick.m_dbLastPrice = DataRes->getDouble("LastPrice");
		_tick.m_intVolume = DataRes->getInt("Volume");
		_tick.m_dbBidPrice1 = DataRes->getDouble("BidPrice");
		_tick.m_intBidVolume1 = DataRes->getInt("BidSize");
		_tick.m_dbAskPrice1 = DataRes->getDouble("AskPrice");
		_tick.m_intAskVolume1 = DataRes->getInt("AskSize");

	}
}


template<typename T>
class CTickData :public MTickDataContainerInterface
{
public:
	CTick * operator[](unsigned int index) { return m_vecTickPointerContainer[index]; };
	void Release() { delete this; };
	virtual void LoadFromDB(
		const char * HostAddress,
		const char * UserName,
		const char * PassWord,
		const char * DataBase,
		const char * TableName,
		const char * instrumentId,
		ptime begin, ptime end)
	{
		LoadFromDatabase<T>(HostAddress,
			UserName,
			PassWord,
			DataBase,
			TableName,
			instrumentId,
			ptime begin, ptime end);
		for_each(m_vecTicks.begin().m_vecTicks.end(), [&m_vecTickPointerContainer](T&tar) {m_vecTickPointerContainer.push_back(&tar);});
		AbandonOverlap(m_vecTickPointerContainer);
	}
	virtual void LoadFromFile(std::tr1::shared_ptr<ifstream> filein)
	{
		boost::archive::binary_iarchive ia(*filein.get());
		ia >> m_vecTicks;
		m_vecTickPointerContainer.clear();
		m_vecTickPointerContainer.reserve(m_vecTicks.size());
		for_each(m_vecTicks.begin().m_vecTicks.end(), [&m_vecTickPointerContainer](T&tar) {m_vecTickPointerContainer.push_back(&tar);});
	}
	virtual void SaveToFile(std::tr1::shared_ptr<ifstream> filein)
	{
		boost::archive::binary_oarchive oa(*filein.get());
		oa << *dynamic_cast<T*>(ret.get());
	}
	vector<CTick*> m_vecTickPointerContainer;
	vector<T> m_vecTicks;
};
