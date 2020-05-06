#ifndef COMMUNICATIONMODULEFORCLIENT_COMMUMODFORCLIEINTERFACE_H
#define COMMUNICATIONMODULEFORCLIENT_COMMUMODFORCLIEINTERFACE_H
#include <sstream>
typedef size_t (*PFNCommunicateType)(char * address, unsigned int port, std::stringstream & in, std::stringstream & out);
#endif