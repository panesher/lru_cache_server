#pragma once

#include <cache/lru_cache.h>
#include <queue/waitfree_spsc_queue.h>
#include <cache/task.h>
#include <memory>

namespace server::cache {

using TaskQueue = queue::WaitFreeSPSCQueue<TaskWithId>;
using ResultQueue = queue::WaitFreeSPSCQueue<Result>;
using Cache = cache::LRUCache<std::string, std::shared_ptr<std::string>>;

class TaskExecutor {
 public:
  TaskExecutor(TaskQueue& queue, ResultQueue& result_queue, Cache& cache);

  void Start();
  void Stop();

 private:
  std::shared_ptr<std::string> Execute(Task&& task);

  std::atomic<bool> started_{false};
  TaskQueue& queue_;
  ResultQueue& result_queue_;
  Cache& cache_;
};

}  // namespace server::cache
