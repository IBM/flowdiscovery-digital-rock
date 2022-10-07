/**
 * \file src/skeleton/neighbours/six_neighbour_calculator.h
 * \brief This file implements a helper to calculate the 6 neighbours of a
 * voxel.
 *
 * \author Alexandre Ashade Lassance Cunha \<aashade@br.ibm.com\>
 * \author Giulia Duncan Coutinho \<coutingi@br.ibm.com\>
 * \copyright © IBM Corp.
 * \date 2015
 */

#ifndef SRC_SKELETON_NEIGHBOURS_SIX_NEIGHBOUR_CALCULATOR_H_
#define SRC_SKELETON_NEIGHBOURS_SIX_NEIGHBOUR_CALCULATOR_H_

#include <vector>
#include <cstdlib>

namespace skeleton {

template<class Point, class PointBuilder>
class SixNeighbourCalculator {
 public:
  using neighbours_type = std::vector<Point>;

 private:
  const PointBuilder build_point_;
  using _coordinate_type = typename Point::coordinate;

  void PushPoint(neighbours_type& neighbours, const Point& point,
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
  SixNeighbourCalculator()
      : build_point_() {
    static_assert(Point::Dimensions == 3, "Expected 3D point.");
  }
  const neighbours_type GetNeighbours(const Point& point) const noexcept {
    neighbours_type neighbours;
    for (_coordinate_type dx = 0; dx != 3; ++dx)
      for (_coordinate_type dy = 0; dy != 3; ++dy)
        for (_coordinate_type dz = 0; dz != 3; ++dz) {
          if (abs(dx - 1) + abs(dy - 1) + abs(dz - 1) != 1)
            continue;
          PushPoint(neighbours, point, dx, dy, dz);
        }
    return neighbours;
  }
};

}  // namespace skeleton

#endif  // SRC_SKELETON_NEIGHBOURS_SIX_NEIGHBOUR_CALCULATOR_H_
