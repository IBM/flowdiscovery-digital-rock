/**
 * \file test/src/morphology/surface_to_volume_tests.h
 * \brief Contains regression tests of the \c GetSurfaceToVolume() function.
 *
 * The targets in the \c EXPECT_EQ statements were obtained...
 *
 * \authors Rodrigo Neumann Barros Ferreira \<rneumann@br.ibm.com\>
 * \copyright © IBM Corp.
 * \date 2018
 */

#ifndef TEST_SRC_MORPHOLOGY_SURFACE_TO_VOLUME_TESTS_H_
#define TEST_SRC_MORPHOLOGY_SURFACE_TO_VOLUME_TESTS_H_

#include <gtest/gtest.h>
#include "src/morphology/morphology.h"

TEST(SurfaceToVolume, 7x7x7_rock_with_3x3x3_pore) {
  // Initialise constants
  const arma::uword L_rock = 7U;
  const arma::uword L_pore = 3U;
  const double tolerance = 1.0e-6;
  const arma::uword first = (L_rock - L_pore) / 2U;
  const arma::uword last = (L_rock + L_pore) / 2U - 1U;

  // Build test sample
  arma::ucube cube(L_rock, L_rock, L_rock, arma::fill::ones);
  cube.subcube(first, first, first, last, last, last).zeros();

  // Calculate targets
  const double pore_surface = static_cast<double>(std::pow(L_pore, 3) - std::pow(L_pore - 2U, 3));
  const double rock_surface = static_cast<double>(std::pow(L_pore + 2U, 3) - std::pow(L_pore, 3));
  const double pore_volume = static_cast<double>(std::pow(L_pore - 2U, 3));
  const double rock_volume = static_cast<double>(std::pow(L_rock, 3) - std::pow(L_pore + 2U, 3));
  const arma::vec surface_to_volume = GetSurfaceToVolume(cube);

  // Test result
  EXPECT_NEAR(surface_to_volume(0), pore_surface / pore_volume, tolerance);
  EXPECT_NEAR(surface_to_volume(1), rock_surface / rock_volume, tolerance);
}

TEST(SurfaceToVolume, 8x8x8_rock_with_3x3x3_pore) {
  // Initialise constants
  const arma::uword L_rock = 8U;
  const arma::uword L_pore = 3U;
  const double tolerance = 1.0e-6;
  const arma::uword first = (L_rock - L_pore) / 2U;
  const arma::uword last = (L_rock + L_pore) / 2U - 1U;

  // Build test sample
  arma::ucube cube(L_rock, L_rock, L_rock, arma::fill::ones);
  cube.subcube(first, first, first, last, last, last).zeros();

  // Calculate targets
  const double pore_surface = static_cast<double>(std::pow(L_pore, 3) - std::pow(L_pore - 2U, 3));
  const double rock_surface = static_cast<double>(std::pow(L_pore + 2U, 3) - std::pow(L_pore, 3));
  const double pore_volume = static_cast<double>(std::pow(L_pore - 2U, 3));
  const double rock_volume = static_cast<double>(std::pow(L_rock, 3) - std::pow(L_pore + 2U, 3));
  const arma::vec surface_to_volume = GetSurfaceToVolume(cube);

  // Test result
  EXPECT_NEAR(surface_to_volume(0), pore_surface / pore_volume, tolerance);
  EXPECT_NEAR(surface_to_volume(1), rock_surface / rock_volume, tolerance);
}

TEST(SurfaceToVolume, 8x8x8_rock_with_4x4x4_pore) {
  // Initialise constants
  const arma::uword L_rock = 8U;
  const arma::uword L_pore = 4U;
  const double tolerance = 1.0e-6;
  const arma::uword first = (L_rock - L_pore) / 2U;
  const arma::uword last = (L_rock + L_pore) / 2U - 1U;

  // Build test sample
  arma::ucube cube(L_rock, L_rock, L_rock, arma::fill::ones);
  cube.subcube(first, first, first, last, last, last).zeros();

  // Calculate targets
  const double pore_surface = static_cast<double>(std::pow(L_pore, 3) - std::pow(L_pore - 2U, 3));
  const double rock_surface = static_cast<double>(std::pow(L_pore + 2U, 3) - std::pow(L_pore, 3));
  const double pore_volume = static_cast<double>(std::pow(L_pore - 2U, 3));
  const double rock_volume = static_cast<double>(std::pow(L_rock, 3) - std::pow(L_pore + 2U, 3));
  const arma::vec surface_to_volume = GetSurfaceToVolume(cube);

  // Test result
  EXPECT_NEAR(surface_to_volume(0), pore_surface / pore_volume, tolerance);
  EXPECT_NEAR(surface_to_volume(1), rock_surface / rock_volume, tolerance);
}

