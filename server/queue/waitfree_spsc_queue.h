#pragma once

#include <atomic>
#include <boost/circular_buffer.hpp>
#include <optional>

namespace server::queue {

template <typename T>
class WaitFreeSPSCQueue {
 public:
  WaitFreeSPSCQueue(size_t buffer_size) : buffer_(buffer_size) {}

  bool TryPush(T&& t) {
    auto head = head_.load(std::memory_order_relaxed);
    if (tail_.load(std::memory_order_acquire) + buffer_.size() == head) {
      return false;
    }

    buffer_[head % buffer_.size()] = std::move(t);
    head_.store(head + 1, std::memory_order_release);
    return true;
  }

  std::optional<T> TryPop() {
    auto tail = tail_.load(std::memory_order_relaxed);
    if (tail == head_.load(std::memory_order_acquire)) {
      return std::nullopt;
    }

    auto result = std::move(buffer_[tail % buffer_.size()]);
    tail_.store(tail + 1, std::memory_order_release);
    return result;
  }

 private:
  std::atomic<uint64_t> head_{0};
  std::atomic<uint64_t> tail_{0};

  std::vector<T> buffer_;
};

}  // namespace server::queue
