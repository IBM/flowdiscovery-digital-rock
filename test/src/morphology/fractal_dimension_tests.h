/**
 * \file test/src/morphology/fractal_dimension_tests.h
 * \brief Contains regression tests of the \c GetFractalDimension() function.
 *
 * The targets in the \c EXPECT_TRUE() statements guarantee that the box size distribution is
 * similar for all phases.
 *
 * \authors Rodrigo Neumann Barros Ferreira \<rneumann@br.ibm.com\>
 * \copyright © IBM Corp.
 * \date 2018
 */

#ifndef TEST_SRC_MORPHOLOGY_FRACTAL_DIMENSION_TESTS_H_
#define TEST_SRC_MORPHOLOGY_FRACTAL_DIMENSION_TESTS_H_

#include <gtest/gtest.h>
#include <string>
#include "src/morphology/morphology.h"

TEST(FractalDimension, Percolation_32x32x32) {
  const std::string &output_folder = "test/results";
  const arma::uword N = 32;
  const double tolerance = 1.0 / N;

  // Create artificial sample and calculate its fractal dimensions
  const arma::ucube cube = arma::randi<arma::ucube>(N, N, N, arma::distr_param(0, 2));
  GetFractalDimension(output_folder, cube);

  // Read output files
  arma::imat pore, surf, rock;
  pore.load(output_folder + "/pore_frac_plot.dat", arma::raw_ascii);
  surf.load(output_folder + "/surf_frac_plot.dat", arma::raw_ascii);
  rock.load(output_folder + "/rock_frac_plot.dat", arma::raw_ascii);
  std::remove((output_folder + "/pore_frac_plot.dat").c_str());
  std::remove((output_folder + "/surf_frac_plot.dat").c_str());
  std::remove((output_folder + "/rock_frac_plot.dat").c_str());

  // Calculate relative differences between files
  arma::mat diff_pore_surf = arma::conv_to<arma::mat>::from(arma::abs(pore - surf)) / pore;
  arma::mat diff_pore_rock = arma::conv_to<arma::mat>::from(arma::abs(pore - rock)) / pore;
  arma::mat diff_surf_rock = arma::conv_to<arma::mat>::from(arma::abs(surf - rock)) / surf;

  EXPECT_TRUE(arma::all(diff_pore_surf.col(1) <= tolerance));
  EXPECT_TRUE(arma::all(diff_pore_rock.col(1) <= tolerance));
  EXPECT_TRUE(arma::all(diff_surf_rock.col(1) <= tolerance));
}

TEST(FractalDimension, Percolation_64x64x64) {
  const std::string &output_folder = "test/results";
  const arma::uword N = 64;
  const double tolerance = 1.0 / N;

  // Create artificial sample and calculate its fractal dimensions
  const arma::ucube cube = arma::randi<arma::ucube>(N, N, N, arma::distr_param(0, 2));
  GetFractalDimension(output_folder, cube);

  // Read output files
  arma::imat pore, surf, rock;
  pore.load(output_folder + "/pore_frac_plot.dat", arma::raw_ascii);
  surf.load(output_folder + "/surf_frac_plot.dat", arma::raw_ascii);
  rock.load(output_folder + "/rock_frac_plot.dat", arma::raw_ascii);
  std::remove((output_folder + "/pore_frac_plot.dat").c_str());
  std::remove((output_folder + "/surf_frac_plot.dat").c_str());
  std::remove((output_folder + "/rock_frac_plot.dat").c_str());

  // Calculate relative differences between files
  arma::mat diff_pore_surf = arma::conv_to<arma::mat>::from(arma::abs(pore - surf)) / pore;
  arma::mat diff_pore_rock = arma::conv_to<arma::mat>::from(arma::abs(pore - rock)) / pore;
  arma::mat diff_surf_rock = arma::conv_to<arma::mat>::from(arma::abs(surf - rock)) / surf;

  EXPECT_TRUE(arma::all(diff_pore_surf.col(1) <= tolerance));
  EXPECT_TRUE(arma::all(diff_pore_rock.col(1) <= tolerance));
  EXPECT_TRUE(arma::all(diff_surf_rock.col(1) <= tolerance));
}

TEST(FractalDimension, Percolation_128x128x128) {
  const std::string &output_folder = "test/results";
  const arma::uword N = 128;
  const double tolerance = 1.0 / N;

  // Create artificial sample and calculate its fractal dimensions
  const arma::ucube cube = arma::randi<arma::ucube>(N, N, N, arma::distr_param(0, 2));
  GetFractalDimension(output_folder, cube);

  // Read output files
  arma::imat pore, surf, rock;
  pore.load(output_folder + "/pore_frac_plot.dat", arma::raw_ascii);
  surf.load(output_folder + "/surf_frac_plot.dat", arma::raw_ascii);
  rock.load(output_folder + "/rock_frac_plot.dat", arma::raw_ascii);
  std::remove((output_folder + "/pore_frac_plot.dat").c_str());
  std::remove((output_folder + "/surf_frac_plot.dat").c_str());
  std::remove((output_folder + "/rock_frac_plot.dat").c_str());

  // Calculate relative differences between files
  arma::mat diff_pore_surf = arma::conv_to<arma::mat>::from(arma::abs(pore - surf)) / pore;
  arma::mat diff_pore_rock = arma::conv_to<arma::mat>::from(arma::abs(pore - rock)) / pore;
  arma::mat diff_surf_rock = arma::conv_to<arma::mat>::from(arma::abs(surf - rock)) / surf;

  EXPECT_TRUE(arma::all(diff_pore_surf.col(1) <= tolerance));
  EXPECT_TRUE(arma::all(diff_pore_rock.col(1) <= tolerance));
  EXPECT_TRUE(arma::all(diff_surf_rock.col(1) <= tolerance));
}

#endif  // TEST_SRC_MORPHOLOGY_FRACTAL_DIMENSION_TESTS_H_
