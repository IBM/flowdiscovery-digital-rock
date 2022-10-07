/**
 * \file test/src/skeleton/network_node_test.h
 *
 * \authors Rodrigo Neumann Barros Ferreira \<rneumann@br.ibm.com\>
 * \copyright © IBM Corp.
 * \date 2018
 */

#ifndef TEST_SRC_SKELETON_NETWORK_NODE_TEST_H_
#define TEST_SRC_SKELETON_NETWORK_NODE_TEST_H_

#include <gtest/gtest.h>
#include "src/skeleton/network/node.h"

namespace skeleton_test {

using test_node_t = skeleton::Node<>;

TEST(Network_Node, Node_Constructor_Test) {
  test_node_t::id_type id = 1;
  test_node_t::point_type point = {{1, 2, 3}};
  test_node_t::annotation_type annotation = 42;
  test_node_t node(id, point, annotation);

  EXPECT_EQ(node.GetId(), 1);
  EXPECT_EQ(node.GetPoint(), point);
  EXPECT_EQ(node.GetAnnotation(), 42);
}

TEST(Network_Node, Node_Temporary_Constructor_Test) {
  test_node_t::point_type point = {{1, 2, 3}};
  test_node_t node(point);

  EXPECT_EQ(node.GetPoint(), point);
}

TEST(Network_Node, Node_CalculateNeighbourPoints_Test) {
  test_node_t::point_type point = {{1, 2, 3}};
  test_node_t node(point);
  test_node_t::neighbours_type neighbours = node.CalculateNeighbourPoints();

  EXPECT_EQ(neighbours[0], test_node_t::point_type({0, 1, 2}));
  EXPECT_EQ(neighbours[1], test_node_t::point_type({0, 1, 3}));
  EXPECT_EQ(neighbours[2], test_node_t::point_type({0, 1, 4}));
  EXPECT_EQ(neighbours[3], test_node_t::point_type({0, 2, 2}));
  EXPECT_EQ(neighbours[4], test_node_t::point_type({0, 2, 3}));
  EXPECT_EQ(neighbours[5], test_node_t::point_type({0, 2, 4}));
  EXPECT_EQ(neighbours[6], test_node_t::point_type({0, 3, 2}));
  EXPECT_EQ(neighbours[7], test_node_t::point_type({0, 3, 3}));
  EXPECT_EQ(neighbours[8], test_node_t::point_type({0, 3, 4}));
  EXPECT_EQ(neighbours[9], test_node_t::point_type({1, 1, 2}));
  EXPECT_EQ(neighbours[10], test_node_t::point_type({1, 1, 3}));
  EXPECT_EQ(neighbours[11], test_node_t::point_type({1, 1, 4}));
  EXPECT_EQ(neighbours[12], test_node_t::point_type({1, 2, 2}));
  EXPECT_EQ(neighbours[13], test_node_t::point_type({1, 2, 4}));
  EXPECT_EQ(neighbours[14], test_node_t::point_type({1, 3, 2}));
  EXPECT_EQ(neighbours[15], test_node_t::point_type({1, 3, 3}));
  EXPECT_EQ(neighbours[16], test_node_t::point_type({1, 3, 4}));
  EXPECT_EQ(neighbours[17], test_node_t::point_type({2, 1, 2}));
  EXPECT_EQ(neighbours[18], test_node_t::point_type({2, 1, 3}));
  EXPECT_EQ(neighbours[19], test_node_t::point_type({2, 1, 4}));
  EXPECT_EQ(neighbours[20], test_node_t::point_type({2, 2, 2}));
  EXPECT_EQ(neighbours[21], test_node_t::point_type({2, 2, 3}));
  EXPECT_EQ(neighbours[22], test_node_t::point_type({2, 2, 4}));
  EXPECT_EQ(neighbours[23], test_node_t::point_type({2, 3, 2}));
  EXPECT_EQ(neighbours[24], test_node_t::point_type({2, 3, 3}));
  EXPECT_EQ(neighbours[25], test_node_t::point_type({2, 3, 4}));
}

TEST(Network_Node, Node_CalculateSquaredDistance_Test) {
  test_node_t node(test_node_t::point_type({0, 0, 0}));
  test_node_t face_neighbour(test_node_t::point_type({1, 0, 0}));
  test_node_t edge_neighbour(test_node_t::point_type({1, 1, 0}));
  test_node_t vertex_neighbour(test_node_t::point_type({1, 1, 1}));

  EXPECT_EQ(node.CalculateSquaredDistance(face_neighbour), 1);
  EXPECT_EQ(face_neighbour.CalculateSquaredDistance(node), 1);
  EXPECT_EQ(node.CalculateSquaredDistance(edge_neighbour), 2);
  EXPECT_EQ(edge_neighbour.CalculateSquaredDistance(node), 2);
  EXPECT_EQ(node.CalculateSquaredDistance(vertex_neighbour), 3);
  EXPECT_EQ(vertex_neighbour.CalculateSquaredDistance(node), 3);
}

TEST(Network_Node, Node_Comparison_Test) {
  test_node_t ooo(test_node_t::point_type({0, 0, 0}));
  test_node_t ool(test_node_t::point_type({0, 0, 1}));
  test_node_t olo(test_node_t::point_type({0, 1, 0}));
  test_node_t oll(test_node_t::point_type({0, 1, 1}));
  test_node_t loo(test_node_t::point_type({1, 0, 0}));
  test_node_t lol(test_node_t::point_type({1, 0, 1}));
  test_node_t llo(test_node_t::point_type({1, 1, 0}));
  test_node_t lll(test_node_t::point_type({1, 1, 1}));

  EXPECT_LT(ooo, ool);
  EXPECT_LT(ool, olo);
  EXPECT_LT(olo, oll);
  EXPECT_LT(oll, loo);
  EXPECT_LT(loo, lol);
  EXPECT_LT(lol, llo);
  EXPECT_LT(llo, lll);
}

TEST(Network_Node, Node_GetPointCoordinate_Test) {
  test_node_t::point_type point = {{1, 2, 3}};
  test_node_t node(point);

  EXPECT_EQ(node.GetPointCoordinate(0), 1);
  EXPECT_EQ(node.GetPointCoordinate(1), 2);
  EXPECT_EQ(node.GetPointCoordinate(2), 3);
}

}  // namespace skeleton_test

#endif  // TEST_SRC_SKELETON_NETWORK_NODE_TEST_H_
