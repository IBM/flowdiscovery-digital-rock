/**
 * \file test/src/segmentation/global_segmentation_tests.h
 * \brief Contains regression tests of greyscale global segmentation methods.
 *
 * The target thresholds in the \c EXPECT_EQ statements were obtained running Fiji's segmentation
 * routines on the \c *.raw files created by \c segmentation_test_utils::CreateRawFromImage().
 *
 * \authors Rodrigo Neumann Barros Ferreira \<rneumann@br.ibm.com\>
 * \copyright © IBM Corp.
 * \date 2017
 */

#ifndef TEST_SRC_SEGMENTATION_GLOBAL_SEGMENTATION_TESTS_H_
#define TEST_SRC_SEGMENTATION_GLOBAL_SEGMENTATION_TESTS_H_

#include <gtest/gtest.h>
#include <string>
#include "src/segmentation/segmentation_utils.h"
#include "src/segmentation/global_segmentation.h"
#include "test/src/utils/segmentation_test_utils.h"

TEST(Segmentation_Global, Cameraman_IsoData_Test) {
  // Test conversion of TIFF image into RAW file
  const std::string &input_file = "test/src/segmentation/input/cameraman.tif";
  const std::string &output_file = "test/results/cameraman.raw";
  segmentation_test_utils::CreateRawFromImage(input_file, output_file);

  // Read generated image into arma::Cube
  arma::Cube<segmentation_test_utils::ImageType> image;
  image.load(output_file, arma::raw_binary);

  // Calculate histogram from image
  arma::Mat<double>::fixed<256, 3> histogram = CalculateGreyscaleStatistics(image);

  // Remove temporary file
  std::remove(output_file.c_str());

  // Perform tests
  EXPECT_EQ(GlobalIsoDataSegmentation(histogram, image),            87);  // IsoData
}

TEST(Segmentation_Global, Cameraman_Otsu_Test) {
  // Test conversion of TIFF image into RAW file
  const std::string &input_file = "test/src/segmentation/input/cameraman.tif";
  const std::string &output_file = "test/results/cameraman.raw";
  segmentation_test_utils::CreateRawFromImage(input_file, output_file);

  // Read generated image into arma::Cube
  arma::Cube<segmentation_test_utils::ImageType> image;
  image.load(output_file, arma::raw_binary);

  // Calculate histogram from image
  arma::Mat<double>::fixed<256, 3> histogram = CalculateGreyscaleStatistics(image);

  // Remove temporary file
  std::remove(output_file.c_str());

  // Perform tests
  EXPECT_EQ(GlobalOtsuSegmentation(histogram, image),               87);  // Otsu
}

TEST(Segmentation_Global, Cameraman_Mean_Test) {
  // Test conversion of TIFF image into RAW file
  const std::string &input_file = "test/src/segmentation/input/cameraman.tif";
  const std::string &output_file = "test/results/cameraman.raw";
  segmentation_test_utils::CreateRawFromImage(input_file, output_file);

  // Read generated image into arma::Cube
  arma::Cube<segmentation_test_utils::ImageType> image;
  image.load(output_file, arma::raw_binary);

  // Calculate histogram from image
  arma::Mat<double>::fixed<256, 3> histogram = CalculateGreyscaleStatistics(image);

  // Remove temporary file
  std::remove(output_file.c_str());

  // Perform tests
  EXPECT_EQ(GlobalMeanSegmentation(histogram, image),              118);  // Mean
}

TEST(Segmentation_Global, Cameraman_Median_Test) {
  // Test conversion of TIFF image into RAW file
  const std::string &input_file = "test/src/segmentation/input/cameraman.tif";
  const std::string &output_file = "test/results/cameraman.raw";
  segmentation_test_utils::CreateRawFromImage(input_file, output_file);

  // Read generated image into arma::Cube
  arma::Cube<segmentation_test_utils::ImageType> image;
  image.load(output_file, arma::raw_binary);

  // Calculate histogram from image
  arma::Mat<double>::fixed<256, 3> histogram = CalculateGreyscaleStatistics(image);

  // Remove temporary file
  std::remove(output_file.c_str());

  // Perform tests
  EXPECT_EQ(GlobalMedianSegmentation(histogram, image),            142);  // Percentile
}

TEST(Segmentation_Global, Cameraman_Li_Test) {
  // Test conversion of TIFF image into RAW file
  const std::string &input_file = "test/src/segmentation/input/cameraman.tif";
  const std::string &output_file = "test/results/cameraman.raw";
  segmentation_test_utils::CreateRawFromImage(input_file, output_file);

  // Read generated image into arma::Cube
  arma::Cube<segmentation_test_utils::ImageType> image;
  image.load(output_file, arma::raw_binary);

  // Calculate histogram from image
  arma::Mat<double>::fixed<256, 3> histogram = CalculateGreyscaleStatistics(image);

  // Remove temporary file
  std::remove(output_file.c_str());

  // Perform tests
  EXPECT_EQ(GlobalLiSegmentation(histogram, image),                 63);  // Li
}

TEST(Segmentation_Global, Cameraman_Minimum_Test) {
  // Test conversion of TIFF image into RAW file
  const std::string &input_file = "test/src/segmentation/input/cameraman.tif";
  const std::string &output_file = "test/results/cameraman.raw";
  segmentation_test_utils::CreateRawFromImage(input_file, output_file);

  // Read generated image into arma::Cube
  arma::Cube<segmentation_test_utils::ImageType> image;
  image.load(output_file, arma::raw_binary);

  // Calculate histogram from image
  arma::Mat<double>::fixed<256, 3> histogram = CalculateGreyscaleStatistics(image);

  // Remove temporary file
  std::remove(output_file.c_str());

  // Perform tests
  EXPECT_EQ(GlobalMinimumSegmentation(histogram, image),            76);  // Minimum
}

TEST(Segmentation_Global, Cameraman_Intermodes_Test) {
  // Test conversion of TIFF image into RAW file
  const std::string &input_file = "test/src/segmentation/input/cameraman.tif";
  const std::string &output_file = "test/results/cameraman.raw";
  segmentation_test_utils::CreateRawFromImage(input_file, output_file);

  // Read generated image into arma::Cube
  arma::Cube<segmentation_test_utils::ImageType> image;
  image.load(output_file, arma::raw_binary);

  // Calculate histogram from image
  arma::Mat<double>::fixed<256, 3> histogram = CalculateGreyscaleStatistics(image);

  // Remove temporary file
  std::remove(output_file.c_str());

  // Perform tests
  EXPECT_EQ(GlobalIntermodesSegmentation(histogram, image),         88);  // Intermodes
}

TEST(Segmentation_Global, Cameraman_Moments_Test) {
  // Test conversion of TIFF image into RAW file
  const std::string &input_file = "test/src/segmentation/input/cameraman.tif";
  const std::string &output_file = "test/results/cameraman.raw";
  segmentation_test_utils::CreateRawFromImage(input_file, output_file);

  // Read generated image into arma::Cube
  arma::Cube<segmentation_test_utils::ImageType> image;
  image.load(output_file, arma::raw_binary);

  // Calculate histogram from image
  arma::Mat<double>::fixed<256, 3> histogram = CalculateGreyscaleStatistics(image);

  // Remove temporary file
  std::remove(output_file.c_str());

  // Perform tests
  EXPECT_EQ(GlobalMomentsSegmentation(histogram, image),           112);  // Moments
}

