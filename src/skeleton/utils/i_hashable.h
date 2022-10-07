/**
 * \file src/skeleton/utils/i_hashable.h
 * \brief This file implements a helper to compute hashcode of points.
 *
 * \author Alexandre Ashade Lassance Cunha \<aashade@br.ibm.com\>
 * \copyright © IBM Corp.
 * \date 2015
 */

#ifndef SRC_SKELETON_UTILS_I_HASHABLE_H_
#define SRC_SKELETON_UTILS_I_HASHABLE_H_

#include <cstddef>
#include "src/skeleton/points/point.h"

namespace skeleton {

class IHashable {
 public:
  virtual ~IHashable() {
  }
  virtual std::size_t GetHash() const noexcept = 0;
};

}  // namespace skeleton

#endif  // SRC_SKELETON_UTILS_I_HASHABLE_H_
