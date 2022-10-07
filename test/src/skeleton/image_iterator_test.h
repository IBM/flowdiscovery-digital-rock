/**
 * \file test/src/skeleton/image_iterator_test.h
 *
 * \author Alexandre Ashade Lassance Cunha \<aashade@br.ibm.com\>
 * \copyright © IBM Corp.
 * \date 2015
 */

#ifndef TEST_SRC_SKELETON_IMAGE_ITERATOR_TEST_H_
#define TEST_SRC_SKELETON_IMAGE_ITERATOR_TEST_H_

#include <gtest/gtest.h>
#include <unordered_set>
#include <vector>
#include "src/skeleton/images/image_iterator.h"

namespace skeleton_test {

using __set = std::unordered_set<int>;


TEST(Image_Iterator,
     Two_Equal_Iterators_Should_remain_Equal_After_Postfix_Increment) {
  // arrange
  __set set;
  set.insert(1);  // dummy
  skeleton::ImageIterator<__set ::const_iterator> image_iterator_first { set
      .cbegin() };
  skeleton::ImageIterator<__set ::const_iterator> image_iterator_second { set
      .cbegin() };

  // act
  image_iterator_first++;
  image_iterator_second++;

  // assert
  EXPECT_EQ(image_iterator_first, image_iterator_second);
}

TEST(Image_Iterator,
     Two_Equal_Iterators_Should_remain_Equal_After_Pretfix_Increment) {
  // arrange
  __set set;
  set.insert(1);  // dummy
  skeleton::ImageIterator<__set ::const_iterator> image_iterator_first { set
      .cbegin() };
  skeleton::ImageIterator<__set ::const_iterator> image_iterator_second { set
      .cbegin() };

  // act
  ++image_iterator_first;
  ++image_iterator_second;

  // assert
  EXPECT_EQ(image_iterator_first, image_iterator_second);
}

TEST(Image_Iterator, Equality_Returns_True_For_Equal_Iterators) {
  // arrange
  __set set;
  set.insert(1);  // dummy
  skeleton::ImageIterator<__set ::const_iterator> image_iterator_first { set
      .cbegin() };
  skeleton::ImageIterator<__set ::const_iterator> image_iterator_second { set
      .cbegin() };

  // act
  auto result = (image_iterator_first == image_iterator_second)
      && (image_iterator_second == image_iterator_first);

  // assert
  EXPECT_TRUE(result);
}

TEST(Image_Iterator, Equality_Returns_False_For_Equal_Iterators) {
  // arrange
  std::unordered_set<int> set;
  set.insert(1);  // dummy
  skeleton::ImageIterator<__set ::const_iterator> image_iterator_first { set
      .cbegin() };
  skeleton::ImageIterator<__set ::const_iterator> image_iterator_second { set
      .cend() };

  // act
  auto result = (image_iterator_first == image_iterator_second)
      && (image_iterator_second == image_iterator_first);

  // assert
  EXPECT_FALSE(result);
}

TEST(Image_Iterator, Inequality_Returns_True_For_Unequal_Iterators) {
  // arrange
  std::unordered_set<int> set;
  set.insert(1);  // dummy
  skeleton::ImageIterator<__set ::const_iterator> image_iterator_first { set
      .cbegin() };
  skeleton::ImageIterator<__set ::const_iterator> image_iterator_second { set
      .cend() };

  // act
  auto result = (image_iterator_first != image_iterator_second)
      && (image_iterator_second != image_iterator_first);

  // assert
  EXPECT_TRUE(result);
}

TEST(Image_Iterator, Inequality_Returns_False_For_Equal_Iterators) {
  // arrange
  std::unordered_set<int> set;
  set.insert(1);  // dummy
  skeleton::ImageIterator<__set ::const_iterator> image_iterator_first { set
      .cbegin() };
  skeleton::ImageIterator<__set ::const_iterator> image_iterator_second { set
      .cbegin() };

  // act
  auto result = (image_iterator_first != image_iterator_second)
      && (image_iterator_second != image_iterator_first);

  // assert
  EXPECT_FALSE(result);
}

TEST(Image_Iterator, Default_Iterators_Are_Equal_To_End_Iterators) {
  // arrange
  std::unordered_set<int> set;
  set.insert(1);  // dummy
  skeleton::ImageIterator<__set ::const_iterator> iterator_end { set.cend() };
  skeleton::ImageIterator<__set ::const_iterator> iterator;

  // act
  auto result = (iterator_end == iterator) && (iterator == iterator_end);

  // assert
  EXPECT_TRUE(result);
}

TEST(Image_Iterator, Default_Iterators_Are_Equal) {
  // arrange
  skeleton::ImageIterator<__set ::const_iterator> iterator_first;
  skeleton::ImageIterator<__set ::const_iterator> iterator_second;

  // act
  auto result = (iterator_first == iterator_second)
      && (iterator_first == iterator_second);

  // assert
  EXPECT_TRUE(result);
}

TEST(Image_Iterator, A_Copy_Constructed_Iterator_Should_Equal_The_Original) {
  // arrange
  __set set;
  set.insert(1);  // dummy
  skeleton::ImageIterator<__set ::const_iterator> iterator { set.cbegin() };

  // act: copy construct new iterator
  skeleton::ImageIterator<__set ::const_iterator> iterator_copy { iterator };

  // assert
  EXPECT_EQ(iterator, iterator_copy);
}

TEST(Image_Iterator, A_Copy_Assigned_Iterator_Should_Equal_The_Original) {
  // arrange
  __set set;
  set.insert(1);  // dummy
  skeleton::ImageIterator<__set ::const_iterator> iterator { set.cbegin() };
  skeleton::ImageIterator<__set ::const_iterator> iterator_copy;

  // act: copy assign new iterator
  iterator_copy = iterator;

  // assert
  EXPECT_EQ(iterator, iterator_copy);
}

TEST(Image_Iterator, Member_Access_Operator_Should_Return_Member_Value) {
  // arrange
  struct dummy {
    int x, y;
  };
  const int expected_x = 1;
  const int expected_y = 2;
  std::vector<dummy> set;
  set.push_back(dummy { expected_x, expected_y });
  skeleton::ImageIterator<std::vector<dummy>::const_iterator> iterator { set
      .cbegin() };

  // act: copy assign new iterator
  auto x_value = iterator->x;
  auto y_value = iterator->y;

  // assert
  EXPECT_EQ(expected_x, x_value);
  EXPECT_EQ(expected_y, y_value);
}

TEST(Image_Iterator, Dereference_Operator_Should_Return_Original_Value) {
  // arrange
  __set set;
  const int expected = 1;
  set.insert(expected);  // dummy
  skeleton::ImageIterator<__set ::const_iterator> iterator { set.cbegin() };

  // act
  auto result = *iterator;

  // assert
  EXPECT_EQ(expected, result);
}

TEST(Image_Iterator, Member_Access_Operator_Should_Return_Const_Pointer) {
  // arrange
  struct dummy {
    int x, y;
  };
  const int expected_x = 1;
  const int expected_y = 2;
  std::vector<dummy> set;
  set.push_back(dummy { expected_x, expected_y });
  skeleton::ImageIterator<std::vector<dummy>::iterator> iterator { set.begin() };

  // act
  skeleton::ImageIterator<std::vector<dummy>::iterator>::pointer result =
      iterator.operator->();

  // assert
  test_utils::is_const_type::Assert(result);
}

TEST(Image_Iterator, Dereference_Operator_Should_Return_Const_Reference) {
  // arrange
  struct dummy {
    int x, y;
  };
  const int expected_x = 1;
  const int expected_y = 2;
  std::vector<dummy> set;
  set.push_back(dummy { expected_x, expected_y });
  skeleton::ImageIterator<std::vector<dummy>::iterator> iterator { set.begin() };

  // act
  skeleton::ImageIterator<std::vector<dummy>::iterator>::reference result =
      *iterator;

  // assert
  test_utils::is_const_type::AssertReference(result);
}

TEST(Image_Iterator, Postfix_Increment_Should_Return_Old_value_And_Increment) {
  // arrange
  const int first_expected = 1;
  const int second_expected = 12;
  std::vector<int> set { first_expected, second_expected };
  skeleton::ImageIterator<std::vector<int>::iterator> iterator { set.begin() };

  auto first_iterator = iterator;
  auto second_iterator = skeleton::ImageIterator<std::vector<int>::iterator> {
      set.begin() + 1 };
  auto expected_final_iterator = skeleton::ImageIterator<
      std::vector<int>::iterator> { set.end() };

  // act
  auto result_before_first_increment = iterator++;
  auto result_before_second_increment = iterator++;

  // assert
  EXPECT_EQ(first_iterator, result_before_first_increment);
  EXPECT_EQ(second_iterator, result_before_second_increment);
  EXPECT_EQ(expected_final_iterator, iterator);
}

}  // namespace skeleton_test

#endif  // TEST_SRC_SKELETON_IMAGE_ITERATOR_TEST_H_