TEST(Segmentation_Global, Cameraman_MaxShannonEntropy_Test) {
  // Test conversion of TIFF image into RAW file
  const std::string &input_file = "test/src/segmentation/input/cameraman.tif";
  const std::string &output_file = "test/results/cameraman.raw";
  segmentation_test_utils::CreateRawFromImage(input_file, output_file);

  // Read generated image into arma::Cube
  arma::Cube<segmentation_test_utils::ImageType> image;
  image.load(output_file, arma::raw_binary);

  // Calculate histogram from image
  arma::Mat<double>::fixed<256, 3> histogram = CalculateGreyscaleStatistics(image);

  // Remove temporary file
  std::remove(output_file.c_str());

  // Perform tests
  EXPECT_EQ(GlobalMaxShannonEntropySegmentation(histogram, image), 195);  // MaxEntropy
}

TEST(Segmentation_Global, Cameraman_Shanbhag_Test) {
  // Test conversion of TIFF image into RAW file
  const std::string &input_file = "test/src/segmentation/input/cameraman.tif";
  const std::string &output_file = "test/results/cameraman.raw";
  segmentation_test_utils::CreateRawFromImage(input_file, output_file);

  // Read generated image into arma::Cube
  arma::Cube<segmentation_test_utils::ImageType> image;
  image.load(output_file, arma::raw_binary);

  // Calculate histogram from image
  arma::Mat<double>::fixed<256, 3> histogram = CalculateGreyscaleStatistics(image);

  // Remove temporary file
  std::remove(output_file.c_str());

  // Perform tests
  EXPECT_EQ(GlobalShanbhagSegmentation(histogram, image),          133);  // Shanbhag
}

TEST(Segmentation_Global, Lena_IsoData_Test) {
  // Test conversion of TIFF image into RAW file
  const std::string &input_file = "test/src/segmentation/input/lena_gray_512.tif";
  const std::string &output_file = "test/results/lena_gray_512.raw";
  segmentation_test_utils::CreateRawFromImage(input_file, output_file);

  // Read generated image into arma::Cube
  arma::Cube<segmentation_test_utils::ImageType> image;
  image.load(output_file, arma::raw_binary);

  // Calculate histogram from image
  arma::Mat<double>::fixed<256, 3> histogram = CalculateGreyscaleStatistics(image);

  // Remove temporary file
  std::remove(output_file.c_str());

  // Perform tests
  EXPECT_EQ(GlobalIsoDataSegmentation(histogram, image),           116);  // IsoData
}

TEST(Segmentation_Global, Lena_Otsu_Test) {
  // Test conversion of TIFF image into RAW file
  const std::string &input_file = "test/src/segmentation/input/lena_gray_512.tif";
  const std::string &output_file = "test/results/lena_gray_512.raw";
  segmentation_test_utils::CreateRawFromImage(input_file, output_file);

  // Read generated image into arma::Cube
  arma::Cube<segmentation_test_utils::ImageType> image;
  image.load(output_file, arma::raw_binary);

  // Calculate histogram from image
  arma::Mat<double>::fixed<256, 3> histogram = CalculateGreyscaleStatistics(image);

  // Remove temporary file
  std::remove(output_file.c_str());

  // Perform tests
  EXPECT_EQ(GlobalOtsuSegmentation(histogram, image),              117);  // Otsu
}

TEST(Segmentation_Global, Lena_Mean_Test) {
  // Test conversion of TIFF image into RAW file
  const std::string &input_file = "test/src/segmentation/input/lena_gray_512.tif";
  const std::string &output_file = "test/results/lena_gray_512.raw";
  segmentation_test_utils::CreateRawFromImage(input_file, output_file);

  // Read generated image into arma::Cube
  arma::Cube<segmentation_test_utils::ImageType> image;
  image.load(output_file, arma::raw_binary);

  // Calculate histogram from image
  arma::Mat<double>::fixed<256, 3> histogram = CalculateGreyscaleStatistics(image);

  // Remove temporary file
  std::remove(output_file.c_str());

  // Perform tests
  EXPECT_EQ(GlobalMeanSegmentation(histogram, image),              124);  // Mean
}

TEST(Segmentation_Global, Lena_Median_Test) {
  // Test conversion of TIFF image into RAW file
  const std::string &input_file = "test/src/segmentation/input/lena_gray_512.tif";
  const std::string &output_file = "test/results/lena_gray_512.raw";
  segmentation_test_utils::CreateRawFromImage(input_file, output_file);

  // Read generated image into arma::Cube
  arma::Cube<segmentation_test_utils::ImageType> image;
  image.load(output_file, arma::raw_binary);

  // Calculate histogram from image
  arma::Mat<double>::fixed<256, 3> histogram = CalculateGreyscaleStatistics(image);

  // Remove temporary file
  std::remove(output_file.c_str());

  // Perform tests
  EXPECT_EQ(GlobalMedianSegmentation(histogram, image),            128);  // Percentile
}

TEST(Segmentation_Global, Lena_Li_Test) {
  // Test conversion of TIFF image into RAW file
  const std::string &input_file = "test/src/segmentation/input/lena_gray_512.tif";
  const std::string &output_file = "test/results/lena_gray_512.raw";
  segmentation_test_utils::CreateRawFromImage(input_file, output_file);

  // Read generated image into arma::Cube
  arma::Cube<segmentation_test_utils::ImageType> image;
  image.load(output_file, arma::raw_binary);

  // Calculate histogram from image
  arma::Mat<double>::fixed<256, 3> histogram = CalculateGreyscaleStatistics(image);

  // Remove temporary file
  std::remove(output_file.c_str());

  // Perform tests
  EXPECT_EQ(GlobalLiSegmentation(histogram, image),                109);  // Li
}

TEST(Segmentation_Global, Lena_Minimum_Test) {
  // Test conversion of TIFF image into RAW file
  const std::string &input_file = "test/src/segmentation/input/lena_gray_512.tif";
  const std::string &output_file = "test/results/lena_gray_512.raw";
  segmentation_test_utils::CreateRawFromImage(input_file, output_file);

  // Read generated image into arma::Cube
  arma::Cube<segmentation_test_utils::ImageType> image;
  image.load(output_file, arma::raw_binary);

  // Calculate histogram from image
  arma::Mat<double>::fixed<256, 3> histogram = CalculateGreyscaleStatistics(image);

  // Remove temporary file
  std::remove(output_file.c_str());

  // Perform tests
  EXPECT_EQ(GlobalMinimumSegmentation(histogram, image),            74);  // Minimum
}

TEST(Segmentation_Global, Lena_Intermodes_Test) {
  // Test conversion of TIFF image into RAW file
  const std::string &input_file = "test/src/segmentation/input/lena_gray_512.tif";
  const std::string &output_file = "test/results/lena_gray_512.raw";
  segmentation_test_utils::CreateRawFromImage(input_file, output_file);

  // Read generated image into arma::Cube
  arma::Cube<segmentation_test_utils::ImageType> image;
  image.load(output_file, arma::raw_binary);

  // Calculate histogram from image
  arma::Mat<double>::fixed<256, 3> histogram = CalculateGreyscaleStatistics(image);

  // Remove temporary file
  std::remove(output_file.c_str());

  // Perform tests
  EXPECT_EQ(GlobalIntermodesSegmentation(histogram, image),         98);  // Intermodes
}

