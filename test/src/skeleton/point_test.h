/**
 * \file test/src/skeleton/point_test.h
 *
 * \author Alexandre Ashade Lassance Cunha \<aashade@br.ibm.com\>
 * \copyright © IBM Corp.
 * \date 2015
 */

#ifndef TEST_SRC_SKELETON_POINT_TEST_H_
#define TEST_SRC_SKELETON_POINT_TEST_H_

#include <gtest/gtest.h>

#include "src/skeleton/points/point.h"
#include "test/src/utils/test_utils.h"

namespace skeleton_test {

TEST(Point, GetNumberOfCoordinates_2D_IsCorrect) {
  skeleton::Point<2> point { { 3, 4 } };
  int expected = 2;
  EXPECT_EQ(point.GetNumberOfCoordinates(), expected);
}

TEST(Point, GetNumberOfCoordinates_3D_IsCorrect) {
  skeleton::Point<3> point { { 3, 4, 5 } };
  int expected = 3;
  EXPECT_EQ(point.GetNumberOfCoordinates(), expected);
}

TEST(Point, GetCoordinate_2D_IsCorrect) {
  skeleton::Point<2> point { { 3, 4 } };
  int expected_first = 3;
  EXPECT_EQ(point.GetCoordinate(0), expected_first);

  int expected_second = 4;
  EXPECT_EQ(point.GetCoordinate(1), expected_second);
}

TEST(Point, GetCoordinate_3D_IsCorrect) {
  skeleton::Point<3> point { { 3, 4, 5 } };
  int expected_first = 3;
  EXPECT_EQ(point.GetCoordinate(0), expected_first);

  int expected_second = 4;
  EXPECT_EQ(point.GetCoordinate(1), expected_second);

  int expected_third = 5;
  EXPECT_EQ(point.GetCoordinate(2), expected_third);
}

TEST(Point, Equality_2D_Returns_True_For_Default_Init_Points) {
  skeleton::Point<2> point_first { };
  skeleton::Point<2> point_second { };

  EXPECT_TRUE(point_first == point_second);
  EXPECT_TRUE(point_second == point_first);
}

TEST(Point, Equality_2D_Returns_True_For_Default_Init_And_Copied_Points) {
  skeleton::Point<2> point_first { };
  skeleton::Point<2> point_second { point_first };

  EXPECT_TRUE(point_first == point_second);
  EXPECT_TRUE(point_second == point_first);
}

TEST(Point, Equality_2D_IsCorrectForEqualPoints) {
  skeleton::Point<2> point_first { { 3, 4 } };
  skeleton::Point<2> point_second { { 3, 4 } };

  EXPECT_TRUE(point_first == point_second);
  EXPECT_TRUE(point_second == point_first);
}

TEST(Point, Equality_2D_IsCorrectForUnequalPoints) {
  skeleton::Point<2> point_first { { 3, 4 } };
  skeleton::Point<2> point_second { { 3, 14 } };

  EXPECT_FALSE(point_first == point_second);
  EXPECT_FALSE(point_second == point_first);
}

TEST(Point, Equality_3D_IsCorrectForEqualPoints) {
  skeleton::Point<3> point_first { { 3, 4, 5 } };
  skeleton::Point<3> point_second { { 3, 4, 5 } };

  EXPECT_TRUE(point_first == point_second);
  EXPECT_TRUE(point_second == point_first);
}

TEST(Point, Equality_3D_IsCorrectForUnequalPoints) {
  skeleton::Point<3> point_first { { 3, 4, 5 } };
  skeleton::Point<3> point_second { { 3, 14, 5 } };

  EXPECT_FALSE(point_first == point_second);
  EXPECT_FALSE(point_second == point_first);
}

TEST(Point, Hash_Returns_Size_t_For_2D_Points) {
  skeleton::Point<2> point { { 3, 4 } };
  skeleton::Hasher<skeleton::Point<2>> hash;

  test_utils::is_same_type<size_t>::Assert(hash(point));
}

TEST(Point, Hash_Returns_Size_t_For_3D_Points) {
  skeleton::Point<3> point { { 3, 4, 5 } };
  skeleton::Hasher<skeleton::Point<3>> hash;

  test_utils::is_same_type<size_t>::Assert(hash(point));
}

TEST(Point, Hash_2D_IsEqualForEqualsPoints) {
  skeleton::Point<2> point_first { { 3, 4 } };
  skeleton::Point<2> point_second { { 3, 4 } };

  skeleton::Hasher<skeleton::Point<2>> hasher;

  EXPECT_TRUE(hasher(point_first) == hasher(point_second));
}

TEST(Point, Hash_3D_IsEqualForEqualsPoints) {
  skeleton::Point<3> point_first { { 3, 4, 5 } };
  skeleton::Point<3> point_second { { 3, 4, 5 } };

  skeleton::Hasher<skeleton::Point<3>> hasher;

  EXPECT_TRUE(hasher(point_first) == hasher(point_second));
}

TEST(Point, Hash_2D_IsNotEqualForDifferentPoints) {
  // NOTE(aashade): equality might hold for unequal points, but with low probability
  // (see definition of hash function of c++ standard).
  skeleton::Point<2> point_first { { 3, 4 } };
  skeleton::Point<2> point_second { { 3, 14 } };

  skeleton::Hasher<skeleton::Point<2>> hasher;

  EXPECT_FALSE(hasher(point_first) == hasher(point_second));
}

TEST(Point, Hash_3D_IsNotEqualForDifferentPoints) {
  // NOTE(aashade): equality might hold for unequal points, but with low probability
  // (see definition of hash function of c++ standard).
  skeleton::Point<3> point_first { { 3, 4, 14 } };
  skeleton::Point<3> point_second { { 3, 14, 3 } };

  skeleton::Hasher<skeleton::Point<3>> hasher;

  EXPECT_FALSE(hasher(point_first) == hasher(point_second));
}

TEST(Point, Comparer_2D_IsNotEqualForDifferentPoints) {
  skeleton::Point<2> point_first { { 3, 4 } };
  skeleton::Point<2> point_second { { 3, 14 } };

  skeleton::Comparer<skeleton::Point<2>> are_equal;

  EXPECT_FALSE(are_equal(point_first, point_second));
}

TEST(Point, Comparer_2D_IsEqualForEqualPoints) {
  skeleton::Point<2> point_first { { 3, 4 } };
  skeleton::Point<2> point_second { { 3, 4 } };

  skeleton::Comparer<skeleton::Point<2>> are_equal;

  EXPECT_TRUE(are_equal(point_first, point_second));
}

TEST(Point, Comparer_3D_IsNotEqualForDifferentPoints) {
  skeleton::Point<3> point_first { { 3, 4, 7 } };
  skeleton::Point<3> point_second { { 3, 14, 7 } };

  skeleton::Comparer<skeleton::Point<3>> are_equal;

  EXPECT_FALSE(are_equal(point_first, point_second));
}

TEST(Point, Comparer_3D_IsEqualForEqualPoints) {
  skeleton::Point<3> point_first { { 3, 4, 6 } };
  skeleton::Point<3> point_second { { 3, 4, 6 } };

  skeleton::Comparer<skeleton::Point<3>> are_equal;

  EXPECT_TRUE(are_equal(point_first, point_second));
}

}  // namespace skeleton_test

#endif  // TEST_SRC_SKELETON_POINT_TEST_H_
