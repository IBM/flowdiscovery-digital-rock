/**
 * \file test/src/utils/segmentation_test_utils.h
 * \brief Contains helper functions for the segmentation tests.
 *
 * \authors Rodrigo Neumann Barros Ferreira \<rneumann@br.ibm.com\>
 * \copyright © IBM Corp.
 * \date 2017
 */

#ifndef TEST_SRC_UTILS_SEGMENTATION_TEST_UTILS_H_
#define TEST_SRC_UTILS_SEGMENTATION_TEST_UTILS_H_

#include <armadillo>
#include <cstdint>
#include <string>

namespace segmentation_test_utils {

using ImageType = uint8_t;
constexpr uint16_t image_size = 512;

bool CreateRawFromImage(std::string input_file, std::string output_file) {
/**
 * This function transforms a TIFF benchmark image into a RAW file suitable for processing.
 *
 * \param[in] input_file    Greyscale TIFF image to be converted
 * \param[in] output_file   Destination file of the 3D greyscale geometry
 * \retval    success       Boolean representing the success of the \c .save() operation
 */
  // Load 8-bit greyscale TIFF image from disk into arma::Mat
  arma::Mat<ImageType> image;
  image.load(input_file, arma::raw_binary);
  image.reshape(image_size, image_size);

  // Convert arma::Mat into arma::Cube
  arma::Cube<ImageType> cube(image_size, image_size, 1);
  cube.slice(0) = image;

  // Save arma::Cube to disk
  bool success = cube.save(output_file, arma::raw_binary);
  return success;
}



bool CreateRawFromLinearGradient(std::string output_file) {
/**
 * This function RAW file suitable for processing from a linear gradient.
 *
 * \param[in] output_file   Destination file of the 3D greyscale geometry
 * \retval    success       Boolean representing the success of the \c .save() operation
 */
  // Create empty cube
  arma::Cube<ImageType> cube(256, 256, 256, arma::fill::zeros);

  // Impose linear greyscale gradient
  uint64_t n = 0U;
  for (auto i = cube.begin(); i != cube.end(); ++i, n++) {
    *i += static_cast<ImageType>(n / 256);
  }

  // Save arma::Cube to disk
  bool success = cube.save(output_file, arma::raw_binary);
  return success;
}

}  // namespace segmentation_test_utils

#endif  // TEST_SRC_UTILS_SEGMENTATION_TEST_UTILS_H_
