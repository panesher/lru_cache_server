#pragma once

#include <atomic>
#include <mutex>
#include <queue>

namespace server::queue {

template <typename T>
class LockQueue {
 public:
  LockQueue() = default;

  bool TryPush(T&& t) {
    std::lock_guard lock(mu_);
    queue_.push(std::move(t));
    is_empty.store(false, std::memory_order_relaxed);
    return true;
  }

  std::optional<T> TryPop() {
    if (is_empty.load(std::memory_order_relaxed)) {
      return std::nullopt;
    }
    std::lock_guard lock(mu_);
    if (queue_.empty()) {
      is_empty.store(true, std::memory_order_relaxed);
      return std::nullopt;
    }
    auto result = std::move(queue_.front());
    queue_.pop();
    return result;
  }

 private:
  std::atomic<bool> is_empty{true};
  std::queue<T> queue_;
  std::mutex mu_;
};

}  // namespace server::queue
