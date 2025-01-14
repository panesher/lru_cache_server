#include "executor.h"

#include <cache/task.h>

#include <atomic>

namespace server::cache {

TaskExecutor::TaskExecutor(TaskQueue& queue, ResultQueue& result_queue,
                           Cache& cache)
    : queue_(queue), result_queue_(result_queue), cache_(cache) {}

void TaskExecutor::Start() {
  started_.store(true, std::memory_order_relaxed);
  while (started_.load(std::memory_order_relaxed)) {
    auto task_with_callback = queue_.TryPop();
    if (!task_with_callback.has_value()) {
      continue;
    }
    auto result = Execute(std::move(task_with_callback->task));

    /// could be missed, should retry query
    result_queue_.TryPush({std::move(result), task_with_callback->conn});
  }
}

std::shared_ptr<std::string> TaskExecutor::Execute(Task&& task) {
  switch (task.type) {
    case TaskType::kPut:
      if (task.value == nullptr) {
        return std::make_shared<std::string>("Value should be defined");
      }
      cache_.Put(task.key, std::move(task.value));
      return std::make_shared<std::string>("OK");
    case TaskType::kGet:
      return cache_.Get(task.key).value_or(
          std::make_shared<std::string>("NOT FOUND"));
    case TaskType::kDelete:
      return cache_.Delete(task.key)
                 ? std::make_shared<std::string>("DELETED")
                 : std::make_shared<std::string>("DOESN'T EXISTS");
  }
}

void TaskExecutor::Stop() { started_.store(false, std::memory_order_relaxed); }

}  // namespace server::cache
