/**
 * \file src/skeleton/centerline/weight_calculator.h
 * \brief This file implements the formula to calculate the cost from vertex
 * A to vertex B.
 *
 * \author Alexandre Ashade Lassance Cunha \<aashade@br.ibm.com\>
 * \author Giulia Duncan Coutinho \<coutingi@br.ibm.com\>
 * \copyright © IBM Corp.
 * \date 2016
 */

#ifndef SRC_SKELETON_CENTERLINE_WEIGHT_CALCULATOR_H_
#define SRC_SKELETON_CENTERLINE_WEIGHT_CALCULATOR_H_

#include <limits>

namespace skeleton {

template<class Vertex, class PointDistanceCalculator>
class WeightCalculator {
 public:
  using point_distance_calculator_type = PointDistanceCalculator;
  using distance_type =
  typename point_distance_calculator_type::real_distance_type;

  const point_distance_calculator_type calculator_;
  WeightCalculator()
      : calculator_() {
  }
  distance_type GetWeight(const Vertex&, const Vertex& second) const {
    auto second_property = static_cast<distance_type>(second.PropertyValue());
    return 1.0 / (1.0 + second_property);
  }
  static distance_type InfiniteDistance() {
    return std::numeric_limits<distance_type>::max();
  }
};
}  // namespace skeleton

#endif  // SRC_SKELETON_CENTERLINE_WEIGHT_CALCULATOR_H_
