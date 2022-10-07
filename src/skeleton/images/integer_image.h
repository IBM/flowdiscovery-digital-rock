/**
 * \file src/skeleton/images/integer_image.h
 * \brief This file implements a helper to convert an armadillo cube to a
 * 3D image.
 *
 * \author Alexandre Ashade Lassance Cunha \<aashade@br.ibm.com\>
 * \copyright © IBM Corp.
 * \date 2015
 */

#ifndef SRC_SKELETON_IMAGES_INTEGER_IMAGE_H_
#define SRC_SKELETON_IMAGES_INTEGER_IMAGE_H_

#include <cstdint>
#include "src/skeleton/images/annotated_image.h"

namespace skeleton {

template<class TPoint, class TInt>
class IntegerImage {
};

template<class TPoint>
class IntegerImage<TPoint, int32_t> : public AnnotatedImage<TPoint, int32_t> {
};

template<class TPoint>
class IntegerImage<TPoint, int16_t> : public AnnotatedImage<TPoint, int16_t> {
};

}  // namespace skeleton

#endif  // SRC_SKELETON_IMAGES_INTEGER_IMAGE_H_
