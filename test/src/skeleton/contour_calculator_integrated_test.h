/**
 * \file test/src/skeleton/contour_calculator_integrated_test.h
 * \brief Tests countour_calculator
 *
 * \author Alexandre Ashade Lassance Cunha \<aashade@br.ibm.com\>
 * \author Giulia Duncan Coutinho \<coutingi@br.ibm.com\>
 * \authors Hugo de Oliveira Barbalho \<hugob@br.ibm.com\>
 * \copyright © IBM Corp.
 * \date 2015
 */

#ifndef TEST_SRC_SKELETON_CONTOUR_CALCULATOR_INTEGRATED_TEST_H_
#define TEST_SRC_SKELETON_CONTOUR_CALCULATOR_INTEGRATED_TEST_H_

#include <gtest/gtest.h>

#include <vector>

#include "src/skeleton/contours/contour_calculator.h"
#include "src/skeleton/images/image.h"
#include "src/skeleton/points/pixel.h"
#include "src/skeleton/points/point_builder.h"
#include "src/skeleton/images/annotated_image.h"
#include "src/skeleton/images/binary_image.h"
#include "src/skeleton/neighbours/six_neighbour_calculator.h"
#include "test/src/skeleton/commons.h"
#include "test/src/utils/test_utils.h"
#include "test/src/utils/geometry_utils.h"

namespace skeleton_test {

using Pixel = skeleton::Pixel;
using _AnnotatedImage2D = skeleton::AnnotatedImage<Pixel>;
using _Image2D = skeleton::Image2D;
using _vec_pixel = std::vector<Pixel>;
using _four_calculator = skeleton::FourNeighbourCalculator<Pixel,
                                                           skeleton::PointBuilder>;

using dcci_voxel = skeleton::Point<3>;
using dcci_vec_voxel = std::vector<dcci_voxel>;
using dcci_annotated_image3d = skeleton::AnnotatedImage<dcci_voxel>;
using dcci_image3d = skeleton::BinaryImage<dcci_voxel>;
using dcci_six_calculator = skeleton::SixNeighbourCalculator<dcci_voxel,
                                                             skeleton::PointBuilder>;

TEST(Contour_Calculator_Integrated,
     ComputeContours_For_3x3x3_Cube_Should_Return_No_Voxel) {
  // arrange
  std::vector<bool> points;
  const dcci_image3d::size width = 3, height = 3, depth = 3;
  points.resize(width * height * depth, dcci_image3d::Foreground);
  dcci_image3d::array sizes = {width, height, depth};
  auto image = dcci_image3d {points, sizes};
  dcci_annotated_image3d annotations;
  skeleton::ContourCalculator<dcci_image3d,
                              dcci_annotated_image3d,
                              dcci_six_calculator> calculator{image, annotations};

  // act
  calculator.ComputeContours();

  // assert.
  dcci_annotated_image3d::annotation_type::label_value pixel_label = 1,
      contour_label = 1;
  for (const auto &point : calculator) {
    auto annotation = annotations.ReadAnnotation(point);
    EXPECT_EQ(contour_label, annotation.contour_label);
    EXPECT_EQ(pixel_label, annotation.pixel_label);
    ++pixel_label;
  }
  EXPECT_EQ(1, pixel_label);  // no contours
}

}  // namespace skeleton_test

#endif  // TEST_SRC_SKELETON_CONTOUR_CALCULATOR_INTEGRATED_TEST_H_
