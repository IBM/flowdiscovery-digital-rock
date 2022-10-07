/**
 * \file src/skeleton/neighbours/twenty_six_neighbour_calculator.h
 * \brief This file implements a helper to calculate the 26 neighbours of a
 * voxel.
 *
 * \author Alexandre Ashade Lassance Cunha \<aashade@br.ibm.com\>
 * \author Giulia Duncan Coutinho \<coutingi@br.ibm.com\>
 * \copyright © IBM Corp.
 * \date 2015
 */

#ifndef SRC_SKELETON_NEIGHBOURS_TWENTY_SIX_NEIGHBOUR_CALCULATOR_H_
#define SRC_SKELETON_NEIGHBOURS_TWENTY_SIX_NEIGHBOUR_CALCULATOR_H_

#include <vector>
#include <cstdlib>

namespace skeleton {

template<class Point, class PointBuilder>
class TwentySixNeighbourCalculator {
 public:
  using neighbours_type = std::vector<Point>;
  using direction_type = uint8_t;

 private:
  const PointBuilder build_point_;
  using _coordinate_type = typename Point::coordinate;

  void PushPoint(neighbours_type &neighbours, const Point &point,
                 _coordinate_type dx, _coordinate_type dy,
                 _coordinate_type dz) const {
    auto point_x = point.GetCoordinate(0);
    auto x = static_cast<_coordinate_type>(dx + point_x - 1);

    auto point_y = point.GetCoordinate(1);
    auto y = static_cast<_coordinate_type>(dy + point_y - 1);

    auto point_z = point.GetCoordinate(2);
    auto z = static_cast<_coordinate_type>(dz + point_z - 1);

    neighbours.push_back(build_point_(x, y, z));
  }

 public:
  TwentySixNeighbourCalculator()
      : build_point_() {
    static_assert(Point::Dimensions == 3, "Expected 3D point.");
  }

  const neighbours_type GetNeighbours(const Point &point) const noexcept {
    neighbours_type neighbours;
    for (_coordinate_type dx = 0; dx != 3; ++dx)
      for (_coordinate_type dy = 0; dy != 3; ++dy)
        for (_coordinate_type dz = 0; dz != 3; ++dz) {
          if (abs(dx - 1) == 1 || abs(dy - 1) == 1 || abs(dz - 1) == 1)
            PushPoint(neighbours, point, dx, dy, dz);
        }
    return neighbours;
  }

  const neighbours_type GetNeighboursInPlane(
      const Point &point,
      const direction_type direction) const noexcept {
    _coordinate_type dx_max = 2, dy_max = 2, dz_max = 2, dx_min = 0, dy_min = 0,
        dz_min = 0;
    if (direction == 0) {
      dx_max = 1;
      dx_min = 1;
    } else if (direction == 1) {
      dy_max = 1;
      dy_min = 1;
    } else if (direction == 2) {
      dz_max = 1;
      dz_min = 1;
    }
    neighbours_type neighbours;
    for (_coordinate_type dx = dx_min; dx <= dx_max; ++dx)
      for (_coordinate_type dy = dy_min; dy <= dy_max; ++dy)
        for (_coordinate_type dz = dz_min; dz <= dz_max; ++dz) {
          if (abs(dx - 1) == 1 || abs(dy - 1) == 1 || abs(dz - 1) == 1)
            PushPoint(neighbours, point, dx, dy, dz);
        }
    return neighbours;
  }
};

}  // namespace skeleton

#endif  // SRC_SKELETON_NEIGHBOURS_TWENTY_SIX_NEIGHBOUR_CALCULATOR_H_
