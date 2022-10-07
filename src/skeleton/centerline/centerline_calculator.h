/**
 * \file src/skeleton/centerline/centerline_calculator.h
 * \brief Implements a generic wrapper for the centerline computation.
 *
 * \author Alexandre Ashade Lassance Cunha \<aashade@br.ibm.com\>
 * \author Giulia Duncan Coutinho \<coutingi@br.ibm.com\>
 * \author Rodrigo Alves Prado da Silva \<rodrigo.alves@ibm.com\>
 * \copyright © IBM Corp.
 * \date 2016
 */

#ifndef SRC_SKELETON_CENTERLINE_CENTERLINE_CALCULATOR_H_
#define SRC_SKELETON_CENTERLINE_CENTERLINE_CALCULATOR_H_

#include <glog/logging.h>
#include <cstdint>
#include <algorithm>
#include <vector>
#include <string>
#include "src/skeleton/centerline/centerline_set.h"

namespace skeleton {

/**
 *  \class CenterlineCalculator centerline_calculator.h "src/skeleton/centerline/centerline_calculator.h"
 *  \brief Main class to calculate centerlines.
 *
 *  @tparam SingleSourceMinPathAlgorithm The single source shortest path
 *  algorithm implementation.
 *  @tparam Skeletonizer The skeleton transform algorithm implementation.
 */
template<class SingleSourceMinPathAlgorithm,
         class Skeletonizer,
         class EndpointsDiscoverer,
         class Gradient,
         class ClustersDiscoverer>
class CenterlineCalculator {
 public:
  /**
   * \brief The type used to represent direction or dimension index.
   */
  using direction_type = uint8_t;

  /**
   * \brief Alias to the templated type Skeletonizer.
   */
  using skeletonizer_type = Skeletonizer;

  /**
   * \brief Alias to the image type used in Skeletonizer.
   */
  using image_type = typename skeletonizer_type::image_type;

  /**
   * \brief Alias to the annotated image type used in Skeletonizer.
   */
  using annotated_image_type = typename skeletonizer_type::annotated_image_type;

  /**
   * \brief Alias to the contour calculator type used in Skeletonizer.
   */
  using contour_calculator = typename skeletonizer_type::contour_calculator_type;

  /**
   * \brief Alias to the path distance calculator type used in Skeletonizer.
   */
  using path_distance_calculator = typename skeletonizer_type::path_calculator_type;

  /**
   * \brief Alias to the template argument SingleSourceMinPathAlgorithm.
   */
  using algorithm_type = SingleSourceMinPathAlgorithm;

  /**
   * \brief Alias to the vertices candidate list type in SingleSourceMinPathAlgorithm.
   */
  using end_vertices_candidate_list_type =
  typename algorithm_type::end_vertices_candidate_list_type;

  /**
   * \brief Alias to the graph type in SingleSourceMinPathAlgorithm.
   */
  using graph_type = typename algorithm_type::graph_type;

  /**
   * \brief Alias to the vertex type in graph_type.
   */
  using vertex_type = typename graph_type::vertex_type;

  /**
   * \brief Alias to the point + annotation, which is the Key of vertex_type.
   */
  using point_and_annotation_type = typename vertex_type::key_type;

  /**
   * \brief Alias to the point type in the annotated_image_type.
   */
  using point_type = typename annotated_image_type::point_type;

  /**
   * \brief Alias to the annotation type in the annotated_image_type.
   */
  using annotation_type = typename annotated_image_type::annotation_type;

  /**
   * \brief Alias to the label value type in the annotation_type.
   */
  using label_type = typename annotation_type::label_value;

  /**
   * \brief Alias to the path weight calculator type in the algorithm_type.
   */
  using weight_calculator_type = typename algorithm_type::weight_calculator_type;

  /**
   * \brief Alias to the distance type in the weight_calculator_type.
   */
  using distance_type = typename weight_calculator_type::distance_type;

