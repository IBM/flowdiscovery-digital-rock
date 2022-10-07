/**
 * \file src/skeleton/images/image.h
 * \brief This file implements a data structure to represent an image.
 *
 * \author Alexandre Ashade Lassance Cunha \<aashade@br.ibm.com\>
 * \author Giulia Duncan Coutinho \<coutingi@br.ibm.com\>
 * \copyright © IBM Corp.
 * \date 2015
 */

#ifndef SRC_SKELETON_IMAGES_IMAGE_H_
#define SRC_SKELETON_IMAGES_IMAGE_H_

#include <cstdint>
#include <utility>
#include <unordered_set>
#include <array>
#include "src/skeleton/points/point.h"
#include "src/skeleton/utils/hasher.h"
#include "src/skeleton/utils/comparer.h"
#include "src/skeleton/images/image_iterator.h"

namespace skeleton {

template<uint16_t dimensions>
struct image_traits {
  using PointType = Point<dimensions>;
  using size = int_fast16_t;
  using ObjectPoints = std::unordered_set<PointType, Hasher<PointType>,
                                          Comparer<PointType>>;
  using inner_iterator_type_ = typename ObjectPoints::const_iterator;
  using iterator = ImageIterator<
      image_traits<dimensions>::inner_iterator_type_>;
};

template<int dimensions,
    class ConstIterator = typename image_traits<dimensions>::iterator>
class Image {
 public:
  using PointType = typename image_traits<dimensions>::PointType;
  using size = typename image_traits<dimensions>::size;
  using ObjectPoints = typename image_traits<dimensions>::ObjectPoints;
  using const_iterator = ConstIterator;
  using point_type = PointType;
  using coordinates_index_type = int_fast8_t;

  Image(const ObjectPoints points, std::array<size, static_cast<std::size_t>(dimensions)> sizes)
      : points_(std::move(points)),
        sizes_(sizes) {
  }
  bool IsOffLimitsPoint(const point_type &point) const noexcept {
    for (std::size_t i = 0; i != point.GetNumberOfCoordinates(); ++i) {
      auto coordinate = point.GetCoordinate(i);
      if (coordinate < 0 || coordinate >= sizes_[i])
        return true;
    }
    return false;
  }
  bool IsLimitPoint(const point_type &point) const noexcept {
    for (std::size_t i = 0; i != point.GetNumberOfCoordinates(); ++i) {
      auto coordinate = point.GetCoordinate(i);
      if (coordinate == 0 || coordinate == sizes_[i] - 1)
        return true;
    }
    return false;
  }
  bool IsObjectPoint(const PointType &point) const noexcept {
    return points_.find(point) != points_.cend();
  }
  uint_fast16_t GetNumberOfDimensions() const noexcept {
    return static_cast<uint_fast16_t>(dimensions);
  }
  bool IsOffLimitsCoordinate(const point_type &point,
                             coordinates_index_type index) const noexcept {
    throw std::exception {};
  }
  bool IsContourPoint(const point_type &point) const {
    throw std::exception{};
  }
  size GetDimensionSize(uint_fast16_t dimension_index) const noexcept {
    if (dimension_index < dimensions)
      return sizes_[static_cast<std::size_t>(dimension_index)];
    return 0;
  }
  size GetAverageDimensionsSize() const noexcept {
    size sum = 0;
    for (const auto &dimension_size : sizes_) {
      sum += dimension_size;
    }
    return sum / GetNumberOfDimensions();
  }
  const_iterator cbegin() const noexcept {
    return const_iterator {points_.cbegin()};
  }
  const_iterator cend() const noexcept {
    return const_iterator {points_.cend()};
  }
  const_iterator begin() const noexcept {
    return const_iterator {points_.cbegin()};
  }
  const_iterator end() const noexcept {
    return const_iterator {points_.cend()};
  }

  bool IsPointWithinBounds(const PointType &point) const noexcept {
    for (auto index = 0; index != GetNumberOfDimensions(); ++index) {
      if (point.GetCoordinate(index) < 0
          || point.GetCoordinate(index) >= GetDimensionSize(index))
        return false;
    }
    return true;
  }

 private:
  const ObjectPoints points_;
  const std::array<size, static_cast<std::size_t>(dimensions)> sizes_;
};

using Image2D = Image<2>;

} /* namespace skeleton */

#endif  // SRC_SKELETON_IMAGES_IMAGE_H_
