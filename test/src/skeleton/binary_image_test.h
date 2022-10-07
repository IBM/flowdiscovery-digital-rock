/**
 * \file test/src/skeleton/binary_image_test.h
 *
 * \author Alexandre Ashade Lassance Cunha \<aashade@br.ibm.com\>
 * \copyright © IBM Corp.
 * \date 2015
 */

#ifndef TEST_SRC_SKELETON_BINARY_IMAGE_TEST_H_
#define TEST_SRC_SKELETON_BINARY_IMAGE_TEST_H_

#include <gtest/gtest.h>
#include <vector>
#include <algorithm>
#include "src/skeleton/images/binary_image.h"
#include "test/src/utils/test_utils.h"

namespace skeleton_test {

using bit_image2d = skeleton::BinaryImage<geometry_utils::Pixel>;

TEST(BinaryImage,
     IsObjectPoint_Returns_False_For_Non_Object_Point_Of_2D_Image) {
  // arrange
  bit_image2d::ObjectPoints points;
  bit_image2d::size width = 10, height = 10;
  points.resize(static_cast<std::size_t>(width * height),
                bit_image2d::Background);
  auto test_pixel = geometry_utils::Pixel {1, 2};
  points[0] = bit_image2d::Foreground;
  bit_image2d image{points, {width, height}};

  // act
  auto result = image.IsObjectPoint(test_pixel);

  // assert
  EXPECT_FALSE(result);
}

TEST(BinaryImage, IsObjectPoint_Returns_True_For_Object_Point_Of_2D_Image) {
  // arrange
  bit_image2d::ObjectPoints points;
  bit_image2d::size width = 10, height = 10;
  points.resize(static_cast<std::size_t>(width * height),
                bit_image2d::Background);
  auto test_pixel = geometry_utils::Pixel {1, 2};
  points[static_cast<std::size_t>(test_pixel.x() + width * test_pixel.y())] =
      bit_image2d::Foreground;
  bit_image2d image{points, {width, height}};

  // act
  auto result = image.IsObjectPoint(test_pixel);

  // assert
  EXPECT_TRUE(result);
}

TEST(BinaryImage, GetNumberOfDimensions_Returns_2_For_2D_Image) {
  // arrange
  bit_image2d::ObjectPoints points;
  bit_image2d::size width = 10, height = 10;
  points.resize(static_cast<std::size_t>(width * height));
  bit_image2d image{points, {width, height}};

  // act
  auto result = image.GetNumberOfDimensions();

  // assert
  EXPECT_EQ(2, result);
}

TEST(BinaryImage, GetDimensionSize_Returns_10_For_Width_10_2D_Image) {
  // arrange
  bit_image2d::ObjectPoints points;
  bit_image2d::size width = 10, height = 2;
  points.resize(static_cast<std::size_t>(width * height));
  bit_image2d image{points, {width, height}};

  // act
  auto result = image.GetDimensionSize(0);

  // assert
  EXPECT_EQ(width, result);
}

TEST(BinaryImage, GetDimensionSize_Returns_10_For_Height_10_2D_Image) {
  // arrange
  bit_image2d::ObjectPoints points;
  bit_image2d::size width = 2, height = 10;
  points.resize(static_cast<std::size_t>(width * height));
  bit_image2d image{points, {width, height}};

  // act
  auto result = image.GetDimensionSize(1);

  // assert
  EXPECT_EQ(height, result);
}

TEST(BinaryImage, Iterator_Should_Return_All_Points_Once) {
  // arrange
  bit_image2d::ObjectPoints points;
  bit_image2d::size width = 2, height = 3;
  points.resize(static_cast<std::size_t>(width * height),
                bit_image2d::Background);
  points[0] = bit_image2d::Foreground;  // {0, 0}
  points[1] = bit_image2d::Foreground;  // {1, 0}
  points[2] = bit_image2d::Foreground;  // {0, 1}

  bit_image2d image{points, {width, height}};

  std::vector<geometry_utils::Pixel> expected = {{0, 0}, {1, 0}, {0, 1},
                                                 {1, 1}, {0, 2}, {1, 2}};

  // act
  std::vector<geometry_utils::Pixel> result;
  for (auto iterator = image.cbegin(); iterator != image.cend(); ++iterator) {
    auto pixel = *iterator;
    result.push_back(pixel);
  }

  // assert
  EXPECT_EQ(expected.size(), result.size());
  EXPECT_TRUE(
      std::is_permutation(image.cbegin(), image.cend(), expected.cbegin()));
}

TEST(BinaryImage, Iterator_Should_Return_All_Points_Once_For_Big_Image) {
  // arrange
  bit_image2d::ObjectPoints points;
  bit_image2d::size width = 50, height = 50;
  points.resize(static_cast<std::size_t>(width * height),
                bit_image2d::Background);
  bit_image2d image{points, {width, height}};
  std::vector<geometry_utils::Pixel> expected;
  expected.reserve(static_cast<std::size_t>(width * height));
  for (geometry_utils::Pixel::coordinate x = 0; x != width; ++x)
    for (geometry_utils::Pixel::coordinate y = 0; y != height; ++y)
      expected.push_back(geometry_utils::Pixel {x, y});

  // act / assert
  EXPECT_TRUE(
      std::is_permutation(image.cbegin(), image.cend(), expected.cbegin()));
}

TEST(BinaryImage, Iterators_Should_Remain_Equal_After_Increment) {
  // arrange
  bit_image2d::ObjectPoints points;
  bit_image2d::size width = 100, height = 100;
  points.resize(static_cast<std::size_t>(width * height),
                bit_image2d::Background);
  bit_image2d image{points, {width, height}};

  auto iterator_first = image.cbegin();
  auto iterator_second = image.cbegin();

  // act / assert
  while (iterator_first != image.cend()) {
    EXPECT_TRUE(iterator_first == iterator_second);
    ++iterator_first;
    ++iterator_second;
  }
}

}  // namespace skeleton_test

#endif  // TEST_SRC_SKELETON_BINARY_IMAGE_TEST_H_
