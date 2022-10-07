/**
 * \file src/skeleton/skeletonizer_by_ift.h
 * \brief This file implements the skeletonizer algorithm.
 *
 * \author Alexandre Ashade Lassance Cunha \<aashade@br.ibm.com\>
 * \author Giulia Duncan Coutinho \<coutingi@br.ibm.com\>
 * \copyright © IBM Corp.
 * \date 2015
 */

#ifndef SRC_SKELETON_SKELETONIZER_BY_IFT_H_
#define SRC_SKELETON_SKELETONIZER_BY_IFT_H_

#include <stack>
#include <utility>
#include <new>
#include <memory>
#include <limits>
#include <algorithm>
#include <tuple>
#include <type_traits>
#include "src/skeleton/heaps/binary_heap.h"
#include "src/skeleton/annotations/annotation_status.h"
#include "src/skeleton/skeleton_family.h"
#include "src/skeleton/skeletonizer_key.h"

namespace skeleton {

template<class Image, class ImageAnnotations, class NeigbourCalculator,
    class ContourCalculator, class PathCostCalculator,
    class IntegerAnnotatedImage, class SkeletonAnnotatedImage>
class SkeletonizerByIFT {
 private:
  const std::unique_ptr<ImageAnnotations> annotations_;
  const std::unique_ptr<const Image> image_;
  const NeigbourCalculator neighbour_calculator_;
  const PathCostCalculator path_calculator_;
  ContourCalculator contour_calculator_;

 public:
  using image_type = Image;
  using contour_calculator_type = ContourCalculator;
  using annotated_image_type = ImageAnnotations;
  using path_calculator_type = PathCostCalculator;

 private:
  using point_type = typename image_type::point_type;
  using annotation_type = typename annotated_image_type::annotation_type;
  using label_value = typename annotation_type::label_value;
  using distance_value = typename annotation_type::distance_value;
  using key_type = SkeletonizerKey<point_type, distance_value>;
  using heap = BinaryHeap<key_type>;
  typename key_type::counter_type counter_value_;

  key_type GetKey(const distance_value distance, const point_type& point)
      noexcept {
    return key_type { distance, counter_value_++, point };
  }

  void InitialiseAnnotationOfSeedPoint(
      const point_type& point, typename key_type::counter_type counter) {
    auto new_annotation = annotations_->ReadAnnotation(point);
    new_annotation.status = AnnotationStatus::inserted;
    new_annotation.ZeroDisplacements();
    new_annotation.SetDistanceValue(0);
    new_annotation.tag = counter;
    new_annotation.point = point;
    annotations_->ModifyAnnotation(point, new_annotation);
  }

  annotation_type SetPointAsRemoved(const point_type& point,
                                    const annotation_type& annotation) {
    auto new_annotation = annotation;
    new_annotation.status = AnnotationStatus::removed;
    annotations_->ModifyAnnotation(point, new_annotation);
    return new_annotation;
  }

  annotation_type GetCurrentAnnotation(const point_type& neighbour) const {
    annotation_type annotation;
    if (annotations_->HasAnnotation(neighbour))
      annotation = annotations_->ReadAnnotation(neighbour);
    else
      annotation = annotation_type { };
    return annotation;
  }

  void PropagateLabelToNeighbour(heap& queue, const point_type& origin,
                                 const point_type& neighbour,
                                 const annotation_type& origin_annotation) {
    if (!image_->IsObjectPoint(neighbour))
      return;
    auto annotation = GetCurrentAnnotation(neighbour);
    if (annotation.status == AnnotationStatus::removed)
      return;
    distance_value distance = path_calculator_.GetConcatenatedPathCost(
        origin, origin_annotation, neighbour);
    if (distance < annotation.distance) {
      // old key backup
      key_type old_key { annotation.distance, annotation.tag, neighbour };
      // update distance
      path_calculator_.UpdatePointPathCost(origin, origin_annotation, neighbour,
                                           annotation);
      annotation.contour_label = origin_annotation.contour_label;
      annotation.pixel_label = origin_annotation.pixel_label;
      annotation.point = origin_annotation.point;
      key_type key = GetKey(distance, neighbour);
      annotation.tag = key.counter;
      if (annotation.status != AnnotationStatus::inserted) {
        queue.Insert(key);
        annotation.status = AnnotationStatus::inserted;
        annotations_->AddPointAnnotation(neighbour, annotation);
      } else {
        queue.IncreasePriority(old_key, key);
        annotations_->ModifyAnnotation(neighbour, annotation);
      }
    }
  }

  void PropagateLabelToNeighboursIfApplicable(
      heap& queue, const point_type& point,
      const annotation_type& point_annotation) {
    auto neighbours = neighbour_calculator_.GetNeighbours(point);
    for (const auto& neighbour : neighbours)
      PropagateLabelToNeighbour(queue, point, neighbour, point_annotation);
  }

  void InitialiseAndEnqueueSeedPoints(heap& queue) {
    contour_calculator_.ComputeContours();
    for (const auto& point : contour_calculator_) {
      distance_value distance = 0;
      key_type key = GetKey(distance, point);
      queue.Insert(key);
      InitialiseAnnotationOfSeedPoint(point, key.counter);
    }
  }

  void ExecuteIftIterations(heap& queue) {
    while (queue.HasElements()) {
      auto key = queue.Remove();
      auto point = key.Point();
      auto annotation = annotations_->ReadAnnotation(point);
      annotation = SetPointAsRemoved(point, annotation);
      PropagateLabelToNeighboursIfApplicable(queue, point, annotation);
    }
  }

