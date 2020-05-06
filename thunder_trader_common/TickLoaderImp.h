#ifndef _TICKLOADERIMP_H
#define _TICKLOADERIMP_H
#include <string>
#include <memory>
#include <algorithm>
#include <vector>
#include <fstream>
#include <set>
#include <sstream>  
#include <istream>
#include <ostream>
#include <algorithm>
#include <unordered_map>
#include <tuple>
#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/serialization/serialization.hpp>  
#include <boost/archive/binary_oarchive.hpp>  
#include <boost/archive/binary_iarchive.hpp>  
#include <boost/serialization/export.hpp>  

#include <boost/property_tree/ptree.hpp>  
#include <boost/property_tree/json_parser.hpp> 
#include <boost/serialization/vector.hpp>  
#include <boost/serialization/split_member.hpp>  
#include <boost/date_time/posix_time/time_serialize.hpp>

#include <mutex>
#include <ctime>
#include "Tick.h"
#include "OptionTick.h"
#include "FutureTick.h"
#include "ForexTick.h"
#include "StockTick.h"
#include "TwsTick.h"
#include <algorithm>
#include "oci.h"
#include "ocilib\ocilib.h"
#include "mysql_driver.h"
#include "cppconn/driver.h"
#include "cppconn/exception.h"
#include "cppconn/resultset.h"
#include "cppconn/prepared_statement.h"
#include "cppconn/prepared_statement.h"
#include <future>
#include <mutex>
#include "TickDataContainer.h"

#include <boost/property_tree/ptree.hpp>  
#include <boost/property_tree/json_parser.hpp> 
using namespace boost::property_tree;

using namespace boost::posix_time;
using namespace boost::gregorian;
using namespace std;
using namespace StrategyData;



template<class T>
void LoadFromDatabase(const char * jsonstring, const char * filename)
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
	const char * filename)
{
	
	
	boost::property_tree::ptree root;
	boost::property_tree::read_json<boost::property_tree::ptree>(stringstream(jsonstring), root);
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
	char sqlBuf[1024];
	sprintf_s(sqlBuf,
		"select "
		"InstrumentID,Volume,AveragePrice,Turnover,OpenInterest,LastPrice,"
		"BidPrice1,BidVolume1,AskPrice1,AskVolume1,"
		"UTCDateTime,Microseconds,PreSettlementPrice,PreClosePrice,OpenPrice,HighestPrice,LowestPrice,ClosePrice,"
		"UpperLimitPrice,LowerLimitPrice"
		" from %s where InstrumentID=\'%s\' and UTCDateTime between '%s' and '%s' order by UTCDateTime,Microseconds  asc",
		config["TableName"].c_str(),
		config["InstrumentID"].c_str(),
		(to_iso_extended_string(begin.date()) + ' ' + to_simple_string(begin.time_of_day())).c_str(),
		(to_iso_extended_string(end.date()) + ' ' + to_simple_string(end.time_of_day())).c_str()
		);
	unsigned int TickCounter = 0;
	auto DataRes = GetResult(sqlBuf, config);
	if (nullptr == DataRes)
		throw std::exception("DataRes is nullptr.");
	std::shared_ptr<ofstream> file(new ofstream(filename, ios::binary), [](ofstream*p) {if (p&&p->is_open())p->close();});
	if (file->is_open() == false)
		throw std::exception("Can not create file");
	boost::archive::binary_oarchive oa(*file);
	auto TickType = TStrategyTickType::FutureTick;
	oa & TickType;
	auto len = DataRes->rowsCount();
	oa & len;
	CFutureTick _tick;
	_tick.m_dbBidPrice[1] = 0.0;
	_tick.m_dbBidPrice[2] = 0.0;
	_tick.m_dbBidPrice[3] = 0.0;
	_tick.m_dbBidPrice[4] = 0.0;
	_tick.m_intBidVolume[1] = 0;
	_tick.m_intBidVolume[2] = 0;
	_tick.m_intBidVolume[3] = 0;
	_tick.m_intBidVolume[4] = 0;
	_tick.m_dbAskPrice[1] = 0.0;
	_tick.m_dbAskPrice[2] = 0.0;
	_tick.m_dbAskPrice[3] = 0.0;
	_tick.m_dbAskPrice[4] = 0.0;
	_tick.m_intAskVolume[1] = 0;
	_tick.m_intAskVolume[2] = 0;
	_tick.m_intAskVolume[3] = 0;
	_tick.m_intAskVolume[4] = 0;
	DataRes->beforeFirst();
	while (DataRes->next())
	{
		strcpy_s(_tick.m_strInstrumentID, DataRes->getString("InstrumentID").c_str());
		_tick.m_intVolume = DataRes->getUInt("Volume");
		_tick.m_dbAveragePrice = DataRes->getDouble("AveragePrice");
		_tick.m_dbTurnover = DataRes->getDouble("Turnover");
		_tick.m_dbOpenInterest = DataRes->getDouble("OpenInterest");
		_tick.m_dbLastPrice = DataRes->getDouble("LastPrice");
		_tick.m_dbOpenPrice = DataRes->getDouble("OpenPrice");
		_tick.m_dbHighestPrice = DataRes->getDouble("HighestPrice");
		_tick.m_dbLowestPrice = DataRes->getDouble("LowestPrice");
		_tick.m_dbClosePrice = DataRes->getDouble("ClosePrice");
		_tick.m_dbPreSettlementPrice = DataRes->getDouble("PreSettlementPrice");
		_tick.m_dbPreClosePrice = DataRes->getDouble("PreClosePrice");
		_tick.m_dbUpperLimitPrice = DataRes->getDouble("UpperLimitPrice");
		_tick.m_dbLowerLimitPrice = DataRes->getDouble("LowerLimitPrice");
		_tick.m_dbBidPrice[0] = DataRes->getDouble("BidPrice1");
		_tick.m_intBidVolume[0] = DataRes->getInt("BidVolume1");
		_tick.m_dbAskPrice[0] = DataRes->getDouble("AskPrice1");
		_tick.m_intAskVolume[0] = DataRes->getInt("AskVolume1");
		
		string at = DataRes->getString("UTCDateTime").c_str();
		auto Microsecond = microseconds(DataRes->getInt64("Microseconds"));
		_tick.m_datetimeUTCDateTime = ptime(
			from_simple_string(at.substr(0, 10)),
			duration_from_string(at.substr(11, 8)) + Microsecond);
		oa & _tick;
	}
}

