/**
 * \file test/src/skeleton/image__image_iterator_point_test.h
 *
 * \author Alexandre Ashade Lassance Cunha \<aashade@br.ibm.com\>
 * \copyright © IBM Corp.
 * \date 2015
 */

#ifndef TEST_SRC_SKELETON_IMAGE__IMAGE_ITERATOR_POINT_TEST_H_
#define TEST_SRC_SKELETON_IMAGE__IMAGE_ITERATOR_POINT_TEST_H_

#include <gtest/gtest.h>
#include <algorithm>
#include "src/skeleton/images/image.h"
#include "src/skeleton/points/pixel.h"
#include "test/src/utils/test_utils.h"

namespace skeleton_test {

TEST(Image__ImageIterator__Point,
    Iteration_Of_2D_Image_Returns_All_Points_Once) {
  // arrange
  skeleton::Image<2>::ObjectPoints points;
  const skeleton::Pixel point_1 { { 1, 2 } };
  const skeleton::Pixel point_2 { { 2, 2 } };
  const skeleton::Pixel point_3 { { 3, 2 } };
  const skeleton::Pixel point_4 { { 1, 3 } };
  const skeleton::Pixel point_5 { { 1, 7 } };
  points.insert(point_1);
  points.insert(point_2);
  points.insert(point_4);
  points.insert(point_3);
  points.insert(point_5);
  skeleton::Image<2> image { points, { 10, 10 } };

  // act
  auto result = std::is_permutation(points.cbegin(), points.cend(),
                                    image.cbegin());

  // assert
  EXPECT_TRUE(result);
}

}  // namespace skeleton_test

#endif  // TEST_SRC_SKELETON_IMAGE__IMAGE_ITERATOR_POINT_TEST_H_
