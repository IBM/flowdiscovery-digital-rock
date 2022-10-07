/**
 * \file src/skeleton/paths/quadratic_path_calculator.h
 * \brief This file implements an eclidean path distance calculator.
 *
 * \author Alexandre Ashade Lassance Cunha \<aashade@br.ibm.com\>
 * \author Giulia Duncan Coutinho \<coutingi@br.ibm.com\>
 * \copyright © IBM Corp.
 * \date 2015
 */

#ifndef SRC_SKELETON_PATHS_QUADRATIC_PATH_CALCULATOR_H_
#define SRC_SKELETON_PATHS_QUADRATIC_PATH_CALCULATOR_H_

#include <cstdlib>
#include <cmath>
#include <limits>

namespace skeleton {

template<class Annotation, class Point>
class QuadraticPathCalculator {
 private:
  using displacement_type = typename Annotation::displacement_value;
  using index_type = typename Point::coordinates_index_type;

 public:
  using distance_value = typename Annotation::distance_value;
  using distance_type = typename Annotation::distance_value;
  using real_distance_type = double;

  static distance_type InfiniteDistance() noexcept {
    return std::numeric_limits<distance_type>::max();
  }
  static real_distance_type InfiniteRealDistance() noexcept {
    return std::numeric_limits<real_distance_type>::max();
  }
  void UpdatePointPathCost(const Point &origin,
                           const Annotation &origin_annotation,
                           const Point &neighbour,
                           Annotation &neighbour_annotation) const noexcept {
    distance_value new_distance = 0;
    for (index_type index = 0; index != origin.GetNumberOfCoordinates();
         ++index) {
      auto annot_index = static_cast<typename
      Annotation::displacements_index_type>(index);
      auto distance = origin_annotation.GetAccumulatedDistance(annot_index)
          + abs(neighbour.GetCoordinate(index) - origin.GetCoordinate(index));
      neighbour_annotation.SetAccumulatedDistance(
          static_cast<typename Annotation::displacements_index_type>(index),
          static_cast<displacement_type>(distance));
      new_distance += static_cast<distance_value>(distance * distance);
    }
    neighbour_annotation.SetDistanceValue(new_distance);
  }

  typename Annotation::distance_value GetConcatenatedPathCost(
      const Point &origin, const Annotation &origin_annotation,
      const Point &neighbour) const noexcept {
    distance_value new_distance = 0;
    for (index_type index = 0; index != origin.GetNumberOfCoordinates();
         ++index) {
      auto annot_index = static_cast<typename
      Annotation::displacements_index_type>(index);
      auto distance = origin_annotation.GetAccumulatedDistance(annot_index)
          + abs(neighbour.GetCoordinate(index) - origin.GetCoordinate(index));
      new_distance += static_cast<distance_value>(distance * distance);
    }
    return new_distance;
  }

  int64_t GetSquaredDistance(const Point &point_first,
                             const Point &point_second) const {
    int64_t squared_distance = 0;
    for (index_type index = 0; index != Point::Dimensions; ++index) {
      auto distance = point_first.GetCoordinate(index)
          - point_second.GetCoordinate(index);
      squared_distance += distance * distance;
    }
    return squared_distance;
  }

  real_distance_type GetWeight(const Point &point_first,
                               const Point &point_second) const {
    auto squared_distance = GetSquaredDistance(point_first, point_second);
    return std::sqrt(static_cast<double>(squared_distance));
  }
};

}  // namespace skeleton

#endif  // SRC_SKELETON_PATHS_QUADRATIC_PATH_CALCULATOR_H_