  int64_t RecomputeMaxDifference(int64_t original_max_difference,
                                 label_value neighbour_label,
                                 label_value point_label) const {
    int64_t difference = static_cast<int64_t>(neighbour_label)
        - static_cast<int64_t>(point_label);
    if (difference > original_max_difference)
      return difference;
    return original_max_difference;
  }

  int64_t RecomputeMaxDifference(int64_t original_max_difference,
                                 label_value neighbour_label,
                                 label_value point_label,
                                 label_value pixels_on_contour) const {
    int64_t difference = abs(  // pls, do not change to std::abs.
        static_cast<int64_t>(neighbour_label)
            - static_cast<int64_t>(point_label));
    if (pixels_on_contour - difference < difference) {
      difference = pixels_on_contour - difference;
    }
    if (difference > original_max_difference)
      return difference;
    return original_max_difference;
  }

  int64_t RoundContourDifference(int64_t difference) const {
    if (difference > 0)
      return contour_calculator_.GetMaximumPixelLabel();
    return 0;
  }

  int64_t RoundPixelDifference(int64_t difference,
                               label_value number_of_pixels_on_contour) const {
    if (difference > number_of_pixels_on_contour / 2)
      return number_of_pixels_on_contour - difference;
    return difference;
  }

  std::tuple<int64_t, int64_t, int64_t> GetMaxDifferences(
      const point_type& point, const annotation_type& point_annotation) const {
    auto neighbours = neighbour_calculator_.GetNeighbours(point);
    auto max_contour_difference = std::numeric_limits<int64_t>::min();
    auto max_pixel_difference = std::numeric_limits<int64_t>::min();
    auto max_point_distance = std::numeric_limits<int64_t>::min();
    auto pixels_on_contour = contour_calculator_.GetNumberOfPixelsOnContour(
        point_annotation.contour_label);
    for (const auto& neighbour : neighbours) {
      if (!annotations_->HasAnnotation(neighbour))
        continue;
      auto neighbour_annotation = annotations_->ReadAnnotation(neighbour);
      auto new_distance = path_calculator_.GetSquaredDistance(
          neighbour_annotation.point, point_annotation.point);
      max_point_distance = std::max(max_point_distance, new_distance);
      max_contour_difference = RecomputeMaxDifference(
          max_contour_difference, neighbour_annotation.contour_label,
          point_annotation.contour_label);
      max_pixel_difference = RecomputeMaxDifference(
          max_pixel_difference, neighbour_annotation.pixel_label,
          point_annotation.pixel_label, pixels_on_contour);
    }
    return std::make_tuple(max_contour_difference, max_pixel_difference,
                           max_point_distance);
  }

  int64_t GetMaximumDifference(const point_type& point) const {
    auto point_annotation = annotations_->ReadAnnotation(point);
    auto max_differences = GetMaxDifferences(point, point_annotation);
    // auto contour_difference = RoundContourDifference(
    // std::get < 0 > (max_differences));
    // auto pixel_difference = std::get < 1 > (max_differences);
    auto max_distance = std::get < 2 > (max_differences);
    // return std::max(contour_difference, pixel_difference);
    return max_distance;
  }

  int64_t IsLocalMaximum(const point_type& point) const {
    const int64_t _true = 1;
    const int64_t _false = 0;
    if (!annotations_->HasAnnotation(point))
      return _false;
    auto point_annotation = annotations_->ReadAnnotation(point);
    auto neighbours = neighbour_calculator_.GetNeighbours(point);

    for (const auto& neighbour : neighbours) {
      if (!annotations_->HasAnnotation(neighbour))
        continue;
      auto neighbour_annotation = annotations_->ReadAnnotation(neighbour);
      auto neighbour_radius = std::sqrt(neighbour_annotation.distance);
      auto point_radius = std::sqrt(point_annotation.distance);
      if (neighbour_radius > point_radius)
        return _false;
    }
    return _true;
  }

 public:
  using skeleton_family = SkeletonFamily<IntegerAnnotatedImage,
  SkeletonAnnotatedImage>;
  explicit SkeletonizerByIFT(std::unique_ptr<const Image> image)
      : annotations_(std::make_unique<ImageAnnotations>()),
        image_(std::move(image)),
        neighbour_calculator_(),
        path_calculator_(),
        contour_calculator_(*image_, *annotations_),
        counter_value_(0) {
    static_assert(
        std::is_same<point_type, typename ImageAnnotations::point_type>::value,
        "Image and ImageAnnotations point_type should be the same.");
  }

  void ComputeSkeleton() {
    heap queue;
    InitialiseAndEnqueueSeedPoints(queue);
    ExecuteIftIterations(queue);
  }

  ImageAnnotations const& GetAnnotations() const noexcept {
    return *annotations_;
  }

  Image const& GetImage() const noexcept {
    return *image_;
  }

  skeleton_family GetSkeletonByLocalMaxima() const {
    IntegerAnnotatedImage difference_image;
    for (const auto& point : *image_) {
      if (!image_->IsObjectPoint(point))
        continue;
      difference_image.AddPointAnnotation(point, IsLocalMaximum(point));
    }
    return skeleton_family { difference_image };
  }

  skeleton_family GetSkeletonFamily() const {
    IntegerAnnotatedImage difference_image;
    for (const auto& point : *image_) {
      if (!image_->IsObjectPoint(point) || !annotations_->HasAnnotation(point))
        continue;
      difference_image.AddPointAnnotation(point, GetMaximumDifference(point));
    }
    return skeleton_family { difference_image };
  }

  ContourCalculator& GetContourCalculator() {
    return contour_calculator_;
  }
};

}  // namespace skeleton

#endif  // SRC_SKELETON_SKELETONIZER_BY_IFT_H_
