/**
 * \file test/src/skeleton/six_neighbour_calculator_test.h
 *
 * \author Alexandre Ashade Lassance Cunha \<aashade@br.ibm.com\>
 * \copyright © IBM Corp.
 * \date 2015
 */

#ifndef TEST_SRC_SKELETON_SIX_NEIGHBOUR_CALCULATOR_TEST_H_
#define TEST_SRC_SKELETON_SIX_NEIGHBOUR_CALCULATOR_TEST_H_

#include <gtest/gtest.h>
#include <algorithm>
#include <vector>
#include "src/skeleton/points/point.h"
#include "src/skeleton/points/point_builder.h"
#include "src/skeleton/neighbours/six_neighbour_calculator.h"
#include "test/src/utils/test_utils.h"

namespace skeleton_test {

using snct_neighbour_calculator = skeleton::SixNeighbourCalculator<
skeleton::Point<3>, skeleton::PointBuilder>;

TEST(Six_Neighbour_Calculator, Neighbours_Are_OfType_Point3) {
  // arrange
  const skeleton::Point<3>::coordinate x = 1, y = 3, z = 0;
  auto base_point = skeleton::Point<3> { { x, y, z } };
  snct_neighbour_calculator calculator;

  // act
  auto neighbours = calculator.GetNeighbours(base_point);

  // assert
  for (const auto &neighbour : neighbours)
    test_utils::is_same_type<skeleton::Point<3>>::Assert(neighbour);
}

TEST(Six_Neighbour_Calculator, Neighbours_Are_Correct_For_Point_135) {
  // arrange
  const skeleton::Point<3>::coordinate x = 1, y = 3, z = 5;
  skeleton::Point<3> base_point { { x, y, z } };
  std::vector<skeleton::Point<3>> expected_neighbours;
  expected_neighbours.push_back(skeleton::Point<3> { { 0, 3, 5 } });
  expected_neighbours.push_back(skeleton::Point<3> { { 2, 3, 5 } });
  expected_neighbours.push_back(skeleton::Point<3> { { 1, 2, 5 } });
  expected_neighbours.push_back(skeleton::Point<3> { { 1, 4, 5 } });
  expected_neighbours.push_back(skeleton::Point<3> { { 1, 3, 4 } });
  expected_neighbours.push_back(skeleton::Point<3> { { 1, 3, 6 } });
  snct_neighbour_calculator calculator;

  // act
  auto neighbours = calculator.GetNeighbours(base_point);

  // assert
  EXPECT_EQ(expected_neighbours.size(), neighbours.size());
  auto are_equal = std::is_permutation(expected_neighbours.cbegin(),
                                       expected_neighbours.cend(),
                                       neighbours.cbegin(),
                                       test_utils::PointsEqual3d);
  EXPECT_TRUE(are_equal);
}

TEST(Six_Neighbour_Calculator, Neighbours_Are_Correct_For_Point_035) {
  // arrange
  const skeleton::Point<3>::coordinate x = 0, y = 3, z = 5;
  skeleton::Point<3> base_point { { x, y, z } };
  std::vector<skeleton::Point<3>> expected_neighbours;
  expected_neighbours.push_back(skeleton::Point<3> { { 1, 3, 5 } });
  expected_neighbours.push_back(skeleton::Point<3> { { 0, 2, 5 } });
  expected_neighbours.push_back(skeleton::Point<3> { { 0, 4, 5 } });
  expected_neighbours.push_back(skeleton::Point<3> { { 0, 3, 4 } });
  expected_neighbours.push_back(skeleton::Point<3> { { 0, 3, 6 } });
  expected_neighbours.push_back(skeleton::Point<3> { { 0xffff, 3, 5 } });
  snct_neighbour_calculator calculator;

  // act
  auto neighbours = calculator.GetNeighbours(base_point);

  // assert
  EXPECT_EQ(expected_neighbours.size(), neighbours.size());
  auto are_equal = std::is_permutation(expected_neighbours.cbegin(),
                                       expected_neighbours.cend(),
                                       neighbours.cbegin(),
                                       test_utils::PointsEqual3d);
  EXPECT_TRUE(are_equal);
}

}  // namespace skeleton_test

#endif  // TEST_SRC_SKELETON_SIX_NEIGHBOUR_CALCULATOR_TEST_H_
