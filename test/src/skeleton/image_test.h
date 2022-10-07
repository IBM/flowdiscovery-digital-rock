/**
 * \file test/src/skeleton/image_test.h
 *
 * \author Alexandre Ashade Lassance Cunha \<aashade@br.ibm.com\>
 * \copyright © IBM Corp.
 * \date 2015
 */

#ifndef TEST_SRC_SKELETON_IMAGE_TEST_H_
#define TEST_SRC_SKELETON_IMAGE_TEST_H_

#include <gtest/gtest.h>
#include "src/skeleton/images/image.h"
#include "src/skeleton/points/pixel.h"
#include "test/src/utils/test_utils.h"

namespace skeleton_test {

TEST(Image, IsObjectPoint_Returns_False_For_Non_Object_Point_Of_2D_Image) {
  // arrange
  skeleton::Image<2>::ObjectPoints points;
  auto test_pixel = skeleton::Pixel { { 1, 2 } };
  points.insert(test_pixel);
  skeleton::Image<2> image { points, { 10, 10 } };
  auto expected = true;

  // act
  auto result = image.IsObjectPoint(test_pixel);

  // assert
  EXPECT_EQ(expected, result);
}

TEST(Image, IsObjectPoint_Returns_True_For_Object_Point_Of_2D_Image) {
  // arrange
  skeleton::Image<2>::ObjectPoints points;
  auto test_pixel = skeleton::Pixel { { 1, 2 } };
  auto other_pixel = skeleton::Pixel { { 0, 0 } };
  points.insert(test_pixel);
  skeleton::Image<2> image { points, { 10, 10 } };
  auto expected = false;

  // act
  auto result = image.IsObjectPoint(other_pixel);

  // assert
  EXPECT_EQ(expected, result);
}

TEST(Image, IsObjectPoint_Returns_False_For_Non_Object_Point_Of_3D_Image) {
  // arrange
  skeleton::Image<3>::ObjectPoints points;
  auto test_pixel = skeleton::Point<3> { { 1, 2, 5 } };
  points.insert(test_pixel);
  skeleton::Image<3> image { points, { 10, 10, 10 } };
  auto expected = true;

  // act
  auto result = image.IsObjectPoint(test_pixel);

  // assert
  EXPECT_EQ(expected, result);
}

TEST(Image, IsObjectPoint_Returns_True_For_Object_Point_Of_3D_Image) {
  // arrange
  skeleton::Image<3>::ObjectPoints points;
  auto test_voxel = skeleton::Point<3> { { 1, 2, 5 } };
  auto other_voxel = skeleton::Point<3> { { 0, 0, 0 } };
  points.insert(test_voxel);
  skeleton::Image<3> image { points, { 10, 10, 10 } };
  auto expected = false;

  // act
  auto result = image.IsObjectPoint(other_voxel);

  // assert
  EXPECT_EQ(expected, result);
}

TEST(Image, GetNumberOfDimensions_Returns_2_For_2D_Image) {
  // arrange
  skeleton::Image<2>::ObjectPoints points;
  skeleton::Image<2> image { points, { 10, 10 } };

  // act
  auto result = image.GetNumberOfDimensions();

  // assert
  EXPECT_EQ(2, result);
}

TEST(Image, GetNumberOfDimensions_Returns_3_For_3D_Image) {
  // arrange
  skeleton::Image<3>::ObjectPoints points;
  skeleton::Image<3> image { points, { 10, 10, 10 } };

  // act
  auto result = image.GetNumberOfDimensions();

  // assert
  EXPECT_EQ(3, result);
}

TEST(Image, GetDimensionSize_Returns_Correct_ValuesFor_2D_Image) {
  // arrange
  const int size_x = 10;
  const int size_y = 20;
  skeleton::Image<2>::ObjectPoints points;
  skeleton::Image<2> image { points, { size_x, size_y } };

  // act
  auto result_x = image.GetDimensionSize(0);
  auto result_y = image.GetDimensionSize(1);

  // assert
  EXPECT_EQ(size_x, result_x);
  EXPECT_EQ(size_y, result_y);
}

TEST(Image, GetDimensionSize_Returns_Correct_ValuesFor_3D_Image) {
  // arrange
  const int size_x = 10;
  const int size_y = 20;
  const int size_z = 30;
  skeleton::Image<3>::ObjectPoints points;
  skeleton::Image<3> image { points, { size_x, size_y, size_z } };

  // act
  auto result_x = image.GetDimensionSize(0);
  auto result_y = image.GetDimensionSize(1);
  auto result_z = image.GetDimensionSize(2);

  // assert
  EXPECT_EQ(size_x, result_x);
  EXPECT_EQ(size_y, result_y);
  EXPECT_EQ(size_z, result_z);
}

}  // namespace skeleton_test

#endif  // TEST_SRC_SKELETON_IMAGE_TEST_H_
