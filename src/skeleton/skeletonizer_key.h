/**
 * \file src/skeleton/skeletonizer_key.h
 * \brief This file implements a data structure to store useful keys to be
 * used by the skeletonizer algorithm.
 *
 * \author Alexandre Ashade Lassance Cunha \<aashade@br.ibm.com\>
 * \copyright © IBM Corp.
 * \date 2015
 */

#ifndef SRC_SKELETON_SKELETONIZER_KEY_H_
#define SRC_SKELETON_SKELETONIZER_KEY_H_

#include <cstdint>
#include <limits>

namespace skeleton {

template<class TPoint, class TDistance, class TCounter = uint32_t>
struct SkeletonizerKey {
  using counter_type = TCounter;
  using distance_value = TDistance;
  using point_type = TPoint;
  using key_value_type = distance_value;

  distance_value distance;
  counter_type counter;
  TPoint point;
  SkeletonizerKey(const distance_value the_distance,
                  const counter_type the_counter,
                  const point_type &the_point)
      : distance(the_distance),
        counter(the_counter),
        point(the_point) {
  }
  SkeletonizerKey()
      : distance(std::numeric_limits<distance_value>::max()),
        counter(std::numeric_limits<counter_type>::max()),
        point() {
  }
  SkeletonizerKey(const SkeletonizerKey &other)
      : distance(other.distance),
        counter(other.counter),
        point(other.point) {
  }
  key_value_type GetValue() const {
    return distance;
  }
  SkeletonizerKey &operator=(const SkeletonizerKey &other) {
    distance = other.distance;
    counter = other.counter;
    point = other.point;
    return *this;
  }
  const point_type &Point() const {
    return point;
  }
  bool operator<(const SkeletonizerKey &other) const noexcept {
    return (distance < other.distance)
        || (distance == other.distance && counter < other.counter);
  }
  bool operator==(const SkeletonizerKey &second) const noexcept {
    return distance == second.distance && counter == second.counter;
  }
  std::size_t GetHash() const {
    return static_cast<std::size_t>(distance) * 919
        + static_cast<std::size_t>(counter);
  }
};

}  // namespace skeleton

#endif  // SRC_SKELETON_SKELETONIZER_KEY_H_