template<>
void LoadFromDatabase<COptionTick>(
	const char * jsonstring,
	const char * filename)
{
	
	boost::property_tree::ptree root;
	boost::property_tree::read_json<boost::property_tree::ptree>(stringstream(jsonstring), root);
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
	char sqlBuf[1024];
	sprintf_s(sqlBuf,
		"select "
		"securityID,tradeQty,latestPrice,"
		"bidPrice1,bidPrice2,bidPrice3, bidPrice4, bidPrice5,"
		"bidQty1, bidQty2, bidQty3, bidQty4, bidQty5,"
		"askPrice1, askPrice2, askPrice3, askPrice4, askPrice5,"
		"askQty1, askQty2, askQty3, askQty4, askQty5,"
		"UTCDateTime,Microseconds,AuctionPrice,TradingPhaseCode "
		"from %s where securityID=\'%s\' and UTCDateTime between '%s' and '%s' order by UTCDateTime,Microseconds asc",
		config["TableName"].c_str(),
		config["InstrumentID"].c_str(),
		(to_iso_extended_string(begin.date()) + ' ' + to_simple_string(begin.time_of_day())).c_str(),
		(to_iso_extended_string(end.date()) + ' ' + to_simple_string(end.time_of_day())).c_str()
		);
	unsigned int TickCounter = 0;
	auto DataRes = GetResult(sqlBuf, config);
	if (nullptr == DataRes)
		throw std::exception("DataRes is nullptr.");
	std::shared_ptr<ofstream> file(new ofstream(filename, ios::binary), [](ofstream*p) {if (p&&p->is_open())p->close();});
	if (file->is_open() == false)
		throw std::exception("Can not create file");
	boost::archive::binary_oarchive oa(*file);
	auto TickType = TStrategyTickType::OptionTick;
	oa & TickType;
	auto len = DataRes->rowsCount();
	oa & len;
	COptionTick _tick;
	_tick.m_dbOpenPrice = 0.0;
	_tick.m_dbHighestPrice = 0.0;
	_tick.m_dbLowestPrice = 0.0;
	_tick.m_dbClosePrice = 0.0;
	DataRes->beforeFirst();
	while (DataRes->next())
	{
		strcpy_s(_tick.m_strInstrumentID, DataRes->getString("securityID").c_str());
		_tick.m_intVolume = DataRes->getUInt("tradeQty");
		_tick.m_dbLastPrice = DataRes->getDouble("latestPrice");
		_tick.m_dbBidPrice[0] = DataRes->getDouble("bidPrice1");
		_tick.m_dbBidPrice[1] = DataRes->getDouble("bidPrice2");
		_tick.m_dbBidPrice[2] = DataRes->getDouble("bidPrice3");
		_tick.m_dbBidPrice[3] = DataRes->getDouble("bidPrice4");
		_tick.m_dbBidPrice[4] = DataRes->getDouble("bidPrice5");
		_tick.m_intBidVolume[0] = DataRes->getInt("bidQty1");
		_tick.m_intBidVolume[1] = DataRes->getInt("bidQty2");
		_tick.m_intBidVolume[2] = DataRes->getInt("bidQty3");
		_tick.m_intBidVolume[3] = DataRes->getInt("bidQty4");
		_tick.m_intBidVolume[4] = DataRes->getInt("bidQty5");
		_tick.m_dbAskPrice[0] = DataRes->getDouble("askPrice1");
		_tick.m_dbAskPrice[1] = DataRes->getDouble("askPrice2");
		_tick.m_dbAskPrice[2] = DataRes->getDouble("askPrice3");
		_tick.m_dbAskPrice[3] = DataRes->getDouble("askPrice4");
		_tick.m_dbAskPrice[4] = DataRes->getDouble("askPrice5");
		_tick.m_intAskVolume[0] = DataRes->getInt("askQty1");
		_tick.m_intAskVolume[1] = DataRes->getInt("askQty2");
		_tick.m_intAskVolume[2] = DataRes->getInt("askQty3");
		_tick.m_intAskVolume[3] = DataRes->getInt("askQty4");
		_tick.m_intAskVolume[4] = DataRes->getInt("askQty5");
		_tick.m_dbAuctionPrice = static_cast<TPriceType>(DataRes->getDouble("AuctionPrice"));
		switch (DataRes->getString("TradingPhaseCode")[0])
		{
		case 'T':_tick.m_enumPhase = TradingPhase;break;
		case 'V':_tick.m_enumPhase = CircuitBreakingPhase;break;
		case 'S':
		case 'C':_tick.m_enumPhase = AuctionPhase;break;
		}
		string at = DataRes->getString("UTCDateTime").c_str();
		auto Microsecond = microseconds(DataRes->getInt64("Microseconds"));
		_tick.m_datetimeUTCDateTime = ptime(
			from_simple_string(at.substr(0, 10)),
			duration_from_string(at.substr(11, 8)) + Microsecond);
		oa & _tick;
	}
}

