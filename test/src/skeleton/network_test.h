/**
 * \file test/src/skeleton/network_test.h
 *
 * \authors Rodrigo Neumann Barros Ferreira \<rneumann@br.ibm.com\>
 * \copyright © IBM Corp.
 * \date 2018
 */

#ifndef TEST_SRC_SKELETON_NETWORK_TEST_H_
#define TEST_SRC_SKELETON_NETWORK_TEST_H_

#include <gtest/gtest.h>
#include "src/skeleton/network/network.h"

namespace skeleton_test {

using test_network_t = skeleton::Network<>;

TEST(Network, Network_Constructor_Test) {
  test_network_t network;

  EXPECT_EQ(network.GetNodeSetSize(), 0);
  EXPECT_EQ(network.GetLinkSetSize(), 0);
}

TEST(Network, Network_InsertNode_Once_Test) {
  test_network_t network;
  test_network_t::node_point_type point = {{1, 2, 3}};
  test_network_t::node_annotation_type annotation = 4;
  test_network_t::node_set_insert_return_type success = network.InsertNode(point, annotation);

  EXPECT_TRUE(success.second);
  EXPECT_EQ(network.GetNodeSetSize(), 1);
  EXPECT_EQ(network.GetLinkSetSize(), 0);
}

TEST(Network, Network_InsertNode_Twice_Test) {
  test_network_t network;
  test_network_t::node_point_type first_point = {{0, 0, 0}};
  test_network_t::node_point_type second_point = {{1, 1, 1}};
  test_network_t::node_annotation_type first_annotation = 1;
  test_network_t::node_annotation_type second_annotation = 4;
  test_network_t::node_set_insert_return_type success1 = network.InsertNode(first_point,
                                                                            first_annotation);
  test_network_t::node_set_insert_return_type success2 = network.InsertNode(second_point,
                                                                            second_annotation);
  EXPECT_TRUE(success1.second);
  EXPECT_TRUE(success2.second);
  EXPECT_EQ(network.GetNodeSetSize(), 2);
  EXPECT_EQ(network.GetLinkSetSize(), 0);
}

TEST(Network, Network_InsertNode_Duplicate_Test) {
  test_network_t network;
  test_network_t::node_point_type first_point = {{0, 0, 0}};
  test_network_t::node_point_type second_point = {{0, 0, 0}};
  test_network_t::node_annotation_type first_annotation = 1;
  test_network_t::node_annotation_type second_annotation = 1;
  test_network_t::node_set_insert_return_type success1 = network.InsertNode(first_point,
                                                                            first_annotation);
  test_network_t::node_set_insert_return_type success2 = network.InsertNode(second_point,
                                                                            second_annotation);
  EXPECT_TRUE(success1.second);
  EXPECT_FALSE(success2.second);
  EXPECT_EQ(network.GetNodeSetSize(), 1);
  EXPECT_EQ(network.GetLinkSetSize(), 0);
}

TEST(Network, Network_CalculateNeighbourNodes_Test) {
  test_network_t network;
  test_network_t::node_point_type first_point = {{0, 0, 0}};
  test_network_t::node_point_type second_point = {{1, 1, 1}};
  test_network_t::node_point_type third_point = {{2, 2, 2}};
  test_network_t::node_annotation_type first_annotation = 1;
  test_network_t::node_annotation_type second_annotation = 4;
  test_network_t::node_annotation_type third_annotation = 9;
  network.InsertNode(first_point, first_annotation);
  network.InsertNode(second_point, second_annotation);
  network.InsertNode(third_point, third_annotation);

  test_network_t::node_type first_node(first_point);
  test_network_t::node_type second_node(second_point);
  test_network_t::node_type third_node(third_point);
  auto neighbours_of_first = network.CalculateNeighbourNodes(first_node);
  auto neighbours_of_second = network.CalculateNeighbourNodes(second_node);
  auto neighbours_of_third = network.CalculateNeighbourNodes(third_node);

  EXPECT_EQ(neighbours_of_first.size(), 1);
  EXPECT_EQ(neighbours_of_second.size(), 2);
  EXPECT_EQ(neighbours_of_third.size(), 1);

  EXPECT_EQ(neighbours_of_first.at(0), second_node);
  EXPECT_EQ(neighbours_of_second.at(0), first_node);
  EXPECT_EQ(neighbours_of_second.at(1), third_node);
  EXPECT_EQ(neighbours_of_third.at(0), second_node);
}

TEST(Network, Network_InsertLink_Test) {
  test_network_t network;
  test_network_t::node_point_type first_point = {{0, 0, 0}};
  test_network_t::node_point_type second_point = {{1, 1, 1}};
  test_network_t::node_annotation_type first_annotation = 1;
  test_network_t::node_annotation_type second_annotation = 4;
  network.InsertNode(first_point, first_annotation);
  network.InsertNode(second_point, second_annotation);

  test_network_t::node_type first_node = *(network.node_cbegin());
  test_network_t::node_type second_node = *(--network.node_cend());
  test_network_t::link_set_insert_return_type success = network.InsertLink(first_node, second_node);

  EXPECT_TRUE(success.second);
  EXPECT_TRUE(network.HasLink(first_node, second_node));

  EXPECT_EQ(network.GetNodeSetSize(), 2);
  EXPECT_EQ(network.GetLinkSetSize(), 1);
}

TEST(Network, Network_CalculateLinkDimensions_Test) {
  test_network_t network;
  test_network_t::node_point_type first_point = {{0, 0, 0}};
  test_network_t::node_point_type second_point = {{0, 0, 1}};
  test_network_t::node_point_type third_point = {{1, 1, 2}};
  test_network_t::node_annotation_type first_annotation = 1;
  test_network_t::node_annotation_type second_annotation = 4;
  test_network_t::node_annotation_type third_annotation = 9;
  network.InsertNode(first_point, first_annotation);
  network.InsertNode(second_point, second_annotation);
  network.InsertNode(third_point, third_annotation);

  test_network_t::node_type first_node = *(network.node_cbegin());
  test_network_t::node_type second_node = *(++network.node_cbegin());
  test_network_t::node_type third_node = *(--network.node_cend());
  test_network_t::link_set_insert_return_type success1 = network.InsertLink(first_node,
                                                                            second_node);
  test_network_t::link_set_insert_return_type success2 = network.InsertLink(second_node,
                                                                            third_node);

  test_network_t::link_type first_link = *(network.link_cbegin());
  test_network_t::link_type second_link = *(--network.link_cend());

  EXPECT_TRUE(success1.second);
  EXPECT_TRUE(success2.second);
  EXPECT_EQ(network.GetNodeSetSize(), 3);
  EXPECT_EQ(network.GetLinkSetSize(), 2);

  EXPECT_EQ(first_link.GetId(), 0);
  EXPECT_EQ(first_link.GetSourceId(), 0);
  EXPECT_EQ(first_link.GetTargetId(), 1);
  EXPECT_EQ(first_link.GetLength(), 1.0);
  EXPECT_EQ(first_link.GetSquaredRadius(), std::sqrt(2.0) * 4.0 / std::sqrt(1.0 + 16.0));
  EXPECT_EQ(second_link.GetId(), 1);
  EXPECT_EQ(second_link.GetSourceId(), 1);
  EXPECT_EQ(second_link.GetTargetId(), 2);
  EXPECT_EQ(second_link.GetLength(), std::sqrt(3.0));
  EXPECT_EQ(second_link.GetSquaredRadius(), std::sqrt(2.0) * 4.0 * 9.0 / std::sqrt(16.0 + 81.0));
}

}  // namespace skeleton_test

#endif  // TEST_SRC_SKELETON_NETWORK_TEST_H_
