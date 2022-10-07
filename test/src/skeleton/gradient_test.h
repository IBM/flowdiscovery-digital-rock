/**
 * \file test/src/skeleton/gradient_test.h
 *
 * \author Hugo de Oliveira Barbalho \<hugob@br.ibm.com\>
 * \copyright © IBM Corp.
 * \date 2017
 */

#ifndef TEST_SRC_SKELETON_GRADIENT_TEST_H_
#define TEST_SRC_SKELETON_GRADIENT_TEST_H_

#include <gtest/gtest.h>
#include <armadillo>
#include <vector>
#include <memory>
#include <utility>
#include "src/skeleton/images/image.h"
#include "src/skeleton/images/arma_2d_i_image_builder.h"
#include "src/skeleton/images/annotated_image.h"
#include "src/skeleton/images/integer_image.h"
#include "src/skeleton/skeletonizer_by_ift.h"
#include "src/skeleton/neighbours/eight_neighbour_calculator.h"
#include "src/skeleton/contours/contour_calculator.h"
#include "test/src/utils/test_utils.h"
#include "src/skeleton/centerline/gradient.h"

namespace skeleton_test {
using image2d = skeleton::BinaryImage<skeleton::Point<2>>;
using point2d_type = skeleton::Point<2>;
using gradient_point2d_type = skeleton::Point<2, double>;
using annotated_image2d =
  skeleton::AnnotatedImage<point2d_type, skeleton::Annotation<point2d_type>>;
using PointNeighbourCalculator = skeleton::EightNeighbourCalculator<point2d_type,
                                                                    skeleton::PointBuilder>;
using Gradient2D = skeleton::Gradient<image2d,
                                      annotated_image2d,
                                      PointNeighbourCalculator,
                                      skeleton::PointBuilder,
                                      gradient_point2d_type>;
using ContourCalculator2D = skeleton::ContourCalculator<image2d,
                                                        annotated_image2d,
                                                        PointNeighbourCalculator>;

using IntegerImage2D = skeleton::IntegerImage<point2d_type, int32_t>;

using annotation_type2d = typename annotated_image2d::annotation_type;
using QuadraticPathCalculator2D =
      skeleton::QuadraticPathCalculator<annotation_type2d, point2d_type>;
using SkeletonImage2D = IntegerImage2D;
using Skeletonizer2D = skeleton::SkeletonizerByIFT<image2d,
                                                   annotated_image2d,
                                                   PointNeighbourCalculator,
                                                   ContourCalculator2D,
                                                   QuadraticPathCalculator2D,
                                                   IntegerImage2D,
                                                   SkeletonImage2D>;
static constexpr double tolerance = 1.0e-5;

TEST(Gradient, ComputeGradient_Should_Build_Gradient_Vector_Field) {
  std::vector<bool> points;
  const image2d::size n_rows = 5, n_cols = 5;
  points.resize(n_rows * n_cols, image2d::Background);
  image2d::array sizes = {n_rows, n_cols};

  for (image2d::size i = 1; i < n_rows - 1; ++i) {
    for (image2d::size j = 1; j < n_cols - 1; ++j) {
      points[static_cast<std::size_t>(i*n_cols + j)] = image2d::Foreground;
    }
  }
  points[2*n_cols + 0] = image2d::Foreground;
  points[2*n_cols + n_cols - 1] = image2d::Foreground;
  /* Square Image:
     ##1##
     #121#
     #141#
     #121#
     ##1##
  */

  image2d image = image2d{points, sizes};
  auto image_ptr = std::make_unique<image2d>(image);

  Skeletonizer2D skeletonizer{std::move(image_ptr)};
  skeletonizer.ComputeSkeleton();

  const annotated_image2d &annotations = skeletonizer.GetAnnotations();
  Gradient2D gradient_calculator{image, annotations};

  gradient_calculator.ComputeGradient();

  std::vector<point2d_type> expected_points;
  std::vector<gradient_point2d_type> expected_gradients;
  expected_points.push_back(point2d_type{{1, 1}});
  expected_gradients.push_back(gradient_point2d_type{{0.496139, 0.868243}});
  expected_points.push_back(point2d_type{{2, 1}});
  expected_gradients.push_back(gradient_point2d_type{{0.0, 1.0}});
  expected_points.push_back(point2d_type{{3, 1}});
  expected_gradients.push_back(gradient_point2d_type{{-0.496139, 0.868243}});
  expected_points.push_back(point2d_type{{0, 2}});
  expected_gradients.push_back(gradient_point2d_type{{1.0, 0.0}});
  expected_points.push_back(point2d_type{{1, 2}});
  expected_gradients.push_back(gradient_point2d_type{{1.0, 0.0}});
  expected_points.push_back(point2d_type{{2, 2}});
  expected_gradients.push_back(gradient_point2d_type{{0.0, 0.0}});
  expected_points.push_back(point2d_type{{3, 2}});
  expected_gradients.push_back(gradient_point2d_type{{-1.0, 0.0}});
  expected_points.push_back(point2d_type{{4, 2}});
  expected_gradients.push_back(gradient_point2d_type{{-1.0, 0.0}});
  expected_points.push_back(point2d_type{{1, 3}});
  expected_gradients.push_back(gradient_point2d_type{{0.496139, -0.868243}});
  expected_points.push_back(point2d_type{{2, 3}});
  expected_gradients.push_back(gradient_point2d_type{{0.0, -1.0}});
  expected_points.push_back(point2d_type{{3, 3}});
  expected_gradients.push_back(gradient_point2d_type{{-0.496139, -0.868243}});

  // assert
  EXPECT_EQ(11, gradient_calculator.size());
  EXPECT_TRUE(std::is_permutation(gradient_calculator.cbegin(),
                                  gradient_calculator.cend(),
                                  expected_points.cbegin()));

  for (size_t i = 0; i < expected_points.size(); ++i) {
    gradient_point2d_type gradient = gradient_calculator.GetGradient(expected_points[i]);
    EXPECT_NEAR(expected_gradients[i].GetCoordinate(0), gradient.GetCoordinate(0), tolerance);
    EXPECT_NEAR(expected_gradients[i].GetCoordinate(1), gradient.GetCoordinate(1), tolerance);
  }
}

TEST(Gradient, ComputeGradient_Should_Single_Gradient_Vector_Ignoring_A_Neighbour) {
  std::vector<bool> points;
  const image2d::size n_rows = 5, n_cols = 5;
  points.resize(n_rows * n_cols, image2d::Background);
  image2d::array sizes = {n_rows, n_cols};

  for (image2d::size i = 1; i < n_rows - 1; ++i) {
    for (image2d::size j = 1; j < n_cols - 1; ++j) {
      points[static_cast<std::size_t>(i*n_cols + j)] = image2d::Foreground;
    }
  }
  points[2*n_cols + 0] = image2d::Foreground;
  points[2*n_cols + n_cols - 1] = image2d::Foreground;
  /* Square Image:
     ##1##
     #121#
     #141#
     #121#
     ##1##
  */

  image2d image = image2d{points, sizes};
  auto image_ptr = std::make_unique<image2d>(image);

  Skeletonizer2D skeletonizer{std::move(image_ptr)};
  skeletonizer.ComputeSkeleton();

  const annotated_image2d &annotations = skeletonizer.GetAnnotations();
  Gradient2D gradient_calculator{image, annotations};

  gradient_point2d_type gradient =
    gradient_calculator.ComputeOnDemand(point2d_type{{2, 2}}, point2d_type{{2, 1}});

  EXPECT_NEAR(0, gradient.GetCoordinate(0), tolerance);
  EXPECT_NEAR(1, gradient.GetCoordinate(1), tolerance);
}

TEST(Gradient, ComputeGradient_Gradient_Vectors_Sum_Should_Be_Zero) {
  std::vector<bool> points;
  const image2d::size n_rows = 5, n_cols = 5;
  points.resize(n_rows * n_cols, image2d::Background);
  image2d::array sizes = {n_rows, n_cols};

  for (image2d::size i = 1; i < n_rows - 1; ++i) {
    for (image2d::size j = 1; j < n_cols - 1; ++j) {
      points[static_cast<std::size_t>(i*n_cols + j)] = image2d::Foreground;
    }
  }
  points[2*n_cols + 0] = image2d::Foreground;
  points[2*n_cols + n_cols - 1] = image2d::Foreground;
  /* Square Image:
     ##1##
     #121#
     #141#
     #121#
     ##1##
  */

  image2d image = image2d{points, sizes};
  auto image_ptr = std::make_unique<image2d>(image);

  Skeletonizer2D skeletonizer{std::move(image_ptr)};
  skeletonizer.ComputeSkeleton();

  const annotated_image2d &annotations = skeletonizer.GetAnnotations();
  Gradient2D gradient_calculator{image, annotations};

  gradient_point2d_type gradient1 =
    gradient_calculator.ComputeOnDemand(point2d_type{{2, 2}}, point2d_type{{2, 1}});
  gradient_point2d_type gradient2 =
    gradient_calculator.ComputeOnDemand(point2d_type{{2, 2}}, point2d_type{{2, 3}});

  EXPECT_TRUE(gradient_calculator.IsGradientSumZero(gradient1,
                                                    gradient2));
}

TEST(Gradient, ComputeGradient_Gradient_Vectors_Sum_Should_Not_Be_Zero) {
  std::vector<bool> points;
  const image2d::size n_rows = 5, n_cols = 5;
  points.resize(n_rows * n_cols, image2d::Background);
  image2d::array sizes = {n_rows, n_cols};

  for (image2d::size i = 1; i < n_rows - 1; ++i) {
    for (image2d::size j = 1; j < n_cols - 1; ++j) {
      points[static_cast<std::size_t>(i*n_cols + j)] = image2d::Foreground;
    }
  }
  points[2*n_cols + 0] = image2d::Foreground;
  points[2*n_cols + n_cols - 1] = image2d::Foreground;
  /* Square Image:
     ##1##
     #121#
     #141#
     #121#
     ##1##
  */

  image2d image = image2d{points, sizes};
  auto image_ptr = std::make_unique<image2d>(image);

  Skeletonizer2D skeletonizer{std::move(image_ptr)};
  skeletonizer.ComputeSkeleton();

  const annotated_image2d &annotations = skeletonizer.GetAnnotations();
  Gradient2D gradient_calculator{image, annotations};

  gradient_point2d_type gradient1 =
    gradient_calculator.ComputeOnDemand(point2d_type{{2, 2}}, point2d_type{{2, 1}});
  gradient_point2d_type gradient2 =
    gradient_calculator.ComputeOnDemand(point2d_type{{2, 2}}, point2d_type{{1, 2}});

  EXPECT_FALSE(gradient_calculator.IsGradientSumZero(gradient1,
                                                    gradient2));
}

TEST(Gradient, ComputeGradient_Should_Create_Gradient_Vector) {
  std::vector<bool> points;
  const image2d::size n_rows = 5, n_cols = 5;
  points.resize(n_rows * n_cols, image2d::Background);
  image2d::array sizes = {n_rows, n_cols};

  for (image2d::size i = 1; i < n_rows - 1; ++i) {
    for (image2d::size j = 1; j < n_cols - 1; ++j) {
      points[static_cast<std::size_t>(i*n_cols + j)] = image2d::Foreground;
    }
  }
  points[2*n_cols + 0] = image2d::Foreground;
  points[2*n_cols + n_cols - 1] = image2d::Foreground;
  /* Square Image:
     ##1##
     #121#
     #141#
     #121#
     ##1##
  */

  image2d image = image2d{points, sizes};
  auto image_ptr = std::make_unique<image2d>(image);

  Skeletonizer2D skeletonizer{std::move(image_ptr)};
  skeletonizer.ComputeSkeleton();

  const annotated_image2d &annotations = skeletonizer.GetAnnotations();
  Gradient2D gradient_calculator{image, annotations};

  gradient_point2d_type gradient1 =
    gradient_calculator.CreateGradientVector(point2d_type{{2, 2}}, point2d_type{{2, 1}});
  gradient_point2d_type gradient2 =
    gradient_calculator.CreateGradientVector(point2d_type{{2, 2}}, point2d_type{{2, 3}});

  EXPECT_NEAR(0, gradient1.GetCoordinate(0), tolerance);
  EXPECT_NEAR(1, gradient1.GetCoordinate(1), tolerance);
  EXPECT_NEAR(0, gradient2.GetCoordinate(0), tolerance);
  EXPECT_NEAR(-1, gradient2.GetCoordinate(1), tolerance);

  EXPECT_TRUE(gradient_calculator.IsGradientSumZero(gradient1,
                                                    gradient2));
}

TEST(Gradient, ComputeStepPenalty_Should_Compute_Penalty) {
  std::vector<bool> points;
  const image2d::size n_rows = 5, n_cols = 5;
  points.resize(n_rows * n_cols, image2d::Background);
  image2d::array sizes = {n_rows, n_cols};

  for (image2d::size i = 1; i < n_rows - 1; ++i) {
    for (image2d::size j = 1; j < n_cols - 1; ++j) {
      points[static_cast<std::size_t>(i*n_cols + j)] = image2d::Foreground;
    }
  }
  points[2*n_cols + 0] = image2d::Foreground;
  points[2*n_cols + n_cols - 1] = image2d::Foreground;
  /* Square Image:
     ##1##
     #121#
     #141#
     #121#
     ##1##
  */

  image2d image = image2d{points, sizes};
  auto image_ptr = std::make_unique<image2d>(image);

  Skeletonizer2D skeletonizer{std::move(image_ptr)};
  skeletonizer.ComputeSkeleton();

  const annotated_image2d &annotations = skeletonizer.GetAnnotations();
  Gradient2D gradient_calculator{image, annotations};

  gradient_calculator.ComputeGradient();

  auto penalty1 = gradient_calculator.ComputeStepPenalty(point2d_type{{0, 2}},
                                                         point2d_type{{1, 1}});
  auto penalty2 = gradient_calculator.ComputeStepPenalty(point2d_type{{0, 2}},
                                                         point2d_type{{1, 2}});
  auto penalty3 = gradient_calculator.ComputeStepPenalty(point2d_type{{0, 2}},
                                                         point2d_type{{1, 3}});


  // assert

  EXPECT_NEAR(0.499999, penalty1, tolerance);
  EXPECT_NEAR(0.0, penalty2, tolerance);
  EXPECT_NEAR(0.499999, penalty3, tolerance);
}

TEST(Gradient, ComputeOnDemand_Should_Compute_Gradient_For_A_Given_Cenario) {
  std::vector<bool> points;
  const image2d::size n_rows = 5, n_cols = 5;
  points.resize(n_rows * n_cols, image2d::Background);
  image2d::array sizes = {n_rows, n_cols};

  for (image2d::size i = 1; i < n_rows - 1; ++i) {
    for (image2d::size j = 1; j < n_cols - 1; ++j) {
      points[static_cast<std::size_t>(i*n_cols + j)] = image2d::Foreground;
    }
  }
  points[2*n_cols + 0] = image2d::Foreground;
  points[2*n_cols + n_cols - 1] = image2d::Foreground;
  /* Square Image:
     ##1##
     #121#
     #141#
     #121#
     ##1##
  */

  image2d image = image2d{points, sizes};
  auto image_ptr = std::make_unique<image2d>(image);

  Skeletonizer2D skeletonizer{std::move(image_ptr)};
  skeletonizer.ComputeSkeleton();

  const annotated_image2d &annotations = skeletonizer.GetAnnotations();
  Gradient2D gradient_calculator{image, annotations};
  gradient_point2d_type gradient1 = gradient_calculator.ComputeOnDemand(point2d_type{{2, 2}});

  gradient_calculator.MarkAsVisited(point2d_type{{0, 2}});
  gradient_calculator.MarkAsVisited(point2d_type{{1, 2}});

  gradient_point2d_type gradient2 = gradient_calculator.ComputeOnDemand(point2d_type{{2, 2}});

  auto penalty1 = gradient_calculator.ComputeStepPenalty(point2d_type{{2, 2}},
                                                         point2d_type{{3, 1}},
                                                         gradient2);
  auto penalty2 = gradient_calculator.ComputeStepPenalty(point2d_type{{2, 2}},
                                                         point2d_type{{3, 2}},
                                                         gradient2);
  auto penalty3 = gradient_calculator.ComputeStepPenalty(point2d_type{{2, 2}},
                                                         point2d_type{{3, 3}},
                                                         gradient2);


  // assert

  EXPECT_NEAR(0.0, gradient1.GetCoordinate(0), tolerance);
  EXPECT_NEAR(0.0, gradient1.GetCoordinate(1), tolerance);

  EXPECT_NEAR(1.0, gradient2.GetCoordinate(0), tolerance);
  EXPECT_NEAR(0.0, gradient2.GetCoordinate(1), tolerance);

  EXPECT_NEAR(0.499999, penalty1, tolerance);
  EXPECT_NEAR(0.0, penalty2, tolerance);
  EXPECT_NEAR(0.499999, penalty3, tolerance);

  // When compute gradient on demand, it should be passed as parameter.
  EXPECT_ANY_THROW(gradient_calculator.ComputeStepPenalty(point2d_type{{2, 2}},
                                                          point2d_type{{3, 1}}));
}
}  // namespace skeleton_test

#endif  // TEST_SRC_SKELETON_GRADIENT_TEST_H_