TEST(Segmentation_Global, Lena_Moments_Test) {
  // Test conversion of TIFF image into RAW file
  const std::string &input_file = "test/src/segmentation/input/lena_gray_512.tif";
  const std::string &output_file = "test/results/lena_gray_512.raw";
  segmentation_test_utils::CreateRawFromImage(input_file, output_file);

  // Read generated image into arma::Cube
  arma::Cube<segmentation_test_utils::ImageType> image;
  image.load(output_file, arma::raw_binary);

  // Calculate histogram from image
  arma::Mat<double>::fixed<256, 3> histogram = CalculateGreyscaleStatistics(image);

  // Remove temporary file
  std::remove(output_file.c_str());

  // Perform tests
  EXPECT_EQ(GlobalMomentsSegmentation(histogram, image),           126);  // Moments
}

TEST(Segmentation_Global, Lena_MaxShannonEntropy_Test) {
  // Test conversion of TIFF image into RAW file
  const std::string &input_file = "test/src/segmentation/input/lena_gray_512.tif";
  const std::string &output_file = "test/results/lena_gray_512.raw";
  segmentation_test_utils::CreateRawFromImage(input_file, output_file);

  // Read generated image into arma::Cube
  arma::Cube<segmentation_test_utils::ImageType> image;
  image.load(output_file, arma::raw_binary);

  // Calculate histogram from image
  arma::Mat<double>::fixed<256, 3> histogram = CalculateGreyscaleStatistics(image);

  // Remove temporary file
  std::remove(output_file.c_str());

  // Perform tests
  EXPECT_EQ(GlobalMaxShannonEntropySegmentation(histogram, image), 122);  // MaxEntropy
}

TEST(Segmentation_Global, Lena_Shanbhag_Test) {
  // Test conversion of TIFF image into RAW file
  const std::string &input_file = "test/src/segmentation/input/lena_gray_512.tif";
  const std::string &output_file = "test/results/lena_gray_512.raw";
  segmentation_test_utils::CreateRawFromImage(input_file, output_file);

  // Read generated image into arma::Cube
  arma::Cube<segmentation_test_utils::ImageType> image;
  image.load(output_file, arma::raw_binary);

  // Calculate histogram from image
  arma::Mat<double>::fixed<256, 3> histogram = CalculateGreyscaleStatistics(image);

  // Remove temporary file
  std::remove(output_file.c_str());

  // Perform tests
  EXPECT_EQ(GlobalShanbhagSegmentation(histogram, image),          120);  // Shanbhag
}

TEST(Segmentation_Global, LivingRoom_IsoData_Test) {
  // Test conversion of TIFF image into RAW file
  const std::string &input_file = "test/src/segmentation/input/livingroom.tif";
  const std::string &output_file = "test/results/livingroom.raw";
  segmentation_test_utils::CreateRawFromImage(input_file, output_file);

  // Read generated image into arma::Cube
  arma::Cube<segmentation_test_utils::ImageType> image;
  image.load(output_file, arma::raw_binary);

  // Calculate histogram from image
  arma::Mat<double>::fixed<256, 3> histogram = CalculateGreyscaleStatistics(image);

  // Remove temporary file
  std::remove(output_file.c_str());

  // Perform tests
  EXPECT_EQ(GlobalIsoDataSegmentation(histogram, image),           102);  // IsoData
}

TEST(Segmentation_Global, LivingRoom_Otsu_Test) {
  // Test conversion of TIFF image into RAW file
  const std::string &input_file = "test/src/segmentation/input/livingroom.tif";
  const std::string &output_file = "test/results/livingroom.raw";
  segmentation_test_utils::CreateRawFromImage(input_file, output_file);

  // Read generated image into arma::Cube
  arma::Cube<segmentation_test_utils::ImageType> image;
  image.load(output_file, arma::raw_binary);

  // Calculate histogram from image
  arma::Mat<double>::fixed<256, 3> histogram = CalculateGreyscaleStatistics(image);

  // Remove temporary file
  std::remove(output_file.c_str());

  // Perform tests
  EXPECT_EQ(GlobalOtsuSegmentation(histogram, image),              105);  // Otsu
}

TEST(Segmentation_Global, LivingRoom_Mean_Test) {
  // Test conversion of TIFF image into RAW file
  const std::string &input_file = "test/src/segmentation/input/livingroom.tif";
  const std::string &output_file = "test/results/livingroom.raw";
  segmentation_test_utils::CreateRawFromImage(input_file, output_file);

  // Read generated image into arma::Cube
  arma::Cube<segmentation_test_utils::ImageType> image;
  image.load(output_file, arma::raw_binary);

  // Calculate histogram from image
  arma::Mat<double>::fixed<256, 3> histogram = CalculateGreyscaleStatistics(image);

  // Remove temporary file
  std::remove(output_file.c_str());

  // Perform tests
  EXPECT_EQ(GlobalMeanSegmentation(histogram, image),              120);  // Mean
}

TEST(Segmentation_Global, LivingRoom_Median_Test) {
  // Test conversion of TIFF image into RAW file
  const std::string &input_file = "test/src/segmentation/input/livingroom.tif";
  const std::string &output_file = "test/results/livingroom.raw";
  segmentation_test_utils::CreateRawFromImage(input_file, output_file);

  // Read generated image into arma::Cube
  arma::Cube<segmentation_test_utils::ImageType> image;
  image.load(output_file, arma::raw_binary);

  // Calculate histogram from image
  arma::Mat<double>::fixed<256, 3> histogram = CalculateGreyscaleStatistics(image);

  // Remove temporary file
  std::remove(output_file.c_str());

  // Perform tests
  EXPECT_EQ(GlobalMedianSegmentation(histogram, image),            125);  // Percentile
}

TEST(Segmentation_Global, LivingRoom_Li_Test) {
  // Test conversion of TIFF image into RAW file
  const std::string &input_file = "test/src/segmentation/input/livingroom.tif";
  const std::string &output_file = "test/results/livingroom.raw";
  segmentation_test_utils::CreateRawFromImage(input_file, output_file);

  // Read generated image into arma::Cube
  arma::Cube<segmentation_test_utils::ImageType> image;
  image.load(output_file, arma::raw_binary);

  // Calculate histogram from image
  arma::Mat<double>::fixed<256, 3> histogram = CalculateGreyscaleStatistics(image);

  // Remove temporary file
  std::remove(output_file.c_str());

  // Perform tests
  EXPECT_EQ(GlobalLiSegmentation(histogram, image),                 92);  // Li
}

TEST(Segmentation_Global, LivingRoom_Minimum_Test) {
  // Test conversion of TIFF image into RAW file
  const std::string &input_file = "test/src/segmentation/input/livingroom.tif";
  const std::string &output_file = "test/results/livingroom.raw";
  segmentation_test_utils::CreateRawFromImage(input_file, output_file);

  // Read generated image into arma::Cube
  arma::Cube<segmentation_test_utils::ImageType> image;
  image.load(output_file, arma::raw_binary);

  // Calculate histogram from image
  arma::Mat<double>::fixed<256, 3> histogram = CalculateGreyscaleStatistics(image);

  // Remove temporary file
  std::remove(output_file.c_str());

  // Perform tests
  EXPECT_EQ(GlobalMinimumSegmentation(histogram, image),            70);  // Minimum
}

