#ifndef HASH_SET_SEQUENTIAL_H
#define HASH_SET_SEQUENTIAL_H

#include <cassert>
#include <vector>
#include <functional>
#include <algorithm>

#include "src/hash_set_base.h"

template <typename T>
class HashSetSequential : public HashSetBase<T> {
 public:
  explicit HashSetSequential(size_t capacity) : capacity_(capacity) {
    table_.resize(capacity);
  }

  bool Add(T elem) final {
    u_long myBucket = hashCode_(elem) % capacity_;
    std::vector<T>& b = table_[myBucket];
    if (std::find(b.begin(), b.end(), elem) != b.end()) {
      /*Element found*/
      return false;
    } else {
      b.push_back(elem);
      setSize_++;
    }
    return true;
  }

  bool Remove(T elem) final {
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
    u_long myBucket = hashCode_(elem) % capacity_;
    std::vector<T>& b = table_[myBucket];
    if (std::find(b.begin(), b.end(), elem) != b.end()) {
      /*Element found*/
      return true;
    } 
    return false;
  }

  [[nodiscard]] size_t Size() const final {
    return static_cast<size_t>(setSize_);
  }
 private:
  std::vector<std::vector<T>> table_;
  size_t capacity_;
  int setSize_ = 0;
  std::hash<T> hashCode_;

};

#endif  // HASH_SET_SEQUENTIAL_H