template<>
void LoadFromDatabase<CForexTick>(
	const char * jsonstring,
	const char * filename)
{
	boost::property_tree::ptree root;
	boost::property_tree::read_json<boost::property_tree::ptree>(stringstream(jsonstring), root);
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
	char sqlBuf[1024];
	sprintf_s(sqlBuf, "select * from %s where (InstrumentID=\'%s\' and Currency=\'%s\') and UTCDateTime between '%s' and '%s' order by TickID  asc",
		config["TableName"].c_str(),
		config["InstrumentID"].c_str(),
		config["Currency"].c_str(),
		(to_iso_extended_string(begin.date()) + ' ' + to_simple_string(begin.time_of_day())).c_str(),
		(to_iso_extended_string(end.date()) + ' ' + to_simple_string(end.time_of_day())).c_str()
		);
	unsigned int TickCounter = 0;
	auto DataRes = GetResult(sqlBuf, config);
	if (nullptr == DataRes)
		throw std::exception("DataRes is nullptr.");
	std::shared_ptr<ofstream> file(new ofstream(filename, ios::binary), [](ofstream*p) {if (p&&p->is_open())p->close();});
	if (file->is_open() == false)
		throw std::exception("Can not create file");
	boost::archive::binary_oarchive oa(*file);
	auto TickType = TStrategyTickType::ForexTick;
	oa & TickType;
	auto len = DataRes->rowsCount();
	oa & len;
	CForexTick _tick;
	_tick.m_dbBidPrice[1] = 0.0;
	_tick.m_dbBidPrice[2] = 0.0;
	_tick.m_dbBidPrice[3] = 0.0;
	_tick.m_dbBidPrice[4] = 0.0;
	_tick.m_intBidVolume[1] = 0;
	_tick.m_intBidVolume[2] = 0;
	_tick.m_intBidVolume[3] = 0;
	_tick.m_intBidVolume[4] = 0;
	_tick.m_dbAskPrice[1] = 0.0;
	_tick.m_dbAskPrice[2] = 0.0;
	_tick.m_dbAskPrice[3] = 0.0;
	_tick.m_dbAskPrice[4] = 0.0;
	_tick.m_intAskVolume[1] = 0;
	_tick.m_intAskVolume[2] = 0;
	_tick.m_intAskVolume[3] = 0;
	_tick.m_intAskVolume[4] = 0;
	DataRes->beforeFirst();
	while (DataRes->next())
	{
		string Instrument = DataRes->getString("InstrumentID").c_str();
		string Currency = DataRes->getString("Currency").c_str();
		strcpy_s(
			_tick.m_strInstrumentID, (Instrument + "." + Currency).c_str()
			);
		_tick.m_intVolume = -1;

		_tick.m_dbBidPrice[0] = DataRes->getDouble("BidPrice");
		_tick.m_intBidVolume[0] = DataRes->getInt("BidSize");
		_tick.m_dbAskPrice[0] = DataRes->getDouble("AskPrice");
		_tick.m_intAskVolume[0] = DataRes->getInt("AskSize");
		_tick.m_dbLastPrice = _tick.m_dbBidPrice[0];
		auto Microsecond = microseconds(DataRes->getInt64("Microseconds"));
		string at = DataRes->getString("UTCDateTime").c_str();
		_tick.m_datetimeUTCDateTime = ptime(
			from_simple_string(at.substr(0, 10)),
			duration_from_string(at.substr(11, 8)) + Microsecond);
		oa & _tick;
	}
}


