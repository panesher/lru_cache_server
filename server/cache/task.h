#pragma once

#include <server/connection-fwd.h>

#include <functional>
#include <memory>
#include <optional>
#include <string>

namespace server::cache {

enum class TaskType {
  kPut = 0,
  kGet = 1,
  kDelete = 2,
};

struct Task {
  TaskType type;
  std::string key;
  std::shared_ptr<std::string> value;
};

struct TaskWithId {
  Task task;
  std::shared_ptr<Connection> conn;
};

struct Result {
  std::shared_ptr<std::string> result;
  std::shared_ptr<Connection> conn;
};

std::optional<Task> ParseTask(const std::string_view& message);

}  // namespace server::cache
