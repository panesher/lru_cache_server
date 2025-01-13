#include "task.h"
#include <string_view>

namespace server::cache {

std::optional<Task> ParseTask(const std::string_view& message) {
  if (message.empty()) {
    return std::nullopt;
  }
  TaskType type = TaskType::kPut;
  if (message[0] == 'D') {
    type = TaskType::kDelete;
  } else if (message[0] == 'G') {
    type = TaskType::kGet;
  } else if (message[0] != 'P') {
    return std::nullopt;
  }

  auto kv = message.substr(2);
  if (type != TaskType::kPut) {
    return Task{
      type = type,
      .key = std::string{kv},
    };
  }

  auto space_pos = kv.find(' ');
  if (space_pos == std::string_view::npos) {
    return std::nullopt;
  }
  return Task{
    .type = type,
    .key = std::string{kv.substr(0, space_pos)},
    .value = std::make_shared<std::string>(kv.substr(space_pos + 1)),
  };
}

}  // namespace server::cache
