/**
 * \file src/skeleton/heaps/binary_heap.h
 * \brief This file implements a generic binary heap.
 *
 * \author Alexandre Ashade Lassance Cunha \<aashade@br.ibm.com\>
 * \copyright © IBM Corp.
 * \date 2015
 */

#ifndef SRC_SKELETON_HEAPS_BINARY_HEAP_H_
#define SRC_SKELETON_HEAPS_BINARY_HEAP_H_

#include <vector>
#include <utility>
#include <unordered_map>
#include <algorithm>

#include "src/skeleton/utils/default_comparer.h"
#include "src/skeleton/utils/comparer.h"
#include "src/skeleton/utils/hasher.h"

namespace skeleton {

/*
 * Min Binary Heap implementation. TKey is the key type,
 * should be MoveConstructible and MoveAssinable and DefaultConstructible.
 * TKeyComparer is default constructible and should have the method
 * bool IsLess (const TKey& left, const TKey& right)
 * that returns the (boolean) result of comparison (left < right).
 * IMPORTANT: the default TKey is defined such that
 * IsLess(any_key, default_key) = true for each any_key that is not default.
 * */

template<class TKey, class TKeyComparer = DefaultComparer<TKey>>
class BinaryHeap {
 public:
  using size_type = std::size_t;

 private:
  std::vector<TKey> elements_;
  const TKeyComparer comparer_;
  using map = std::unordered_map<TKey, size_type, Hasher<TKey>, Comparer<TKey>>;
  map key_to_index_;

  size_type Parent(size_type index) const noexcept {
    return (index - 1) / 2;
  }
  size_type Left(size_type index) const noexcept {
    return 1 + (index * 2);
  }
  size_type Right(size_type index) const noexcept {
    return 2 + (index * 2);
  }
  bool IsValid(const size_type index) const noexcept {
    return index < HeapSize();
  }
  void MinHeapify(const size_type index) noexcept {
    auto left = Left(index);
    auto right = Right(index);
    size_type smallest;
    if (IsValid(left)
        && comparer_.IsLess(elements_.at(left), elements_.at(index))) {
      smallest = left;
    } else {
      smallest = index;
    }
    if (IsValid(right)
        && comparer_.IsLess(elements_.at(right), elements_.at(smallest))) {
      smallest = right;
    }
    if (smallest != index) {
      std::swap(elements_.at(index), elements_.at(smallest));
      key_to_index_.at(elements_.at(index)) = index;
      key_to_index_.at(elements_.at(smallest)) = smallest;
      MinHeapify(smallest);
    }
  }

 public:
  BinaryHeap()
      : comparer_(),
        key_to_index_() {
  }
  size_type HeapSize() const noexcept {
    return elements_.size();
  }
  void IncreasePriority(const TKey& old_priority, const TKey& new_priority) {
    if (comparer_.IsLess(old_priority, new_priority))
      throw std::exception { };  // value is not decreasing as expected.
    auto old_index = key_to_index_.at(old_priority);
    IncreasePriority(old_index, new_priority);
  }
  void IncreasePriority(size_type index, const TKey& new_priority) {
    const auto& old_priority = elements_.at(index);
    if (comparer_.IsLess(elements_.at(index), new_priority))
      throw std::exception { };  // value is not decreasing as expected.
    key_to_index_.erase(old_priority);
    elements_.at(index) = new_priority;
    key_to_index_.insert(std::make_pair(new_priority, index));
    while (index > 0
        && comparer_.IsLess(elements_.at(index), elements_.at(Parent(index)))) {
      auto parent_index = Parent(index);
      std::swap(elements_.at(index), elements_.at(parent_index));
      key_to_index_.at(elements_.at(index)) = index;
      key_to_index_.at(elements_.at(parent_index)) = parent_index;
      index = Parent(index);
    }
  }
  void Insert(const TKey& key) {
    TKey maximum_key;
    elements_.push_back(maximum_key);
    IncreasePriority(HeapSize() - 1, key);
  }
  const TKey Remove() {
    if (HeapSize() < 1)
      throw std::exception { };
    const TKey minimum = First();  // copy key to remove
    elements_.at(0) = elements_.at(HeapSize() - 1);
    elements_.pop_back();
    key_to_index_.erase(minimum);
    MinHeapify(0);
    return minimum;
  }
  const TKey& First() const {
    return elements_.at(0);
  }
  void Reserve(size_type number_of_elements) {
    elements_.reserve(number_of_elements);
  }
  bool IsEmpty() const noexcept {
    return elements_.empty();
  }
  bool HasElements() const noexcept {
    return !IsEmpty();
  }
};

} /* namespace skeleton */

#endif  // SRC_SKELETON_HEAPS_BINARY_HEAP_H_
