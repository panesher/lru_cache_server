#include "connection.h"

#include <cache/executor.h>
#include <cache/task.h>
#include <server/server.h>

#include <boost/bind.hpp>
#include <memory>

namespace server {

Connection::Connection(const size_t id, cache::TaskQueue& queue, Server& server)
    : id_(id),
      queue_(queue),
      server_(server),
      socket_(server.IoContext()),
      input_stream_(&read_buffer_) {}

void Connection::Start() { Read(); }

void Connection::Read() {
  boost::asio::async_read_until(socket_, read_buffer_, '\n',
                                boost::bind(&Connection::OnRead, this, _1, _2));
}

void Connection::OnRead(const boost::system::error_code& ec,
                        size_t bytes_transferred) {
  if (ec) {
    socket_.close();
    server_.RemoveConnection(Id());
    return;
  }
  std::string message;
  std::getline(input_stream_, message);
  Read();
  auto task = cache::ParseTask(message);
  if (!task) {
    Write(std::make_shared<std::string>("Failed to parse"));
  }
  if (!queue_.TryPush({std::move(*task), shared_from_this()})) {
    Write(std::make_shared<std::string>("Server overloaded"));
  }
}

void Connection::Write(const std::shared_ptr<std::string> message) {
  socket_.async_send(boost::asio::buffer(*message + '\n'),
                     [message](const boost::system::error_code& ec,
                               size_t bytes_transferred) {});
}

}  // namespace server
