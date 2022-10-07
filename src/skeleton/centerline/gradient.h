/**
 * \file src/skeleton/centerline/gradient.h
 * \brief This file computes gradient for each pore voxel of a image.
 *
 * \author Hugo de Oliveira Barbalho \<hugob@br.ibm.com\>
 * \copyright © IBM Corp.
 * \date 2017
 */

#ifndef SRC_SKELETON_CENTERLINE_GRADIENT_H_
#define SRC_SKELETON_CENTERLINE_GRADIENT_H_

#include <unordered_map>
#include <set>
#include <queue>
#include <vector>
#include <memory>
#include <utility>
#include "src/skeleton/utils/hasher.h"

namespace skeleton {

/**
 * \brief Compute the gradient vector, which indicates the farther
 * way direction from the nearest object wall.
 *
 * @tparam Image The implementation of a image
 * @tparam ImageAnnotations The implementation of an annotated image.
 * */
template<class Image, class ImageAnnotations,
         class NeigbourCalculator, class PointBuilder, class GradientPoint>
class Gradient {
 public:
  using image_type = Image;
  using image_annotations_type = ImageAnnotations;
  using gradient_point_type = GradientPoint;

 private:
  using point_type = typename image_type::point_type;
  using gradient_coordinate = typename gradient_point_type::coordinate;
  using index_type = typename point_type::coordinates_index_type;
  using hasher_type = Hasher<point_type>;
  using comparer_type = Comparer<point_type>;
  using annotations_type = std::unordered_map<point_type, gradient_point_type,
                                              hasher_type, comparer_type>;
  using queue_type = std::queue<point_type>;
  using points_list_type = std::vector<point_type>;
  using annotations_visited_type  = std::set<point_type>;

  annotations_visited_type visited_;
  points_list_type labeled_points_;
  queue_type queue_;
  const std::unique_ptr<annotations_type> annotations_;
  const image_annotations_type *image_annotations_;
  const image_type *image_;
  const NeigbourCalculator neighbour_calculator_;
  static constexpr double tolerance = 1.0e-5;

  gradient_coordinate DotProduct(const gradient_point_type point_a,
                                 const gradient_point_type point_b) const {
    gradient_coordinate dot_product = 0.0;

    for (index_type index = 0; index != point_type::Dimensions; ++index) {
      dot_product += point_a.GetCoordinate(index) * point_b.GetCoordinate(index);
    }

    return dot_product;
  }

  void Normalise(gradient_point_type &gradient) {
    gradient_coordinate distance = 0.0;
    for (index_type index = 0; index != point_type::Dimensions; ++index) {
      distance = distance + gradient.GetCoordinate(index) * gradient.GetCoordinate(index);
    }
    if (distance > tolerance) {
      distance = sqrt(distance);

      for (index_type index = 0; index != point_type::Dimensions; ++index) {
        gradient_coordinate value = gradient.GetCoordinate(index) / distance;
        gradient.SetCoordinate(index, value);
      }
    }
  }

   /**
   * \brief Given a point of the image, such method compute the
   * normalised gradient vector.
   * \param point The instance of a read-only point of type point_type.
   */
  gradient_point_type Compute(const point_type &point) {
    auto neighbours = neighbour_calculator_.GetNeighbours(point);
    gradient_point_type gradient;
    for (index_type index = 0; index != point_type::Dimensions; ++index) {
      gradient.SetCoordinate(index, 0);
    }
    gradient_coordinate delta;
    for (const point_type& neighbour : neighbours) {
      if (image_->IsObjectPoint(neighbour) && !IsVisited(neighbour)) {
        auto distance = image_annotations_->ReadAnnotation(neighbour).distance;
        for (index_type index = 0; index != point_type::Dimensions; ++index) {
          delta = neighbour.GetCoordinate(index) - point.GetCoordinate(index);
          gradient_coordinate value = gradient.GetCoordinate(index) +
                                      delta * static_cast<gradient_coordinate>(distance);
          gradient.SetCoordinate(index, value);
        }
      }
    }
    return gradient;
  }

