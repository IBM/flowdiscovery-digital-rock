/**
 * \file src/skeleton/utils/hasher.h
 * \brief This file implements a generic hasher class.
 *
 * \author Alexandre Ashade Lassance Cunha \<aashade@br.ibm.com\>
 * \copyright © IBM Corp.
 * \date 2015
 */

#ifndef SRC_SKELETON_UTILS_HASHER_H_
#define SRC_SKELETON_UTILS_HASHER_H_

#include <cstddef>

namespace skeleton {

template<class T>
class Hasher {
 public:
  std::size_t operator()(const T& object) const noexcept {
    return object.GetHash();
  }
};

}  // namespace skeleton

#endif  // SRC_SKELETON_UTILS_HASHER_H_
