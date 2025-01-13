#pragma once

#include <string>
#include <optional>
#include <functional>

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
  size_t id;
};

struct Result {
  std::shared_ptr<std::string> result;
  size_t id;
};

std::optional<Task> ParseTask(const std::string_view& message);

}  // namespace server::cache