TEST(Segmentation_Global, LivingRoom_Intermodes_Test) {
  // Test conversion of TIFF image into RAW file
  const std::string &input_file = "test/src/segmentation/input/livingroom.tif";
  const std::string &output_file = "test/results/livingroom.raw";
  segmentation_test_utils::CreateRawFromImage(input_file, output_file);

  // Read generated image into arma::Cube
  arma::Cube<segmentation_test_utils::ImageType> image;
  image.load(output_file, arma::raw_binary);

  // Calculate histogram from image
  arma::Mat<double>::fixed<256, 3> histogram = CalculateGreyscaleStatistics(image);

  // Remove temporary file
  std::remove(output_file.c_str());

  // Perform tests
  EXPECT_EQ(GlobalIntermodesSegmentation(histogram, image),         98);  // Intermodes
}

TEST(Segmentation_Global, LivingRoom_Moments_Test) {
  // Test conversion of TIFF image into RAW file
  const std::string &input_file = "test/src/segmentation/input/livingroom.tif";
  const std::string &output_file = "test/results/livingroom.raw";
  segmentation_test_utils::CreateRawFromImage(input_file, output_file);

  // Read generated image into arma::Cube
  arma::Cube<segmentation_test_utils::ImageType> image;
  image.load(output_file, arma::raw_binary);

  // Calculate histogram from image
  arma::Mat<double>::fixed<256, 3> histogram = CalculateGreyscaleStatistics(image);

  // Remove temporary file
  std::remove(output_file.c_str());

  // Perform tests
  EXPECT_EQ(GlobalMomentsSegmentation(histogram, image),           116);  // Moments
}

TEST(Segmentation_Global, LivingRoom_MaxShannonEntropy_Test) {
  // Test conversion of TIFF image into RAW file
  const std::string &input_file = "test/src/segmentation/input/livingroom.tif";
  const std::string &output_file = "test/results/livingroom.raw";
  segmentation_test_utils::CreateRawFromImage(input_file, output_file);

  // Read generated image into arma::Cube
  arma::Cube<segmentation_test_utils::ImageType> image;
  image.load(output_file, arma::raw_binary);

  // Calculate histogram from image
  arma::Mat<double>::fixed<256, 3> histogram = CalculateGreyscaleStatistics(image);

  // Remove temporary file
  std::remove(output_file.c_str());

  // Perform tests
  EXPECT_EQ(GlobalMaxShannonEntropySegmentation(histogram, image),  93);  // MaxEntropy
}

TEST(Segmentation_Global, LivingRoom_Shanbhag_Test) {
  // Test conversion of TIFF image into RAW file
  const std::string &input_file = "test/src/segmentation/input/livingroom.tif";
  const std::string &output_file = "test/results/livingroom.raw";
  segmentation_test_utils::CreateRawFromImage(input_file, output_file);

  // Read generated image into arma::Cube
  arma::Cube<segmentation_test_utils::ImageType> image;
  image.load(output_file, arma::raw_binary);

  // Calculate histogram from image
  arma::Mat<double>::fixed<256, 3> histogram = CalculateGreyscaleStatistics(image);

  // Remove temporary file
  std::remove(output_file.c_str());

  // Perform tests
  EXPECT_EQ(GlobalShanbhagSegmentation(histogram, image),          109);  // Shanbhag
}

TEST(Segmentation_Global, Mandril_IsoData_Test) {
  // Test conversion of TIFF image into RAW file
  const std::string &input_file = "test/src/segmentation/input/mandril_gray.tif";
  const std::string &output_file = "test/results/mandril_gray.raw";
  segmentation_test_utils::CreateRawFromImage(input_file, output_file);

  // Read generated image into arma::Cube
  arma::Cube<segmentation_test_utils::ImageType> image;
  image.load(output_file, arma::raw_binary);

  // Calculate histogram from image
  arma::Mat<double>::fixed<256, 3> histogram = CalculateGreyscaleStatistics(image);

  // Remove temporary file
  std::remove(output_file.c_str());

  // Perform tests
  EXPECT_EQ(GlobalIsoDataSegmentation(histogram, image),           125);  // IsoData
}

TEST(Segmentation_Global, Mandril_Otsu_Test) {
  // Test conversion of TIFF image into RAW file
  const std::string &input_file = "test/src/segmentation/input/mandril_gray.tif";
  const std::string &output_file = "test/results/mandril_gray.raw";
  segmentation_test_utils::CreateRawFromImage(input_file, output_file);

  // Read generated image into arma::Cube
  arma::Cube<segmentation_test_utils::ImageType> image;
  image.load(output_file, arma::raw_binary);

  // Calculate histogram from image
  arma::Mat<double>::fixed<256, 3> histogram = CalculateGreyscaleStatistics(image);

  // Remove temporary file
  std::remove(output_file.c_str());

  // Perform tests
  EXPECT_EQ(GlobalOtsuSegmentation(histogram, image),              127);  // Otsu
}

TEST(Segmentation_Global, Mandril_Mean_Test) {
  // Test conversion of TIFF image into RAW file
  const std::string &input_file = "test/src/segmentation/input/mandril_gray.tif";
  const std::string &output_file = "test/results/mandril_gray.raw";
  segmentation_test_utils::CreateRawFromImage(input_file, output_file);

  // Read generated image into arma::Cube
  arma::Cube<segmentation_test_utils::ImageType> image;
  image.load(output_file, arma::raw_binary);

  // Calculate histogram from image
  arma::Mat<double>::fixed<256, 3> histogram = CalculateGreyscaleStatistics(image);

  // Remove temporary file
  std::remove(output_file.c_str());

  // Perform tests
  EXPECT_EQ(GlobalMeanSegmentation(histogram, image),              128);  // Mean
}

TEST(Segmentation_Global, Mandril_Median_Test) {
  // Test conversion of TIFF image into RAW file
  const std::string &input_file = "test/src/segmentation/input/mandril_gray.tif";
  const std::string &output_file = "test/results/mandril_gray.raw";
  segmentation_test_utils::CreateRawFromImage(input_file, output_file);

  // Read generated image into arma::Cube
  arma::Cube<segmentation_test_utils::ImageType> image;
  image.load(output_file, arma::raw_binary);

  // Calculate histogram from image
  arma::Mat<double>::fixed<256, 3> histogram = CalculateGreyscaleStatistics(image);

  // Remove temporary file
  std::remove(output_file.c_str());

  // Perform tests
  EXPECT_EQ(GlobalMedianSegmentation(histogram, image),            129);  // Percentile
}

TEST(Segmentation_Global, Mandril_Li_Test) {
  // Test conversion of TIFF image into RAW file
  const std::string &input_file = "test/src/segmentation/input/mandril_gray.tif";
  const std::string &output_file = "test/results/mandril_gray.raw";
  segmentation_test_utils::CreateRawFromImage(input_file, output_file);

  // Read generated image into arma::Cube
  arma::Cube<segmentation_test_utils::ImageType> image;
  image.load(output_file, arma::raw_binary);

  // Calculate histogram from image
  arma::Mat<double>::fixed<256, 3> histogram = CalculateGreyscaleStatistics(image);

  // Remove temporary file
  std::remove(output_file.c_str());

  // Perform tests
  EXPECT_EQ(GlobalLiSegmentation(histogram, image),                120);  // Li
}

