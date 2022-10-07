/**
 * \file test/src/skeleton/centerline_test.h
 *
 * \author Hugo de Oliveira Barbalho \<hugob@br.ibm.com\>
 * \copyright © IBM Corp.
 * \date 2017
 */

#ifndef TEST_SRC_SKELETON_CENTERLINE_TEST_H_
#define TEST_SRC_SKELETON_CENTERLINE_TEST_H_

#include <gtest/gtest.h>
#include <vector>
#include <utility>
#include <array>
#include <limits>

#include "test/src/utils/test_utils.h"
#include "src/skeleton/centerline/centerline.h"
#include "src/skeleton/points/point.h"
#include "src/skeleton/images/annotated_image.h"
#include "src/skeleton/graph/annotated_vertex_helper.h"
#include "src/skeleton/neighbours/twenty_six_neighbour_calculator.h"
#include "src/skeleton/centerline/vertex_neighbour_calculator.h"
#include "src/skeleton/centerline/vertex.h"
#include "src/skeleton/graph/memory_graph.h"
#include "src/skeleton/images/arma_3d_binary_image_builder.h"
#include "src/skeleton/centerline/memory_graph_builder.h"
#include "src/skeleton/contours/contour_calculator.h"
#include "src/skeleton/images/integer_image.h"
#include "src/skeleton/centerline/centerline_set.h"
#include "src/skeleton/skeletonizer_by_ift.h"

namespace skeleton_test {

using image3d = skeleton::BinaryImage<skeleton::Point<3>>;
using point_type = skeleton::Point<3>;
using distance_type = double;

using annotated_image3d =
  skeleton::AnnotatedImage<point_type, skeleton::Annotation<point_type>>;
using point_builder = skeleton::PointBuilder;
using neighbour_calculator3d =
  skeleton::TwentySixNeighbourCalculator<point_type, point_builder>;

using annotation_type3d = typename
  annotated_image3d::annotation_type;
using quadratic_path_calculator3d =
  skeleton::QuadraticPathCalculator<annotation_type3d, point_type>;

using real_distance_type =
  typename quadratic_path_calculator3d::real_distance_type;
using point_and_distance =
  skeleton::Vertex<point_type, distance_type>;
using annotated_vertex =
  skeleton::AnnotatedVertexHelper<point_and_distance, real_distance_type>;
using graph_neighbour_calculator = skeleton::VertexNeighbourCalculator< point_and_distance,
                                                                        neighbour_calculator3d,
                                                                        annotated_image3d>;
using graph_type = skeleton::MemoryGraph<annotated_vertex,
                                         graph_neighbour_calculator>;
using graph_builder_type =
  skeleton::MemoryGraphBuilder< graph_type, annotated_image3d, graph_neighbour_calculator>;

using contour_calculator3d =
  skeleton::ContourCalculator<image3d, annotated_image3d, neighbour_calculator3d>;

using integer_image3d = skeleton::IntegerImage<point_type, int32_t>;


using skeleton_image3d = integer_image3d;

using skeletonizer3d = skeleton::SkeletonizerByIFT< image3d,
                                                    annotated_image3d,
                                                    neighbour_calculator3d,
                                                    contour_calculator3d,
                                                    quadratic_path_calculator3d,
                                                    integer_image3d,
                                                    skeleton_image3d>;

using path_distance_calculator = typename skeletonizer3d::path_calculator_type;

using set_type = skeleton::CenterlineSet<graph_type, path_distance_calculator>;

class CenterLineCreator {
 private:
  point_type *points_;
  point_and_distance *vertices_key_;
  void InitialisePoints() {
    points_ = new point_type[10];
    vertices_key_ = new point_and_distance[10];

    points_[0] = point_type{{0, 1, 2}};
    points_[1] = point_type{{0, 2, 2}};
    points_[2] = point_type{{0, 2, 3}};
    points_[3] = point_type{{0, 2, 4}};
    points_[4] = point_type{{0, 3, 4}};
    points_[5] = point_type{{1, 2, 3}};
    points_[6] = point_type{{1, 2, 4}};
    points_[7] = point_type{{1, 3, 4}};
    points_[8] = point_type{{2, 2, 3}};
    points_[9] = point_type{{2, 3, 3}};

    for (int i = 0; i != 10; ++i)
      vertices_key_[i] = point_and_distance{points_[i], static_cast<double>(i+1)};
  }

  void add_points_to_path(graph_type &path, const std::vector<point_and_distance> points_keys) {
    for (std::size_t i = 0; i != points_keys.size(); ++i) {
      if (!path.HasVertex(points_keys[i]))
        path.Insert(annotated_vertex{points_keys[i], 1.0});
      auto &vertex_annotation = path[points_keys[i]];
      vertex_annotation.SetDistance(1.0);
      if (i != 0)
        vertex_annotation.SetPredecessor(points_keys[i-1]);
    }
  }

  graph_type create_graph(const std::size_t nrows,
                         const std::size_t ncols,
                         const std::size_t length) {
    const arma::Cube<uint8_t> cube = arma::zeros<arma::Cube<uint8_t>>(nrows, ncols, length);
    skeleton::Arma3DBinaryImageBuilder<uint8_t> builder;

    // act
    auto result = builder.BuildImage(cube);

    skeletonizer3d skeletonizer{std::move(result)};
    skeletonizer.ComputeSkeleton();
    const auto &annotations = skeletonizer.GetAnnotations();

    graph_builder_type graph_builder;
    graph_type path = graph_builder.Build(annotations, skeletonizer.GetImage());
    return path;
  }

 public:
  void TearDown() {
    delete[] points_;
    delete[] vertices_key_;
  }

  point_type GetPoint(int i) {
    return points_[i];
  }

  point_and_distance GetVertexKey(int i) {
    return vertices_key_[i];
  }

  graph_type BuildPathWithInfinityDistance() {
    InitialisePoints();
    graph_type path = create_graph(5, 5, 5);
    path.Insert(annotated_vertex{GetVertexKey(0), std::numeric_limits<double>::max()});
    return path;
  }