  /**
   * \brief Alias to the type used to represent image sizes.
   */
  using size_type = typename point_type::coordinate;

  /**
   * \brief Alias to the coordinate value type in the point_type.
   */
  using coordinate_type = typename point_type::coordinate;

  /**
   * \brief Alias to the distance type in the weight_calculator_type.
   */
  using coordinate_index_type = typename point_type::coordinates_index_type;

  /**
   * \brief Auxiliary type. Used to discover sources and sinks of centerlines.
   */
  using endpoints_discoverer = EndpointsDiscoverer;

  /**
   * \brief Auxiliary type. Used to discover clusters of maximal vertices.
   */
  using clusters_discoverer_type = ClustersDiscoverer;

  using set_type = CenterlineSet<graph_type, path_distance_calculator>;

 private:
  static constexpr uint16_t dimensions = point_type::Dimensions;
  const std::array<size_type, dimensions> sizes_;

  /**
   * \brief This method computes the source (a.k.a. inlet) points of the face
   * where the centerlines should run from. It does so by computing the
   * centers of each distinct pore of the face.
   *
   * \param num_coords The total number of dimensions of the original image.
   * Typically, it equals 3.
   *
   * \param discoverer An instance to a class that is able to find center
   * points of the pores of a plane, which is typically a face.
   *
   * \return A std::vector whose elements are the source points.
   */
  std::vector<point_and_annotation_type> GetSourcePoints(
      direction_type num_coords, endpoints_discoverer &discoverer) const {
    std::vector<point_and_annotation_type> result;
    for (direction_type coord_index = 0; coord_index != num_coords;
         ++coord_index) {
      auto points = discoverer.GetPoints(coord_index, 0);
      result.insert(std::end(result), std::begin(points), std::end(points));
    }

    return result;
  }

  /**
   * \brief This method computes the sink (a.k.a. outlet) points of the face
   * where the centerlines should arrive at.
   *
   * \sa GetSourcePoints
   */
  std::vector<point_and_annotation_type> GetEndPoints(
      direction_type num_coords, endpoints_discoverer &discoverer) const {
    std::vector<point_and_annotation_type> result;
    for (direction_type coord_index = 0; coord_index != num_coords;
         ++coord_index) {
      auto points = discoverer.GetPoints(coord_index, 0);
      result.insert(std::end(result), std::begin(points), std::end(points));
      auto points_opposite =
          discoverer.GetPoints(coord_index, sizes_[coord_index] -
              1);
      result.insert(std::end(result), std::begin(points_opposite), std::end(points_opposite));
    }

    return result;
  }

  bool ExecuteSingleComputation(
      algorithm_type &algorithm,
      const vertex_type &source_vertex,
      set_type &centerlines,
      std::vector<point_and_annotation_type> &end_points,
      Gradient &gradient_calculator,
      const graph_type &graph) const {
    auto executed = algorithm.ExecuteGradient(source_vertex, gradient_calculator);

    // Log progress of centerline extraction algorithm
    LOG(INFO) << "Algorithm done";

    if (!executed)
      return false;
    auto const &result = algorithm.GetResult();
    for (const auto &end_point : end_points)
      centerlines.AddSet(result, graph, end_point);

    auto const &end_vertices = algorithm.GetEndCandidatesList();
    for (const auto &pair : end_vertices) {
      centerlines.AddSet(result, graph, pair.first, pair.second);
    }
    return true;
  }

  direction_type GetDirection(const point_and_annotation_type &source_point) const {
    auto point = source_point.Point();
    for (std::size_t index = 0; index != point.GetNumberOfCoordinates(); ++index) {
      auto value = point.GetCoordinate(index);
      if (value == sizes_[index]-1 || value == 0)
        return index;
    }
    throw std::exception{};
  }

