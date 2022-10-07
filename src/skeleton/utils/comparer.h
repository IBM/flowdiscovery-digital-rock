/**
 * \file src/skeleton/utils/comparer.h
 * \brief This file implements a data structure to represent a voxel.
 *
 * \author Alexandre Ashade Lassance Cunha \<aashade@br.ibm.com\>
 * \copyright © IBM Corp.
 * \date 2015
 */


#ifndef SRC_SKELETON_UTILS_COMPARER_H_
#define SRC_SKELETON_UTILS_COMPARER_H_

#include <string>
#include "src/skeleton/utils/i_equatable.h"

namespace skeleton {

template<class T>
class Comparer {
 public:
  bool operator()(const T& x, const T& y) const noexcept {
    return x == y;
  }
  typedef T first_argument_type;
  typedef T second_argument_type;
  typedef bool result_type;
};

}  // namespace skeleton

#endif  // SRC_SKELETON_UTILS_COMPARER_H_
