/**
 * \file test/src/skeleton/dijkstra_test.h
 *
 * \author Hugo de Oliveira Barbalho \<hugob@br.ibm.com\>
 * \copyright © IBM Corp.
 * \date 2017
 */

#ifndef TEST_SRC_SKELETON_DIJKSTRA_TEST_H_
#define TEST_SRC_SKELETON_DIJKSTRA_TEST_H_

#include <gtest/gtest.h>
#include <vector>
#include <array>
#include <limits>
#include <memory>
#include <utility>

#include "test/src/utils/test_utils.h"
#include "test/src/utils/geometry_utils.h"
#include "src/skeleton/centerline/centerline.h"
#include "src/skeleton/points/point.h"
#include "src/skeleton/images/annotated_image.h"
#include "src/skeleton/graph/annotated_vertex_helper.h"
#include "src/skeleton/neighbours/twenty_six_neighbour_calculator.h"
#include "src/skeleton/centerline/vertex_neighbour_calculator.h"
#include "src/skeleton/centerline/vertex.h"
#include "src/skeleton/graph/memory_graph.h"
#include "src/skeleton/graph/dijkstra.h"
#include "src/skeleton/images/arma_3d_binary_image_builder.h"
#include "src/skeleton/centerline/memory_graph_builder.h"
#include "src/skeleton/contours/contour_calculator.h"
#include "src/skeleton/images/integer_image.h"
#include "src/skeleton/centerline/centerline_set.h"
#include "src/skeleton/skeletonizer_by_ift.h"
#include "src/skeleton/centerline/weight_calculator.h"
#include "src/skeleton/centerline/gradient.h"
#include "src/skeleton/heaps/handle_based_binary_heap.h"
#include "src/skeleton/centerline/centerline_centerpoint_discoverer.h"
#include "src/skeleton/centerline/centerline_calculator.h"
#include "src/skeleton/centerline/maximal_clusters_discoverer.h"

namespace skeleton_test {

using image3d = skeleton::BinaryImage<skeleton::Point<3>>;
using point_type = skeleton::Point<3>;
using gradient_point3d_type = skeleton::Point<3, double>;
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

using weight_calculator_type = skeleton::WeightCalculator<point_and_distance,
                                                          path_distance_calculator>;

using gradient_type = skeleton::Gradient<image3d,
                                         annotated_image3d,
                                         neighbour_calculator3d,
                                         skeleton::PointBuilder,
                                         gradient_point3d_type>;

using label_type = int64_t;

using clusters_discoverer_type = skeleton::MaximalClustersDiscoverer<graph_type>;

using dijkstra_type = skeleton::Dijkstra<graph_type,
                                         graph_type,
                                         path_distance_calculator,
                                         weight_calculator_type,
                                         gradient_type,
                                         clusters_discoverer_type,
                                         skeleton::HandleBasedBinaryHeap>;

using end_points_discoverer_type = skeleton::CenterpointDiscoverer<annotated_image3d,
                                                                   neighbour_calculator3d,
                                                                   point_and_distance,
                                                                   image3d>;

using centerline_calculator_type = skeleton::CenterlineCalculator<dijkstra_type,
                                                                  skeletonizer3d,
                                                                  end_points_discoverer_type,
                                                                  gradient_type,
                                                                  clusters_discoverer_type>;

class DijkstraTest : public ::testing::Test {
 protected:
  image3d *image_;
  skeletonizer3d *skeletonizer_;
  graph_type *graph_;
  constexpr static image3d::size n_rows = 20;
  constexpr static image3d::size n_cols = 20;
  constexpr static image3d::size n_slices = 20;
  constexpr static image3d::array sizes = {n_rows, n_cols, n_slices};
  std::vector<std::vector<point_and_distance>> image_slice;

  DijkstraTest() {
    image_ = NULL;
    skeletonizer_ = NULL;
  }

  virtual ~DijkstraTest() {
    if (image_)
      delete image_;
    if (skeletonizer_)
      delete skeletonizer_;
  }

  virtual void SetUp() {
    std::vector<bool> points =
    geometry_utils::FileUtils::ReadRawImage("test/src/skeleton/input/binary_image.raw",
                                            n_rows,
                                            n_cols,
                                            n_slices,
                                            0);
    image_ = new image3d(points, sizes);
    auto image_ptr = std::make_unique<image3d>(*image_);
    skeletonizer_ = new skeletonizer3d(std::move(image_ptr));

    skeletonizer_->ComputeSkeleton();
    graph_builder_type graph_builder{};
    graph_ = new graph_type(graph_builder.Build(skeletonizer_->GetAnnotations(),
                                                skeletonizer_->GetImage()));
    image_slice.resize(20, std::vector<point_and_distance>(20));

    for (const auto &pair : *graph_) {
      point_and_distance vertex_key = pair.first;
      if (vertex_key.Point().GetCoordinate(0) == 10)
        image_slice[vertex_key.Point().GetCoordinate(1)]
                  [vertex_key.Point().GetCoordinate(2)] = vertex_key;
    }
  }