TEST(Segmentation_Global, Mandril_Minimum_Test) {
  // Test conversion of TIFF image into RAW file
  const std::string &input_file = "test/src/segmentation/input/mandril_gray.tif";
  const std::string &output_file = "test/results/mandril_gray.raw";
  segmentation_test_utils::CreateRawFromImage(input_file, output_file);

  // Read generated image into arma::Cube
  arma::Cube<segmentation_test_utils::ImageType> image;
  image.load(output_file, arma::raw_binary);

  // Calculate histogram from image
  arma::Mat<double>::fixed<256, 3> histogram = CalculateGreyscaleStatistics(image);

  // Remove temporary file
  std::remove(output_file.c_str());

  // Perform tests
  EXPECT_EQ(GlobalMinimumSegmentation(histogram, image),           158);  // Minimum
}

TEST(Segmentation_Global, Mandril_Intermodes_Test) {
  // Test conversion of TIFF image into RAW file
  const std::string &input_file = "test/src/segmentation/input/mandril_gray.tif";
  const std::string &output_file = "test/results/mandril_gray.raw";
  segmentation_test_utils::CreateRawFromImage(input_file, output_file);

  // Read generated image into arma::Cube
  arma::Cube<segmentation_test_utils::ImageType> image;
  image.load(output_file, arma::raw_binary);

  // Calculate histogram from image
  arma::Mat<double>::fixed<256, 3> histogram = CalculateGreyscaleStatistics(image);

  // Remove temporary file
  std::remove(output_file.c_str());

  // Perform tests
  EXPECT_EQ(GlobalIntermodesSegmentation(histogram, image),        148);  // Intermodes
}

TEST(Segmentation_Global, Mandril_Moments_Test) {
  // Test conversion of TIFF image into RAW file
  const std::string &input_file = "test/src/segmentation/input/mandril_gray.tif";
  const std::string &output_file = "test/results/mandril_gray.raw";
  segmentation_test_utils::CreateRawFromImage(input_file, output_file);

  // Read generated image into arma::Cube
  arma::Cube<segmentation_test_utils::ImageType> image;
  image.load(output_file, arma::raw_binary);

  // Calculate histogram from image
  arma::Mat<double>::fixed<256, 3> histogram = CalculateGreyscaleStatistics(image);

  // Remove temporary file
  std::remove(output_file.c_str());

  // Perform tests
  EXPECT_EQ(GlobalMomentsSegmentation(histogram, image),           122);  // Moments
}

TEST(Segmentation_Global, Mandril_MaxShannonEntropy_Test) {
  // Test conversion of TIFF image into RAW file
  const std::string &input_file = "test/src/segmentation/input/mandril_gray.tif";
  const std::string &output_file = "test/results/mandril_gray.raw";
  segmentation_test_utils::CreateRawFromImage(input_file, output_file);

  // Read generated image into arma::Cube
  arma::Cube<segmentation_test_utils::ImageType> image;
  image.load(output_file, arma::raw_binary);

  // Calculate histogram from image
  arma::Mat<double>::fixed<256, 3> histogram = CalculateGreyscaleStatistics(image);

  // Remove temporary file
  std::remove(output_file.c_str());

  // Perform tests
  EXPECT_EQ(GlobalMaxShannonEntropySegmentation(histogram, image), 105);  // MaxEntropy
}

TEST(Segmentation_Global, Mandril_Shanbhag_Test) {
  // Test conversion of TIFF image into RAW file
  const std::string &input_file = "test/src/segmentation/input/mandril_gray.tif";
  const std::string &output_file = "test/results/mandril_gray.raw";
  segmentation_test_utils::CreateRawFromImage(input_file, output_file);

  // Read generated image into arma::Cube
  arma::Cube<segmentation_test_utils::ImageType> image;
  image.load(output_file, arma::raw_binary);

  // Calculate histogram from image
  arma::Mat<double>::fixed<256, 3> histogram = CalculateGreyscaleStatistics(image);

  // Remove temporary file
  std::remove(output_file.c_str());

  // Perform tests
  EXPECT_EQ(GlobalShanbhagSegmentation(histogram, image),          128);  // Shanbhag
}

TEST(Segmentation_Global, Pirate_IsoData_Test) {
  // Test conversion of TIFF image into RAW file
  const std::string &input_file = "test/src/segmentation/input/pirate.tif";
  const std::string &output_file = "test/results/pirate.raw";
  segmentation_test_utils::CreateRawFromImage(input_file, output_file);

  // Read generated image into arma::Cube
  arma::Cube<segmentation_test_utils::ImageType> image;
  image.load(output_file, arma::raw_binary);

  // Calculate histogram from image
  arma::Mat<double>::fixed<256, 3> histogram = CalculateGreyscaleStatistics(image);

  // Remove temporary file
  std::remove(output_file.c_str());

  // Perform tests
  EXPECT_EQ(GlobalIsoDataSegmentation(histogram, image),           107);  // IsoData
}

TEST(Segmentation_Global, Pirate_Otsu_Test) {
  // Test conversion of TIFF image into RAW file
  const std::string &input_file = "test/src/segmentation/input/pirate.tif";
  const std::string &output_file = "test/results/pirate.raw";
  segmentation_test_utils::CreateRawFromImage(input_file, output_file);

  // Read generated image into arma::Cube
  arma::Cube<segmentation_test_utils::ImageType> image;
  image.load(output_file, arma::raw_binary);

  // Calculate histogram from image
  arma::Mat<double>::fixed<256, 3> histogram = CalculateGreyscaleStatistics(image);

  // Remove temporary file
  std::remove(output_file.c_str());

  // Perform tests
  EXPECT_EQ(GlobalOtsuSegmentation(histogram, image),              107);  // Otsu
}

TEST(Segmentation_Global, Pirate_Mean_Test) {
  // Test conversion of TIFF image into RAW file
  const std::string &input_file = "test/src/segmentation/input/pirate.tif";
  const std::string &output_file = "test/results/pirate.raw";
  segmentation_test_utils::CreateRawFromImage(input_file, output_file);

  // Read generated image into arma::Cube
  arma::Cube<segmentation_test_utils::ImageType> image;
  image.load(output_file, arma::raw_binary);

  // Calculate histogram from image
  arma::Mat<double>::fixed<256, 3> histogram = CalculateGreyscaleStatistics(image);

  // Remove temporary file
  std::remove(output_file.c_str());

  // Perform tests
  EXPECT_EQ(GlobalMeanSegmentation(histogram, image),              111);  // Mean
}

TEST(Segmentation_Global, Pirate_Median_Test) {
  // Test conversion of TIFF image into RAW file
  const std::string &input_file = "test/src/segmentation/input/pirate.tif";
  const std::string &output_file = "test/results/pirate.raw";
  segmentation_test_utils::CreateRawFromImage(input_file, output_file);

  // Read generated image into arma::Cube
  arma::Cube<segmentation_test_utils::ImageType> image;
  image.load(output_file, arma::raw_binary);

  // Calculate histogram from image
  arma::Mat<double>::fixed<256, 3> histogram = CalculateGreyscaleStatistics(image);

  // Remove temporary file
  std::remove(output_file.c_str());

  // Perform tests
  EXPECT_EQ(GlobalMedianSegmentation(histogram, image),            117);  // Percentile
}

