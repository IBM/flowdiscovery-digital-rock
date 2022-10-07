/**
 * \file src/skeleton/neighbours/four_neighbour_calculator.h
 * \brief This file implements a helper to calculate the 4 neighbours of a
 * pixel.
 *
 * \author Alexandre Ashade Lassance Cunha \<aashade@br.ibm.com\>
 * \author Giulia Duncan Coutinho \<coutingi@br.ibm.com\>
 * \copyright © IBM Corp.
 * \date 2015
 */

#ifndef SRC_SKELETON_NEIGHBOURS_FOUR_NEIGHBOUR_CALCULATOR_H_
#define SRC_SKELETON_NEIGHBOURS_FOUR_NEIGHBOUR_CALCULATOR_H_

#include <vector>
#include <cstdlib>

namespace skeleton {

template<class Point, class PointBuilder>
class FourNeighbourCalculator {
 private:
  const PointBuilder build_point_;
 public:
  using neighbours_type = std::vector<Point>;

  FourNeighbourCalculator()
      : build_point_() {
    static_assert(Point::Dimensions == 2, "Expected bidimensional point.");
  }
  const neighbours_type GetNeighbours(const Point& pixel) const noexcept {
    neighbours_type neighbours;
    using _coordinate_type = typename Point::coordinate;
    for (_coordinate_type dx = 0; dx != 3; ++dx)
      for (_coordinate_type dy = 0; dy != 3; ++dy) {
        if (dx + dy != 1 && dx + dy != 3)
          continue;
        auto x = static_cast<_coordinate_type>(dx + pixel.GetCoordinate(0) - 1);
        auto y = static_cast<_coordinate_type>(dy + pixel.GetCoordinate(1) - 1);
        neighbours.push_back(build_point_(x, y));
      }
    return neighbours;
  }
};

}  // namespace skeleton

#endif  // SRC_SKELETON_NEIGHBOURS_FOUR_NEIGHBOUR_CALCULATOR_H_
