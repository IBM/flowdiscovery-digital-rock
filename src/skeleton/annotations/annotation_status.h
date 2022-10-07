/**
 * \file src/skeleton/annotations/annotation_status.h
 * \brief This file implements the data structure that represents an
 * enumerable status for the annotation. Useful for skeleton algorithm.
 *
 * \author Alexandre Ashade Lassance Cunha \<aashade@br.ibm.com\>
 * \copyright © IBM Corp.
 * \date 2015
 */

#include <cstdint>

#ifndef SRC_SKELETON_ANNOTATIONS_ANNOTATION_STATUS_H_
#define SRC_SKELETON_ANNOTATIONS_ANNOTATION_STATUS_H_

enum class AnnotationStatus : int8_t {
  initial,
  inserted,
  removed
};

#endif  // SRC_SKELETON_ANNOTATIONS_ANNOTATION_STATUS_H_
