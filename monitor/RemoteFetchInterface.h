#pragma once
#pragma region ptree
#ifndef BOOST_SPIRIT_THREADSAFE
#define BOOST_SPIRIT_THREADSAFE
#endif
#include <boost/property_tree/ptree.hpp>  
#include <boost/property_tree/json_parser.hpp> 
using namespace boost::property_tree;
#pragma endregion
class MRemoteFetchInterface
{
public:
    virtual bool Fetch(const ptree & in, ptree & out) = 0;
};