  virtual void TearDown() {}
};

TEST_F(DijkstraTest, Should_Skeletonize_And_Set_LocalMaximal_Vertices) {
  EXPECT_EQ(10, image_slice[0][15].PropertyValue());
  EXPECT_EQ(5, image_slice[1][16].PropertyValue());
  EXPECT_EQ(10, image_slice[4][15].PropertyValue());
  EXPECT_EQ(11, image_slice[5][15].PropertyValue());
  EXPECT_TRUE(graph_->IsLocalMaximalVertex(image_slice[0][15]));
  EXPECT_FALSE(graph_->IsLocalMaximalVertex(image_slice[1][16]));
  EXPECT_FALSE(graph_->IsLocalMaximalVertex(image_slice[4][15]));
  EXPECT_TRUE(graph_->IsLocalMaximalVertex(image_slice[5][15]));
}

TEST_F(DijkstraTest, Should_Compute_Centerline) {
  clusters_discoverer_type clusters_discoverer{*graph_};
  centerline_calculator_type calculator{image_->Sizes()};
  dijkstra_type algorithm{*graph_,
                          clusters_discoverer};
  auto centerlines =
        calculator.ComputeCenterlines(*graph_,
                                      skeletonizer_->GetImage(),
                                      skeletonizer_->GetAnnotations(),
                                      algorithm);
}

TEST_F(DijkstraTest, Should_Discover_Clusters) {
  clusters_discoverer_type clusters_discoverer{*graph_};
  clusters_discoverer.Execute();
  EXPECT_GE(clusters_discoverer.GetClusterLabel(image_slice[0][15]), 0);
  EXPECT_GE(clusters_discoverer.GetClusterLabel(image_slice[1][15]), 0);
  EXPECT_GE(clusters_discoverer.GetClusterLabel(image_slice[2][15]), 0);
  EXPECT_GE(clusters_discoverer.GetClusterLabel(image_slice[3][15]), 0);
  EXPECT_GE(clusters_discoverer.GetClusterLabel(image_slice[5][15]), 0);
  EXPECT_GE(clusters_discoverer.GetClusterLabel(image_slice[15][15]), 0);

  EXPECT_EQ(clusters_discoverer.GetClusterLabel(image_slice[4][15]), -1);
  EXPECT_EQ(clusters_discoverer.GetClusterLabel(image_slice[0][14]), -1);

  EXPECT_EQ(clusters_discoverer.GetClusterLabel(image_slice[0][15]),
            clusters_discoverer.GetClusterLabel(image_slice[1][15]));
  EXPECT_EQ(clusters_discoverer.GetClusterLabel(image_slice[1][15]),
            clusters_discoverer.GetClusterLabel(image_slice[2][15]));
  EXPECT_EQ(clusters_discoverer.GetClusterLabel(image_slice[2][15]),
            clusters_discoverer.GetClusterLabel(image_slice[3][15]));

  EXPECT_NE(clusters_discoverer.GetClusterLabel(image_slice[3][15]),
            clusters_discoverer.GetClusterLabel(image_slice[5][15]));
  EXPECT_NE(clusters_discoverer.GetClusterLabel(image_slice[3][15]),
            clusters_discoverer.GetClusterLabel(image_slice[15][15]));
  EXPECT_NE(clusters_discoverer.GetClusterLabel(image_slice[5][15]),
            clusters_discoverer.GetClusterLabel(image_slice[15][15]));
}

TEST_F(DijkstraTest, Should_Compute_Path_Labels) {
  clusters_discoverer_type clusters_discoverer{*graph_};
  clusters_discoverer.Execute();

  centerline_calculator_type calculator{image_->Sizes()};
  dijkstra_type algorithm{*graph_,
                          clusters_discoverer};

  calculator.ComputeCenterlines(*graph_,
                                skeletonizer_->GetImage(),
                                skeletonizer_->GetAnnotations(),
                                algorithm);

  auto centerlines = algorithm.GetResult();
  auto pred_14_15 = centerlines[image_slice[14][15]].Predecessor();

  label_type path_label = algorithm.GetLabelPath(image_slice[15][15]);
  label_type vertex_label = clusters_discoverer.GetClusterLabel(image_slice[15][15]);

  EXPECT_GE(vertex_label, 0);
  EXPECT_GE(algorithm.GetLabelPath(pred_14_15), 0);
  EXPECT_EQ(clusters_discoverer.GetClusterLabel(image_slice[14][15]), -1);

  EXPECT_GE(path_label, vertex_label);
  EXPECT_EQ(algorithm.GetLabelPath(image_slice[15][15]),
            path_label);
  EXPECT_EQ(algorithm.GetLabelPath(image_slice[14][15]),
            algorithm.GetLabelPath(pred_14_15));
}

TEST_F(DijkstraTest, Should_Compute_Cycle_Connections) {
  clusters_discoverer_type clusters_discoverer{*graph_};
  clusters_discoverer.Execute();

  centerline_calculator_type calculator{image_->Sizes()};
  dijkstra_type algorithm{*graph_,
                          clusters_discoverer};

  calculator.ComputeCenterlines(*graph_,
                                skeletonizer_->GetImage(),
                                skeletonizer_->GetAnnotations(),
                                algorithm);

  auto end_list = algorithm.GetEndCandidatesList();
  EXPECT_GT(end_list.size(), 0);
  int64_t find = 0;
  for (const auto &pair : end_list) {
    if (pair.first.Point() == image_slice[15][13].Point() &&
       pair.second.Point() == image_slice[15][14].Point())
       find++;
  }
  EXPECT_EQ(find, 1);
}

}  // namespace skeleton_test

#endif  // TEST_SRC_SKELETON_DIJKSTRA_TEST_H_
