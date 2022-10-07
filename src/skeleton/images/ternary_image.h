/**
 * \file src/skeleton/images/ternary_image.h
 * \brief This file implements a data structure to represente a ternary
 * image, that is able to distinguish between objects, non-objects and contours.
 *
 * \author Alexandre Ashade Lassance Cunha \<aashade@br.ibm.com\>
 * \author Giulia Duncan Coutinho \<coutingi@br.ibm.com\>
 * \copyright © IBM Corp.
 * \date 2016
 */

#ifndef SRC_SKELETON_IMAGES_TERNARY_IMAGE_H_
#define SRC_SKELETON_IMAGES_TERNARY_IMAGE_H_

#include <armadillo>
#include <cstdint>
#include <iterator>
#include <array>
#include <limits>
#include <vector>

#include "src/skeleton/images/image_iterator.h"

namespace skeleton {

template<class Point>
class TernaryImage {
 public:
  using point_type = Point;
  using my_type = TernaryImage<point_type>;
  using size = typename point_type::coordinate;
  using coordinates_index_type = typename point_type::coordinates_index_type;
  using coordinate_type = typename point_type::coordinate;
  using ObjectPoints = arma::Cube<uint8_t>;
  using array = std::array<size, Point::Dimensions>;

  class Iterator {
   private:
    array sizes_;
    array currents_;
    using array_size_type = typename array::size_type;

    bool IsEnd() const noexcept {
      return currents_[sizes_.size() - 1] >= sizes_[sizes_.size() - 1];
    }

   public:
    using iterator_category = std::forward_iterator_tag;
    using value_type = Point;
    using difference_type = std::ptrdiff_t;
    using pointer = value_type const *;
    using reference = value_type const;  // not a reference!
    explicit Iterator(const array &sizes)
        : sizes_(sizes),
          currents_() {
      currents_.fill(0);  // iterator to begin
    }
    Iterator()
        : sizes_(),
          currents_() {
      currents_.fill(std::numeric_limits<size>::max());  // past end iterator
    }
    Iterator(const Iterator &other)
        : sizes_(other.sizes_),
          currents_(other.currents_) {
    }
    Iterator &operator=(const Iterator &other) noexcept {
      sizes_ = other.sizes_;
      currents_ = other.currents_;
      return *this;
    }
    bool operator==(const Iterator &other) const noexcept {
      return (sizes_ == other.sizes_ && currents_ == other.currents_)
          || IsEnd();
    }
    bool operator!=(const Iterator &other) const noexcept {
      return !(operator==(other));
    }
    reference operator*() const {
      return Point {currents_};
    }
    Iterator &operator++() {  // prefix increment (++x)
      typename array::size_type index = 0;
      currents_[0]++;
      while (index < sizes_.size() - 1 && currents_[index] >= sizes_[index]) {
        currents_[index] = 0;
        currents_[index + 1]++;
        index = index + 1;
      }
      return *this;
    }
    Iterator operator++(int) {  // postfix increment (x++)
      Iterator old{*this};
      operator++();
      return old;
    }
  };

  using const_iterator = Iterator;

 private:
  static const bool bit_0_ = false;
  static const bool bit_1_ = true;

  using coordinates_type = typename Point::coordinates_type;

  const ObjectPoints &points_;
  const array sizes_;
  std::size_t size_;

  void FillPoint(coordinates_index_type dimension, coordinate_type coord,
                 coordinates_index_type current_dimension, array new_sizes,
                 ObjectPoints &new_image,
                 coordinates_type current_coordinates) const noexcept {
    current_coordinates[current_dimension] = coord;
    auto slice_coords = current_coordinates;
    slice_coords[dimension] = 0;
    new_image.at(ConvertToLinearIndex(Point {slice_coords}, new_sizes)) =
        points_.at(ConvertToLinearIndex(Point {current_coordinates}));
  }

  void FillSlice(coordinates_index_type dimension, coordinate_type value,
                 coordinates_index_type current_dimension, array new_sizes,
                 ObjectPoints &new_image,
                 coordinates_type current_coordinates) const noexcept {
    if (current_dimension == Point::Dimensions - 1) {
      if (current_dimension != dimension) {
        for (coordinate_type coord = 0; coord != new_sizes[current_dimension];
             ++coord) {
          FillPoint(dimension, coord, current_dimension, new_sizes, new_image,
                    current_coordinates);
        }
      } else {
        FillPoint(dimension, value, current_dimension, new_sizes, new_image,
                  current_coordinates);
      }
    } else {
      if (current_dimension != dimension) {
        for (coordinate_type coord = 0; coord != new_sizes[current_dimension];
             ++coord) {
          current_coordinates[current_dimension] = coord;
          FillSlice(dimension, value, current_dimension + 1, new_sizes,
                    new_image, current_coordinates);
        }
      } else {
        current_coordinates[current_dimension] = value;
        FillSlice(dimension, value, current_dimension + 1, new_sizes, new_image,
                  current_coordinates);
      }
    }
  }

