

#include "Order.h"
#include <string>
#include "StrategyData.h"
#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <fstream>
#include <algorithm>
#include <unordered_map>
#ifndef BOOST_SPIRIT_THREADSAFE
#define BOOST_SPIRIT_THREADSAFE
#endif
#include <boost/property_tree/ptree.hpp>  
#include <boost/property_tree/json_parser.hpp> 
#include <boost/date_time/posix_time/posix_time.hpp>
using namespace std::tr1;
using namespace boost::posix_time;
using namespace std;
using namespace StrategyData;


bool LoadStrategyParam(string filename,  CParNode * ppar)
{
	if (NULL == ppar)
		return false;
	unordered_map<string, string> pars;
	ifstream inFile(filename, ios::binary);
	if (inFile.is_open())
	{
		try {
			boost::property_tree::ptree in_config;
			boost::property_tree::read_json(inFile, in_config);
			auto ParamNode = in_config.find("param");
			if (ParamNode != in_config.not_found())
			{
				for (auto & node : ParamNode->second)
					pars[node.first] = node.second.data();
			}
		}
		catch (std::exception & err)
		{
			return false;
		}
		inFile.close();
	}
	else return false;

	for (unsigned int index = 0;strlen(ppar[index].m_arrayParname) != 0;index++)
	{
		if (pars.find(ppar[index].m_arrayParname) != pars.end())
		{
			if (nullptr != ppar[index].m_pIntAddress)
				*ppar[index].m_pIntAddress = atoi(pars[ppar[index].m_arrayParname].c_str());
			else if (nullptr != ppar[index].m_pDoubleAddress)
				*ppar[index].m_pDoubleAddress = atof(pars[ppar[index].m_arrayParname].c_str());
			else if (nullptr != ppar[index].m_pTimeDuraAddress)
				*ppar[index].m_pTimeDuraAddress = duration_from_string(pars[ppar[index].m_arrayParname]);
			else if (nullptr != ppar[index].m_pStringAddress)
				snprintf(ppar[index].m_pStringAddress, ppar[index].m_intOption,"%s",
					pars[ppar[index].m_arrayParname].c_str());
		}
	}
	return true;
}
