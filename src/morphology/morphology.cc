/**
 * \file src/morphology/morphology.cc
 * \brief Implements mathematical morphology algorithms.
 *
 * \authors Rodrigo Neumann Barros Ferreira \<rneumann@br.ibm.com\>
 * \copyright © IBM Corp.
 * \date 2015
 *
 * This source file implements algorithms related to mathematical morphology.
 * They take as input an \c arma::ucube indicating the rock binary geometry and
 * returns its morphological properties, such as the fractal dimensions of each
 * phase (rock / pore) and the surface-to-volume ratio of the pore structure.
 */

#include "src/morphology/morphology.h"
#include <cassert>
#include <algorithm>
#include "src/cluster_label/cluster_label.h"

arma::Col<double> GetSurfaceToVolume(arma::ucube &c) {
/**
 * This function reads the \c arma::ucube containing the rock geometry and
 * distinguishes between pore (0), solid surface (1) and solid bulk (2) voxels.
 * It identifies solid surface voxels as having at least one voxel of the other phase as nearest
 * neighbour.
 *
 * \param[in] c         Voxel cube representing the rock geometry.
 * \retval    surf2vol  Surface-to-volume of the pore (0) and rock (1) phases.
 */
  // Create arma::uvec for the neighbours with the required length
  arma::uvec nn(26);  // Nearest neighbour around (i,j,k)

  // Create arma::uword to store the number of voxels of each type
  arma::uword pore_volume_voxels = 0U;
  arma::uword rock_volume_voxels = 0U;
  arma::uword pore_surface_voxels = 0U;
  arma::uword rock_surface_voxels = 0U;

  // Sweep arma::ucube checking if flagged sites are at the boundary
  for (arma::uword k = 0U; k != c.n_slices; ++k)    // For every slice
  for (arma::uword j = 0U; j != c.n_cols;   ++j)    // For every column
  for (arma::uword i = 0U; i != c.n_rows;   ++i) {  // For every row
    AllNeighbours(i, j, k, c, nn);

    if (c(i, j, k)) {                                   // Voxel belongs to rock phase
      rock_surface_voxels += arma::any(nn == 0U);       // Count voxel as rock surface
      rock_volume_voxels += arma::all(nn);              // Count voxel as rock volume
      c(i, j, k) += arma::all(nn);                      // Voxel flagged as rock volume
    } else {                                            // Voxel belongs to pore phase
      pore_surface_voxels += arma::any(nn == 1U);       // Count voxel as pore surface
      pore_volume_voxels += arma::all(nn != 1U);        // Count voxel as pore volume
    }
  }

  // Calculate ratio between number of boundary and pore voxels
  arma::vec surf2vol = {static_cast<double>(pore_surface_voxels) / pore_volume_voxels,
                        static_cast<double>(rock_surface_voxels) / rock_volume_voxels};

  return surf2vol;
}  // end of GetSurfaceToVolume() function



void GetFractalDimension(const std::string &output_folder, const arma::ucube &c) {
/**
 * This function estimates the fractal dimension of the flagged cluster using the box-counting
 * method. This method consists in calculating the number of boxes of each size required to cover
 * the object.
 *
 * \f[
 *    D = - \frac{\partial \ln{N(\epsilon)}}{\partial \ln{\epsilon}} \, ,
 * \f]
 *
 * where \f$ \epsilon \f$ is the linear dimension of the counting boxes.
 *
 * \param[in] output_folder Name of the folder to which output files are written.
 * \param[in] c             Voxel cube representing the rock geometry.
 * \retval    frac_dim      Fractal dimension of the flagged cluster
 */
  // Calculate constant values and ranges
  const arma::uvec::fixed<3> dimensions = {c.n_rows, c.n_cols, c.n_slices};
  const arma::uword max_exponent = static_cast<arma::uword>(std::ceil(std::log2(dimensions.max())));
  const arma::uword max_size = static_cast<arma::uword>(std::exp2(max_exponent));
  const arma::uvec exponents = arma::regspace<arma::uvec>(max_exponent, 0U);
  const arma::uvec::fixed<3> phases({0U, 1U, 2U});

  // Allocate temporary data containers
  arma::ucube cube(max_size, max_size, max_size, arma::fill::zeros);
  arma::umat number_of_boxes(max_exponent + 1U, 3U, arma::fill::zeros);

  // For each phase: pore, surface and bulk rock
  for (const auto &phase : phases) {
    cube(0U, 0U, 0U, arma::size(c)) = (c == phase);

    // For each box size: 2^0, 2^1, ..., 2^max_exponent
    for (const auto &exponent : exponents) {
      const arma::uword size = static_cast<arma::uword>(std::exp2(max_exponent - exponent));
      const arma::urowvec array = arma::regspace<arma::urowvec>(0U, size, max_size - size);
      const arma::uword half_size = size / 2;

      // Sweep cube and count number of boxes containing object voxels
      arma::uword counter = 0U;
      for (const auto &k : array) for (const auto &j : array) for (const auto &i : array) {
        cube(i, j, k) = cube(i, j, k)
                     || cube(i + half_size, j, k)
                     || cube(i, j + half_size, k)
                     || cube(i, j, k + half_size)
                     || cube(i, j + half_size, k + half_size)
                     || cube(i + half_size, j, k + half_size)
                     || cube(i + half_size, j + half_size, k)
                     || cube(i + half_size, j + half_size, k + half_size);
        counter += cube(i, j, k);
      }
      number_of_boxes(exponent, phase) = counter;
    }
  }

  // Consolidate output data
  const arma::uvec sizes = arma::flipud(arma::exp2(exponents));
  const arma::umat boxes = arma::flipud(number_of_boxes);

  // Write data points to file
  const arma::umat pore_frac_plot = arma::join_rows(sizes, boxes.col(0));
  const arma::umat surf_frac_plot = arma::join_rows(sizes, boxes.col(1));
  const arma::umat rock_frac_plot = arma::join_rows(sizes, boxes.col(2));
  pore_frac_plot.save(output_folder + "/pore_frac_plot.dat", arma::raw_ascii);
  surf_frac_plot.save(output_folder + "/surf_frac_plot.dat", arma::raw_ascii);
  rock_frac_plot.save(output_folder + "/rock_frac_plot.dat", arma::raw_ascii);
}  // end of GetFractalDimension() function