  std::pair<graph_type, graph_type> BuildPathWithTwoBranchPoints2() {
    InitialisePoints();
    graph_type path1 = create_graph(5, 5, 5);
    graph_type path2 = create_graph(5, 5, 5);

    std::vector<point_and_distance> v[3];
    v[0].push_back(GetVertexKey(0));
    v[0].push_back(GetVertexKey(1));
    v[0].push_back(GetVertexKey(2));
    v[0].push_back(GetVertexKey(5));
    v[0].push_back(GetVertexKey(8));
    v[0].push_back(GetVertexKey(9));

    v[1].push_back(GetVertexKey(0));
    v[1].push_back(GetVertexKey(1));
    v[1].push_back(GetVertexKey(2));
    v[1].push_back(GetVertexKey(3));
    v[1].push_back(GetVertexKey(4));

    v[2].push_back(GetVertexKey(2));
    v[2].push_back(GetVertexKey(5));
    v[2].push_back(GetVertexKey(8));
    v[2].push_back(GetVertexKey(6));
    v[2].push_back(GetVertexKey(7));

    add_points_to_path(path1, v[0]);
    add_points_to_path(path2, v[1]);
    add_points_to_path(path2, v[2]);

    return std::make_pair(path1, path2);
  }

  std::pair<graph_type, graph_type> BuildPathWithTwoBranchPoints() {
    InitialisePoints();
    graph_type path1 = create_graph(5, 5, 5);
    graph_type path2 = create_graph(5, 5, 5);

    std::vector<point_and_distance> v[3];
    v[0].push_back(GetVertexKey(0));
    v[0].push_back(GetVertexKey(1));
    v[0].push_back(GetVertexKey(2));
    v[0].push_back(GetVertexKey(3));
    v[0].push_back(GetVertexKey(4));

    v[1].push_back(GetVertexKey(2));
    v[1].push_back(GetVertexKey(5));
    v[1].push_back(GetVertexKey(6));
    v[1].push_back(GetVertexKey(7));

    v[2].push_back(GetVertexKey(0));
    v[2].push_back(GetVertexKey(1));
    v[2].push_back(GetVertexKey(2));
    v[2].push_back(GetVertexKey(5));
    v[2].push_back(GetVertexKey(8));
    v[2].push_back(GetVertexKey(9));

    add_points_to_path(path1, v[0]);
    add_points_to_path(path1, v[1]);
    add_points_to_path(path2, v[2]);

    return std::make_pair(path1, path2);
  }

  std::pair<graph_type, graph_type> BuildPathWithNoBranchPoints2() {
    InitialisePoints();

    graph_type path1 = create_graph(5, 5, 5);
    graph_type path2 = create_graph(5, 5, 5);

    std::vector<point_and_distance> v[2];
    v[0].push_back(GetVertexKey(2));
    v[0].push_back(GetVertexKey(3));
    v[0].push_back(GetVertexKey(4));

    v[1].push_back(GetVertexKey(2));
    v[1].push_back(GetVertexKey(5));
    v[1].push_back(GetVertexKey(6));
    v[1].push_back(GetVertexKey(7));

    add_points_to_path(path1, v[0]);
    add_points_to_path(path2, v[1]);

    return std::make_pair(path1, path2);
  }

  graph_type BuildPathWithNoBranchPoints() {
    InitialisePoints();
    graph_type path = create_graph(5, 5, 5);

    std::vector<point_and_distance> v[2];
    v[0].push_back(GetVertexKey(2));
    v[0].push_back(GetVertexKey(3));
    v[0].push_back(GetVertexKey(4));

    v[1].push_back(GetVertexKey(5));
    v[1].push_back(GetVertexKey(6));
    v[1].push_back(GetVertexKey(7));

    add_points_to_path(path, v[0]);
    add_points_to_path(path, v[1]);

    return path;
  }

  graph_type BuildSimplePath() {
    InitialisePoints();
    graph_type path = create_graph(5, 5, 5);

    std::vector<point_and_distance> v;
    v.push_back(GetVertexKey(0));

    add_points_to_path(path, v);
    return path;
  }

  std::pair<graph_type, graph_type> BuildTwoPathsWithOneBranchPoint() {
    InitialisePoints();
    graph_type path1 = create_graph(5, 5, 5);
    graph_type path2 = create_graph(5, 5, 5);

    std::vector<point_and_distance> v[3];
    v[0].push_back(GetVertexKey(0));
    v[0].push_back(GetVertexKey(1));
    v[0].push_back(GetVertexKey(2));
    v[0].push_back(GetVertexKey(3));
    v[0].push_back(GetVertexKey(4));

    v[1].push_back(GetVertexKey(2));
    v[1].push_back(GetVertexKey(5));
    v[1].push_back(GetVertexKey(6));
    v[1].push_back(GetVertexKey(7));

    v[2].push_back(GetVertexKey(9));
    v[2].push_back(GetVertexKey(8));
    v[2].push_back(GetVertexKey(2));
    v[2].push_back(GetVertexKey(5));
    v[2].push_back(GetVertexKey(6));
    v[2].push_back(GetVertexKey(7));

    add_points_to_path(path1, v[0]);
    add_points_to_path(path1, v[1]);
    add_points_to_path(path2, v[2]);

    return std::make_pair(path1, path2);
  }

  std::array<graph_type, 3> BuildThreePathsWithTwoBranchPoints() {
    InitialisePoints();
    graph_type path1 = create_graph(5, 5, 5);
    graph_type path2 = create_graph(5, 5, 5);
    graph_type path3 = create_graph(5, 5, 5);

    std::vector<point_and_distance> v[3];
    v[0].push_back(GetVertexKey(0));
    v[0].push_back(GetVertexKey(1));
    v[0].push_back(GetVertexKey(2));
    v[0].push_back(GetVertexKey(3));
    v[0].push_back(GetVertexKey(4));

    v[1].push_back(GetVertexKey(8));
    v[1].push_back(GetVertexKey(1));
    v[1].push_back(GetVertexKey(2));
    v[1].push_back(GetVertexKey(3));
    v[1].push_back(GetVertexKey(5));
    v[1].push_back(GetVertexKey(6));

    v[2].push_back(GetVertexKey(7));
    v[2].push_back(GetVertexKey(6));

    add_points_to_path(path1, v[0]);
    add_points_to_path(path2, v[1]);
    add_points_to_path(path3, v[2]);

    std::array<graph_type, 3> paths = {path1, path2, path3};

    return paths;
  }