template<>
void LoadFromDatabase<CStockTick>(
	const char * jsonstring,
	const char * filename)
{
	

	boost::property_tree::ptree root;
	boost::property_tree::read_json<boost::property_tree::ptree>(stringstream(jsonstring), root);
	map<string, string> config;
	for (auto & node : root)
		config[node.first] = node.second.data();

	if (config.find("Source") == config.end())
		throw std::exception("Can not find <Source> in the json string.");

	if (config["Source"] == "QuantFund.SH.StockTick")
	{
		if (config.find("TableName") == config.end())
			throw std::exception("Can not find <TableName> in the json string.");
		if (config.find("InstrumentID") == config.end())
			throw std::exception("Can not find <InstrumentID> in the json string.");
		if (config.find("ExchangeID") == config.end())
			throw std::exception("Can not find <ExchangeID> in the json string.");
		if (config.find("BeginTime") == config.end())
			throw std::exception("Can not find <BeginTime> in the json string.");
		if (config.find("EndTime") == config.end())
			throw std::exception("Can not find <EndTime> in the json string.");
		ptime begin = from_iso_string(config["BeginTime"]);
		ptime end = from_iso_string(config["EndTime"]);
		char sqlBuf[1024];
		sprintf_s(sqlBuf,
			"select securityID,exchangeID,preClosePrice,openPrice,upperLimitPrice,lowerLimitPrice,UTCDateTime,"
			"Microseconds,latestPrice,highestPrice,lowestPrice,tradeQty,askPrice1,askPrice2,askPrice3,"
			"askPrice4,askPrice5,bidPrice1, bidPrice2, bidPrice3, bidPrice4, bidPrice5, "
			"askQty1, askQty2, askQty3, askQty4, askQty5, "
			"bidQty1, bidQty2, bidQty3, bidQty4, bidQty5 from %s where "
			"securityID=\'%s\' and exchangeID=\'%s\' and UTCDateTime between '%s' and '%s' order by UTCDateTime,Microseconds  asc",
			config["TableName"].c_str(),
			config["InstrumentID"].c_str(),
			config["ExchangeID"].c_str(),
			(to_iso_extended_string(begin.date()) + ' ' + to_simple_string(begin.time_of_day())).c_str(),
			(to_iso_extended_string(end.date()) + ' ' + to_simple_string(end.time_of_day())).c_str()
			);
		unsigned int TickCounter = 0;
		auto DataRes = GetResult(sqlBuf, config);
		if (nullptr == DataRes)
			throw std::exception("DataRes is nullptr.");
		std::shared_ptr<ofstream> file(new ofstream(filename, ios::binary), [](ofstream*p) {if (p&&p->is_open())p->close();});
		if (file->is_open() == false)
			throw std::exception("Can not create file");
		boost::archive::binary_oarchive oa(*file);
		auto TickType = TStrategyTickType::StockTick;
		oa & TickType;
		auto len = DataRes->rowsCount();
		oa & len;
		CStockTick _tick;
		DataRes->beforeFirst();
		while (DataRes->next())
		{
			strcpy_s(_tick.m_strInstrumentID, DataRes->getString("securityID").c_str());
			_tick.m_intVolume = DataRes->getUInt("tradeQty");
			_tick.m_dbLastPrice = DataRes->getDouble("latestPrice");

			_tick.m_dbUpperLimitPrice = DataRes->getUInt("upperLimitPrice");
			_tick.m_dbLowerLimitPrice = DataRes->getUInt("lowerLimitPrice");

			_tick.m_dbOpenPrice = DataRes->getDouble("openPrice");
			_tick.m_dbHighestPrice = DataRes->getDouble("highestPrice");
			_tick.m_dbLowestPrice = DataRes->getDouble("lowestPrice");
			_tick.m_dbPreClosePrice = DataRes->getDouble("preClosePrice");

			_tick.m_dbBidPrice[0] = DataRes->getDouble("bidPrice1");
			_tick.m_dbBidPrice[1] = DataRes->getDouble("bidPrice2");
			_tick.m_dbBidPrice[2] = DataRes->getDouble("bidPrice3");
			_tick.m_dbBidPrice[3] = DataRes->getDouble("bidPrice4");
			_tick.m_dbBidPrice[4] = DataRes->getDouble("bidPrice5");
			_tick.m_intBidVolume[0] = DataRes->getInt("bidQty1");
			_tick.m_intBidVolume[1] = DataRes->getInt("bidQty2");
			_tick.m_intBidVolume[2] = DataRes->getInt("bidQty3");
			_tick.m_intBidVolume[3] = DataRes->getInt("bidQty4");
			_tick.m_intBidVolume[4] = DataRes->getInt("bidQty5");
			_tick.m_dbAskPrice[0] = DataRes->getDouble("askPrice1");
			_tick.m_dbAskPrice[1] = DataRes->getDouble("askPrice2");
			_tick.m_dbAskPrice[2] = DataRes->getDouble("askPrice3");
			_tick.m_dbAskPrice[3] = DataRes->getDouble("askPrice4");
			_tick.m_dbAskPrice[4] = DataRes->getDouble("askPrice5");
			_tick.m_intAskVolume[0] = DataRes->getInt("askQty1");
			_tick.m_intAskVolume[1] = DataRes->getInt("askQty2");
			_tick.m_intAskVolume[2] = DataRes->getInt("askQty3");
			_tick.m_intAskVolume[3] = DataRes->getInt("askQty4");
			_tick.m_intAskVolume[4] = DataRes->getInt("askQty5");
			string at = DataRes->getString("UTCDateTime").c_str();
			auto Microsecond = microseconds(DataRes->getInt64("Microseconds"));
			_tick.m_datetimeUTCDateTime = ptime(
				from_simple_string(at.substr(0, 10)),
				duration_from_string(at.substr(11, 8)) + Microsecond);
			oa & _tick;

		}
	}
	else if (config["Source"] == "QuantTech.STOCK.T_STOCK_TICK_")
	{
		if (config.find("HostAddress") == config.end())
			throw std::exception("Can not find <HostAddress> in the json string.");
		if (config.find("UserName") == config.end())
			throw std::exception("Can not find <UserName> in the json string.");
		if (config.find("PassWord") == config.end())
			throw std::exception("Can not find <PassWord> in the json string.");
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
		ptime begin = from_iso_string(config["BeginTime"]) + hours(8);
		ptime end = from_iso_string(config["EndTime"]) + hours(8);
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
				config["TableName"].c_str(),
				(unsigned int)begin.date().year(),
				begin.date().month().as_number(),
				config["InstrumentID"].c_str(),

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
	}
	else if (config["Source"] == "QuantTech.HFDB.HF02T00_00_2016")
	{
		if (config.find("HostAddress") == config.end())
			throw std::exception("Can not find <HostAddress> in the json string.");
		if (config.find("UserName") == config.end())
			throw std::exception("Can not find <UserName> in the json string.");
		if (config.find("PassWord") == config.end())
			throw std::exception("Can not find <PassWord> in the json string.");
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
		ptime begin = from_iso_string(config["BeginTime"]) + hours(8);
		ptime end = from_iso_string(config["EndTime"]) + hours(8);
		string OverAllSQL;

		string TimeCondition;
		char sqlBuf[1024];


		sprintf_s(sqlBuf, "select "
			"F_MKCODE, F_CODE, F_NAME, F_DAY, F_DATE, F_TIME, F_0001, F_0002, F_0003, "
			"F_0004, F_0005, F_0008, F_0009, "
			"F_0013, F_0014, F_0015, F_0016, F_0017, F_0018, F_0019, F_0020, F_0021, "
			"F_0022, F_0023, F_0024, F_0025, F_0026, F_0027, F_0028, F_0029, F_0030, "
			"F_0031, F_0032 "
			"from %s "
			"where F_CODE=\'%s\' "
			"and F_DAY between "
			"to_date('%s', 'yyyy-mm-dd hh24:mi:ss') and "
			"to_date('%s', 'yyyy-mm-dd hh24:mi:ss') ",
			config["TableName"].c_str(),
			config["InstrumentID"].c_str(),
			(to_iso_extended_string(begin.date()) + ' ' + to_simple_string(begin.time_of_day())).c_str(),
			(to_iso_extended_string(end.date()) + ' ' + to_simple_string(end.time_of_day())).c_str()
			);
		OverAllSQL = sqlBuf;
		OverAllSQL = "(" + OverAllSQL + ")" + " order by F_DAY asc";
		int ret = OCI_Initialize(NULL, NULL, OCI_ENV_DEFAULT);
		if (!ret)
			throw std::exception("OCI_Initialize error.");

		conn = OCI_ConnectionCreate(
			config["HostAddress"].c_str(),
			config["UserName"].c_str(),
			config["PassWord"].c_str(), OCI_SESSION_DEFAULT);
		if (NULL == conn)
			throw std::exception("OCI_ConnectionCreate error.");

		stmt = OCI_StatementCreate(conn);
		if (NULL == stmt)
			throw std::exception("OCI_StatementCreate error.");
		if (!OCI_ExecuteStmt(stmt, OverAllSQL.c_str()))
			throw std::exception("OCI_StatementCreate error.");
		rs = OCI_GetResultset(stmt);

		if (NULL == rs)
			throw std::exception("OCI_GetResultset error.");
		enum _field {
			F_MKCODE = 1, F_CODE, F_NAME, F_DAY, F_DATE, F_TIME, F_0001_PreClosePrice, F_0002_OpenPrice, F_0003_HighestPrice,
			F_0004_LowestPrice, F_0005_LastPrice, F_0008_Volume, F_0009_Turnover,
			F_0013_AskPrice1, F_0014_AskPrice2, F_0015_AskPrice3, F_0016_AskPrice4, F_0017_AskPrice5,
			F_0018_BidPrice1, F_0019_BidPrice2, F_0020_BidPrice3, F_0021_BidPrice4, F_0022_BidPrice5,
			F_0023_AskVolume1, F_0024_AskVolume2, F_0025_AskVolume3, F_0026_AskVolume4, F_0027_AskVolume5,
			F_0028_BidVolume1, F_0029_BidVolume2, F_0030_BidVolume3, F_0031_BidVolume4, F_0032_BidVolume5
		};
		std::shared_ptr<ofstream> file(new ofstream(filename, ios::binary), [](ofstream*p) {if (p&&p->is_open())p->close();});
		if (file->is_open() == false)
			throw std::exception("Can not create file");
		boost::archive::binary_oarchive oa(*file);
		auto TickType = TStrategyTickType::StockTick;
		oa & TickType;
		CStockTick tick;
		while (OCI_FetchNext(rs))
		{
			strcpy_s(tick.m_strInstrumentID, OCI_GetString(rs, F_CODE));
			auto date = OCI_GetDate(rs, F_DAY);
			char buf[20] = { 0 };
			OCI_DateToText(date, "YYYY/MM/DD HH24:MI:SS", sizeof(buf), buf);
			string at = buf;
			auto Microsecond = microseconds(0);
			tick.m_datetimeUTCDateTime = ptime(
				from_simple_string(at.substr(0, 10)),
				duration_from_string(at.substr(11, 8)) + Microsecond);
			tick.m_datetimeUTCDateTime -= hours(8);

			tick.m_dbLastPrice = OCI_GetDouble(rs, F_0005_LastPrice);
			tick.m_intVolume = OCI_GetInt(rs, F_0008_Volume);
			tick.m_dbPreClosePrice = OCI_GetDouble(rs, F_0001_PreClosePrice);



			tick.m_dbOpenPrice = OCI_GetDouble(rs, F_0002_OpenPrice);
			tick.m_dbHighestPrice = OCI_GetDouble(rs, F_0003_HighestPrice);
			tick.m_dbLowestPrice = OCI_GetDouble(rs, F_0004_LowestPrice);

			tick.m_dbAskPrice[0] = OCI_GetDouble(rs, F_0013_AskPrice1);
			tick.m_dbAskPrice[1] = OCI_GetDouble(rs, F_0014_AskPrice2);
			tick.m_dbAskPrice[2] = OCI_GetDouble(rs, F_0015_AskPrice3);
			tick.m_dbAskPrice[3] = OCI_GetDouble(rs, F_0016_AskPrice4);
			tick.m_dbAskPrice[4] = OCI_GetDouble(rs, F_0017_AskPrice5);

			tick.m_dbBidPrice[0] = OCI_GetDouble(rs, F_0018_BidPrice1);
			tick.m_dbBidPrice[1] = OCI_GetDouble(rs, F_0019_BidPrice2);
			tick.m_dbBidPrice[2] = OCI_GetDouble(rs, F_0020_BidPrice3);
			tick.m_dbBidPrice[3] = OCI_GetDouble(rs, F_0021_BidPrice4);
			tick.m_dbBidPrice[4] = OCI_GetDouble(rs, F_0022_BidPrice5);


			tick.m_intAskVolume[0] = OCI_GetInt(rs, F_0023_AskVolume1);
			tick.m_intAskVolume[1] = OCI_GetInt(rs, F_0024_AskVolume2);
			tick.m_intAskVolume[2] = OCI_GetInt(rs, F_0025_AskVolume3);
			tick.m_intAskVolume[3] = OCI_GetInt(rs, F_0026_AskVolume4);
			tick.m_intAskVolume[4] = OCI_GetInt(rs, F_0027_AskVolume5);

			tick.m_intBidVolume[0] = OCI_GetInt(rs, F_0028_BidVolume1);
			tick.m_intBidVolume[1] = OCI_GetInt(rs, F_0029_BidVolume2);
			tick.m_intBidVolume[2] = OCI_GetInt(rs, F_0030_BidVolume3);
			tick.m_intBidVolume[3] = OCI_GetInt(rs, F_0031_BidVolume4);
			tick.m_intBidVolume[4] = OCI_GetInt(rs, F_0032_BidVolume5);
			oa & tick;
		}
		OCI_StatementFree(stmt);
		OCI_ConnectionFree(conn);
		OCI_Cleanup();
	}
};

