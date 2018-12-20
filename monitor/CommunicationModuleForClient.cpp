// CommunicationModuleForClient.cpp : 定义 DLL 应用程序的导出函数。
//

#include "stdafx.h"
#include <sstream>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <memory>
#include <iostream>
using namespace std;

#define MAX_ASIO_READ_BUFFER_LENGTH 102400
size_t ReadComplete(char * buf, size_t maxlen,const boost::system::error_code & err, size_t bytes)
{
    if (err)
        return 0;

    if (bytes < sizeof(int32_t))
        return 1;
    if (bytes >= maxlen)
        return 0;
    else
    {
        size_t  length = *(size_t*)buf;
        if (length < bytes)
            return 0;
        else
            return 1;
    }
}

void Communicate(const char * address, unsigned int port,const std::stringstream & in, std::stringstream & out)
{
    try {
        using namespace boost::asio;
        if (in.str().size() >= MAX_ASIO_READ_BUFFER_LENGTH - sizeof(int32_t))
            throw std::runtime_error("The input string is too long.Is must shorter than MAX_ASIO_READ_BUFFER_LENGTH-sizeof(int32_t)");
        char recvbuf[MAX_ASIO_READ_BUFFER_LENGTH];
        ip::tcp::endpoint ep(ip::address::from_string(address), port);
        io_service service;
        boost::asio::ip::tcp::socket sock_(service);
        sock_.connect(ep);
        size_t PacketLength = in.str().size() + sizeof(int32_t);
        std::unique_ptr<char[]> sendbuf(new char[PacketLength]);
        *((size_t*)sendbuf.get()) = PacketLength;
        strncpy(sendbuf.get() + sizeof(int32_t), in.str().c_str(), in.str().size());
        sock_.write_some(buffer(sendbuf.get(), PacketLength));
        auto rcvlen = read(
            sock_,
            buffer(recvbuf),
            boost::bind(&ReadComplete, recvbuf, sizeof(recvbuf), _1, _2)
            );
        recvbuf[rcvlen] = 0;
        out << recvbuf + sizeof(int32_t);
    }
    catch (std::exception & err)
    {
        cout << err.what() << endl;
    }
}
