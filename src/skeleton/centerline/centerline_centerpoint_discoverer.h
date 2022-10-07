/**
 * \file src/skeleton/centerline/centerline_centerpoint_discoverer.h
 * \brief This file implements the logic to find in / out points of the
 * centerlines.
 *
 * \author Alexandre Ashade Lassance Cunha \<aashade@br.ibm.com\>
 * \author Giulia Duncan Coutinho \<coutingi@br.ibm.com\>
 * \copyright © IBM Corp.
 * \date 2016
 */

#ifndef SRC_SKELETON_CENTERLINE_CENTERLINE_CENTERPOINT_DISCOVERER_H_
#define SRC_SKELETON_CENTERLINE_CENTERLINE_CENTERPOINT_DISCOVERER_H_

#include <vector>
#include <cstdint>
#include <unordered_set>
#include <queue>

#include "src/skeleton/utils/comparer.h"
#include "src/skeleton/utils/hasher.h"

namespace skeleton {

template<class AnnotatedImage, class NeighbourCalculator, class Vertex, class
Image>
class CenterpointDiscoverer {
 public:
  using annot_img_type = AnnotatedImage;
  using image_type = Image;
  using neighbour_calculator_type = NeighbourCalculator;

  using neighbours_list_type = typename NeighbourCalculator::neighbours_type;

  /**
   * \brief Alias to the point + annotation, which is the vertex_type.
   */
  using point_and_annotation_type = Vertex;

  /**
   * \brief Alias to the point type.
   */
  using point_type = typename annot_img_type::point_type;

  /**
   * \brief Alias to the type of the annotated property that each vertex has.
   * Typically, this is a distance value.
   */
  using point_property_type = typename
  annot_img_type::annotation_type::distance_value;

  /**
   * \brief Alias to the coordinate value type in the point_type.
   */
  using coordinate_type = typename point_type::coordinate;

  /**
   * \brief The type used to represent direction or dimension index.
   */
  using direction_type = typename neighbour_calculator_type::direction_type;

 private:
  std::unordered_set<point_type, Hasher < point_type>, Comparer <point_type>>
  used_points_;
  const annot_img_type &annotated_image_;
  neighbour_calculator_type neighbour_calc_;
  const image_type &image_;

  bool HasUsed(const point_type &point) const {
    return used_points_.find(point) != used_points_.end();
  }

  void MarkUsed(const point_type &point) {
    used_points_.insert(point);
  }

  void MarkAllUnused() {
    used_points_.clear();
  }

  const point_property_type GetProperty(const point_type &point) const {
    return annotated_image_.ReadAnnotation(point).distance;
  }

  void UpdateMaxAndEnqueueNeighbours(point_property_type &current_max,
                                     point_type &current_center,
                                     const neighbours_list_type &neighbours,
                                     std::queue<point_type> &queue) {
    for (const auto &neighbour : neighbours) {
      if (!image_.IsObjectPoint(neighbour)
          || !annotated_image_.HasAnnotation(neighbour)
          || HasUsed(neighbour))
        continue;
      queue.push(neighbour);
      auto candidate_property = GetProperty(neighbour);
      if (candidate_property > current_max) {
        current_center = neighbour;
        current_max = candidate_property;
      }
    }
  }

  point_and_annotation_type ComputeBoundedCenter(
      const point_type &point,
      const point_property_type &property,
      const direction_type direction) {
    auto current_max = property;
    auto current_center = point;
    std::queue<point_type> queue;
    queue.push(point);
    while (!queue.empty()) {
      auto next_point = queue.front();
      queue.pop();
      if (HasUsed(next_point))
        continue;
      MarkUsed(next_point);
      if (!image_.IsObjectPoint(point))
        continue;
      auto neighbours = neighbour_calc_.GetNeighboursInPlane(next_point,
                                                             direction);
      UpdateMaxAndEnqueueNeighbours(current_max,
                                    current_center,
                                    neighbours,
                                    queue);
    }
    return point_and_annotation_type {current_center,
                                      static_cast<double>(current_max)};
  }

  bool IsInPlane(const point_type &point,
                 const direction_type direction,
                 const coordinate_type coordinate) const {
    return point.GetCoordinate(direction) == coordinate;
  }

 public:
  explicit CenterpointDiscoverer(const annot_img_type &graph, const
  image_type &image)
      : annotated_image_(graph), image_(image) {
  }

  std::vector<point_and_annotation_type> GetPoints(
      const direction_type direction,
      const coordinate_type coordinate) {
    MarkAllUnused();
    std::vector<point_and_annotation_type> output;
    for (const auto &pt_and_annot : annotated_image_) {
      const auto &point = pt_and_annot.first;
      if (!image_.IsObjectPoint(point)
          || !IsInPlane(point, direction, coordinate)
          || HasUsed(point))
        continue;
      auto property = pt_and_annot.second.distance;
      auto new_center = ComputeBoundedCenter(point, property, direction);
      output.push_back(new_center);
    }
    return output;
  }
};

}  // namespace skeleton

#endif  // SRC_SKELETON_CENTERLINE_CENTERLINE_CENTERPOINT_DISCOVERER_H_
