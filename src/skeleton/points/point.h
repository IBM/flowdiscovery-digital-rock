/**
 * \file src/skeleton/points/point.h
 * \brief This file implements a data structure to represent a general point
 * in N dimensions.
 *
 * \author Alexandre Ashade Lassance Cunha \<aashade@br.ibm.com\>
 * \author Giulia Duncan Coutinho \<coutingi@br.ibm.com\>
 * \copyright © IBM Corp.
 * \date 2015
 */

#ifndef SRC_SKELETON_POINTS_POINT_H_
#define SRC_SKELETON_POINTS_POINT_H_

#include <cstdint>
#include <vector>
#include <array>
#include <algorithm>
#include <initializer_list>
#include "src/skeleton/utils/hasher.h"
#include "src/skeleton/utils/comparer.h"

namespace skeleton {

template<uint16_t dimensions, class TCoordinate = uint16_t>
class Point {
 public:
  using coordinate = TCoordinate;
  using my_type = Point<dimensions, coordinate>;
  using Points = std::vector<my_type>;
  using Coordinates = std::array<coordinate, dimensions>;
  using coordinates_type = Coordinates;
  using coordinates_index_type = typename Coordinates::size_type;

  static constexpr uint16_t Dimensions = dimensions;

 private:
  Coordinates values_;

 public:
  Point(const Coordinates &values)  // NOLINT
      : values_(values) {
  }
  Point(const Point &other)
      : values_(other.values_) {
  }
  template<class TOther>
  explicit Point(const Point<dimensions, TOther> &other)
      : values_() {
    for (coordinates_index_type index = 0; index != dimensions; ++index)
      values_[index] = static_cast<coordinate>(other.GetCoordinate(index));
  }
  Point()
      : values_() /* zero-initialised */ {
  }

  bool IsFaceNeighbour(const Point &other) const {
    coordinate diff = 0;
    for (coordinates_index_type i = 0; i != Dimensions; ++i) {
      coordinate prev_diff = diff;
      if (GetCoordinate(i) > other.GetCoordinate(i))
        diff += GetCoordinate(i) - other.GetCoordinate(i);
      else
        diff += other.GetCoordinate(i) - GetCoordinate(i);
      if (diff - prev_diff > 1) return false;
    }
    return diff == 1;
  }

  bool IsVertexNeighbour(const Point &other) const {
    coordinate diff = 0;
    for (coordinates_index_type i = 0; i != Dimensions; ++i) {
      coordinate prev_diff = diff;
      if (GetCoordinate(i) > other.GetCoordinate(i))
        diff += GetCoordinate(i) - other.GetCoordinate(i);
      else
        diff += other.GetCoordinate(i) - GetCoordinate(i);
      if (diff - prev_diff > 1) return false;
    }
    return diff == 3;
  }

  bool IsEdgeNeighbour(const Point &other) const {
    coordinate diff = 0;
    for (coordinates_index_type i = 0; i != Dimensions; ++i) {
      coordinate prev_diff = diff;
      if (GetCoordinate(i) > other.GetCoordinate(i))
        diff += GetCoordinate(i) - other.GetCoordinate(i);
      else
        diff += other.GetCoordinate(i) - GetCoordinate(i);
      if (diff - prev_diff > 1) return false;
    }
    return diff == 2;
  }

  bool IsNeighbour(const Point &other) const {
    coordinate diff = 0;
    for (coordinates_index_type i = 0; i != Dimensions; ++i) {
      coordinate prev_diff = diff;
      if (GetCoordinate(i) > other.GetCoordinate(i))
        diff += GetCoordinate(i) - other.GetCoordinate(i);
      else
        diff += other.GetCoordinate(i) - GetCoordinate(i);
      if (diff - prev_diff > 1) return false;
    }
    return diff <= 3 && diff >= 1;
  }

  coordinate GetCoordinate(coordinates_index_type index) const {
    return values_[index];
  }
  void SetCoordinate(coordinates_index_type index, coordinate value) {
    values_[index] = value;
  }
  constexpr coordinates_index_type GetNumberOfCoordinates() const noexcept {
    return dimensions;
  }
  inline bool operator==(const my_type &other) const noexcept {
    return std::equal(values_.begin(), values_.end(), other.values_.begin());
  }
  inline bool operator!=(const my_type &other) const noexcept {
    return !std::equal(values_.begin(), values_.end(), other.values_.begin());
  }
  inline bool operator<(const my_type &other) const noexcept {
    for (coordinates_index_type index = 0; index != dimensions; ++index) {
      if (values_[index] < other.values_[index])
        return true;
      else if (values_[index] > other.values_[index])
        return false;
    }
    return false;
  }
  inline bool operator>(const my_type &other) const noexcept {
    for (auto index = 0; index != dimensions; ++index) {
      if (values_[index] > other.values_[index])
        return true;
      else if (values_[index] < other.values_[index])
        return false;
    }
    return false;
  }
  my_type &operator=(const my_type &other) {
    values_ = other.values_;
    return *this;
  }
  my_type operator+(const my_type &other) const noexcept {
    Coordinates new_values;
    for (auto index = 0; index != dimensions; ++index)
      new_values[index] = values_[index] + other.values_[index];
    return my_type {new_values};
  }
  my_type operator*(const double scalar) const noexcept {
    Coordinates new_values;
    for (auto index = 0; index != dimensions; ++index)
      new_values[index] = values_[index] * scalar;
    return my_type {new_values};
  }
  my_type operator/(const double scalar) const noexcept {
    return operator*(1.0 / scalar);
  }
  my_type operator-(const my_type &other) const noexcept {
    return operator+(other.operator*(-1.0));
  }
};

template<uint16_t dimensions, class TCoordinate>
class Hasher<Point<dimensions, TCoordinate>> {
 private:
  using point_type = Point<dimensions, TCoordinate>;
  std::size_t GetHash(const point_type &point) const noexcept {
    const std::size_t prime = 919;
    std::hash<uint16_t> integer_hasher;
    std::size_t hash = 0;
    for (typename point_type::coordinates_index_type index = 0;
         index != point.GetNumberOfCoordinates(); ++index) {
      hash = hash * prime + integer_hasher(point.GetCoordinate(index));
    }
    return hash;
  }
 public:
  std::size_t operator()(const point_type &object) const noexcept {
    return GetHash(object);
  }
};

template<uint16_t dimensions, class TCoordinate>
class Comparer<Point<dimensions, TCoordinate>> {
 public:
  using T = Point<dimensions, TCoordinate>;
  bool operator()(const T &x, const T &y) const noexcept {
    return x == y;
  }
  typedef T first_argument_type;
  typedef T second_argument_type;
  typedef bool result_type;
};

}  // namespace skeleton

#endif  // SRC_SKELETON_POINTS_POINT_H_
