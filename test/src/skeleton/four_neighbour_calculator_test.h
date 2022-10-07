/**
 * \file test/src/skeleton/four_neighbour_calculator_test.h
 *
 * \author Alexandre Ashade Lassance Cunha \<aashade@br.ibm.com\>
 * \copyright © IBM Corp.
 * \date 2015
 */

#ifndef TEST_SRC_SKELETON_FOUR_NEIGHBOUR_CALCULATOR_TEST_H_
#define TEST_SRC_SKELETON_FOUR_NEIGHBOUR_CALCULATOR_TEST_H_

#include <gtest/gtest.h>
#include <algorithm>
#include <vector>
#include "src/skeleton/points/point.h"
#include "src/skeleton/points/point_builder.h"
#include "src/skeleton/neighbours/four_neighbour_calculator.h"
#include "test/src/utils/test_utils.h"

namespace skeleton_test {

using neighbour_calculator = skeleton::FourNeighbourCalculator<
skeleton::Point<2>, skeleton::PointBuilder>;
TEST(Four_Neighbour_Calculator, Neighbours_Are_OfType_Point2) {
  // arrange
  const skeleton::Point<2>::coordinate x = 1, y = 3;
  auto base_point = skeleton::Point<2> { { x, y } };
  neighbour_calculator calculator;

  // act
  auto neighbours = calculator.GetNeighbours(base_point);

  // assert
  for (const auto &neighbour : neighbours) {
    test_utils::is_same_type<skeleton::Point<2>>::Assert(neighbour);
  }
}

TEST(Four_Neighbour_Calculator, Neighbours_Are_Correct) {
  // arrange
  const skeleton::Point<2>::coordinate x = 1, y = 3;
  skeleton::Point<2> base_point { { x, y } };
  std::vector<skeleton::Point<2>> expected_neighbours;
  expected_neighbours.push_back(skeleton::Point<2> { { 0, 3 } });
  expected_neighbours.push_back(skeleton::Point<2> { { 1, 4 } });
  expected_neighbours.push_back(skeleton::Point<2> { { 2, 3 } });
  expected_neighbours.push_back(skeleton::Point<2> { { 1, 2 } });
  neighbour_calculator calculator;

  // act
  auto neighbours = calculator.GetNeighbours(base_point);

  // assert
  EXPECT_EQ(expected_neighbours.size(), neighbours.size());
  auto are_equal = std::is_permutation(expected_neighbours.cbegin(),
                                       expected_neighbours.cend(),
                                       neighbours.cbegin(),
                                       test_utils::PointsEqual);
  EXPECT_TRUE(are_equal);
}

}  // namespace skeleton_test

#endif  // TEST_SRC_SKELETON_FOUR_NEIGHBOUR_CALCULATOR_TEST_H_
