/**
 * \file test/src/skeleton/network_link_test.h
 *
 * \authors Rodrigo Neumann Barros Ferreira \<rneumann@br.ibm.com\>
 * \copyright © IBM Corp.
 * \date 2018
 */

#ifndef TEST_SRC_SKELETON_NETWORK_LINK_TEST_H_
#define TEST_SRC_SKELETON_NETWORK_LINK_TEST_H_

#include <gtest/gtest.h>
#include "src/skeleton/network/link.h"

namespace skeleton_test {

using test_link_t = skeleton::Link;

TEST(Network_Link, Link_Constructor_Test) {
  test_link_t::id_type link_id = 1;
  test_link_t::id_type source_id = 2;
  test_link_t::id_type target_id = 3;
  test_link_t::link_geometry_type length = 4.0;
  test_link_t::link_geometry_type squared_radius = 5.0;
  test_link_t link(link_id, source_id, target_id, length, squared_radius);

  EXPECT_EQ(link.GetId(), 1);
  EXPECT_EQ(link.GetSourceId(), 2);
  EXPECT_EQ(link.GetTargetId(), 3);
  EXPECT_DOUBLE_EQ(link.GetLength(), 4.0);
  EXPECT_DOUBLE_EQ(link.GetSquaredRadius(), 5.0);
}

TEST(Network_Link, Link_Temporary_Constructor_Test) {
  test_link_t::id_type source_id = 2;
  test_link_t::id_type target_id = 3;
  test_link_t link(source_id, target_id);

  EXPECT_EQ(link.GetSourceId(), 2);
  EXPECT_EQ(link.GetTargetId(), 3);
}

TEST(Network_Link, Link_Comparison_Test) {
  test_link_t zero_to_one(0, 1);
  test_link_t zero_to_two(0, 2);
  test_link_t one_to_two(1, 2);

  EXPECT_LT(zero_to_one, zero_to_two);
  EXPECT_LT(zero_to_one, one_to_two);
  EXPECT_LT(zero_to_two, one_to_two);
}

}  // namespace skeleton_test

#endif  // TEST_SRC_SKELETON_NETWORK_LINK_TEST_H_
