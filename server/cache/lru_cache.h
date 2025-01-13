#pragma once

#include <list>
#include <unordered_map>

namespace server::cache {

template <typename Key, typename Value>
class LRUCache {
 public:
  LRUCache(int capacity) : capacity_(capacity) {}

  std::optional<Value> Get(const Key &key) {
    if (auto map_it = lru_map_.find(key); map_it != lru_map_.end()) {
      auto value = map_it->second->second;
      MoveFront(map_it->second);

      return value;
    }
    return std::nullopt;
  }

  void Put(const Key &key, Value &&value) {
    if (auto map_it = lru_map_.find(key); map_it != lru_map_.end()) {
      map_it->second->second = std::move(value);
      return MoveFront(map_it->second);
    }

    if (lru_list_.size() >= capacity_) {
      auto &[back_key, _] = lru_list_.back();
      lru_map_.erase(back_key);
      lru_list_.pop_back();
    }

    lru_list_.emplace_front(key, std::move(value));
    auto it = lru_list_.begin();
    lru_map_[key] = it;
  }

  bool Delete(const Key &key) {
    if (auto map_it = lru_map_.find(key); map_it != lru_map_.end()) {
      lru_list_.erase(map_it->second);
      lru_map_.erase(map_it);
      return true;
    }

    return false;
  }

  size_t Size() const {
    return lru_list_.size();
  }

 private:
  using List = std::list<std::pair<Key, Value>>;
  using Iterator = List::iterator;

  void MoveFront(Iterator &it) {
    if (it != lru_list_.begin()) {
      lru_list_.splice(lru_list_.begin(), lru_list_, it);
    }
  }

  std::unordered_map<Key, Iterator> lru_map_;
  List lru_list_;
  size_t capacity_;
};

}  // namespace server::cache
