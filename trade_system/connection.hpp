//
// connection.hpp
// ~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2015 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef HTTP_SERVER2_CONNECTION_HPP
#define HTTP_SERVER2_CONNECTION_HPP

#include <boost/asio.hpp>
#include <boost/array.hpp>
#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include "CommuModForServInterface.h"
#include <string>
namespace http {
namespace server2 {
#define MAX_READ_BUFFER_LENGTH 102400
#define MAX_WRITE_BUFFER_LENGTH 102400
/// Represents a single connection from a client.
class connection
  : public boost::enable_shared_from_this<connection>,
    private boost::noncopyable
{
public:
  /// Construct a connection with the given io_service.
  explicit connection(
	  boost::asio::io_service& io_service,
	  CCommuModForServSpi * handler
	  );

  /// Get the socket associated with the connection.
  boost::asio::ip::tcp::socket& socket();

  
  size_t ReadComplete(const boost::system::error_code & err, size_t bytes);
  /// Start the first asynchronous operation for the connection.
  void start();

private:
  /// Handle completion of a read operation.
  void handle_read(const boost::system::error_code& e,
      std::size_t bytes_transferred);

  /// Handle completion of a write operation.
  void handle_write(const boost::system::error_code& e);

  /// Socket for the connection.
  boost::asio::ip::tcp::socket socket_;

  /// The handler used to process the incoming request.
  CCommuModForServSpi * request_handler_;

  /// Buffer for incoming data.
  char read_buffer_[MAX_READ_BUFFER_LENGTH];
  char write_buffer_[MAX_WRITE_BUFFER_LENGTH];
 
};

typedef boost::shared_ptr<connection> connection_ptr;

} // namespace server2
} // namespace http

#endif // HTTP_SERVER2_CONNECTION_HPP
