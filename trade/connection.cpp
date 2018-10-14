
#include "connection.hpp"
#include <vector>
#include <boost/bind.hpp>
#include <sstream>
#include <iostream>
#include <string>

#ifndef BOOST_SPIRIT_THREADSAFE
#define BOOST_SPIRIT_THREADSAFE
#endif
#include <boost/property_tree/ptree.hpp>  
#include <boost/property_tree/json_parser.hpp> 
using namespace boost::property_tree;
using namespace std;


namespace http {
namespace server2 {

connection::connection(boost::asio::io_service& io_service,
	CCommuModForServSpi * handler)
  : socket_(io_service),
    request_handler_(handler)
{
}

boost::asio::ip::tcp::socket& connection::socket()
{
  return socket_;
}

size_t connection::ReadComplete(const boost::system::error_code & err, size_t bytes)
{
	if (err)
		return 0;
	if (bytes >= MAX_READ_BUFFER_LENGTH)
		return 0;
	if (bytes < sizeof(int32_t))
		return 1;
	else
	{
		if (*(int32_t*)read_buffer_ <= static_cast<int32_t>(bytes))
		{	
			read_buffer_[*(int32_t*)read_buffer_] = 0;
			return 0;
		}
		else
			return 1;
	}
}

void connection::start()
{

	async_read(
		socket_,
		boost::asio::buffer(read_buffer_, MAX_READ_BUFFER_LENGTH),
		boost::bind(&connection::ReadComplete, this, _1, _2),
		boost::bind(&connection::handle_read, shared_from_this(),
			boost::asio::placeholders::error,
			boost::asio::placeholders::bytes_transferred)
		);
}

void connection::handle_read(const boost::system::error_code& e,
    std::size_t bytes_transferred)
{
  if (!e)
  {
	  std::stringstream in_ss, out_ss;
	  ptree in_tree, out_tree;
	 
	try {
		in_ss << read_buffer_ + sizeof(int32_t);
		read_json(in_ss, in_tree);
		request_handler_->OnCommunicate(in_tree, out_tree);
		write_json(out_ss, out_tree);
		if (out_ss.str().size() > MAX_WRITE_BUFFER_LENGTH - sizeof(int32_t))
			throw std::runtime_error("The size of output is longer than MAX_WRITE_BUFFER_LENGTH");
		*((int32_t*)write_buffer_)= out_ss.str().size()+sizeof(int32_t);
		strcpy(write_buffer_ + sizeof(int32_t), out_ss.str().c_str());
		write_buffer_[out_ss.str().size() + sizeof(int32_t)] = 0;
	}
	catch (std::exception &err)
	{
		ptree error;
		error.put("error", err.what());
		std::stringstream ss_error;
		write_json(ss_error, error);
		
		*((int32_t*)write_buffer_) = strlen(ss_error.str().c_str())+sizeof(int32_t);
		strcpy(write_buffer_ + sizeof(int32_t), ss_error.str().c_str());
		write_buffer_[ss_error.str().size() + sizeof(int32_t)] = 0;
	}
	catch (...)
	{
		ptree error;
		error.put("error", "CommunicateError");
		std::stringstream ss_error;
		write_json(ss_error, error);

		*((int32_t*)write_buffer_) = strlen(ss_error.str().c_str()) + sizeof(int32_t);
		strcpy(write_buffer_ + sizeof(int32_t), ss_error.str().c_str());
		write_buffer_[ss_error.str().size() + sizeof(int32_t)] = 0;
	}

	boost::asio::async_write(socket_, boost::asio::buffer(write_buffer_),
		boost::bind(&connection::handle_write, shared_from_this(),
			boost::asio::placeholders::error));
  }

}

void connection::handle_write(const boost::system::error_code& e)
{
  if (!e)
  {
    boost::system::error_code ignored_ec;
    socket_.shutdown(boost::asio::ip::tcp::socket::shutdown_both, ignored_ec);
  }

}

} // namespace server2
} // namespace http
