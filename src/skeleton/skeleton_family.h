/**
 * \file src/skeleton/skeleton_family.h
 * \brief This file implements a data structure to store skeletons.
 *
 * \author Alexandre Ashade Lassance Cunha \<aashade@br.ibm.com\>
 * \copyright © IBM Corp.
 * \date 2015
 */

#ifndef SRC_SKELETON_SKELETON_FAMILY_H_
#define SRC_SKELETON_SKELETON_FAMILY_H_

#include <utility>

namespace skeleton {

template<class IntegerAnnotatedImage, class SkeletonAnnotatedImage>
class SkeletonFamily {
 private:
  IntegerAnnotatedImage difference_image_;
  using point_type = typename IntegerAnnotatedImage::point_type;

 public:
  using skeleton_level_type = typename IntegerAnnotatedImage::annotation_type;
  explicit SkeletonFamily(IntegerAnnotatedImage& image)
      : difference_image_(std::move(image)) {
  }
  SkeletonFamily(const SkeletonFamily& other) = delete;
  SkeletonFamily& operator=(const SkeletonFamily &other) = delete;
  SkeletonFamily(SkeletonFamily&& other)
      : difference_image_(std::move(other.difference_image_)) {
  }
  SkeletonFamily& operator=(SkeletonFamily&& other) = delete;
  SkeletonAnnotatedImage GetSkeleton(skeleton_level_type level) const noexcept {
    SkeletonAnnotatedImage skeleton_points;
    for (const auto& point_and_difference : difference_image_) {
      const auto& point = point_and_difference.first;
      auto difference = point_and_difference.second;
      if (difference >= level)
        skeleton_points.AddPointAnnotation(point, difference);
    }
    return skeleton_points;
  }
  IntegerAnnotatedImage const& GetDifferenceImage() const noexcept {
    return difference_image_;
  }
};

}  // namespace skeleton

#endif  // SRC_SKELETON_SKELETON_FAMILY_H_