  graph_type BuildPathWithOneBranchPoint() {
    InitialisePoints();
    graph_type path = create_graph(5, 5, 5);

    std::vector<point_and_distance> v[2];
    v[0].push_back(GetVertexKey(0));
    v[0].push_back(GetVertexKey(1));
    v[0].push_back(GetVertexKey(2));
    v[0].push_back(GetVertexKey(3));
    v[0].push_back(GetVertexKey(4));

    v[1].push_back(GetVertexKey(2));
    v[1].push_back(GetVertexKey(5));
    v[1].push_back(GetVertexKey(6));
    v[1].push_back(GetVertexKey(7));

    add_points_to_path(path, v[0]);
    add_points_to_path(path, v[1]);

    return path;
  }
};

class CenterLineNode {
 private:
  point_type point_;
  distance_type distance_;

 public:
  CenterLineNode(const point_type &point, const distance_type distance)
      : point_(point),
        distance_(distance) {
  }
  point_type Point() const {
    return point_;
  }

  distance_type Distance() const {
    return distance_;
  }
};

using centerline_node_type = CenterLineNode;

using centerline_type = skeleton::Centerline<centerline_node_type>;

TEST(Centerline, Split_Should_Create_New_Centerline) {
  std::vector<centerline_node_type> nodes;
  point_type points[5] = { point_type{{0, 1, 1}}, point_type{{0, 2, 2}},
                          point_type{{0, 2, 3}}, point_type{{0, 2, 4}},
                          point_type{{0, 3, 4}} };

  for (int i = 0; i != 5; ++i)
    nodes.push_back(centerline_node_type {points[i], 0.0});

  centerline_type centerline(nodes);
  EXPECT_EQ(5, centerline.GetNumPoints());

  centerline_type centerline_part2 = centerline.split(3);

  EXPECT_EQ(4, centerline.GetNumPoints());
  EXPECT_EQ(2, centerline_part2.GetNumPoints());

  EXPECT_TRUE(test_utils::PointsEqual3d(centerline[0].Point(), points[0]));
  EXPECT_TRUE(test_utils::PointsEqual3d(centerline[1].Point(), points[1]));
  EXPECT_TRUE(test_utils::PointsEqual3d(centerline[2].Point(), points[2]));
  EXPECT_TRUE(test_utils::PointsEqual3d(centerline[3].Point(), points[3]));
  EXPECT_TRUE(test_utils::PointsEqual3d(centerline_part2[0].Point(), points[3]));
  EXPECT_TRUE(test_utils::PointsEqual3d(centerline_part2[1].Point(), points[4]));
}

TEST(Centerline, ShouldDetectBranchs) {
  CenterLineCreator creator;
  auto path = creator.BuildPathWithOneBranchPoint();
  set_type centerlines {};

  EXPECT_TRUE(path.HasVertex(creator.GetVertexKey(0)));
  EXPECT_TRUE(path.HasVertex(creator.GetVertexKey(1)));
  EXPECT_TRUE(path.HasVertex(creator.GetVertexKey(2)));
  EXPECT_TRUE(path.HasVertex(creator.GetVertexKey(3)));
  EXPECT_TRUE(path.HasVertex(creator.GetVertexKey(4)));
  EXPECT_TRUE(path.HasVertex(creator.GetVertexKey(5)));
  EXPECT_TRUE(path.HasVertex(creator.GetVertexKey(6)));
  EXPECT_TRUE(path.HasVertex(creator.GetVertexKey(7)));

  centerlines.AddSet(path, creator.GetVertexKey(4));
  centerlines.AddSet(path, creator.GetVertexKey(7));

  EXPECT_FALSE(centerlines.IsBranch(creator.GetVertexKey(0)));
  EXPECT_FALSE(centerlines.IsBranch(creator.GetVertexKey(1)));
  EXPECT_TRUE(centerlines.IsBranch(creator.GetVertexKey(2)));
  EXPECT_FALSE(centerlines.IsBranch(creator.GetVertexKey(3)));
  EXPECT_FALSE(centerlines.IsBranch(creator.GetVertexKey(4)));
  EXPECT_FALSE(centerlines.IsBranch(creator.GetVertexKey(5)));
  EXPECT_FALSE(centerlines.IsBranch(creator.GetVertexKey(6)));
  EXPECT_FALSE(centerlines.IsBranch(creator.GetVertexKey(7)));

  creator.TearDown();
}

TEST(Centerline, SourceAndEndPointShouldNotBeBranchPoints) {
  CenterLineCreator creator;
  auto path = creator.BuildPathWithOneBranchPoint();
  set_type centerlines {};

  EXPECT_TRUE(path.HasVertex(creator.GetVertexKey(0)));
  EXPECT_TRUE(path.HasVertex(creator.GetVertexKey(1)));
  EXPECT_TRUE(path.HasVertex(creator.GetVertexKey(2)));
  EXPECT_TRUE(path.HasVertex(creator.GetVertexKey(3)));
  EXPECT_TRUE(path.HasVertex(creator.GetVertexKey(4)));

  centerlines.AddSet(path, creator.GetVertexKey(4));

  EXPECT_FALSE(centerlines.IsBranch(creator.GetVertexKey(0)));
  EXPECT_FALSE(centerlines.IsBranch(creator.GetVertexKey(1)));
  EXPECT_FALSE(centerlines.IsBranch(creator.GetVertexKey(2)));
  EXPECT_FALSE(centerlines.IsBranch(creator.GetVertexKey(3)));
  EXPECT_FALSE(centerlines.IsBranch(creator.GetVertexKey(4)));

  creator.TearDown();
}

TEST(Centerline, ShouldHaveNoBranchPoints) {
  CenterLineCreator creator;
  auto path = creator.BuildPathWithNoBranchPoints();
  set_type centerlines {};

  EXPECT_FALSE(path.HasVertex(creator.GetVertexKey(0)));
  EXPECT_FALSE(path.HasVertex(creator.GetVertexKey(1)));
  EXPECT_TRUE(path.HasVertex(creator.GetVertexKey(2)));
  EXPECT_TRUE(path.HasVertex(creator.GetVertexKey(3)));
  EXPECT_TRUE(path.HasVertex(creator.GetVertexKey(4)));
  EXPECT_TRUE(path.HasVertex(creator.GetVertexKey(5)));
  EXPECT_TRUE(path.HasVertex(creator.GetVertexKey(6)));
  EXPECT_TRUE(path.HasVertex(creator.GetVertexKey(7)));

  centerlines.AddSet(path, creator.GetVertexKey(4));
  centerlines.AddSet(path, creator.GetVertexKey(7));

  EXPECT_FALSE(centerlines.IsBranch(creator.GetVertexKey(2)));
  EXPECT_FALSE(centerlines.IsBranch(creator.GetVertexKey(3)));
  EXPECT_FALSE(centerlines.IsBranch(creator.GetVertexKey(4)));
  EXPECT_FALSE(centerlines.IsBranch(creator.GetVertexKey(5)));
  EXPECT_FALSE(centerlines.IsBranch(creator.GetVertexKey(6)));
  EXPECT_FALSE(centerlines.IsBranch(creator.GetVertexKey(7)));

  creator.TearDown();
}

TEST(Centerline, ShouldHaveNoBranchPoints2) {
  CenterLineCreator creator;
  auto paths = creator.BuildPathWithNoBranchPoints2();
  set_type centerlines {};

  EXPECT_FALSE(paths.first.HasVertex(creator.GetVertexKey(0)));
  EXPECT_FALSE(paths.second.HasVertex(creator.GetVertexKey(0)));
  EXPECT_FALSE(paths.first.HasVertex(creator.GetVertexKey(1)));
  EXPECT_FALSE(paths.second.HasVertex(creator.GetVertexKey(1)));
  EXPECT_TRUE(paths.first.HasVertex(creator.GetVertexKey(2)));
  EXPECT_TRUE(paths.second.HasVertex(creator.GetVertexKey(2)));
  EXPECT_TRUE(paths.first.HasVertex(creator.GetVertexKey(3)));
  EXPECT_TRUE(paths.first.HasVertex(creator.GetVertexKey(4)));
  EXPECT_TRUE(paths.second.HasVertex(creator.GetVertexKey(5)));
  EXPECT_TRUE(paths.second.HasVertex(creator.GetVertexKey(6)));
  EXPECT_TRUE(paths.second.HasVertex(creator.GetVertexKey(7)));

  centerlines.AddSet(paths.first, creator.GetVertexKey(2));
  centerlines.AddSet(paths.second, creator.GetVertexKey(2));

  EXPECT_FALSE(centerlines.IsBranch(creator.GetVertexKey(2)));
  EXPECT_FALSE(centerlines.IsBranch(creator.GetVertexKey(3)));
  EXPECT_FALSE(centerlines.IsBranch(creator.GetVertexKey(4)));
  EXPECT_FALSE(centerlines.IsBranch(creator.GetVertexKey(5)));
  EXPECT_FALSE(centerlines.IsBranch(creator.GetVertexKey(6)));
  EXPECT_FALSE(centerlines.IsBranch(creator.GetVertexKey(7)));

  creator.TearDown();
}

TEST(Centerline, ShouldSplitCenterlinesByBranchPoints) {
  CenterLineCreator creator;
  auto path = creator.BuildPathWithOneBranchPoint();
  set_type centerlines {};

  EXPECT_EQ(0, centerlines.size());
  centerlines.AddSet(path, creator.GetVertexKey(4));
  EXPECT_EQ(1, centerlines.size());
  centerlines.SplitByBranchPoints();
  EXPECT_EQ(1, centerlines.size());
  centerlines.AddSet(path, creator.GetVertexKey(7));
  EXPECT_EQ(2, centerlines.size());
  centerlines.SplitByBranchPoints();
  EXPECT_EQ(3, centerlines.size());

  EXPECT_EQ(3, centerlines[0].GetNumPoints());
  EXPECT_EQ(4, centerlines[1].GetNumPoints());
  EXPECT_EQ(3, centerlines[2].GetNumPoints());

  EXPECT_TRUE(test_utils::PointsEqual3d(centerlines[0][0].Point().Point(), creator.GetPoint(4)));
  EXPECT_TRUE(test_utils::PointsEqual3d(centerlines[0][1].Point().Point(), creator.GetPoint(3)));
  EXPECT_TRUE(test_utils::PointsEqual3d(centerlines[0][2].Point().Point(), creator.GetPoint(2)));

  EXPECT_TRUE(test_utils::PointsEqual3d(centerlines[1][0].Point().Point(), creator.GetPoint(7)));
  EXPECT_TRUE(test_utils::PointsEqual3d(centerlines[1][1].Point().Point(), creator.GetPoint(6)));
  EXPECT_TRUE(test_utils::PointsEqual3d(centerlines[1][2].Point().Point(), creator.GetPoint(5)));
  EXPECT_TRUE(test_utils::PointsEqual3d(centerlines[1][3].Point().Point(), creator.GetPoint(2)));

  EXPECT_TRUE(test_utils::PointsEqual3d(centerlines[2][0].Point().Point(), creator.GetPoint(2)));
  EXPECT_TRUE(test_utils::PointsEqual3d(centerlines[2][1].Point().Point(), creator.GetPoint(1)));
  EXPECT_TRUE(test_utils::PointsEqual3d(centerlines[2][2].Point().Point(), creator.GetPoint(0)));

  creator.TearDown();
}

TEST(Centerline, ShouldSplitCenterlinesByTwoBranchPoints) {
  CenterLineCreator creator;
  auto paths = creator.BuildPathWithTwoBranchPoints();
  set_type centerlines {};

  EXPECT_EQ(0, centerlines.size());
  centerlines.AddSet(paths.first, creator.GetVertexKey(4));
  EXPECT_EQ(1, centerlines.size());
  EXPECT_FALSE(centerlines.IsBranch(creator.GetVertexKey(0)));
  EXPECT_FALSE(centerlines.IsBranch(creator.GetVertexKey(1)));
  EXPECT_FALSE(centerlines.IsBranch(creator.GetVertexKey(2)));
  EXPECT_FALSE(centerlines.IsBranch(creator.GetVertexKey(3)));
  EXPECT_FALSE(centerlines.IsBranch(creator.GetVertexKey(4)));

  centerlines.AddSet(paths.first, creator.GetVertexKey(7));
  EXPECT_EQ(2, centerlines.size());
  EXPECT_FALSE(centerlines.IsBranch(creator.GetVertexKey(0)));
  EXPECT_FALSE(centerlines.IsBranch(creator.GetVertexKey(1)));
  EXPECT_TRUE(centerlines.IsBranch(creator.GetVertexKey(2)));
  EXPECT_FALSE(centerlines.IsBranch(creator.GetVertexKey(3)));
  EXPECT_FALSE(centerlines.IsBranch(creator.GetVertexKey(4)));
  EXPECT_FALSE(centerlines.IsBranch(creator.GetVertexKey(5)));
  EXPECT_FALSE(centerlines.IsBranch(creator.GetVertexKey(6)));
  EXPECT_FALSE(centerlines.IsBranch(creator.GetVertexKey(7)));

  centerlines.AddSet(paths.second, creator.GetVertexKey(9));
  EXPECT_EQ(3, centerlines.size());
  EXPECT_FALSE(centerlines.IsBranch(creator.GetVertexKey(0)));
  EXPECT_FALSE(centerlines.IsBranch(creator.GetVertexKey(1)));
  EXPECT_TRUE(centerlines.IsBranch(creator.GetVertexKey(2)));
  EXPECT_FALSE(centerlines.IsBranch(creator.GetVertexKey(3)));
  EXPECT_FALSE(centerlines.IsBranch(creator.GetVertexKey(4)));
  EXPECT_TRUE(centerlines.IsBranch(creator.GetVertexKey(5)));
  EXPECT_FALSE(centerlines.IsBranch(creator.GetVertexKey(6)));
  EXPECT_FALSE(centerlines.IsBranch(creator.GetVertexKey(8)));
  EXPECT_FALSE(centerlines.IsBranch(creator.GetVertexKey(9)));

  centerlines.SplitByBranchPoints();

  EXPECT_EQ(5, centerlines.size());

  EXPECT_EQ(3, centerlines[0].GetNumPoints());
  EXPECT_EQ(3, centerlines[1].GetNumPoints());
  EXPECT_EQ(3, centerlines[2].GetNumPoints());
  EXPECT_EQ(3, centerlines[3].GetNumPoints());
  EXPECT_EQ(2, centerlines[4].GetNumPoints());

  EXPECT_TRUE(test_utils::PointsEqual3d(centerlines[0][0].Point().Point(), creator.GetPoint(4)));
  EXPECT_TRUE(test_utils::PointsEqual3d(centerlines[0][1].Point().Point(), creator.GetPoint(3)));
  EXPECT_TRUE(test_utils::PointsEqual3d(centerlines[0][2].Point().Point(), creator.GetPoint(2)));

  EXPECT_TRUE(test_utils::PointsEqual3d(centerlines[1][0].Point().Point(), creator.GetPoint(7)));
  EXPECT_TRUE(test_utils::PointsEqual3d(centerlines[1][1].Point().Point(), creator.GetPoint(6)));
  EXPECT_TRUE(test_utils::PointsEqual3d(centerlines[1][2].Point().Point(), creator.GetPoint(5)));

  EXPECT_TRUE(test_utils::PointsEqual3d(centerlines[2][0].Point().Point(), creator.GetPoint(9)));
  EXPECT_TRUE(test_utils::PointsEqual3d(centerlines[2][1].Point().Point(), creator.GetPoint(8)));
  EXPECT_TRUE(test_utils::PointsEqual3d(centerlines[2][2].Point().Point(), creator.GetPoint(5)));

  EXPECT_TRUE(test_utils::PointsEqual3d(centerlines[3][0].Point().Point(), creator.GetPoint(2)));
  EXPECT_TRUE(test_utils::PointsEqual3d(centerlines[3][1].Point().Point(), creator.GetPoint(1)));
  EXPECT_TRUE(test_utils::PointsEqual3d(centerlines[3][2].Point().Point(), creator.GetPoint(0)));

  EXPECT_TRUE(test_utils::PointsEqual3d(centerlines[4][0].Point().Point(), creator.GetPoint(5)));
  EXPECT_TRUE(test_utils::PointsEqual3d(centerlines[4][1].Point().Point(), creator.GetPoint(2)));

  creator.TearDown();
}

TEST(Centerline, ShouldSplitACenterlineTwice) {
  CenterLineCreator creator;
  auto paths = creator.BuildPathWithTwoBranchPoints2();
  set_type centerlines {};
  centerlines.AddSet(paths.first, creator.GetVertexKey(9));
  centerlines.AddSet(paths.second, creator.GetVertexKey(4));
  centerlines.AddSet(paths.second, creator.GetVertexKey(7));
  EXPECT_EQ(3, centerlines.size());

  centerlines.SplitByBranchPoints();

  EXPECT_EQ(5, centerlines.size());
  EXPECT_EQ(2, centerlines[0].GetNumPoints());
  EXPECT_EQ(3, centerlines[1].GetNumPoints());
  EXPECT_EQ(3, centerlines[2].GetNumPoints());
  EXPECT_EQ(3, centerlines[3].GetNumPoints());
  EXPECT_EQ(3, centerlines[4].GetNumPoints());

  EXPECT_TRUE(test_utils::PointsEqual3d(centerlines[0][0].Point().Point(), creator.GetPoint(9)));
  EXPECT_TRUE(test_utils::PointsEqual3d(centerlines[0][1].Point().Point(), creator.GetPoint(8)));

  EXPECT_TRUE(test_utils::PointsEqual3d(centerlines[1][0].Point().Point(), creator.GetPoint(4)));
  EXPECT_TRUE(test_utils::PointsEqual3d(centerlines[1][1].Point().Point(), creator.GetPoint(3)));
  EXPECT_TRUE(test_utils::PointsEqual3d(centerlines[1][2].Point().Point(), creator.GetPoint(2)));

  EXPECT_TRUE(test_utils::PointsEqual3d(centerlines[2][0].Point().Point(), creator.GetPoint(7)));
  EXPECT_TRUE(test_utils::PointsEqual3d(centerlines[2][1].Point().Point(), creator.GetPoint(6)));
  EXPECT_TRUE(test_utils::PointsEqual3d(centerlines[2][2].Point().Point(), creator.GetPoint(8)));

  EXPECT_TRUE(test_utils::PointsEqual3d(centerlines[3][0].Point().Point(), creator.GetPoint(8)));
  EXPECT_TRUE(test_utils::PointsEqual3d(centerlines[3][1].Point().Point(), creator.GetPoint(5)));
  EXPECT_TRUE(test_utils::PointsEqual3d(centerlines[3][2].Point().Point(), creator.GetPoint(2)));

  EXPECT_TRUE(test_utils::PointsEqual3d(centerlines[4][0].Point().Point(), creator.GetPoint(2)));
  EXPECT_TRUE(test_utils::PointsEqual3d(centerlines[4][1].Point().Point(), creator.GetPoint(1)));
  EXPECT_TRUE(test_utils::PointsEqual3d(centerlines[4][2].Point().Point(), creator.GetPoint(0)));

  creator.TearDown();
}

TEST(Centerline, ShouldNotMissACenterline) {
  CenterLineCreator creator;
  auto paths = creator.BuildTwoPathsWithOneBranchPoint();
  set_type centerlines {};
  centerlines.AddSet(paths.first, creator.GetVertexKey(4));
  centerlines.AddSet(paths.first, creator.GetVertexKey(7));
  centerlines.AddSet(paths.second, creator.GetVertexKey(7));
  EXPECT_EQ(3, centerlines.size());

  EXPECT_EQ(5, centerlines[0].GetNumPoints());
  EXPECT_EQ(4, centerlines[1].GetNumPoints());
  EXPECT_EQ(3, centerlines[2].GetNumPoints());

  EXPECT_TRUE(test_utils::PointsEqual3d(centerlines[0][0].Point().Point(), creator.GetPoint(4)));
  EXPECT_TRUE(test_utils::PointsEqual3d(centerlines[0][1].Point().Point(), creator.GetPoint(3)));
  EXPECT_TRUE(test_utils::PointsEqual3d(centerlines[0][2].Point().Point(), creator.GetPoint(2)));
  EXPECT_TRUE(test_utils::PointsEqual3d(centerlines[0][3].Point().Point(), creator.GetPoint(1)));
  EXPECT_TRUE(test_utils::PointsEqual3d(centerlines[0][4].Point().Point(), creator.GetPoint(0)));

  EXPECT_TRUE(test_utils::PointsEqual3d(centerlines[1][0].Point().Point(), creator.GetPoint(7)));
  EXPECT_TRUE(test_utils::PointsEqual3d(centerlines[1][1].Point().Point(), creator.GetPoint(6)));
  EXPECT_TRUE(test_utils::PointsEqual3d(centerlines[1][2].Point().Point(), creator.GetPoint(5)));
  EXPECT_TRUE(test_utils::PointsEqual3d(centerlines[1][3].Point().Point(), creator.GetPoint(2)));

  EXPECT_TRUE(test_utils::PointsEqual3d(centerlines[2][0].Point().Point(), creator.GetPoint(2)));
  EXPECT_TRUE(test_utils::PointsEqual3d(centerlines[2][1].Point().Point(), creator.GetPoint(8)));
  EXPECT_TRUE(test_utils::PointsEqual3d(centerlines[2][2].Point().Point(), creator.GetPoint(9)));

  centerlines.SplitByBranchPoints();

  EXPECT_FALSE(centerlines.IsBranch(creator.GetVertexKey(0)));
  EXPECT_FALSE(centerlines.IsBranch(creator.GetVertexKey(1)));
  EXPECT_TRUE(centerlines.IsBranch(creator.GetVertexKey(2)));
  EXPECT_FALSE(centerlines.IsBranch(creator.GetVertexKey(3)));
  EXPECT_FALSE(centerlines.IsBranch(creator.GetVertexKey(4)));
  EXPECT_FALSE(centerlines.IsBranch(creator.GetVertexKey(5)));
  EXPECT_FALSE(centerlines.IsBranch(creator.GetVertexKey(6)));
  EXPECT_FALSE(centerlines.IsBranch(creator.GetVertexKey(7)));
  EXPECT_FALSE(centerlines.IsBranch(creator.GetVertexKey(8)));
  EXPECT_FALSE(centerlines.IsBranch(creator.GetVertexKey(9)));

  EXPECT_EQ(4, centerlines.size());
  EXPECT_EQ(3, centerlines[0].GetNumPoints());
  EXPECT_EQ(4, centerlines[1].GetNumPoints());
  EXPECT_EQ(3, centerlines[2].GetNumPoints());
  EXPECT_EQ(3, centerlines[3].GetNumPoints());


  EXPECT_TRUE(test_utils::PointsEqual3d(centerlines[0][0].Point().Point(), creator.GetPoint(4)));
  EXPECT_TRUE(test_utils::PointsEqual3d(centerlines[0][1].Point().Point(), creator.GetPoint(3)));
  EXPECT_TRUE(test_utils::PointsEqual3d(centerlines[0][2].Point().Point(), creator.GetPoint(2)));

  EXPECT_TRUE(test_utils::PointsEqual3d(centerlines[1][0].Point().Point(), creator.GetPoint(7)));
  EXPECT_TRUE(test_utils::PointsEqual3d(centerlines[1][1].Point().Point(), creator.GetPoint(6)));
  EXPECT_TRUE(test_utils::PointsEqual3d(centerlines[1][2].Point().Point(), creator.GetPoint(5)));
  EXPECT_TRUE(test_utils::PointsEqual3d(centerlines[1][3].Point().Point(), creator.GetPoint(2)));

  EXPECT_TRUE(test_utils::PointsEqual3d(centerlines[2][0].Point().Point(), creator.GetPoint(2)));
  EXPECT_TRUE(test_utils::PointsEqual3d(centerlines[2][1].Point().Point(), creator.GetPoint(8)));
  EXPECT_TRUE(test_utils::PointsEqual3d(centerlines[2][2].Point().Point(), creator.GetPoint(9)));

  EXPECT_TRUE(test_utils::PointsEqual3d(centerlines[3][0].Point().Point(), creator.GetPoint(2)));
  EXPECT_TRUE(test_utils::PointsEqual3d(centerlines[3][1].Point().Point(), creator.GetPoint(1)));
  EXPECT_TRUE(test_utils::PointsEqual3d(centerlines[3][2].Point().Point(), creator.GetPoint(0)));

  creator.TearDown();
}

TEST(Centerline, ShouldNotMissACenterlineWithTwoBranchPoints) {
  CenterLineCreator creator;
  auto paths = creator.BuildThreePathsWithTwoBranchPoints();
  set_type centerlines {};
  centerlines.AddSet(paths[0], creator.GetVertexKey(4));
  centerlines.AddSet(paths[1], creator.GetVertexKey(6));
  centerlines.AddSet(paths[2], creator.GetVertexKey(6));

  EXPECT_EQ(4, centerlines.size());

  EXPECT_EQ(5, centerlines[0].GetNumPoints());
  EXPECT_EQ(3, centerlines[1].GetNumPoints());
  EXPECT_EQ(2, centerlines[2].GetNumPoints());
  EXPECT_EQ(2, centerlines[3].GetNumPoints());

  EXPECT_TRUE(test_utils::PointsEqual3d(centerlines[0][0].Point().Point(), creator.GetPoint(4)));
  EXPECT_TRUE(test_utils::PointsEqual3d(centerlines[0][1].Point().Point(), creator.GetPoint(3)));
  EXPECT_TRUE(test_utils::PointsEqual3d(centerlines[0][2].Point().Point(), creator.GetPoint(2)));
  EXPECT_TRUE(test_utils::PointsEqual3d(centerlines[0][3].Point().Point(), creator.GetPoint(1)));
  EXPECT_TRUE(test_utils::PointsEqual3d(centerlines[0][4].Point().Point(), creator.GetPoint(0)));

  EXPECT_TRUE(test_utils::PointsEqual3d(centerlines[1][0].Point().Point(), creator.GetPoint(6)));
  EXPECT_TRUE(test_utils::PointsEqual3d(centerlines[1][1].Point().Point(), creator.GetPoint(5)));
  EXPECT_TRUE(test_utils::PointsEqual3d(centerlines[1][2].Point().Point(), creator.GetPoint(3)));

  EXPECT_TRUE(test_utils::PointsEqual3d(centerlines[2][0].Point().Point(), creator.GetPoint(1)));
  EXPECT_TRUE(test_utils::PointsEqual3d(centerlines[2][1].Point().Point(), creator.GetPoint(8)));

  EXPECT_TRUE(test_utils::PointsEqual3d(centerlines[3][0].Point().Point(), creator.GetPoint(6)));
  EXPECT_TRUE(test_utils::PointsEqual3d(centerlines[3][1].Point().Point(), creator.GetPoint(7)));

  centerlines.SplitByBranchPoints();

  EXPECT_FALSE(centerlines.IsBranch(creator.GetVertexKey(0)));
  EXPECT_TRUE(centerlines.IsBranch(creator.GetVertexKey(1)));
  EXPECT_FALSE(centerlines.IsBranch(creator.GetVertexKey(2)));
  EXPECT_TRUE(centerlines.IsBranch(creator.GetVertexKey(3)));
  EXPECT_FALSE(centerlines.IsBranch(creator.GetVertexKey(4)));
  EXPECT_FALSE(centerlines.IsBranch(creator.GetVertexKey(5)));
  EXPECT_TRUE(centerlines.IsBranch(creator.GetVertexKey(6)));
  EXPECT_FALSE(centerlines.IsBranch(creator.GetVertexKey(7)));
  EXPECT_FALSE(centerlines.IsBranch(creator.GetVertexKey(8)));

  EXPECT_EQ(6, centerlines.size());
  EXPECT_EQ(2, centerlines[0].GetNumPoints());
  EXPECT_EQ(3, centerlines[1].GetNumPoints());
  EXPECT_EQ(2, centerlines[2].GetNumPoints());
  EXPECT_EQ(2, centerlines[3].GetNumPoints());
  EXPECT_EQ(3, centerlines[4].GetNumPoints());
  EXPECT_EQ(2, centerlines[5].GetNumPoints());


  EXPECT_TRUE(test_utils::PointsEqual3d(centerlines[0][0].Point().Point(), creator.GetPoint(4)));
  EXPECT_TRUE(test_utils::PointsEqual3d(centerlines[0][1].Point().Point(), creator.GetPoint(3)));

  EXPECT_TRUE(test_utils::PointsEqual3d(centerlines[1][0].Point().Point(), creator.GetPoint(6)));
  EXPECT_TRUE(test_utils::PointsEqual3d(centerlines[1][1].Point().Point(), creator.GetPoint(5)));
  EXPECT_TRUE(test_utils::PointsEqual3d(centerlines[1][2].Point().Point(), creator.GetPoint(3)));

  EXPECT_TRUE(test_utils::PointsEqual3d(centerlines[2][0].Point().Point(), creator.GetPoint(1)));
  EXPECT_TRUE(test_utils::PointsEqual3d(centerlines[2][1].Point().Point(), creator.GetPoint(8)));

  EXPECT_TRUE(test_utils::PointsEqual3d(centerlines[3][0].Point().Point(), creator.GetPoint(6)));
  EXPECT_TRUE(test_utils::PointsEqual3d(centerlines[3][1].Point().Point(), creator.GetPoint(7)));

  EXPECT_TRUE(test_utils::PointsEqual3d(centerlines[4][0].Point().Point(), creator.GetPoint(3)));
  EXPECT_TRUE(test_utils::PointsEqual3d(centerlines[4][1].Point().Point(), creator.GetPoint(2)));
  EXPECT_TRUE(test_utils::PointsEqual3d(centerlines[4][2].Point().Point(), creator.GetPoint(1)));

  EXPECT_TRUE(test_utils::PointsEqual3d(centerlines[5][0].Point().Point(), creator.GetPoint(1)));
  EXPECT_TRUE(test_utils::PointsEqual3d(centerlines[5][1].Point().Point(), creator.GetPoint(0)));

  creator.TearDown();
}

TEST(Centerline, ShouldBuildSimpleCenterline) {
  CenterLineCreator creator;
  auto path = creator.BuildSimplePath();
  set_type centerlines {};
  centerlines.AddSet(path, creator.GetVertexKey(0));

  EXPECT_EQ(1, centerlines.size());
  EXPECT_EQ(1, centerlines[0].GetNumPoints());
  EXPECT_FALSE(centerlines.IsBranch(creator.GetVertexKey(0)));
}

TEST(Centerline, ShouldNotBuildCenterline) {
  CenterLineCreator creator;
  auto path = creator.BuildSimplePath();
  set_type centerlines {};
  centerlines.AddSet(path, creator.GetVertexKey(1));

  EXPECT_EQ(0, centerlines.size());
}

TEST(Centerline, ShouldNotBuildCenterlineForInfinityDistance) {
  CenterLineCreator creator;
  auto path = creator.BuildPathWithInfinityDistance();
  auto annotation = path[creator.GetVertexKey(0)];
  set_type centerlines {};
  centerlines.AddSet(path, creator.GetVertexKey(0));

  EXPECT_FALSE(annotation.HasFiniteDistance());

  EXPECT_EQ(0, centerlines.size());
}


TEST(Centerline, ShouldNotChangeBeginIterator) {
  CenterLineCreator creator;
  auto paths = creator.BuildTwoPathsWithOneBranchPoint();
  set_type centerlines {};
  centerlines.AddSet(paths.first, creator.GetVertexKey(4));
  centerlines.AddSet(paths.first, creator.GetVertexKey(7));
  centerlines.AddSet(paths.second, creator.GetVertexKey(7));

  auto iterator_first = centerlines.begin();
  auto iterator_second = centerlines.begin();
  iterator_first++;
  iterator_second++;

  EXPECT_EQ(iterator_first, iterator_second);
}

TEST(Centerline, ShouldNotChangeEndIterator) {
  CenterLineCreator creator;
  auto paths = creator.BuildTwoPathsWithOneBranchPoint();
  set_type centerlines {};
  centerlines.AddSet(paths.first, creator.GetVertexKey(4));
  centerlines.AddSet(paths.first, creator.GetVertexKey(7));
  centerlines.AddSet(paths.second, creator.GetVertexKey(7));

  auto iterator_first = centerlines.end();
  auto iterator_second = centerlines.end();
  iterator_first--;
  iterator_second--;

  EXPECT_EQ(iterator_first, iterator_second);
}

TEST(Centerline, ShouldComputeStatistics) {
  CenterLineCreator creator;
  auto paths = creator.BuildTwoPathsWithOneBranchPoint();
  set_type centerlines {};
  centerlines.AddSet(paths.first, creator.GetVertexKey(4));
  centerlines.AddSet(paths.first, creator.GetVertexKey(7));
  centerlines.AddSet(paths.second, creator.GetVertexKey(7));

  auto statistics = centerlines.Statistics();

  std::vector<double> sizes;
  std::vector<double> tortousities;
  std::vector<double> averages;
  for (const auto &stat : statistics) {
    sizes.push_back(stat.Size());
    tortousities.push_back(stat.Tortuosity());
    averages.push_back(stat.AveragePropertyValue());
  }
  std::vector<double> expected_sizes;
  expected_sizes.push_back(4);
  expected_sizes.push_back(3);
  expected_sizes.push_back(3);

  std::vector<double> expected_tortousities;
  expected_tortousities.push_back(0.414213562);
  expected_tortousities.push_back(0.732050808);
  expected_tortousities.push_back(0.341640786);

  std::vector<double> expected_averages;
  expected_averages.push_back(1.676466469);
  expected_averages.push_back(2.413929747);
  expected_averages.push_back(2.631442823);

  EXPECT_EQ(3, sizes.size());

  EXPECT_TRUE(std::is_permutation(expected_sizes.cbegin(),
                                  expected_sizes.cend(),
                                  sizes.cbegin(),
                                  test_utils::DoubleEqual));
  EXPECT_TRUE(std::is_permutation(expected_tortousities.cbegin(),
                                  expected_tortousities.cend(),
                                  tortousities.cbegin(),
                                  test_utils::DoubleEqual));
  EXPECT_TRUE(std::is_permutation(expected_averages.cbegin(),
                                  expected_averages.cend(),
                                  averages.cbegin(),
                                  test_utils::DoubleEqual));
}

TEST(Centerline, ShouldComputeStatisticsAfterSplit) {
  CenterLineCreator creator;
  auto paths = creator.BuildTwoPathsWithOneBranchPoint();
  set_type centerlines {};
  centerlines.AddSet(paths.first, creator.GetVertexKey(4));
  centerlines.AddSet(paths.first, creator.GetVertexKey(7));
  centerlines.AddSet(paths.second, creator.GetVertexKey(7));

  centerlines.SplitByBranchPoints();

  auto statistics = centerlines.Statistics();

  std::vector<double> sizes;
  std::vector<double> tortousities;
  std::vector<double> averages;
  for (const auto &stat : statistics) {
    sizes.push_back(stat.Size());
    tortousities.push_back(stat.Tortuosity());
    averages.push_back(stat.AveragePropertyValue());
  }
  std::vector<double> expected_sizes;
  expected_sizes.push_back(2);
  expected_sizes.push_back(3);
  expected_sizes.push_back(3);
  expected_sizes.push_back(2);

  std::vector<double> expected_tortousities;
  expected_tortousities.push_back(0.414213562);
  expected_tortousities.push_back(0.732050808);
  expected_tortousities.push_back(0.341640786);
  expected_tortousities.push_back(0.414213562);

  std::vector<double> expected_averages;
  expected_averages.push_back(1.989372928);
  expected_averages.push_back(2.413929747);
  expected_averages.push_back(2.631442823);
  expected_averages.push_back(1.382088123);

  EXPECT_EQ(4, sizes.size());

  EXPECT_TRUE(std::is_permutation(expected_sizes.cbegin(),
                                  expected_sizes.cend(),
                                  sizes.cbegin(),
                                  test_utils::DoubleEqual));
  EXPECT_TRUE(std::is_permutation(expected_tortousities.cbegin(),
                                  expected_tortousities.cend(),
                                  tortousities.cbegin(),
                                  test_utils::DoubleEqual));
  EXPECT_TRUE(std::is_permutation(expected_averages.cbegin(),
                                  expected_averages.cend(),
                                  averages.cbegin(),
                                  test_utils::DoubleEqual));
}

}  // namespace skeleton_test

#endif  // TEST_SRC_SKELETON_CENTERLINE_TEST_H_
