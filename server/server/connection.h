#pragma once

#include <cache/executor.h>

#include <boost/asio.hpp>
#include <memory>

namespace server {

class Server;

class Connection : public std::enable_shared_from_this<Connection> {
 public:
  Connection(const size_t id, cache::TaskQueue& queue, Server& server);

  const size_t Id() const { return id_; }

  void Start();

  boost::asio::ip::tcp::socket& Socket() { return socket_; }

  void Write(const std::shared_ptr<std::string> message);

 private:
  void Read();
  void OnRead(const boost::system::error_code& ec, size_t bytes_transferred);

  const size_t id_;
  cache::TaskQueue& queue_;
  Server& server_;
  boost::asio::ip::tcp::socket socket_;
  boost::asio::streambuf read_buffer_;
  std::istream input_stream_;
};

}  // namespace server
