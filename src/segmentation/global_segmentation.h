/**
 * \file src/segmentation/global_segmentation.h
 * \brief Function declaration for several global segmentation algorithms.
 *
 * \authors Rodrigo Neumann Barros Ferreira \<rneumann@br.ibm.com\>
 * \copyright © IBM Corp.
 * \date 2014
 *
 * This header file contains the function declarations for several global
 * segmentation algorithms that calculate the optimal threshold level for
 * segmenting the sample into pore and solid space.
 */

#ifndef SRC_SEGMENTATION_GLOBAL_SEGMENTATION_H_
#define SRC_SEGMENTATION_GLOBAL_SEGMENTATION_H_

#include <armadillo>
#include <cstdint>

/// Global Manual segmentation
arma::sword GlobalManualSegmentation(const arma::sword user_t, arma::Cube<uint8_t> &xi_);

/// Global IsoData segmentation algorithm
arma::sword GlobalIsoDataSegmentation(const arma::mat::fixed<256, 3>  &hist_,
                                      arma::Cube<uint8_t>             &xi_);

/// Global Otsu segmentation algorithm
arma::sword GlobalOtsuSegmentation(const arma::mat::fixed<256, 3> &hist_,
                                   arma::Cube<uint8_t>            &xi_);

/// Global Mean segmentation algorithm
arma::sword GlobalMeanSegmentation(const arma::mat::fixed<256, 3> &hist_,
                                   arma::Cube<uint8_t>            &xi_);

/// Global Median segmentation algorithm
arma::sword GlobalMedianSegmentation(const arma::mat::fixed<256, 3> &hist_,
                                     arma::Cube<uint8_t>            &xi_);

/// Global Li segmentation algorithm
arma::sword GlobalLiSegmentation(const arma::mat::fixed<256, 3> &hist_,
                                 arma::Cube<uint8_t>            &xi_);

/// Global Minimum segmentation algorithm
arma::sword GlobalMinimumSegmentation(const arma::mat::fixed<256, 3> &hist_,
                                      arma::Cube<uint8_t>            &xi_);

/// Global Intermodes segmentation algorithm
arma::sword GlobalIntermodesSegmentation(const arma::mat::fixed<256, 3> &hist_,
                                         arma::Cube<uint8_t>            &xi_);

/// Global Moments segmentation algorithm
arma::sword GlobalMomentsSegmentation(const arma::mat::fixed<256, 3> &hist_,
                                      arma::Cube<uint8_t>            &xi_);

/// Global MaxShannonEntropy segmentation algorithm
arma::sword GlobalMaxShannonEntropySegmentation(const arma::mat::fixed<256, 3> &hist_,
                                                arma::Cube<uint8_t>            &xi_);

/// Global Shanbhag segmentation algorithm
arma::sword GlobalShanbhagSegmentation(const arma::mat::fixed<256, 3> &hist_,
                                       arma::Cube<uint8_t>            &xi_);

#endif  // SRC_SEGMENTATION_GLOBAL_SEGMENTATION_H_
