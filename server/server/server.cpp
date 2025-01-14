#include <cache/executor.h>
#include <server/connection.h>
#include <server/server.h>

#include <boost/asio.hpp>
#include <boost/bind.hpp>

namespace server {

Server::Server(const size_t port, cache::TaskQueue& queue,
               cache::ResultQueue& result_queue)
    : io_context(),
      acceptor_(io_context, boost::asio::ip::tcp::endpoint(
                                boost::asio::ip::tcp::v4(), port)),
      queue_(queue),
      result_queue_(result_queue) {}

void Server::Serve() {
  auto connection = AddConnection();
  acceptor_.async_accept(
      connection->Socket(),
      boost::bind(&Server::AcceptHandler, this, connection, _1));
  boost::asio::ip::tcp::socket socket(io_context);

  io_context.run();
}

void Server::Stop() { io_context.stop(); }

void Server::Dispatch() {
  while (true) {
    auto result = result_queue_.TryPop();
    if (!result.has_value()) {
      continue;
    }
    result->conn->Write(result->result);
  }
}

std::shared_ptr<Connection> Server::AddConnection() {
  auto [connection, _] = connections_.emplace(
      connection_id_,
      std::make_shared<Connection>(connection_id_, queue_, *this));
  connection_id_++;
  return connection->second;
}

void Server::RemoveConnection(const size_t id) {
  connections_.erase(id);
}

void Server::AcceptHandler(
    const std::shared_ptr<Connection> acceptred_connection,
    const boost::system::error_code& ec) {
  if (!ec) {
    acceptred_connection->Start();
  } else {
    RemoveConnection(acceptred_connection->Id());
  }
  auto connection = AddConnection();
  acceptor_.async_accept(
      connection->Socket(),
      boost::bind(&Server::AcceptHandler, this, connection, _1));
}

}  // namespace server
