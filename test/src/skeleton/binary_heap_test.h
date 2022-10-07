/**
 * \file test/src/skeleton/binary_heap_test.h
 *
 * \author Alexandre Ashade Lassance Cunha \<aashade@br.ibm.com\>
 * \copyright © IBM Corp.
 * \date 2015
 */

#ifndef TEST_SRC_SKELETON_BINARY_HEAP_TEST_H_
#define TEST_SRC_SKELETON_BINARY_HEAP_TEST_H_

#include <gtest/gtest.h>
#include <cstdint>

#include "src/skeleton/heaps/binary_heap.h"
#include "src/skeleton/utils/i_hashable.h"
#include "test/src/utils/test_utils.h"

namespace skeleton_test {

template<class Inner>
class Key {
};

template<>
class Key<int32_t> : public skeleton::IHashable, public skeleton::IEquatable<
    Key<int32_t>> {
  int value_;
 public:
  Key()
      : value_(INT32_MAX) {
  }
  explicit Key(int32_t value)
      : value_(value) {
  }
  Key(const Key<int32_t>& other)
      : value_(other.value_) {
  }
  Key<int32_t>& operator=(const Key<int32_t>& other) {
    value_ = other.value_;
    return *this;
  }
  bool operator <(const Key<int32_t>& other) const {
    return value_ < other.value_;
  }
  bool operator ==(const Key<int32_t>& other) const {
    return value_ == other.value_;
  }
  bool Equals(const Key<int32_t>& other) const noexcept override {
    return value_ == other.value_;
  }
  std::size_t GetHash() const noexcept override {
    skeleton::Hasher<int32_t> hasher;  // specialised in test_utils.h
    return hasher(value_);
  }
};

TEST(Binary_Heap, Insert_Should_Create_New_Heap_Element) {
  // arrange
  skeleton::BinaryHeap<Key<int32_t>> heap;
  Key<int32_t> key = Key<int32_t>(10);
  // initial assert
  EXPECT_TRUE(heap.IsEmpty());

  // act
  heap.Insert(key);

  // assert
  EXPECT_FALSE(heap.IsEmpty());
}

TEST(Binary_Heap, First_Returns_Highest_Priority_Element_Perm1) {
  // arrange
  skeleton::BinaryHeap<Key<int32_t>> heap;
  Key<int32_t> medium = Key<int32_t>(3),
               high = Key<int32_t>(1),
               low = Key<int32_t>(10);
  heap.Insert(low);
  heap.Insert(medium);
  heap.Insert(high);

  // act
  auto result = heap.First();
  auto result_remove = heap.Remove();

  // assert
  EXPECT_EQ(high, result);
  EXPECT_EQ(high, result_remove);
}
TEST(Binary_Heap, First_Returns_Highest_Priority_Element_Perm2) {
  // arrange
  skeleton::BinaryHeap<Key<int32_t>> heap;
  Key<int32_t> medium = Key<int32_t>(3),
               high = Key<int32_t>(1),
               low = Key<int32_t>(10);
  heap.Insert(low);
  heap.Insert(high);
  heap.Insert(medium);

  // act
  auto result = heap.First();
  auto result_remove = heap.Remove();

  // assert
  EXPECT_EQ(high, result);
  EXPECT_EQ(high, result_remove);
}
TEST(Binary_Heap, First_Returns_Highest_Priority_Element_Perm3) {
  // arrange
  skeleton::BinaryHeap<Key<int32_t>> heap;
  Key<int32_t> medium = Key<int32_t>(3),
               high = Key<int32_t>(1),
               low = Key<int32_t>(10);
  heap.Insert(medium);
  heap.Insert(low);
  heap.Insert(high);

  // act
  auto result = heap.First();
  auto result_remove = heap.Remove();

  // assert
  EXPECT_EQ(high, result);
  EXPECT_EQ(high, result_remove);
}
TEST(Binary_Heap, First_Returns_Highest_Priority_Element_Perm4) {
  // arrange
  skeleton::BinaryHeap<Key<int32_t>> heap;
  Key<int32_t> medium = Key<int32_t>(3),
               high = Key<int32_t>(1),
               low = Key<int32_t>(10);
  heap.Insert(medium);
  heap.Insert(high);
  heap.Insert(low);

  // act
  auto result = heap.First();
  auto result_remove = heap.Remove();

  // assert
  EXPECT_EQ(high, result);
  EXPECT_EQ(high, result_remove);
}
TEST(Binary_Heap, First_Returns_Highest_Priority_Element_Perm5) {
  // arrange
  skeleton::BinaryHeap<Key<int32_t>> heap;
  Key<int32_t> medium = Key<int32_t>(3),
               high = Key<int32_t>(1),
               low = Key<int32_t>(10);
  heap.Insert(high);
  heap.Insert(medium);
  heap.Insert(low);

  // act
  auto result = heap.First();
  auto result_remove = heap.Remove();

  // assert
  EXPECT_EQ(high, result);
  EXPECT_EQ(high, result_remove);
}
TEST(Binary_Heap, First_Returns_Highest_Priority_Element_Perm6) {
  // arrange
  skeleton::BinaryHeap<Key<int32_t>> heap;
  Key<int32_t> medium = Key<int32_t>(3),
               high = Key<int32_t>(1),
               low = Key<int32_t>(10);
  heap.Insert(high);
  heap.Insert(low);
  heap.Insert(medium);

  // act
  auto result = heap.First();
  auto result_remove = heap.Remove();

  // assert
  EXPECT_EQ(high, result);
  EXPECT_EQ(high, result_remove);
}
TEST(Binary_Heap, Remove_Should_Throw_Exception_For_Empty_Heap) {
  // arrange
  skeleton::BinaryHeap<Key<int32_t>> heap;
  // act and assert
  EXPECT_ANY_THROW(heap.Remove());
}
TEST(Binary_Heap, First_Should_Not_Remove) {
// arrange
  skeleton::BinaryHeap<Key<int32_t>> heap;
  Key<int32_t> medium = Key<int32_t>(3),
               high = Key<int32_t>(1),
               low = Key<int32_t>(10);
  heap.Insert(medium);
  heap.Insert(high);
  heap.Insert(low);

// act
  auto result = heap.First();

// assert
  EXPECT_EQ(high, result);
  EXPECT_EQ(3, heap.HeapSize());
}
TEST(Binary_Heap, InsertThenRemove_Respects_Priority_Order) {
// arrange
  skeleton::BinaryHeap<Key<int32_t>> heap;
  Key<int32_t> medium = Key<int32_t>(3),
               high = Key<int32_t>(1),
               low = Key<int32_t>(10);

// act
  heap.Insert(medium);
  heap.Insert(high);
  heap.Insert(low);

// assert
  EXPECT_EQ(high, heap.Remove());
  EXPECT_EQ(medium, heap.Remove());
  EXPECT_EQ(low, heap.Remove());
}
TEST(Binary_Heap, IncreasePriority_Should_Tranform_Low_To_High_Priority) {
// arrange
  skeleton::BinaryHeap<Key<int32_t>> heap;
  Key<int32_t> medium = Key<int32_t>(3),
               high = Key<int32_t>(1),
               low = Key<int32_t>(10),
               super_high = Key<int32_t>(0);
  skeleton::BinaryHeap<Key<int32_t>>::size_type low_index = 2;
  heap.Insert(medium);
  heap.Insert(high);
  heap.Insert(low);

// act
  heap.IncreasePriority(low_index, super_high);

// assert
  EXPECT_EQ(super_high, heap.Remove());
  EXPECT_EQ(high, heap.Remove());
  EXPECT_EQ(medium, heap.Remove());
}

TEST(Binary_Heap, IncreasePriority_Should_Replace_Low_With_High_Priority) {
// arrange
  skeleton::BinaryHeap<Key<int32_t>> heap;
  Key<int32_t> medium = Key<int32_t>(3),
               high = Key<int32_t>(1),
               low = Key<int32_t>(10),
               super_high = Key<int32_t>(0);
  heap.Insert(medium);
  heap.Insert(high);
  heap.Insert(low);

// act
  heap.IncreasePriority(low, super_high);

// assert
  EXPECT_EQ(super_high, heap.Remove());
  EXPECT_EQ(high, heap.Remove());
  EXPECT_EQ(medium, heap.Remove());
  EXPECT_TRUE(heap.IsEmpty());
}

TEST(Binary_Heap, IncreasePriority_Should_Replace_Low_With_Medium_Priority) {
// arrange
  skeleton::BinaryHeap<Key<int32_t>> heap;
  Key<int32_t> medium = Key<int32_t>(3),
               high = Key<int32_t>(1),
               low = Key<int32_t>(10),
               new_medium = Key<int32_t>(2);
  heap.Insert(medium);
  heap.Insert(high);
  heap.Insert(low);

// act
  heap.IncreasePriority(low, new_medium);

// assert
  EXPECT_EQ(high, heap.Remove());
  EXPECT_EQ(new_medium, heap.Remove());
  EXPECT_EQ(medium, heap.Remove());
  EXPECT_TRUE(heap.IsEmpty());
}

TEST(Binary_Heap, IncreasePriority_Should_Not_Change_Priority) {
// arrange
  skeleton::BinaryHeap<Key<int32_t>> heap;
  Key<int32_t> medium = Key<int32_t>(3),
               high = Key<int32_t>(1),
               low = Key<int32_t>(10),
               worst = Key<int32_t>(11);
  skeleton::BinaryHeap<Key<int32_t>>::size_type low_index = 2;
  heap.Insert(medium);
  heap.Insert(high);
  heap.Insert(low);

// act
  EXPECT_ANY_THROW(heap.IncreasePriority(low_index, worst));
}

TEST(Binary_Heap, IncreasePriority_Should_Not_Change_Priority_By_Key) {
// arrange
  skeleton::BinaryHeap<Key<int32_t>> heap;
  Key<int32_t> medium = Key<int32_t>(3),
               high = Key<int32_t>(1),
               low = Key<int32_t>(10),
               worst = Key<int32_t>(11);
  heap.Insert(medium);
  heap.Insert(high);
  heap.Insert(low);

// act
  EXPECT_ANY_THROW(heap.IncreasePriority(low, worst));
}

TEST(Binary_Heap, Remove_Should_Returns_Highest_Priority_Element_With_4_Elements) {
  // arrange
  skeleton::BinaryHeap<Key<int32_t>> heap;
  Key<int32_t> k1 = Key<int32_t>(1),
               k2 = Key<int32_t>(3),
               k3 = Key<int32_t>(2),
               k4 = Key<int32_t>(4);
  heap.Insert(k1);
  heap.Insert(k2);
  heap.Insert(k3);
  heap.Insert(k4);

  // act
  auto result1 = heap.Remove();
  auto result2 = heap.Remove();
  auto result3 = heap.Remove();
  auto result4 = heap.Remove();

  // assert
  EXPECT_EQ(k1, result1);
  EXPECT_EQ(k3, result2);
  EXPECT_EQ(k2, result3);
  EXPECT_EQ(k4, result4);
}

}  // namespace skeleton_test

#endif  // TEST_SRC_SKELETON_BINARY_HEAP_TEST_H_