 public:
  static const bool Foreground = bit_0_;
  static const bool Background = bit_1_;

  static std::size_t ConvertToLinearIndex(
      const point_type &point, const std::array<size, Point::Dimensions> &sizes)
  noexcept {
    std::size_t index = static_cast<std::size_t>(point.GetCoordinate(0));
    std::size_t product_of_sizes = 1;
    for (std::size_t coordinate = 1;
         coordinate != static_cast<std::size_t>(point.GetNumberOfCoordinates());
         ++coordinate) {
      product_of_sizes *= static_cast<std::size_t>(sizes[coordinate - 1]);
      index += product_of_sizes * static_cast<std::size_t>(
          point.GetCoordinate(coordinate));
    }
    return index;
  }

  std::size_t ConvertToLinearIndex(const point_type &point) const noexcept {
    return ConvertToLinearIndex(point, sizes_);
  }

  TernaryImage(const arma::Cube<uint8_t> &points_values, const array sizes)
      : points_(points_values), sizes_(sizes) {
    static_assert(Point::Dimensions == 3, "Expected 3D Point.");
    size_ = 1;
    for (typename array::size_type index = 0; index != sizes_.size(); ++index)
      size_ *= static_cast<std::size_t>(sizes_[index]);
  }
  TernaryImage(const TernaryImage &other)
      : points_(other.points_),
        sizes_(other.sizes_),
        size_(other.size_) {
  }
  const array &Sizes() const noexcept {
    return sizes_;
  }
  std::size_t GetTotalSize() const {
    return size_;
  }
  bool IsOffLimitsPoint(const point_type &point) const noexcept {
    for (coordinates_index_type i = 0; i != point.GetNumberOfCoordinates();
         ++i) {
      auto coordinate = point.GetCoordinate(i);
      if (coordinate < 0 || coordinate >= sizes_[static_cast<std::size_t>(i)])
        return true;
    }
    return false;
  }
  bool IsOffLimitsCoordinate(const point_type &point,
                             coordinates_index_type index) const noexcept {
    auto coordinate = point.GetCoordinate(index);
    if (coordinate < 0 || coordinate >= sizes_[static_cast<std::size_t>(index)])
      return true;
    return false;
  }
  bool IsLimitPoint(const point_type &point) const noexcept {
    for (coordinates_index_type i = 0; i != point.GetNumberOfCoordinates();
         ++i) {
      auto coordinate = point.GetCoordinate(i);
      if (coordinate == 0 || coordinate >= sizes_[i] - 1)
        return true;
    }
    return false;
  }
  bool IsObjectPoint(const point_type &point) const noexcept {
    if (IsOffLimitsPoint(point))
      return false;
    return points_(point.GetCoordinate(0), point.GetCoordinate(1), point
        .GetCoordinate(2)) == 0;
  }
  bool IsContourPoint(const point_type &point) const noexcept {
    if (IsOffLimitsPoint(point))
      return false;
    return points_(point.GetCoordinate(0), point.GetCoordinate(1), point
        .GetCoordinate(2)) == 1;
  }
  int_fast8_t GetNumberOfDimensions() const noexcept {
    return Point::Dimensions;
  }
  size GetDimensionSize(int_fast8_t dimension_index) const noexcept {
    if (dimension_index < Point::Dimensions)
      return sizes_[static_cast<std::size_t>(dimension_index)];
    return 0;
  }
  static std::size_t GetTotalPoints(const array &sizes) noexcept {
    std::size_t total = 1;
    for (const auto &dimension_size : sizes)
      total *= dimension_size;
    return total;
  }
  size GetAverageDimensionsSize() const noexcept {
    std::size_t sum = 0;
    for (const auto &dimension_size : sizes_)
      sum += dimension_size;
    auto num_dimensions = static_cast<std::size_t>(GetNumberOfDimensions());
    return static_cast<size>(sum / num_dimensions);
  }
  my_type GetSlice(coordinates_index_type dimension,
                   coordinate_type value) const noexcept {
    array new_sizes = sizes_;
    new_sizes[dimension] = 1;
    ObjectPoints new_points;
    new_points.resize(GetTotalPoints(new_sizes));
    coordinates_type zero_coordinates{};
    FillSlice(dimension, value, 0, new_sizes, new_points, zero_coordinates);
    return my_type {new_points, new_sizes};
  }
  const_iterator cbegin() const noexcept {
    return const_iterator {sizes_};
  }
  const_iterator cend() const noexcept {
    return const_iterator {};
  }
  const_iterator begin() const noexcept {
    return const_iterator {sizes_};
  }
  const_iterator end() const noexcept {
    return const_iterator {};
  }
};

}  // namespace skeleton

#endif  // SRC_SKELETON_IMAGES_TERNARY_IMAGE_H_
