/**
 * \file src/skeleton/points/point_builder_base.h
 * \brief This file implements a helper to create a point.
 *
 * \author Alexandre Ashade Lassance Cunha \<aashade@br.ibm.com\>
 * \author Rodrigo Neumann Barros Ferreira \<rneumann@br.ibm.com\>
 * \copyright © IBM Corp.
 * \date 2016
 */

#ifndef SRC_SKELETON_POINTS_POINT_BUILDER_BASE_H_
#define SRC_SKELETON_POINTS_POINT_BUILDER_BASE_H_

#include "src/skeleton/points/point.h"

namespace skeleton {

template<class TCoordinate>
class PointBuilderBase {
 public:
  using point2_type = Point<2, TCoordinate>;
  using point3_type = Point<3, TCoordinate>;
  using point2_coordinate_type = typename point2_type::coordinate;
  using point3_coordinate_type = typename point3_type::coordinate;
  using point2_coordinates_type = typename point2_type::Coordinates;
  using point3_coordinates_type = typename point3_type::Coordinates;

  static point2_type Get(point2_coordinate_type x,
                         point2_coordinate_type y) {
    point2_coordinates_type output{{x, y}};
    return point2_type(output);
  }

  static point3_type Get(point3_coordinate_type x,
                         point3_coordinate_type y,
                         point3_coordinate_type z) {
    point3_coordinates_type output{{x, y, z}};
    return point3_type(output);
  }

  point2_type operator ()(point2_coordinate_type x,
                          point2_coordinate_type y) const {
    return Get(x, y);
  }

  point3_type operator ()(point3_coordinate_type x,
                          point3_coordinate_type y,
                          point3_coordinate_type z) const {
    return Get(x, y, z);
  }
};

}  // namespace skeleton

#endif  // SRC_SKELETON_POINTS_POINT_BUILDER_BASE_H_
