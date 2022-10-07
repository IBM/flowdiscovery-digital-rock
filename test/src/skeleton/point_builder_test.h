/**
 * \file test/src/skeleton/point_builder_test.h
 *
 * \author Alexandre Ashade Lassance Cunha \<aashade@br.ibm.com\>
 * \copyright © IBM Corp.
 * \date 2015
 */

#ifndef TEST_SRC_SKELETON_POINT_BUILDER_TEST_H_
#define TEST_SRC_SKELETON_POINT_BUILDER_TEST_H_

#include <gtest/gtest.h>

#include "src/skeleton/points/point_builder.h"
#include "test/src/utils/test_utils.h"

namespace skeleton_test {

TEST(Point_Builder, PointBuilder_IsBuildingCorrect2DPoint) {
  // arrange
  const skeleton::Point<2>::coordinate x = 1, y = 3;
  skeleton::Point<2> expected { { x, y } };

  // act
  auto result = skeleton::PointBuilder::Get(x, y);

  // assert
  test_utils::is_same_type<skeleton::Point<2>>::Assert(result);
  EXPECT_TRUE(test_utils::PointsEqual(expected, result));
}

}  // namespace skeleton_test

#endif  // TEST_SRC_SKELETON_POINT_BUILDER_TEST_H_
