/**
 * \file src/skeleton/centerline/vertex.h
 * \brief This file implements the data structure that represents a graph
 * vertex that could have an attached property.
 *
 * \author Alexandre Ashade Lassance Cunha \<aashade@br.ibm.com\>
 * \copyright © IBM Corp.
 * \date 2016
 */

#ifndef SRC_SKELETON_CENTERLINE_VERTEX_H_
#define SRC_SKELETON_CENTERLINE_VERTEX_H_

#include "src/skeleton/utils/hasher.h"

namespace skeleton {

template<class TPoint, class PointProperty, class PointHasher = Hasher<TPoint>>
class Vertex {
 public:
  using point_type = TPoint;
  using point_property_type = PointProperty;
  using point_hasher_type = PointHasher;

 private:
  point_type point_;
  point_property_type property_value_;

 public:
  Vertex(const point_type& point, const point_property_type& property)
      : point_(point),
        property_value_(property) {
  }
  Vertex()
      : point_(),
        property_value_() {
  }
  Vertex(const Vertex&) = default;
  Vertex& operator =(const Vertex& other) {
    point_ = other.point_;
    property_value_ = other.property_value_;
    return *this;
  }
  bool operator ==(const Vertex& other) const noexcept {
    return point_ == other.point_;
  }

  std::size_t GetHash() const {
    point_hasher_type hasher;
    return hasher(point_);
  }

  const point_type& Point() const noexcept {
    return point_;
  }

  const point_property_type& PropertyValue() const noexcept {
    return property_value_;
  }
};
}  // namespace skeleton

#endif  // SRC_SKELETON_CENTERLINE_VERTEX_H_
