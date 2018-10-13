#ifndef _TICKDATACONTAINER_H
#define _TICKDATACONTAINER_H
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
#include "StrategyData.h"

#pragma region ptree
#include <boost/property_tree/ptree.hpp>  
#include <boost/property_tree/json_parser.hpp> 
using namespace boost::property_tree;
#pragma endregion
using namespace boost::posix_time;
using namespace boost::gregorian;
using namespace std;
using namespace StrategyData;
void AbandonOverlap(vector<CTick*>&result);
class MTickDataContainerInterface
{
public:
	virtual TStrategyTickType GetTickType() = 0;
	virtual CTick* operator[](unsigned int) = 0;
	virtual vector<CTick*>::iterator begin() = 0;
	virtual vector<CTick*>::iterator end() = 0;
	virtual void Release() = 0;
	virtual void LoadFromDB(const char * jsonstring, std::mutex & mtx) = 0;
	virtual void SaveToFile(ostream & file) = 0;
	virtual size_t GetLength() = 0;
};

template<class T, TStrategyTickType Type>
class CTickData :public MTickDataContainerInterface
{
public:
	CTickData<T,Type>() : m_enumTickType(Type) {};
	virtual TStrategyTickType GetTickType() { return m_enumTickType; };
	virtual CTick * operator[](unsigned int index) { return m_vecTickPointerContainer[index]; };
	virtual void Release() { delete this; };
	virtual void LoadFromDB(const char * jsonstring,std::mutex & mtx)
	{
		m_vecTicks.clear();
		m_vecTickPointerContainer.clear();
		std::unique_lock<std::mutex> _lock(mtx,std::defer_lock);
		_lock.lock();
		LoadFromDatabase<T>(jsonstring, m_vecTicks);
		_lock.unlock();
		m_vecTickPointerContainer.reserve(m_vecTicks.size());
		for_each(m_vecTicks.begin(),
			m_vecTicks.end(),
			[this](T&tar) {m_vecTickPointerContainer.push_back(&tar);});
		AbandonOverlap(m_vecTickPointerContainer);

	}
	/*virtual void LoadFromFile(istream & file)
	{
		m_vecTicks.clear();
		m_vecTickPointerContainer.clear();
		boost::archive::binary_iarchive ia(file);
		ia >> *this;
		m_vecTickPointerContainer.reserve(m_vecTicks.size());
		for_each(
			m_vecTicks.begin(),
			m_vecTicks.end(),
			[this](T&tar) {m_vecTickPointerContainer.push_back(&tar);});
	}*/
	virtual void SaveToFile(ostream & file)
	{
		boost::archive::binary_oarchive oa(file);
		oa & m_enumTickType;
		auto len = m_vecTicks.size();
		oa & len;
		for (size_t i = 0;i < len;i++)
			oa & m_vecTicks[i];
	}
	virtual size_t GetLength() { return m_vecTicks.size(); };
	virtual vector<CTick*>::iterator begin() { return m_vecTickPointerContainer.begin(); };
	virtual vector<CTick*>::iterator end() { return m_vecTickPointerContainer.end(); };
	vector<CTick*> m_vecTickPointerContainer;
	vector<T> m_vecTicks;
	TStrategyTickType m_enumTickType;
};
#endif
