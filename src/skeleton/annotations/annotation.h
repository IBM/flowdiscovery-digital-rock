/**
 * \file src/skeleton/annotations/annotation.h
 * \brief This file implements the data structure that represents a image
 * annotation.
 *
 * \author Alexandre Ashade Lassance Cunha \<aashade@br.ibm.com\>
 * \author Giulia Duncan Coutinho \<coutingi@br.ibm.com\>
 * \copyright © IBM Corp.
 * \date 2015
 */

#ifndef SRC_SKELETON_ANNOTATIONS_ANNOTATION_H_
#define SRC_SKELETON_ANNOTATIONS_ANNOTATION_H_

#include <cstdint>
#include <memory>
#include <limits>
#include <array>
#include "src/skeleton/annotations/annotation_status.h"

namespace skeleton {

template<class Point>
class Annotation {
 private:
  static constexpr uint16_t dimensions = Point::Dimensions;

 public:
  using displacement_value = int32_t;
  using point_type = Point;
  using label_value = uint32_t;
  using tag_value = uint32_t;
  using distance_value = uint32_t;
  using status_value = AnnotationStatus;
  using displacements_type = std::array<displacement_value, dimensions>;
  using displacements_index_type = typename displacements_type::size_type;

  distance_value distance;
  displacements_type displacements;
  label_value contour_label;
  label_value pixel_label;
  tag_value tag;
  status_value status;
  point_type point;

  Annotation(void)
    : distance(static_cast<uint32_t>(std::numeric_limits<displacement_value>::max())),
      displacements(),
      contour_label(0),
      pixel_label(0),
      tag(0),
      status(status_value::initial),
      point() {
        displacements.fill(std::numeric_limits<displacement_value>::max());
      }

  displacement_value GetAccumulatedDistance(displacements_index_type index) const {
    return displacements.at(index);
  }

  void ZeroDisplacements(void) {
    displacements.fill(0);
  }

  void SetAccumulatedDistance(displacements_index_type index, displacement_value value) {
    displacements.at(index) = value;
  }

  void SetDistanceValue(distance_value value) {
    distance = value;
  }
};

}  // namespace skeleton

#endif  // SRC_SKELETON_ANNOTATIONS_ANNOTATION_H_