TEST(SurfaceToVolume, 8x8x8_rock_with_5x5x5_pore) {
  // Initialise constants
  const arma::uword L_rock = 8U;
  const arma::uword L_pore = 5U;
  const double tolerance = 1.0e-6;
  const arma::uword first = (L_rock - L_pore) / 2U;
  const arma::uword last = (L_rock + L_pore) / 2U - 1U;

  // Build test sample
  arma::ucube cube(L_rock, L_rock, L_rock, arma::fill::ones);
  cube.subcube(first, first, first, last, last, last).zeros();

  // Calculate targets
  const double pore_surface = static_cast<double>(std::pow(L_pore, 3) - std::pow(L_pore - 2U, 3));
  const double rock_surface = static_cast<double>(std::pow(L_pore + 2U, 3) - std::pow(L_pore, 3));
  const double pore_volume = static_cast<double>(std::pow(L_pore - 2U, 3));
  const double rock_volume = static_cast<double>(std::pow(L_rock, 3) - std::pow(L_pore + 2U, 3));
  const arma::vec surface_to_volume = GetSurfaceToVolume(cube);

  // Test result
  EXPECT_NEAR(surface_to_volume(0), pore_surface / pore_volume, tolerance);
  EXPECT_NEAR(surface_to_volume(1), rock_surface / rock_volume, tolerance);
}

TEST(SurfaceToVolume, 9x9x9_rock_with_3x3x3_pore) {
  // Initialise constants
  const arma::uword L_rock = 9U;
  const arma::uword L_pore = 3U;
  const double tolerance = 1.0e-6;
  const arma::uword first = (L_rock - L_pore) / 2U;
  const arma::uword last = (L_rock + L_pore) / 2U - 1U;

  // Build test sample
  arma::ucube cube(L_rock, L_rock, L_rock, arma::fill::ones);
  cube.subcube(first, first, first, last, last, last).zeros();

  // Calculate targets
  const double pore_surface = static_cast<double>(std::pow(L_pore, 3) - std::pow(L_pore - 2U, 3));
  const double rock_surface = static_cast<double>(std::pow(L_pore + 2U, 3) - std::pow(L_pore, 3));
  const double pore_volume = static_cast<double>(std::pow(L_pore - 2U, 3));
  const double rock_volume = static_cast<double>(std::pow(L_rock, 3) - std::pow(L_pore + 2U, 3));
  const arma::vec surface_to_volume = GetSurfaceToVolume(cube);

  // Test result
  EXPECT_NEAR(surface_to_volume(0), pore_surface / pore_volume, tolerance);
  EXPECT_NEAR(surface_to_volume(1), rock_surface / rock_volume, tolerance);
}

TEST(SurfaceToVolume, 9x9x9_rock_with_5x5x5_pore) {
  // Initialise constants
  const arma::uword L_rock = 9U;
  const arma::uword L_pore = 5U;
  const double tolerance = 1.0e-6;
  const arma::uword first = (L_rock - L_pore) / 2U;
  const arma::uword last = (L_rock + L_pore) / 2U - 1U;

  // Build test sample
  arma::ucube cube(L_rock, L_rock, L_rock, arma::fill::ones);
  cube.subcube(first, first, first, last, last, last).zeros();

  // Calculate targets
  const double pore_surface = static_cast<double>(std::pow(L_pore, 3) - std::pow(L_pore - 2U, 3));
  const double rock_surface = static_cast<double>(std::pow(L_pore + 2U, 3) - std::pow(L_pore, 3));
  const double pore_volume = static_cast<double>(std::pow(L_pore - 2U, 3));
  const double rock_volume = static_cast<double>(std::pow(L_rock, 3) - std::pow(L_pore + 2U, 3));
  const arma::vec surface_to_volume = GetSurfaceToVolume(cube);

  // Test result
  EXPECT_NEAR(surface_to_volume(0), pore_surface / pore_volume, tolerance);
  EXPECT_NEAR(surface_to_volume(1), rock_surface / rock_volume, tolerance);
}