TEST(Segmentation_Global, Pirate_Li_Test) {
  // Test conversion of TIFF image into RAW file
  const std::string &input_file = "test/src/segmentation/input/pirate.tif";
  const std::string &output_file = "test/results/pirate.raw";
  segmentation_test_utils::CreateRawFromImage(input_file, output_file);

  // Read generated image into arma::Cube
  arma::Cube<segmentation_test_utils::ImageType> image;
  image.load(output_file, arma::raw_binary);

  // Calculate histogram from image
  arma::Mat<double>::fixed<256, 3> histogram = CalculateGreyscaleStatistics(image);

  // Remove temporary file
  std::remove(output_file.c_str());

  // Perform tests
  EXPECT_EQ(GlobalLiSegmentation(histogram, image),                 98);  // Li
}

TEST(Segmentation_Global, Pirate_Minimum_Test) {
  // Test conversion of TIFF image into RAW file
  const std::string &input_file = "test/src/segmentation/input/pirate.tif";
  const std::string &output_file = "test/results/pirate.raw";
  segmentation_test_utils::CreateRawFromImage(input_file, output_file);

  // Read generated image into arma::Cube
  arma::Cube<segmentation_test_utils::ImageType> image;
  image.load(output_file, arma::raw_binary);

  // Calculate histogram from image
  arma::Mat<double>::fixed<256, 3> histogram = CalculateGreyscaleStatistics(image);

  // Remove temporary file
  std::remove(output_file.c_str());

  // Perform tests
  EXPECT_EQ(GlobalMinimumSegmentation(histogram, image),            70);  // Minimum
}

TEST(Segmentation_Global, Pirate_Intermodes_Test) {
  // Test conversion of TIFF image into RAW file
  const std::string &input_file = "test/src/segmentation/input/pirate.tif";
  const std::string &output_file = "test/results/pirate.raw";
  segmentation_test_utils::CreateRawFromImage(input_file, output_file);

  // Read generated image into arma::Cube
  arma::Cube<segmentation_test_utils::ImageType> image;
  image.load(output_file, arma::raw_binary);

  // Calculate histogram from image
  arma::Mat<double>::fixed<256, 3> histogram = CalculateGreyscaleStatistics(image);

  // Remove temporary file
  std::remove(output_file.c_str());

  // Perform tests
  EXPECT_EQ(GlobalIntermodesSegmentation(histogram, image),         93);  // Intermodes
}

TEST(Segmentation_Global, Pirate_Moments_Test) {
  // Test conversion of TIFF image into RAW file
  const std::string &input_file = "test/src/segmentation/input/pirate.tif";
  const std::string &output_file = "test/results/pirate.raw";
  segmentation_test_utils::CreateRawFromImage(input_file, output_file);

  // Read generated image into arma::Cube
  arma::Cube<segmentation_test_utils::ImageType> image;
  image.load(output_file, arma::raw_binary);

  // Calculate histogram from image
  arma::Mat<double>::fixed<256, 3> histogram = CalculateGreyscaleStatistics(image);

  // Remove temporary file
  std::remove(output_file.c_str());

  // Perform tests
  EXPECT_EQ(GlobalMomentsSegmentation(histogram, image),           116);  // Moments
}

TEST(Segmentation_Global, Pirate_MaxShannonEntropy_Test) {
  // Test conversion of TIFF image into RAW file
  const std::string &input_file = "test/src/segmentation/input/pirate.tif";
  const std::string &output_file = "test/results/pirate.raw";
  segmentation_test_utils::CreateRawFromImage(input_file, output_file);

  // Read generated image into arma::Cube
  arma::Cube<segmentation_test_utils::ImageType> image;
  image.load(output_file, arma::raw_binary);

  // Calculate histogram from image
  arma::Mat<double>::fixed<256, 3> histogram = CalculateGreyscaleStatistics(image);

  // Remove temporary file
  std::remove(output_file.c_str());

  // Perform tests
  EXPECT_EQ(GlobalMaxShannonEntropySegmentation(histogram, image), 124);  // MaxEntropy
}

TEST(Segmentation_Global, Pirate_Shanbhag_Test) {
  // Test conversion of TIFF image into RAW file
  const std::string &input_file = "test/src/segmentation/input/pirate.tif";
  const std::string &output_file = "test/results/pirate.raw";
  segmentation_test_utils::CreateRawFromImage(input_file, output_file);

  // Read generated image into arma::Cube
  arma::Cube<segmentation_test_utils::ImageType> image;
  image.load(output_file, arma::raw_binary);

  // Calculate histogram from image
  arma::Mat<double>::fixed<256, 3> histogram = CalculateGreyscaleStatistics(image);

  // Remove temporary file
  std::remove(output_file.c_str());

  // Perform tests
  EXPECT_EQ(GlobalShanbhagSegmentation(histogram, image),          120);  // Shanbhag
}

TEST(Segmentation_Global, WomanBlonde_IsoData_Test) {
  // Test conversion of TIFF image into RAW file
  const std::string &input_file = "test/src/segmentation/input/woman_blonde.tif";
  const std::string &output_file = "test/results/woman_blonde.raw";
  segmentation_test_utils::CreateRawFromImage(input_file, output_file);

  // Read generated image into arma::Cube
  arma::Cube<segmentation_test_utils::ImageType> image;
  image.load(output_file, arma::raw_binary);

  // Calculate histogram from image
  arma::Mat<double>::fixed<256, 3> histogram = CalculateGreyscaleStatistics(image);

  // Remove temporary file
  std::remove(output_file.c_str());

  // Perform tests
  EXPECT_EQ(GlobalIsoDataSegmentation(histogram, image),           122);  // IsoData
}

TEST(Segmentation_Global, WomanBlonde_Otsu_Test) {
  // Test conversion of TIFF image into RAW file
  const std::string &input_file = "test/src/segmentation/input/woman_blonde.tif";
  const std::string &output_file = "test/results/woman_blonde.raw";
  segmentation_test_utils::CreateRawFromImage(input_file, output_file);

  // Read generated image into arma::Cube
  arma::Cube<segmentation_test_utils::ImageType> image;
  image.load(output_file, arma::raw_binary);

  // Calculate histogram from image
  arma::Mat<double>::fixed<256, 3> histogram = CalculateGreyscaleStatistics(image);

  // Remove temporary file
  std::remove(output_file.c_str());

  // Perform tests
  EXPECT_EQ(GlobalOtsuSegmentation(histogram, image),              122);  // Otsu
}

TEST(Segmentation_Global, WomanBlonde_Mean_Test) {
  // Test conversion of TIFF image into RAW file
  const std::string &input_file = "test/src/segmentation/input/woman_blonde.tif";
  const std::string &output_file = "test/results/woman_blonde.raw";
  segmentation_test_utils::CreateRawFromImage(input_file, output_file);

  // Read generated image into arma::Cube
  arma::Cube<segmentation_test_utils::ImageType> image;
  image.load(output_file, arma::raw_binary);

  // Calculate histogram from image
  arma::Mat<double>::fixed<256, 3> histogram = CalculateGreyscaleStatistics(image);

  // Remove temporary file
  std::remove(output_file.c_str());

  // Perform tests
  EXPECT_EQ(GlobalMeanSegmentation(histogram, image),              135);  // Mean
}