  /**
   * \brief Given a point of the image and another point of the same image,
   * such method compute the normalised gradient vector for the first argument
   * point ignoring the second one.
   * \param point The instance of a read-only point of type point_type.
   * \param point_ignored The instance of a read-only point of type point_type.
   */
  gradient_point_type Compute(const point_type &point, const point_type &point_ignored) {
    auto neighbours = neighbour_calculator_.GetNeighbours(point);
    gradient_point_type gradient;
    for (index_type index = 0; index != point_type::Dimensions; ++index) {
      gradient.SetCoordinate(index, 0);
    }
    gradient_coordinate delta;
    for (const point_type& neighbour : neighbours) {
      if (!image_->IsObjectPoint(neighbour) || neighbour == point_ignored) continue;
      if (neighbour.IsVertexNeighbour(point)) continue;

      auto distance = image_annotations_->ReadAnnotation(neighbour).distance;
      for (index_type index = 0; index != point_type::Dimensions; ++index) {
        delta = neighbour.GetCoordinate(index) - point.GetCoordinate(index);
        gradient_coordinate value = gradient.GetCoordinate(index) +
                                    delta * static_cast<gradient_coordinate>(distance);
        gradient.SetCoordinate(index, value);
      }
    }
    return gradient;
  }

  void ComputeAndEnqueue(const point_type &point) {
    labeled_points_.push_back(point);
    gradient_point_type gradient = Compute(point);
    Normalise(gradient);
    annotations_->insert(std::make_pair(point, gradient));
  }

 public:
  using gradient_iterator = typename points_list_type::const_iterator;

  explicit Gradient(const image_type &image,
                    const image_annotations_type &image_annotations):
           annotations_(std::make_unique<annotations_type>()),
           image_annotations_(&image_annotations),
           image_(&image),
           neighbour_calculator_() {
  }

  void ComputeGradient() {
    labeled_points_.clear();
    for (const auto& point : *image_) {
      if (image_->IsObjectPoint(point))
        ComputeAndEnqueue(point);
    }
  }

  gradient_point_type ComputeOnDemand(const point_type &point) {
    gradient_point_type gradient = Compute(point);
    Normalise(gradient);
    return gradient;
  }

  gradient_point_type ComputeOnDemand(const point_type &point, const point_type &point_ignored) {
    gradient_point_type gradient = Compute(point, point_ignored);
    Normalise(gradient);
    return gradient;
  }

  void MarkAsVisited(const point_type &point) {
    visited_.insert(point);
  }

  void UnMarkAsVisited(const point_type &point) {
    visited_.erase(point);
  }

  bool IsVisited(const point_type point) const {
    return visited_.find(point) != visited_.end();
  }

  double ComputeStepPenalty(const point_type from, const point_type to) {
    gradient_point_type gradient = GetGradient(from);
    return ComputeStepPenalty(from, to, gradient);
  }

  double ComputeStepPenalty(const point_type from,
                            const point_type to,
                            const gradient_point_type gradient) {
    gradient_point_type direction;
    for (index_type index = 0; index != point_type::Dimensions; ++index) {
      direction.SetCoordinate(index, to.GetCoordinate(index) - from.GetCoordinate(index));
    }

    Normalise(direction);
    auto dot_product = DotProduct(direction, gradient);

    return 1.0 - dot_product * dot_product;
  }

  double ComputeStepPenalty(const gradient_point_type gradient1,
                            const gradient_point_type gradient2) {
    auto dot_product = DotProduct(gradient1, gradient2);
    return 1.0 - dot_product * dot_product;
  }

  gradient_point_type GetGradient(const point_type point) const {
    return annotations_->at(point);
  }

  bool IsGradientZero(const gradient_point_type gradient) const {
    for (index_type index = 0; index != gradient_point_type::Dimensions; ++index) {
      if (gradient.GetCoordinate(index) < -tolerance ||
          gradient.GetCoordinate(index) > tolerance)
          return false;
    }
    return true;
  }

  bool IsGradientSumZero(const gradient_point_type gradient_a,
                         const gradient_point_type gradient_b) const {
    for (index_type index = 0; index != gradient_point_type::Dimensions; ++index) {
      if (gradient_a.GetCoordinate(index) + gradient_b.GetCoordinate(index) < -tolerance ||
          gradient_a.GetCoordinate(index) + gradient_b.GetCoordinate(index) > tolerance)
        return false;
    }
    return true;
  }

  gradient_point_type CreateGradientVector(const point_type &from,
                                           const point_type &to) {
    gradient_point_type gradient_vector;
    for (index_type index = 0; index != gradient_point_type::Dimensions; ++index) {
      auto coord_value = from.GetCoordinate(index) - to.GetCoordinate(index);
      gradient_vector.SetCoordinate(index, coord_value);
    }
    return gradient_vector;
  }

  gradient_iterator begin() const {
    return labeled_points_.begin();
  }

  gradient_iterator end() const {
    return labeled_points_.end();
  }

  gradient_iterator cbegin() const {
    return labeled_points_.cbegin();
  }

  gradient_iterator cend() const {
    return labeled_points_.cend();
  }

  std::size_t size() const {
    return labeled_points_.size();
  }
};
}  // namespace skeleton
#endif  // SRC_SKELETON_CENTERLINE_GRADIENT_H_