TEST(SurfaceToVolume, 7x7x7_pore_with_3x3x3_rock) {
  // Initialise constants
  const arma::uword L_rock = 3U;
  const arma::uword L_pore = 7U;
  const double tolerance = 1.0e-6;
  const arma::uword first = (L_pore - L_rock) / 2U;
  const arma::uword last = (L_rock + L_pore) / 2U - 1U;

  // Build test sample
  arma::ucube cube(L_pore, L_pore, L_pore, arma::fill::zeros);
  cube.subcube(first, first, first, last, last, last).ones();

  // Calculate targets
  const double pore_surface = static_cast<double>(std::pow(L_rock + 2U, 3) - std::pow(L_rock, 3));
  const double rock_surface = static_cast<double>(std::pow(L_rock, 3) - std::pow(L_rock - 2U, 3));
  const double pore_volume = static_cast<double>(std::pow(L_pore, 3) - std::pow(L_rock + 2U, 3));
  const double rock_volume = static_cast<double>(std::pow(L_rock - 2U, 3));
  const arma::vec surface_to_volume = GetSurfaceToVolume(cube);

  // Test result
  EXPECT_NEAR(surface_to_volume(0), pore_surface / pore_volume, tolerance);
  EXPECT_NEAR(surface_to_volume(1), rock_surface / rock_volume, tolerance);
}

TEST(SurfaceToVolume, 8x8x8_pore_with_3x3x3_rock) {
  // Initialise constants
  const arma::uword L_rock = 3U;
  const arma::uword L_pore = 8U;
  const double tolerance = 1.0e-6;
  const arma::uword first = (L_pore - L_rock) / 2U;
  const arma::uword last = (L_rock + L_pore) / 2U - 1U;

  // Build test sample
  arma::ucube cube(L_pore, L_pore, L_pore, arma::fill::zeros);
  cube.subcube(first, first, first, last, last, last).ones();

  // Calculate targets
  const double pore_surface = static_cast<double>(std::pow(L_rock + 2U, 3) - std::pow(L_rock, 3));
  const double rock_surface = static_cast<double>(std::pow(L_rock, 3) - std::pow(L_rock - 2U, 3));
  const double pore_volume = static_cast<double>(std::pow(L_pore, 3) - std::pow(L_rock + 2U, 3));
  const double rock_volume = static_cast<double>(std::pow(L_rock - 2U, 3));
  const arma::vec surface_to_volume = GetSurfaceToVolume(cube);

  // Test result
  EXPECT_NEAR(surface_to_volume(0), pore_surface / pore_volume, tolerance);
  EXPECT_NEAR(surface_to_volume(1), rock_surface / rock_volume, tolerance);
}

TEST(SurfaceToVolume, 8x8x8_pore_with_4x4x4_rock) {
  // Initialise constants
  const arma::uword L_rock = 4U;
  const arma::uword L_pore = 8U;
  const double tolerance = 1.0e-6;
  const arma::uword first = (L_pore - L_rock) / 2U;
  const arma::uword last = (L_rock + L_pore) / 2U - 1U;

  // Build test sample
  arma::ucube cube(L_pore, L_pore, L_pore, arma::fill::zeros);
  cube.subcube(first, first, first, last, last, last).ones();

  // Calculate targets
  const double pore_surface = static_cast<double>(std::pow(L_rock + 2U, 3) - std::pow(L_rock, 3));
  const double rock_surface = static_cast<double>(std::pow(L_rock, 3) - std::pow(L_rock - 2U, 3));
  const double pore_volume = static_cast<double>(std::pow(L_pore, 3) - std::pow(L_rock + 2U, 3));
  const double rock_volume = static_cast<double>(std::pow(L_rock - 2U, 3));
  const arma::vec surface_to_volume = GetSurfaceToVolume(cube);

  // Test result
  EXPECT_NEAR(surface_to_volume(0), pore_surface / pore_volume, tolerance);
  EXPECT_NEAR(surface_to_volume(1), rock_surface / rock_volume, tolerance);
}

