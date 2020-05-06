#pragma once
#include <sstream>
typedef size_t (*PFNCommunicateType)(char * address, unsigned int port, std::stringstream & in, std::stringstream & out);