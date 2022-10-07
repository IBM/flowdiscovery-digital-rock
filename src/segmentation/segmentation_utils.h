/**
 * \file src/segmentation/segmentation_utils.h
 * \brief Function declaration for several helper functions for image segmentation algorithms.
 *
 * \authors Rodrigo Neumann Barros Ferreira \<rneumann@br.ibm.com\>
 * \copyright © IBM Corp.
 * \date 2017
 *
 * This header file contains the function declarations for several helper functions for image
 * segmentation algorithms.
 */

#ifndef SRC_SEGMENTATION_SEGMENTATION_UTILS_H_
#define SRC_SEGMENTATION_SEGMENTATION_UTILS_H_

#include <armadillo>
#include <cstdint>

/// Calculates the normalised and accumulated greyscale histograms
arma::Mat<double>::fixed<256, 3> CalculateGreyscaleStatistics(const arma::Cube<uint8_t> &xi_gs);

/// Smooths normalised histogram by applying a central moving average filter
void MovingAverageFilter(arma::Mat<double>::fixed<256, 3> &histogram,
                         const arma::uword window_size = 3U);

/// Calculates mean greyscale level in the histogram within specified range
double CalculateMeanLevel(const arma::Mat<double>::fixed<256, 3> &histogram,
                          const arma::uword l_start = 0U,
                          const arma::uword l_end = 255U);

/// Finds all local maxima and returns their location
arma::Col<arma::uword> FindLocalMaxima(const arma::Col<double>::fixed<256> &normalised_histogram,
                                       const arma::uword l_start = 0U,
                                       const arma::uword l_end = 255U);

/// Finds all local minima and returns their location
arma::Col<arma::uword> FindLocalMinima(const arma::Col<double>::fixed<256> &normalised_histogram,
                                       const arma::uword l_start = 0U,
                                       const arma::uword l_end = 255U);

/// Calculates an entropy-like quantity \f$ \mathcal{F}(x) = -x \log{x} \f$
double EntropyLikeFunction(const double x);

#endif  // SRC_SEGMENTATION_SEGMENTATION_UTILS_H_
