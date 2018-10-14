#ifndef _COMPRETRADESYSTEMHEADERS_COMMUMODFORSERVINTERFACE_H
#define _COMPRETRADESYSTEMHEADERS_COMMUMODFORSERVINTERFACE_H

#ifdef WIN32

#ifdef _EXPORT 
#define EXPORT_INTERFACE __declspec(dllexport) 
#else
#define EXPORT_INTERFACE __declspec(dllimport) 
#endif

#else 
#define EXPORT_INTERFACE
#endif
#include <sstream>
#include <memory>

#ifndef BOOST_SPIRIT_THREADSAFE
#define BOOST_SPIRIT_THREADSAFE
#endif
#include <boost/property_tree/ptree.hpp>  
#include <boost/property_tree/json_parser.hpp> 
using namespace boost::property_tree;

class CCommuModForServSpi
{
public:
	virtual void OnCommunicate(const ptree & in, ptree & out) = 0;
};

class  MCommuModForServInterface
{
public:
	static MCommuModForServInterface * CreateApi(
		const char * address, 
		unsigned int _Port, 
		CCommuModForServSpi * _Spi, 
		size_t _threadCount);
	virtual bool StartListen() = 0;
	virtual void StopListen() = 0;
	virtual void Release() = 0;
};
#endif
