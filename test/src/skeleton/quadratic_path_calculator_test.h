/**
 * \file test/src/skeleton/quadratic_path_calculator_test.h
 *
 * \author Alexandre Ashade Lassance Cunha \<aashade@br.ibm.com\>
 * \copyright © IBM Corp.
 * \date 2015
 */

#ifndef TEST_SRC_SKELETON_QUADRATIC_PATH_CALCULATOR_TEST_H_
#define TEST_SRC_SKELETON_QUADRATIC_PATH_CALCULATOR_TEST_H_

#include <gtest/gtest.h>
#include <cstdint>
#include <exception>
#include "src/skeleton/paths/quadratic_path_calculator.h"
#include "test/src/utils/test_utils.h"

namespace skeleton_test {

class Annotation {
 public:
  using distance_value = int;
  using displacement_value = distance_value;
  using displacements_index_type = int;

 private:
  distance_value x_, y_, distance_;

 public:
  Annotation() {
  }
  Annotation(distance_value x, distance_value y)
      : x_(x),
        y_(y) {
  }
  distance_value GetAccumulatedDistance(int index) const {
    if (index == 0)
      return x();
    else if (index == 1)
      return y();
    throw std::exception {};
  }
  void SetAccumulatedDistance(int index, distance_value value) {
    if (index == 0)
      x_ = value;
    else if (index == 1)
      y_ = value;
  }
  distance_value x() const {
    return x_;
  }
  distance_value y() const {
    return y_;
  }
  distance_value distance() const {
    return distance_;
  }
  void SetDistanceValue(distance_value value) {
    distance_ = value;
  }
};

TEST(QuadraticPathCalculator,
     UpdatePointPathCost_Should_Fill_Coordinates_Differences) {
// arrange
  geometry_utils::Pixel first{1, -2};
  geometry_utils::Pixel second{-1, 9};
  skeleton::QuadraticPathCalculator<Annotation, geometry_utils::Pixel>
      calculator;
  Annotation first_annotation{0, 0};
  Annotation second_annotation{0, 0};

  Annotation::distance_value expected_x = 2;
  Annotation::distance_value expected_y = 11;
  Annotation::distance_value expected_distance = 125;

// act
  calculator.UpdatePointPathCost(first, first_annotation, second,
                                 second_annotation);

// assert
  EXPECT_EQ(expected_x, second_annotation.x());
  EXPECT_EQ(expected_y, second_annotation.y());
  EXPECT_EQ(expected_distance, second_annotation.distance());
}

TEST(QuadraticPathCalculator,
     GetConcatenatedPathCost_Should_Fill_Coordinates_Differences) {
// arrange
  geometry_utils::Pixel first{1, -2};
  geometry_utils::Pixel second{-1, 9};
  skeleton::QuadraticPathCalculator<Annotation, geometry_utils::Pixel>
      calculator;
  Annotation first_annotation{0, 0};

  Annotation::distance_value expected = 125;

// act
  auto result = calculator.GetConcatenatedPathCost(first, first_annotation,
                                                   second);

// assert
  EXPECT_EQ(expected, result);
}

}  // namespace skeleton_test

#endif  // TEST_SRC_SKELETON_QUADRATIC_PATH_CALCULATOR_TEST_H_
