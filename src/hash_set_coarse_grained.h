#ifndef HASH_SET_COARSE_GRAINED_H
#define HASH_SET_COARSE_GRAINED_H

#include <cassert>
#include <mutex>
#include <vector>
#include <functional>
#include <algorithm>

#include "src/hash_set_base.h"

template <typename T>
class HashSetCoarseGrained : public HashSetBase<T> {
 public:
  explicit HashSetCoarseGrained(size_t initial_capacity) {
    table_.resize(initial_capacity);
  }

  bool Add(T elem) final override {
    std::scoped_lock<std::mutex> lock(mutex_);
    u_long myBucket = hashCode_(elem) % capacity_;
    std::vector<T>& b = table_[myBucket];
    if (std::find(b.begin(), b.end(), elem) != b.end()) {
      /*Element found*/
      return false;
    } else {
      b.push_back(elem);
      setSize_++;
    }
    if (static_cast<u_long>(setSize_) / table_.size() > 4) {
      u_long oldCapacity = table_.size();
      u_long newCapacity = 2 * oldCapacity;
      std::vector<std::vector<T>> oldTable = table_;
      table_.resize(newCapacity);
      for (auto bucket: oldTable) {
        for (auto x: bucket) {
          table_[hashCode_(x) % table_.size()].push_back(x);
        }
      }
    }
    return true;
  }

  bool Remove(T elem) final {
    std::scoped_lock<std::mutex> lock(mutex_);
    u_long myBucket = hashCode_(elem) % capacity_;
    std::vector<T>& b = table_[myBucket];
    if (std::find(b.begin(), b.end(), elem) != b.end()) {
      /*Element found*/
      for (std::vector<int>::iterator it = b.begin(); it != b.end();) {
          if (*it == elem) it = b.erase(it);
          else ++it;
      }
      setSize_--;
    } else {
      return false;
    }
    return true;
  }

  [[nodiscard]] bool Contains(T elem) final {
    std::scoped_lock<std::mutex> lock(mutex_);
    u_long myBucket = hashCode_(elem) % capacity_;
    std::vector<T>& b = table_[myBucket];
    if (std::find(b.begin(), b.end(), elem) != b.end()) {
      /*Element found*/
      return true;
    } 
    return false;
  }

  [[nodiscard]] size_t Size() const final {
    std::scoped_lock<std::mutex> {mutex_};
    return static_cast<size_t>(setSize_);
  }
 private:
  mutable std::mutex mutex_;
  std::vector<std::vector<T>> table_;
  size_t capacity_;
  int setSize_ = 0;
  std::hash<T> hashCode_;
};

#endif  // HASH_SET_COARSE_GRAINED_H
