/**
 * \file src/skeleton/centerline/vertex_neighbour_calculator.h
 * \brief This file implements a wrapper to the point neighbour calculator.
 *
 * \author Alexandre Ashade Lassance Cunha \<aashade@br.ibm.com\>
 * \copyright © IBM Corp.
 * \date 2016
 */

#ifndef SRC_SKELETON_CENTERLINE_VERTEX_NEIGHBOUR_CALCULATOR_H_
#define SRC_SKELETON_CENTERLINE_VERTEX_NEIGHBOUR_CALCULATOR_H_

#include <vector>

namespace skeleton {

template<class Vertex, class PointNeighbourCalculator, class AnnotatedImage>
class VertexNeighbourCalculator {
 public:
  using vertex_type = Vertex;
  using point_calculator_type = PointNeighbourCalculator;
  using neighbours_type = std::vector<vertex_type>;

 private:
  const point_calculator_type point_calculator_;
  using annotated_image_type = AnnotatedImage;

  const annotated_image_type &image_;
  using point_type = typename vertex_type::point_type;
  using property_type = typename vertex_type::point_property_type;

 public:
  explicit VertexNeighbourCalculator(const annotated_image_type &image)
      : point_calculator_(),
        image_(image) {
  }

  VertexNeighbourCalculator(const VertexNeighbourCalculator &other)
      : point_calculator_(other.point_calculator_),
        image_(other.image_) {
  }

  neighbours_type GetNeighbours(const vertex_type &vertex) const noexcept {
    auto pt_neighbours = point_calculator_.GetNeighbours(vertex.Point());
    neighbours_type neighbours;
    for (const auto &pt_neighbour : pt_neighbours) {
      if (!image_.HasAnnotation(pt_neighbour))
        continue;
      auto annotation = image_.ReadAnnotation(pt_neighbour);
      auto distance = static_cast<property_type>(annotation.distance);
      neighbours.push_back(vertex_type {pt_neighbour, distance});
    }
    return neighbours;
  }
};
}  // namespace skeleton

#endif  // SRC_SKELETON_CENTERLINE_VERTEX_NEIGHBOUR_CALCULATOR_H_
