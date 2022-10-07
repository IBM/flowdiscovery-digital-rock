/**
 * \file src/skeleton/neighbours/eight_neighbour_calculator.h
 * \brief This file implements a helper to calculate the 8 neighbours of a
 * pixel.
 *
 * \author Hugo Barbalho \<hugob@br.ibm.com\>
 * \copyright © IBM Corp.
 * \date 2017
 */

#ifndef SRC_SKELETON_NEIGHBOURS_EIGHT_NEIGHBOUR_CALCULATOR_H_
#define SRC_SKELETON_NEIGHBOURS_EIGHT_NEIGHBOUR_CALCULATOR_H_

#include <vector>
#include <cstdlib>

namespace skeleton {

template<class Point, class PointBuilder>
class EightNeighbourCalculator {
 private:
  const PointBuilder build_point_;
 public:
  using neighbours_type = std::vector<Point>;

  EightNeighbourCalculator()
      : build_point_() {
    static_assert(Point::Dimensions == 2, "Expected bidimensional point.");
  }
  const neighbours_type GetNeighbours(const Point& pixel) const noexcept {
    neighbours_type neighbours;
    using _coordinate_type = typename Point::coordinate;
    for (_coordinate_type dx = 0; dx != 3; ++dx)
      for (_coordinate_type dy = 0; dy != 3; ++dy) {
        if (dx == 1 && dy == 1) continue;
        auto x = static_cast<_coordinate_type>(dx + pixel.GetCoordinate(0) - 1);
        auto y = static_cast<_coordinate_type>(dy + pixel.GetCoordinate(1) - 1);
        neighbours.push_back(build_point_(x, y));
      }
    return neighbours;
  }
};

}  // namespace skeleton

#endif  // SRC_SKELETON_NEIGHBOURS_EIGHT_NEIGHBOUR_CALCULATOR_H_