TEST(Segmentation_Global, WomanBlonde_Median_Test) {
  // Test conversion of TIFF image into RAW file
  const std::string &input_file = "test/src/segmentation/input/woman_blonde.tif";
  const std::string &output_file = "test/results/woman_blonde.raw";
  segmentation_test_utils::CreateRawFromImage(input_file, output_file);

  // Read generated image into arma::Cube
  arma::Cube<segmentation_test_utils::ImageType> image;
  image.load(output_file, arma::raw_binary);

  // Calculate histogram from image
  arma::Mat<double>::fixed<256, 3> histogram = CalculateGreyscaleStatistics(image);

  // Remove temporary file
  std::remove(output_file.c_str());

  // Perform tests
  EXPECT_EQ(GlobalMedianSegmentation(histogram, image),            144);  // Percentile
}

TEST(Segmentation_Global, WomanBlonde_Li_Test) {
  // Test conversion of TIFF image into RAW file
  const std::string &input_file = "test/src/segmentation/input/woman_blonde.tif";
  const std::string &output_file = "test/results/woman_blonde.raw";
  segmentation_test_utils::CreateRawFromImage(input_file, output_file);

  // Read generated image into arma::Cube
  arma::Cube<segmentation_test_utils::ImageType> image;
  image.load(output_file, arma::raw_binary);

  // Calculate histogram from image
  arma::Mat<double>::fixed<256, 3> histogram = CalculateGreyscaleStatistics(image);

  // Remove temporary file
  std::remove(output_file.c_str());

  // Perform tests
  EXPECT_EQ(GlobalLiSegmentation(histogram, image),                116);  // Li
}

TEST(Segmentation_Global, WomanBlonde_Minimum_Test) {
  // Test conversion of TIFF image into RAW file
  const std::string &input_file = "test/src/segmentation/input/woman_blonde.tif";
  const std::string &output_file = "test/results/woman_blonde.raw";
  segmentation_test_utils::CreateRawFromImage(input_file, output_file);

  // Read generated image into arma::Cube
  arma::Cube<segmentation_test_utils::ImageType> image;
  image.load(output_file, arma::raw_binary);

  // Calculate histogram from image
  arma::Mat<double>::fixed<256, 3> histogram = CalculateGreyscaleStatistics(image);

  // Remove temporary file
  std::remove(output_file.c_str());

  // Perform tests
  EXPECT_EQ(GlobalMinimumSegmentation(histogram, image),           105);  // Minimum
}

TEST(Segmentation_Global, WomanBlonde_Intermodes_Test) {
  // Test conversion of TIFF image into RAW file
  const std::string &input_file = "test/src/segmentation/input/woman_blonde.tif";
  const std::string &output_file = "test/results/woman_blonde.raw";
  segmentation_test_utils::CreateRawFromImage(input_file, output_file);

  // Read generated image into arma::Cube
  arma::Cube<segmentation_test_utils::ImageType> image;
  image.load(output_file, arma::raw_binary);

  // Calculate histogram from image
  arma::Mat<double>::fixed<256, 3> histogram = CalculateGreyscaleStatistics(image);

  // Remove temporary file
  std::remove(output_file.c_str());

  // Perform tests
  EXPECT_EQ(GlobalIntermodesSegmentation(histogram, image),        120);  // Intermodes
}

TEST(Segmentation_Global, WomanBlonde_Moments_Test) {
  // Test conversion of TIFF image into RAW file
  const std::string &input_file = "test/src/segmentation/input/woman_blonde.tif";
  const std::string &output_file = "test/results/woman_blonde.raw";
  segmentation_test_utils::CreateRawFromImage(input_file, output_file);

  // Read generated image into arma::Cube
  arma::Cube<segmentation_test_utils::ImageType> image;
  image.load(output_file, arma::raw_binary);

  // Calculate histogram from image
  arma::Mat<double>::fixed<256, 3> histogram = CalculateGreyscaleStatistics(image);

  // Remove temporary file
  std::remove(output_file.c_str());

  // Perform tests
  EXPECT_EQ(GlobalMomentsSegmentation(histogram, image),           122);  // Moments
}

TEST(Segmentation_Global, WomanBlonde_MaxShannonEntropy_Test) {
  // Test conversion of TIFF image into RAW file
  const std::string &input_file = "test/src/segmentation/input/woman_blonde.tif";
  const std::string &output_file = "test/results/woman_blonde.raw";
  segmentation_test_utils::CreateRawFromImage(input_file, output_file);

  // Read generated image into arma::Cube
  arma::Cube<segmentation_test_utils::ImageType> image;
  image.load(output_file, arma::raw_binary);

  // Calculate histogram from image
  arma::Mat<double>::fixed<256, 3> histogram = CalculateGreyscaleStatistics(image);

  // Remove temporary file
  std::remove(output_file.c_str());

  // Perform tests
  EXPECT_EQ(GlobalMaxShannonEntropySegmentation(histogram, image), 124);  // MaxEntropy
}

TEST(Segmentation_Global, WomanBlonde_Shanbhag_Test) {
  // Test conversion of TIFF image into RAW file
  const std::string &input_file = "test/src/segmentation/input/woman_blonde.tif";
  const std::string &output_file = "test/results/woman_blonde.raw";
  segmentation_test_utils::CreateRawFromImage(input_file, output_file);

  // Read generated image into arma::Cube
  arma::Cube<segmentation_test_utils::ImageType> image;
  image.load(output_file, arma::raw_binary);

  // Calculate histogram from image
  arma::Mat<double>::fixed<256, 3> histogram = CalculateGreyscaleStatistics(image);

  // Remove temporary file
  std::remove(output_file.c_str());

  // Perform tests
  EXPECT_EQ(GlobalShanbhagSegmentation(histogram, image),          128);  // Shanbhag
}

TEST(Segmentation_Global, WomanDarkHair_IsoData_Test) {
  // Test conversion of TIFF image into RAW file
  const std::string &input_file = "test/src/segmentation/input/woman_darkhair.tif";
  const std::string &output_file = "test/results/woman_darkhair.raw";
  segmentation_test_utils::CreateRawFromImage(input_file, output_file);

  // Read generated image into arma::Cube
  arma::Cube<segmentation_test_utils::ImageType> image;
  image.load(output_file, arma::raw_binary);

  // Calculate histogram from image
  arma::Mat<double>::fixed<256, 3> histogram = CalculateGreyscaleStatistics(image);

  // Remove temporary file
  std::remove(output_file.c_str());

  // Perform tests
  EXPECT_EQ(GlobalIsoDataSegmentation(histogram, image),           121);  // IsoData
}

