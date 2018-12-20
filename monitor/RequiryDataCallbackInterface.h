#pragma once
#include <boost/property_tree/ptree.hpp>  
#include <boost/property_tree/json_parser.hpp> 
using namespace boost::property_tree;
typedef int TCustomParam;
class MRequiryDataCallbackInterface
{
public:
    virtual void OnQuiry(ptree &, TCustomParam) = 0;
};
