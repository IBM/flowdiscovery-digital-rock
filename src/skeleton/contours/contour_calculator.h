/**
 * \file src/skeleton/contours/contour_calculator.h
 * \brief This file implements the contour calculator algorithm in any number
 * of dimensions.
 *
 * \author Alexandre Ashade Lassance Cunha \<aashade@br.ibm.com\>
 * \author Giulia Duncan Coutinho \<coutingi@br.ibm.com\>
 * \authors Hugo de Oliveira Barbalho \<hugob@br.ibm.com\>
 * \copyright © IBM Corp.
 * \date 2015
 */

#ifndef SRC_SKELETON_CONTOURS_CONTOUR_CALCULATOR_H_
#define SRC_SKELETON_CONTOURS_CONTOUR_CALCULATOR_H_

#include <vector>
#include <queue>
#include <memory>
#include <algorithm>
#include <functional>

namespace skeleton {

template<class Image, class AnnotatedImage, class NeighbourCalculator>
class ContourCalculator {
 private:
  using image_type = Image;
  using PointType = typename Image::point_type;
  using stack = std::queue<PointType>;
  using stack_pointer = std::unique_ptr<stack>;
  using annotation_type = typename AnnotatedImage::annotation_type;
  using Neighbours = typename NeighbourCalculator::neighbours_type;
  using index_type = typename PointType::coordinates_index_type;
  using label_value = typename annotation_type::label_value;
  using coordinates_index_type = typename image_type::coordinates_index_type;

  label_value maximum_pixel_label_;
  std::vector<label_value> pixels_per_contour_;

  NeighbourCalculator neighbour_calculator_;
  const Image &image_;
  AnnotatedImage &annotated_image_;
  using PointsListType = std::vector<PointType>;
  PointsListType labeled_points_;
  coordinates_index_type off_check_excluded_;

  annotation_type BuildAnnotation(label_value contour_label,
                                  label_value pixel_label) const noexcept {
    annotation_type annotation{};
    annotation.contour_label = contour_label;
    annotation.pixel_label = pixel_label;
    return annotation;
  }

  const Neighbours GetNeighbours(const PointType &point) const noexcept {
    return neighbour_calculator_.GetNeighbours(point);
  }

  bool IsContourPoint(const PointType &point) const noexcept {
    return image_.IsContourPoint(point);
  }

  void EnqueueObjectNeighbours(const Neighbours &neighbours,
                               stack &points_stack) const noexcept {
    for (const auto &neighbour : neighbours)
      if (image_.IsContourPoint(neighbour))
        points_stack.push(neighbour);
  }

  bool TryAddAnnotationToContourPoint(const PointType &point,
                                      const Neighbours &neighbours,
                                      label_value pixel_label,
                                      label_value contour_label,
                                      stack &points_stack) noexcept {
    annotation_type annotation = BuildAnnotation(contour_label, pixel_label);
    if (annotated_image_.TryAddAnnotation(point, annotation)) {
      labeled_points_.push_back(point);
      EnqueueObjectNeighbours(neighbours, points_stack);
      return true;
    }
    return false;
  }

  bool TryAddAndPush(const PointType &point, label_value pixel_label,
                     label_value contour_label, stack &points_stack) noexcept {
    if (IsContourPoint(point)) {
      auto neighbours = GetNeighbours(point);
      return TryAddAnnotationToContourPoint(point,
                                            neighbours,
                                            pixel_label,
                                            contour_label,
                                            points_stack);
    }
    return false;
  }

  bool BuildContourFromPoint(stack &points_stack, label_value contour_label) {
    label_value pixel_label = 1;
    bool has_built = false;
    while (!points_stack.empty()) {
      const PointType current_point = points_stack.front();
      points_stack.pop();
      bool has_added = TryAddAndPush(current_point, pixel_label, contour_label,
                                     points_stack);
      if (has_added) {
        ++pixel_label;
        has_built = true;
      }
    }
    if (has_built) {
      auto max_label = pixel_label;
      pixels_per_contour_.push_back(max_label);
      if (max_label > maximum_pixel_label_)
        maximum_pixel_label_ = max_label;
    }
    return has_built;
  }

  bool BuildFromStartIfUnlabeled(const PointType &start,
                                 label_value contour_label) {
    stack points_stack;
    points_stack.push(start);
    return BuildContourFromPoint(points_stack, contour_label);
  }

 public:
  using contour_iterator = typename PointsListType::const_iterator;

  ContourCalculator(const Image &image, AnnotatedImage &annotated_image)
      : image_(image),
        annotated_image_(annotated_image) {
    labeled_points_.reserve(static_cast<std::size_t>(image
        .GetAverageDimensionsSize()));
  }

  void ComputeContours() {
    label_value contour_label = 1;
    pixels_per_contour_.clear();
    maximum_pixel_label_ = 0;
    labeled_points_.clear();
    for (const auto &point : image_) {
      if (!image_.IsContourPoint(point))
        continue;
      bool has_built = BuildFromStartIfUnlabeled(point, contour_label);
      if (has_built)
        contour_label = contour_label + 1;
    }
  }

  label_value GetMaximumPixelLabel() const noexcept {
    return maximum_pixel_label_;
  }

  label_value GetMaximumPixelLabelForContour(label_value contour_label) const {
    return pixels_per_contour_.at(contour_label - 1);
  }

  label_value GetNumberOfPixelsOnContour(label_value contour_label) const {
    return pixels_per_contour_.at(contour_label - 1);
  }

  contour_iterator cstart() const {
    return labeled_points_.cbegin();
  }

  contour_iterator cend() const {
    return labeled_points_.cend();
  }

  contour_iterator begin() const {
    return labeled_points_.cbegin();
  }

  contour_iterator end() const {
    return labeled_points_.cend();
  }
};

}  // namespace skeleton

#endif  // SRC_SKELETON_CONTOURS_CONTOUR_CALCULATOR_H_