TEST(SurfaceToVolume, 8x8x8_pore_with_5x5x5_rock) {
  // Initialise constants
  const arma::uword L_rock = 5U;
  const arma::uword L_pore = 8U;
  const double tolerance = 1.0e-6;
  const arma::uword first = (L_pore - L_rock) / 2U;
  const arma::uword last = (L_rock + L_pore) / 2U - 1U;

  // Build test sample
  arma::ucube cube(L_pore, L_pore, L_pore, arma::fill::zeros);
  cube.subcube(first, first, first, last, last, last).ones();

  // Calculate targets
  const double pore_surface = static_cast<double>(std::pow(L_rock + 2U, 3) - std::pow(L_rock, 3));
  const double rock_surface = static_cast<double>(std::pow(L_rock, 3) - std::pow(L_rock - 2U, 3));
  const double pore_volume = static_cast<double>(std::pow(L_pore, 3) - std::pow(L_rock + 2U, 3));
  const double rock_volume = static_cast<double>(std::pow(L_rock - 2U, 3));
  const arma::vec surface_to_volume = GetSurfaceToVolume(cube);

  // Test result
  EXPECT_NEAR(surface_to_volume(0), pore_surface / pore_volume, tolerance);
  EXPECT_NEAR(surface_to_volume(1), rock_surface / rock_volume, tolerance);
}

TEST(SurfaceToVolume, 9x9x9_pore_with_3x3x3_rock) {
  // Initialise constants
  const arma::uword L_rock = 3U;
  const arma::uword L_pore = 9U;
  const double tolerance = 1.0e-6;
  const arma::uword first = (L_pore - L_rock) / 2U;
  const arma::uword last = (L_rock + L_pore) / 2U - 1U;

  // Build test sample
  arma::ucube cube(L_pore, L_pore, L_pore, arma::fill::zeros);
  cube.subcube(first, first, first, last, last, last).ones();

  // Calculate targets
  const double pore_surface = static_cast<double>(std::pow(L_rock + 2U, 3) - std::pow(L_rock, 3));
  const double rock_surface = static_cast<double>(std::pow(L_rock, 3) - std::pow(L_rock - 2U, 3));
  const double pore_volume = static_cast<double>(std::pow(L_pore, 3) - std::pow(L_rock + 2U, 3));
  const double rock_volume = static_cast<double>(std::pow(L_rock - 2U, 3));
  const arma::vec surface_to_volume = GetSurfaceToVolume(cube);

  // Test result
  EXPECT_NEAR(surface_to_volume(0), pore_surface / pore_volume, tolerance);
  EXPECT_NEAR(surface_to_volume(1), rock_surface / rock_volume, tolerance);
}

TEST(SurfaceToVolume, 9x9x9_pore_with_5x5x5_rock) {
  // Initialise constants
  const arma::uword L_rock = 5U;
  const arma::uword L_pore = 9U;
  const double tolerance = 1.0e-6;
  const arma::uword first = (L_pore - L_rock) / 2U;
  const arma::uword last = (L_rock + L_pore) / 2U - 1U;

  // Build test sample
  arma::ucube cube(L_pore, L_pore, L_pore, arma::fill::zeros);
  cube.subcube(first, first, first, last, last, last).ones();

  // Calculate targets
  const double pore_surface = static_cast<double>(std::pow(L_rock + 2U, 3) - std::pow(L_rock, 3));
  const double rock_surface = static_cast<double>(std::pow(L_rock, 3) - std::pow(L_rock - 2U, 3));
  const double pore_volume = static_cast<double>(std::pow(L_pore, 3) - std::pow(L_rock + 2U, 3));
  const double rock_volume = static_cast<double>(std::pow(L_rock - 2U, 3));
  const arma::vec surface_to_volume = GetSurfaceToVolume(cube);

  // Test result
  EXPECT_NEAR(surface_to_volume(0), pore_surface / pore_volume, tolerance);
  EXPECT_NEAR(surface_to_volume(1), rock_surface / rock_volume, tolerance);
}

#endif  // TEST_SRC_MORPHOLOGY_SURFACE_TO_VOLUME_TESTS_H_
