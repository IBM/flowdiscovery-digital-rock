/**
 * \file src/morphology/morphology.h
 * \brief Function declaration for mathematical morphology algorithms.
 *
 * \authors Rodrigo Neumann Barros Ferreira \<rneumann@br.ibm.com\>
 * \copyright © IBM Corp.
 * \date 2015
 *
 * This header file contains the function declarations for algorithms that
 * determine the morphological properties of the infinite cluster.
 */

#ifndef SRC_MORPHOLOGY_MORPHOLOGY_H_
#define SRC_MORPHOLOGY_MORPHOLOGY_H_

#include <armadillo>
#include <string>

/// Distinguishes surface from bulk voxels and obtains surface-to-volume ratio
arma::Col<double> GetSurfaceToVolume(arma::ucube &c);

/// Determines the box-counting fractal dimension of the infinite cluster
void GetFractalDimension(const std::string &output_folder, const arma::ucube &c);

#endif  // SRC_MORPHOLOGY_MORPHOLOGY_H_
