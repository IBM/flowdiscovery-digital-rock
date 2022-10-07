/**
 * \file test/src/segmentation/helper_segmentation_tests.h
 * \brief Contains regression tests of helper methods used in greyscale segmentation.
 *
 * \authors Rodrigo Neumann Barros Ferreira \<rneumann@br.ibm.com\>
 * \copyright © IBM Corp.
 * \date 2017
 */

#ifndef TEST_SRC_SEGMENTATION_HELPER_SEGMENTATION_TESTS_H_
#define TEST_SRC_SEGMENTATION_HELPER_SEGMENTATION_TESTS_H_

#include <gtest/gtest.h>
#include <string>
#include "src/segmentation/segmentation_utils.h"
#include "test/src/utils/segmentation_test_utils.h"

TEST(Segmentation_Helper, CreateRawFromImage_Test) {
  // Test conversion of TIFF image into RAW file
  const std::string &input_file = "test/src/segmentation/input/cameraman.tif";
  const std::string &output_file = "test/results/converted_image.raw";
  bool it_works = segmentation_test_utils::CreateRawFromImage(input_file, output_file);

  // Remove temporary file
  std::remove(output_file.c_str());

  // Perform tests
  EXPECT_TRUE(it_works);
}

TEST(Segmentation_Helper, CreateRawFromLinearGradient_Test) {
  // Create image file from linear gradient function
  const std::string &output_file = "test/results/linear_image.raw";
  bool it_works = segmentation_test_utils::CreateRawFromLinearGradient(output_file);

  // Remove temporary file
  std::remove(output_file.c_str());

  // Perform tests
  EXPECT_TRUE(it_works);
}

TEST(Segmentation_Helper, CalculateGreyscaleStatistics_Test) {
  // Create image file from linear gradient function
  const std::string &output_file = "test/results/linear_image.raw";
  segmentation_test_utils::CreateRawFromLinearGradient(output_file);

  // Read linear image into arma::Cube
  arma::Cube<segmentation_test_utils::ImageType> image;
  image.load(output_file, arma::raw_binary);

  // Calculate histogram from image
  arma::Mat<double>::fixed<256, 3> histogram = CalculateGreyscaleStatistics(image);

  // Remove temporary file
  std::remove(output_file.c_str());

  // Perform tests
  arma::vec linear = arma::linspace<arma::vec>(1.0 / 256.0, 1, 256);
  EXPECT_TRUE(arma::all(histogram.col(1) == linear(0)));
  EXPECT_TRUE(arma::approx_equal(histogram.col(2), linear, "absdiff", 1.0e-12));
}

TEST(Segmentation_Helper, MovingAverageFilter_Test) {
  // Create image file from linear gradient function
  const std::string &output_file = "test/results/linear_image.raw";
  segmentation_test_utils::CreateRawFromLinearGradient(output_file);

  // Read generated image into arma::Cube
  arma::Cube<segmentation_test_utils::ImageType> image;
  image.load(output_file, arma::raw_binary);

  // Calculate histogram from image
  arma::Mat<double>::fixed<256, 3> histogram = CalculateGreyscaleStatistics(image);
  arma::Mat<double>::fixed<256, 3> smoothed = histogram;
  MovingAverageFilter(smoothed);

  // Remove temporary file
  std::remove(output_file.c_str());

  // Perform tests
  EXPECT_TRUE(arma::approx_equal(histogram, smoothed, "absdiff", 1.0e-12));
}

TEST(Segmentation_Helper, FindLocalMaxima_Test) {
  // Create empty vector
  arma::vec::fixed<256> v;

  // Generate local maxima and test if they are being detected correctly
  for (auto N = 1U; N != 10U; ++N) {
    arma::uvec max_idx = arma::linspace<arma::uvec>(1, 254, N);
    v.ones();
    v.elem(max_idx) += 1;
    EXPECT_TRUE(arma::approx_equal(FindLocalMaxima(v), max_idx, "absdiff", 1.0e-12));
    EXPECT_EQ(FindLocalMaxima(v).n_elem, N);
  }
}

TEST(Segmentation_Helper, FindLocalMinima_Test) {
  // Create empty vector
  arma::vec::fixed<256> v;

  // Generate local minima and test if they are being detected correctly
  for (auto N = 1U; N != 10U; ++N) {
    arma::uvec min_idx = arma::linspace<arma::uvec>(1, 254, N);
    v.ones();
    v.elem(min_idx) -= 1;
    EXPECT_TRUE(arma::approx_equal(FindLocalMinima(v), min_idx, "absdiff", 1.0e-12));
    EXPECT_EQ(FindLocalMinima(v).n_elem, N);
  }
}

#endif  // TEST_SRC_SEGMENTATION_HELPER_SEGMENTATION_TESTS_H_