TEST(Segmentation_Global, WomanDarkHair_Otsu_Test) {
  // Test conversion of TIFF image into RAW file
  const std::string &input_file = "test/src/segmentation/input/woman_darkhair.tif";
  const std::string &output_file = "test/results/woman_darkhair.raw";
  segmentation_test_utils::CreateRawFromImage(input_file, output_file);

  // Read generated image into arma::Cube
  arma::Cube<segmentation_test_utils::ImageType> image;
  image.load(output_file, arma::raw_binary);

  // Calculate histogram from image
  arma::Mat<double>::fixed<256, 3> histogram = CalculateGreyscaleStatistics(image);

  // Remove temporary file
  std::remove(output_file.c_str());

  // Perform tests
  EXPECT_EQ(GlobalOtsuSegmentation(histogram, image),              121);  // Otsu
}

TEST(Segmentation_Global, WomanDarkHair_Mean_Test) {
  // Test conversion of TIFF image into RAW file
  const std::string &input_file = "test/src/segmentation/input/woman_darkhair.tif";
  const std::string &output_file = "test/results/woman_darkhair.raw";
  segmentation_test_utils::CreateRawFromImage(input_file, output_file);

  // Read generated image into arma::Cube
  arma::Cube<segmentation_test_utils::ImageType> image;
  image.load(output_file, arma::raw_binary);

  // Calculate histogram from image
  arma::Mat<double>::fixed<256, 3> histogram = CalculateGreyscaleStatistics(image);

  // Remove temporary file
  std::remove(output_file.c_str());

  // Perform tests
  EXPECT_EQ(GlobalMeanSegmentation(histogram, image),              107);  // Mean
}

TEST(Segmentation_Global, WomanDarkHair_Median_Test) {
  // Test conversion of TIFF image into RAW file
  const std::string &input_file = "test/src/segmentation/input/woman_darkhair.tif";
  const std::string &output_file = "test/results/woman_darkhair.raw";
  segmentation_test_utils::CreateRawFromImage(input_file, output_file);

  // Read generated image into arma::Cube
  arma::Cube<segmentation_test_utils::ImageType> image;
  image.load(output_file, arma::raw_binary);

  // Calculate histogram from image
  arma::Mat<double>::fixed<256, 3> histogram = CalculateGreyscaleStatistics(image);

  // Remove temporary file
  std::remove(output_file.c_str());

  // Perform tests
  EXPECT_EQ(GlobalMedianSegmentation(histogram, image),             88);  // Percentile
}

TEST(Segmentation_Global, WomanDarkHair_Li_Test) {
  // Test conversion of TIFF image into RAW file
  const std::string &input_file = "test/src/segmentation/input/woman_darkhair.tif";
  const std::string &output_file = "test/results/woman_darkhair.raw";
  segmentation_test_utils::CreateRawFromImage(input_file, output_file);

  // Read generated image into arma::Cube
  arma::Cube<segmentation_test_utils::ImageType> image;
  image.load(output_file, arma::raw_binary);

  // Calculate histogram from image
  arma::Mat<double>::fixed<256, 3> histogram = CalculateGreyscaleStatistics(image);

  // Remove temporary file
  std::remove(output_file.c_str());

  // Perform tests
  EXPECT_EQ(GlobalLiSegmentation(histogram, image),                108);  // Li
}

TEST(Segmentation_Global, WomanDarkHair_Minimum_Test) {
  // Test conversion of TIFF image into RAW file
  const std::string &input_file = "test/src/segmentation/input/woman_darkhair.tif";
  const std::string &output_file = "test/results/woman_darkhair.raw";
  segmentation_test_utils::CreateRawFromImage(input_file, output_file);

  // Read generated image into arma::Cube
  arma::Cube<segmentation_test_utils::ImageType> image;
  image.load(output_file, arma::raw_binary);

  // Calculate histogram from image
  arma::Mat<double>::fixed<256, 3> histogram = CalculateGreyscaleStatistics(image);

  // Remove temporary file
  std::remove(output_file.c_str());

  // Perform tests
  EXPECT_EQ(GlobalMinimumSegmentation(histogram, image),           117);  // Minimum
}

TEST(Segmentation_Global, WomanDarkHair_Intermodes_Test) {
  // Test conversion of TIFF image into RAW file
  const std::string &input_file = "test/src/segmentation/input/woman_darkhair.tif";
  const std::string &output_file = "test/results/woman_darkhair.raw";
  segmentation_test_utils::CreateRawFromImage(input_file, output_file);

  // Read generated image into arma::Cube
  arma::Cube<segmentation_test_utils::ImageType> image;
  image.load(output_file, arma::raw_binary);

  // Calculate histogram from image
  arma::Mat<double>::fixed<256, 3> histogram = CalculateGreyscaleStatistics(image);

  // Remove temporary file
  std::remove(output_file.c_str());

  // Perform tests
  EXPECT_EQ(GlobalIntermodesSegmentation(histogram, image),         97);  // Intermodes
}

TEST(Segmentation_Global, WomanDarkHair_Moments_Test) {
  // Test conversion of TIFF image into RAW file
  const std::string &input_file = "test/src/segmentation/input/woman_darkhair.tif";
  const std::string &output_file = "test/results/woman_darkhair.raw";
  segmentation_test_utils::CreateRawFromImage(input_file, output_file);

  // Read generated image into arma::Cube
  arma::Cube<segmentation_test_utils::ImageType> image;
  image.load(output_file, arma::raw_binary);

  // Calculate histogram from image
  arma::Mat<double>::fixed<256, 3> histogram = CalculateGreyscaleStatistics(image);

  // Remove temporary file
  std::remove(output_file.c_str());

  // Perform tests
  EXPECT_EQ(GlobalMomentsSegmentation(histogram, image),           132);  // Moments
}

TEST(Segmentation_Global, WomanDarkHair_MaxShannonEntropy_Test) {
  // Test conversion of TIFF image into RAW file
  const std::string &input_file = "test/src/segmentation/input/woman_darkhair.tif";
  const std::string &output_file = "test/results/woman_darkhair.raw";
  segmentation_test_utils::CreateRawFromImage(input_file, output_file);

  // Read generated image into arma::Cube
  arma::Cube<segmentation_test_utils::ImageType> image;
  image.load(output_file, arma::raw_binary);

  // Calculate histogram from image
  arma::Mat<double>::fixed<256, 3> histogram = CalculateGreyscaleStatistics(image);

  // Remove temporary file
  std::remove(output_file.c_str());

  // Perform tests
  EXPECT_EQ(GlobalMaxShannonEntropySegmentation(histogram, image), 151);  // MaxEntropy
}

TEST(Segmentation_Global, WomanDarkHair_Shanbhag_Test) {
  // Test conversion of TIFF image into RAW file
  const std::string &input_file = "test/src/segmentation/input/woman_darkhair.tif";
  const std::string &output_file = "test/results/woman_darkhair.raw";
  segmentation_test_utils::CreateRawFromImage(input_file, output_file);

  // Read generated image into arma::Cube
  arma::Cube<segmentation_test_utils::ImageType> image;
  image.load(output_file, arma::raw_binary);

  // Calculate histogram from image
  arma::Mat<double>::fixed<256, 3> histogram = CalculateGreyscaleStatistics(image);

  // Remove temporary file
  std::remove(output_file.c_str());

  // Perform tests
  EXPECT_EQ(GlobalShanbhagSegmentation(histogram, image),          168);  // Shanbhag
}

#endif  // TEST_SRC_SEGMENTATION_GLOBAL_SEGMENTATION_TESTS_H_
