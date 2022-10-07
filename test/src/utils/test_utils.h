/**
 * \file test/src/utils/test_utils.h
 *
 * \author Alexandre Ashade Lassance Cunha \<aashade@br.ibm.com\>
 * \author Giulia Duncan Coutinho \<coutingi@br.ibm.com\>
 * \copyright © IBM Corp.
 * \date 2015
 */

#ifndef TEST_SRC_UTILS_TEST_UTILS_H_
#define TEST_SRC_UTILS_TEST_UTILS_H_

#include <gtest/gtest.h>
#include <unistd.h>
#include <libgen.h>
#include <type_traits>
#include <string>

#include "src/skeleton/utils/hasher.h"

namespace test_utils {
const double precision = 1.0e-5;
template<typename T>
struct is_same_type {
  template<typename TVar>
  static bool Check(const TVar&) {
    return std::is_same<T, TVar>::value;
  }

  template<typename TVar>
  static void Assert(const TVar& var) {
    EXPECT_TRUE(Check(var));
  }
};

struct is_const_type {
  template<class T>
  static void Assert(T) {
    EXPECT_TRUE(std::is_const < T > ::value);
  }

  template<class T>
  static void Assert(T*) {
    EXPECT_TRUE(std::is_const < T > ::value);
  }

  template<class T>
  static void AssertReference(T&) {
    EXPECT_TRUE(std::is_const < T > ::value);
  }
};

bool PointsEqual(const skeleton::Point<2>& first,
                 const skeleton::Point<2>& second) {
  return first.GetCoordinate(0) == second.GetCoordinate(0)
      && first.GetCoordinate(1) == second.GetCoordinate(1);
}

bool PointsEqual3d(const skeleton::Point<3>& first,
                   const skeleton::Point<3>& second) {
  return first.GetCoordinate(0) == second.GetCoordinate(0)
      && first.GetCoordinate(1) == second.GetCoordinate(1)
      && first.GetCoordinate(2) == second.GetCoordinate(2);
}

bool DoubleEqual(const double a, const double b) {
  return a - b > -precision && a - b < precision;
}

}  // namespace test_utils

namespace skeleton {
template<>
class Hasher<int32_t> {
 public:
  std::size_t operator()(const int32_t& object) const noexcept {
    std::hash<int32_t> hasher;
    return hasher(object);
  }
};
}  // namespace skeleton

#endif  // TEST_SRC_UTILS_TEST_UTILS_H_
