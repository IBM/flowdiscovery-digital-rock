/**
 * \file src/skeleton/utils/i_equatable.h
 * \brief This file implements an interface to encapsulate equality
 * comparisons.
 *
 * \author Alexandre Ashade Lassance Cunha \<aashade@br.ibm.com\>
 * \copyright © IBM Corp.
 * \date 2015
 */

#ifndef SRC_SKELETON_UTILS_I_EQUATABLE_H_
#define SRC_SKELETON_UTILS_I_EQUATABLE_H_

namespace skeleton {

template<class T>
class IEquatable {
 public:
  virtual ~IEquatable() {
  }
  virtual bool Equals(const T& other) const noexcept = 0;
  inline bool operator ==(const T& other) const noexcept {
    return Equals(other);
  }
};
}  // namespace skeleton

#endif  // SRC_SKELETON_UTILS_I_EQUATABLE_H_