template<>
void LoadFromDatabase<CTwsTick>(
	const char * jsonstring,
	const char * filename
	)
{
	boost::property_tree::ptree root;
	boost::property_tree::read_json<boost::property_tree::ptree>(stringstream(jsonstring), root);
	map<string, string> config;
	for (auto & node : root)
		config[node.first] = node.second.data();

	if (config.find("TableName") == config.end())
		throw std::exception("Can not find <TableName> in the json string.");
	if (config.find("Symbol") == config.end())
		throw std::exception("Can not find <Symbol> in the json string.");
	if (config.find("Expiry") == config.end())
		throw std::exception("Can not find <Expiry> in the json string.");
	if (config.find("BeginTime") == config.end())
		throw std::exception("Can not find <BeginTime> in the json string.");
	if (config.find("EndTime") == config.end())
		throw std::exception("Can not find <EndTime> in the json string.");
	ptime begin = from_iso_string(config["BeginTime"]);
	ptime end = from_iso_string(config["EndTime"]);
	char sqlBuf[1024];
	sprintf_s(sqlBuf, "select Symbol,Expiry,AskPrice,BidPrice,LastPrice,AskSize,"
		"BidSize,Volume,UTCDateTime,Microseconds from %s where (Symbol=\'%s\' and  Expiry=\'%s\') and UTCDateTime between '%s' and '%s' order by UTCDateTime,Microseconds,TickID  asc",
		config["TableName"].c_str(),
		config["Symbol"].c_str(),
		config["Expiry"].c_str(),
		(to_iso_extended_string(begin.date()) + ' ' + to_simple_string(begin.time_of_day())).c_str(),
		(to_iso_extended_string(end.date()) + ' ' + to_simple_string(end.time_of_day())).c_str()
		);
	unsigned int TickCounter = 0;
	auto DataRes = GetResult(sqlBuf, config);
	if (nullptr == DataRes)
		throw std::exception("DataRes is nullptr.");
	std::shared_ptr<ofstream> file(new ofstream(filename, ios::binary), [](ofstream*p) {if (p&&p->is_open())p->close();});
	if (file->is_open() == false)
		throw std::exception("Can not create file");
	boost::archive::binary_oarchive oa(*file);
	auto TickType = TStrategyTickType::TwsTick;
	oa & TickType;
	auto len = DataRes->rowsCount();
	oa & len;
	CTwsTick _tick;
	_tick.m_dbBidPrice[1] = 0.0;
	_tick.m_dbBidPrice[2] = 0.0;
	_tick.m_dbBidPrice[3] = 0.0;
	_tick.m_dbBidPrice[4] = 0.0;
	_tick.m_intBidVolume[1] = 0;
	_tick.m_intBidVolume[2] = 0;
	_tick.m_intBidVolume[3] = 0;
	_tick.m_intBidVolume[4] = 0;
	_tick.m_dbAskPrice[1] = 0.0;
	_tick.m_dbAskPrice[2] = 0.0;
	_tick.m_dbAskPrice[3] = 0.0;
	_tick.m_dbAskPrice[4] = 0.0;
	_tick.m_intAskVolume[1] = 0;
	_tick.m_intAskVolume[2] = 0;
	_tick.m_intAskVolume[3] = 0;
	_tick.m_intAskVolume[4] = 0;
	DataRes->beforeFirst();
	while (DataRes->next())
	{
		auto Microsecond = microseconds(DataRes->getInt64("Microseconds"));
		string at = DataRes->getString("UTCDateTime").c_str();
		_tick.m_datetimeUTCDateTime = ptime(from_simple_string(at.substr(0, 10)), duration_from_string(at.substr(11, 8)) + Microsecond);
		strcpy_s(_tick.m_strExpiry, DataRes->getString("Expiry").c_str());
		strcpy_s(_tick.m_strInstrumentID, DataRes->getString("Symbol").c_str());
		strcat_s(_tick.m_strInstrumentID, _tick.m_strExpiry);
		_tick.m_dbLastPrice = DataRes->getDouble("LastPrice");
		_tick.m_intVolume = DataRes->getInt("Volume");
		_tick.m_dbBidPrice[0] = DataRes->getDouble("BidPrice");
		_tick.m_intBidVolume[0] = DataRes->getInt("BidSize");
		_tick.m_dbAskPrice[0] = DataRes->getDouble("AskPrice");
		_tick.m_intAskVolume[0] = DataRes->getInt("AskSize");
		oa & _tick;
	}
}


#endif
