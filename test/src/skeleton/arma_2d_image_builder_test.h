/**
 * \file test/src/skeleton/arma_2d_image_builder_test.h
 *
 * \author Alexandre Ashade Lassance Cunha \<aashade@br.ibm.com\>
 * \copyright © IBM Corp.
 * \date 2015
 */

#ifndef TEST_SRC_SKELETON_ARMA_2D_IMAGE_BUILDER_TEST_H_
#define TEST_SRC_SKELETON_ARMA_2D_IMAGE_BUILDER_TEST_H_

#include <gtest/gtest.h>
#include <armadillo>
#include "src/skeleton/points/pixel.h"
#include "src/skeleton/images/image.h"
#include "src/skeleton/images/arma_2d_i_image_builder.h"
#include "test/src/utils/test_utils.h"

namespace skeleton_test {

TEST(Arma2DImageBuilder,
    BuildImage_Should_Construct_3x2_Image_From_Mat_With_Zeros) {
  // arrange
  const int expected_number_of_rows = 3;
  const int expected_number_of_columns = 2;
  const arma::Mat<uint8_t> matrix = arma::zeros<arma::Mat<uint8_t>>(expected_number_of_rows,
                                                                    expected_number_of_columns);
  skeleton::Arma2DImageBuilder<uint8_t> builder;

  // act
  auto result = builder.BuildImage(matrix);

  // assert
  EXPECT_EQ(expected_number_of_rows, result->GetDimensionSize(0));
  EXPECT_EQ(expected_number_of_columns, result->GetDimensionSize(1));

  EXPECT_TRUE(result->IsObjectPoint(skeleton::Pixel { { 0, 0 } }));
  EXPECT_TRUE(result->IsObjectPoint(skeleton::Pixel { { 1, 0 } }));
  EXPECT_TRUE(result->IsObjectPoint(skeleton::Pixel { { 2, 0 } }));
  EXPECT_TRUE(result->IsObjectPoint(skeleton::Pixel { { 0, 1 } }));
  EXPECT_TRUE(result->IsObjectPoint(skeleton::Pixel { { 1, 1 } }));
  EXPECT_TRUE(result->IsObjectPoint(skeleton::Pixel { { 2, 1 } }));
}

TEST(Arma2DImageBuilder,
    BuildImage_Should_Build_Image_With_A_Single_Pixel_From_Mat_With_One_Zero) {
  // arrange
  const int expected_number_of_rows = 3;
  const int expected_number_of_columns = 2;
  arma::Mat<uint8_t> matrix = arma::ones<arma::Mat<uint8_t>>(expected_number_of_rows,
                                                             expected_number_of_columns);
  matrix(0, 0) = 0;
  skeleton::Arma2DImageBuilder<uint8_t> builder;

  // act
  auto result = builder.BuildImage(matrix);

  // assert
  EXPECT_EQ(expected_number_of_rows, result->GetDimensionSize(0));
  EXPECT_EQ(expected_number_of_columns, result->GetDimensionSize(1));

  EXPECT_TRUE(result->IsObjectPoint(skeleton::Pixel { { 0, 0 } }));
  EXPECT_FALSE(result->IsObjectPoint(skeleton::Pixel { { 1, 0 } }));
  EXPECT_FALSE(result->IsObjectPoint(skeleton::Pixel { { 2, 0 } }));
  EXPECT_FALSE(result->IsObjectPoint(skeleton::Pixel { { 0, 1 } }));
  EXPECT_FALSE(result->IsObjectPoint(skeleton::Pixel { { 1, 1 } }));
  EXPECT_FALSE(result->IsObjectPoint(skeleton::Pixel { { 2, 1 } }));
}

}  // namespace skeleton_test

#endif  // TEST_SRC_SKELETON_ARMA_2D_IMAGE_BUILDER_TEST_H_
