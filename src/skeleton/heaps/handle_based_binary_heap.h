/**
 * \file src/skeleton/heaps/handle_based_binary_heap.h
 * \brief This file implements a generic binary heap.
 *
 * \author Alexandre Ashade Lassance Cunha \<aashade@br.ibm.com\>
 * \author Giulia Duncan Coutinho \<coutingi@br.ibm.com\>
 * \copyright © IBM Corp.
 * \date 2017
 */

#ifndef SRC_SKELETON_HEAPS_HANDLE_BASED_BINARY_HEAP_H_
#define SRC_SKELETON_HEAPS_HANDLE_BASED_BINARY_HEAP_H_

#include <vector>
#include <utility>
#include <unordered_map>
#include <algorithm>
#include <exception>

#include "src/skeleton/utils/default_comparer.h"
#include "src/skeleton/utils/comparer.h"

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
class HandleBasedBinaryHeap {
 public:
  using size_type = std::size_t;

  /**
   * \brief Auxiliary class to define a handle to an element in the queue. This handle should be
   * kept by the user of the queue to keep track of the element's position in the queue.
   */
  class Handle {
   private:
    TKey priority_;
    size_type index_;
   public:
    size_type Index() const noexcept {
      return index_;
    }
    const TKey &Priority() const noexcept {
      return priority_;
    }
    void SetPriority(const TKey &new_priority) {
      priority_ = new_priority;
    }
    void SetIndex(const size_type index) {
      index_ = index;
    }
    Handle(TKey priority, size_type index) : priority_(priority), index_(index) {
    }
  };

  using handle = Handle *const;  // const pointer to Handle

 private:
  std::vector<Handle *> elements_;
  const TKeyComparer comparer_;
  int64_t decreaseCounter_, removeCounter_;

  size_type Parent(size_type index) const noexcept {
    return (index - 1) / 2;
  }
  size_type Left(size_type index) const noexcept {
    return 1 + (index * 2);
  }
  size_type Right(size_type index) const noexcept {
    return 2 + (index * 2);
  }
  size_type HeapSize() const noexcept {
    return elements_.size();
  }
  bool IsValid(const size_type index) const noexcept {
    return index < HeapSize();
  }
  void MinHeapify(const size_type index) noexcept {
    auto left = Left(index);
    auto right = Right(index);
    size_type smallest;
    if (IsValid(left)
        && IsLess(elements_.at(left), elements_.at(index))) {
      smallest = left;
    } else {
      smallest = index;
    }
    if (IsValid(right)
        && IsLess(elements_.at(right), elements_.at(smallest))) {
      smallest = right;
    }
    if (smallest != index) {
      Swap(index, smallest);
      MinHeapify(smallest);
    }
  }

  void Swap(size_type index1, size_type index2) {
    handle copy_of_2 = elements_.at(index2);
    elements_[index2] = elements_.at(index1);
    elements_[index1] = copy_of_2;

    elements_[index1]->SetIndex(index1);
    elements_[index2]->SetIndex(index2);
  }

  bool IsLess(handle left, handle right) const {
    return comparer_.IsLess(left->Priority(), right->Priority());
  }

 public:
  HandleBasedBinaryHeap()
      : comparer_(), decreaseCounter_(0), removeCounter_(0) {
  }

  ~HandleBasedBinaryHeap() {
    for (Handle *element : elements_)
      delete element;
    elements_.clear();
  }

  /**
   * \brief Moves an specific element to the beginning of the queue (by decreasing the value of
   * its priority). The new priority should not exist in the queue.
   */
  void DecreasePriority(handle old_priority_handle, const TKey &new_priority) {
    if (comparer_.IsLess(old_priority_handle->Priority(), new_priority))
      throw std::exception {};  // value is not decreasing as expected.
    auto index = old_priority_handle->Index();
    old_priority_handle->SetPriority(new_priority);
    while (index > 0
        && IsLess(elements_.at(index), elements_.at(Parent(index)))
        ) {
      auto parent_index = Parent(index);
      Swap(index, parent_index);
      index = parent_index;
    }
    ++decreaseCounter_;
  }

  /**
   * \brief Inserts a new priority on the queue and returns a handle to its position. This
   * positions should be kept if this element is supposed to change priority at any time.
   */
  Handle* Insert(const TKey &key) {
    TKey maximum_key;
    auto maximum_handle = new Handle{maximum_key, HeapSize()};
    elements_.push_back(maximum_handle);
    DecreasePriority(maximum_handle, key);
    return maximum_handle;
  }

  /**
   * \brief Removes the first element of the queue (the one having the minimum key value).
   */
  const TKey Remove() {
    if (HeapSize() < 1)
      throw std::exception {};
    const TKey minimum = First();  // copy key to remove
    delete elements_.at(0);
    elements_.at(0) = elements_.at(HeapSize() - 1);
    elements_.pop_back();
    elements_[0]->SetIndex(0);
    MinHeapify(0);
    ++removeCounter_;
    return minimum;
  }
  const TKey &First() const {
    return elements_.at(0)->Priority();
  }

  /**
   * \brief Pre-allocates space to avoid online reallocation.
   */
  void Reserve(size_type number_of_elements) {
    elements_.reserve(number_of_elements);
  }

  /**
   * \brief Returns true if the queue has no elements, and false otherwise.
   */
  bool IsEmpty() const noexcept {
    return elements_.empty();
  }

  /**
   * \brief Returns the logical negation of IsEmpty().
   */
  bool HasElements() const noexcept {
    return !IsEmpty();
  }

  int64_t RemoveCounter() const {
    return removeCounter_;
  }

  int64_t DecreaseCounter() const {
    return decreaseCounter_;
  }
};

} /* namespace skeleton */

#endif  // SRC_SKELETON_HEAPS_HANDLE_BASED_BINARY_HEAP_H_
