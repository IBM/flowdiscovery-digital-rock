/**
 * \file src/skeleton/utils/default_comparer.h
 * \brief This file implements a data structure to compare objects of same type.
 *
 * \author Alexandre Ashade Lassance Cunha \<aashade@br.ibm.com\>
 * \copyright © IBM Corp.
 * \date 2015
 */

#ifndef SRC_SKELETON_UTILS_DEFAULT_COMPARER_H_
#define SRC_SKELETON_UTILS_DEFAULT_COMPARER_H_

namespace skeleton {

template<class T>
class DefaultComparer {
 public:
  bool IsLess(const T& left, const T& right) const {
    return left < right;
  }
};

}  // namespace skeleton

#endif  // SRC_SKELETON_UTILS_DEFAULT_COMPARER_H_
