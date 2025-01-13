#pragma once

#include <cache/executor.h>
#include <server/connection.h>

#include <boost/asio.hpp>
#include <memory>
#include <string>
#include <unordered_map>

namespace server {

class Server {
 public:
  Server(const size_t port, cache::TaskQueue& queue,
         cache::ResultQueue& result_queue_);

  void Serve();
  void Stop();

  void Dispatch();

  boost::asio::io_context& IoContext() { return io_context; }

 private:
  void AcceptHandler(const std::shared_ptr<Connection> connection,
                     const boost::system::error_code& ec);
  std::shared_ptr<Connection> AddConnection();

  friend class Connection;
  void RemoveConnection(const size_t id);

  boost::asio::io_context io_context;
  boost::asio::ip::tcp::acceptor acceptor_;
  std::unordered_map<size_t, std::shared_ptr<Connection>> connections_;
  size_t connection_id_{0};
  cache::TaskQueue& queue_;
  cache::ResultQueue& result_queue_;
};

}  // namespace server