  std::vector<point_and_annotation_type> FilterEnds(
      const point_and_annotation_type &source,
      const std::vector<point_and_annotation_type> &ends) const {
    std::vector<point_and_annotation_type> output;
    auto source_point = source.Point();
    auto source_direction = GetDirection(source);
    for (const auto end : ends) {
      auto end_point = end.Point();
      if (end_point.GetCoordinate(source_direction) !=
          source_point.GetCoordinate(source_direction))
        output.push_back(end);
    }
    return output;
  }

  set_type
  ComputeForSources(const std::vector<point_and_annotation_type> &source_points,
                    std::vector<point_and_annotation_type> &end_points,
                    algorithm_type &algorithm,
                    Gradient &gradient_calculator,
                    const graph_type &graph) const {
    // Log progress of centerline extraction algorithm
    LOG(INFO) << "Start of centerlines extraction.";

    set_type centerlines{};
    for (const auto &source_point : source_points) {
      auto executed = ExecuteSingleComputation(
          algorithm,
          vertex_type{source_point},
          centerlines,
          end_points,
          gradient_calculator,
          graph);
      if (executed)
        break;
    }

    // Log progress of centerline extraction algorithm
    LOG(INFO) << "End of centerlines extraction.";

    centerlines.SplitByBranchPoints();
    return centerlines;
  }

  set_type ExecuteComputeCenterlines(
      const graph_type &graph,
      const image_type &original_image,
      const annotated_image_type &annotated_image,
      algorithm_type &algorithm) {
    endpoints_discoverer discoverer{annotated_image, original_image};
    auto source_points = GetSourcePoints(
      static_cast<direction_type>(original_image.GetNumberOfDimensions()), discoverer);
    auto end_points = GetEndPoints(
      static_cast<direction_type>(original_image.GetNumberOfDimensions()), discoverer);

    Gradient gradient_calculator{original_image, annotated_image};
    auto centerlines = ComputeForSources(source_points,
                                         end_points,
                                         algorithm,
                                         gradient_calculator,
                                         graph);
    return centerlines;
  }

 public:
  /**
   * \brief Constructs an instance of this class.
   *
   * \param sizes The sizes of each dimension of the image to compute
   * centerlines.
   */
  explicit CenterlineCalculator(const std::array<size_type, dimensions> sizes)
      : sizes_(sizes) {
  }

  /**
   * \brief This method generates centerlines for an image represented by a
   * graph.
   *
   * \details This method computes centerlines with ends defined on two
   * opposite faces of the image. Currently, it does not support computing
   * centerlines between two non-parallel faces.
   *
   * This method tries to compute one single centerline for each pair of
   * in / out holes of the given faces. IMPORTANT: this method is still experimental.
   * Computation is still slow and sequential.
   *
   * \param graph A graph representation of the image. Typically constructed
   * using the class GraphBuilder.
   *
   * \param original_image The original image, used to determine
   * in / out points, which represent the ends of the centerlines.
   *
   * \param annotated_image The image annotated with the distance map.
   *
   * \return An instance of CenterlineSet.
   */
  set_type ComputeCenterlines(graph_type &graph,
                              const image_type &original_image,
                              const annotated_image_type &annotated_image) {
    clusters_discoverer_type clusters_discoverer{graph};

    // Log progress of centerline extraction algorithm
    LOG(INFO) << "Cluster discovering...";
    clusters_discoverer.Execute();
    LOG(INFO) << "Done!";

    algorithm_type algorithm{graph, clusters_discoverer};
    auto centerlines = ComputeCenterlines(graph, original_image,
                                          annotated_image, algorithm);
    return centerlines;
  }

  set_type ComputeCenterlines(const graph_type &graph,
                              const image_type &original_image,
                              const annotated_image_type &annotated_image,
                              algorithm_type &algorithm) {
    auto centerlines = ExecuteComputeCenterlines(graph, original_image,
                                                 annotated_image, algorithm);
    return centerlines;
  }
};
}  // namespace skeleton

#endif  // SRC_SKELETON_CENTERLINE_CENTERLINE_CALCULATOR_H_
