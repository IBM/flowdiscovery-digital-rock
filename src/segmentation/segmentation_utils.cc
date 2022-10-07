/**
 * \file src/segmentation/segmentation_utils.cc
 * \brief Implements several helper functions for image segmentation algorithms.
 *
 * \authors Rodrigo Neumann Barros Ferreira \<rneumann@br.ibm.com\>
 * \copyright © IBM Corp.
 * \date 2017
 *
 * This source file implements several helper functions for image segmentation algorithms.
 */

#include "src/segmentation/segmentation_utils.h"
#include <algorithm>
#include <cassert>
#include <cstdint>
#include <vector>

arma::Mat<double>::fixed<256, 3> CalculateGreyscaleStatistics(const arma::Cube<uint8_t> &xi_gs) {
/**
 * The \c CalculateGreyscaleStatistics() method calculates the normalised and accumulated
 * histograms of greyscale levels in the \f$ \xi_{gs}(\vec{r}) \f$ \c arma::Cube.
 *
 * The result is written to an \c raw_ascii file with 3 columns:
 * \f$ H_0, H_1, H_2\f$. The first, \f$ H_0(l) \f$, holds the \f$ l^{th} \f$
 * greyscale level, where \f$ l \in [0-255] \f$. The second, \f$ H_1(l) \f$,
 * represents the normalised histogram and holds the percentage of sites with
 * greyscale level \f$ l \f$. The third column, \f$ H_2(l) \f$, represents the
 * accumulated histogram and holds the percentage of sites with level
 * \f$ i \leq l \f$.
 *
 * \f{eqnarray*}{
 *    H_0(l) &=& l \\
 *    H_1(l) &=& \frac{1}{N_{site}}
 *      \int_V \delta \left[ \xi_{gs}(\vec{r})-l \right] \, \mathrm{d}\vec{r}\\
 *    H_2(l) &=& \sum_{i=0}^l H_1(i)
 * \f}
 *
 * \param[in] xi_gs     3D greyscale geometry to be segmented.
 * \retval    histogram Histogram matrix containing \f$ \left[ H_0(l), H_1(l), H_2(l) \right] \f$.
 */
  // Sweep the whole arma::Cube and counts the number of occurrences of each level
  arma::uvec counter(256, arma::fill::zeros);
  for (auto i = xi_gs.cbegin(); i != xi_gs.cend(); ++i) ++counter(*i);

  // Calculate the normalised and accumulated histogram based on counter
  arma::mat::fixed<256, 3> histogram;
  histogram.col(0) = arma::regspace<arma::vec>(0, 255);
  histogram.col(1) = arma::conv_to<arma::vec>::from(counter) / static_cast<double>(xi_gs.n_elem);
  histogram.col(2) = arma::cumsum(histogram.col(1));
  return histogram;
}  // end of CalculateGreyscaleStatistics function



void MovingAverageFilter(arma::Mat<double>::fixed<256, 3> &histogram,
                         const arma::uword window_size) {
/**
 * This function reads the image histogram \f$ H(l) \f$ and performs a central moving average on the
 * normalised histogram \f$ H_1(l) \f$. The window size \f$ W \f$ must be a odd number, so that the
 * half-width \f$ w = \frac{W-1}{2} \f$ is an integer. When approaching the edges of the histogram,
 * i. e., \f$ l = 0 \f$ and \f$ l = 255 \f$, we decrease the window to the maximum central window
 * size possible.
 *
 * \f{equation*}
 *  H_1^{\prime}(l) =
 *    \begin{cases}
 *      \frac{1}{2l+1} \sum_{i=0}^{2l} H_1(i), & \text{if} \, 0 \leq l \lt w \\
 *      \frac{1}{W} \sum_{i=l-w}^{l+w} H_1(i), & \text{if} \, w \leq l \lt 256 - w \\
 *      \frac{1}{2(255-l)+1} \sum_{i=2l-255}^{255} H_1(i), & \text{if} \, 256 - w \leq l \lt 256
 *    \end{cases}
 * \f}
 *
 * \param[in]       window_size   Window size for performing averages.
 * \param[in,out]   histogram     Greyscale level histogram.
 */
  // Check whether the window size is an odd number
  assert(window_size % 2 == 1);

  // Initialise variables
  arma::vec::fixed<256> x(histogram.col(1)), y(arma::fill::zeros);
  arma::uword half_width = (window_size - 1) / 2, adaptive_window;

  // Average histogram edges with adaptive window sizes
  for (auto l = 0U; l != half_width; ++l) {
    adaptive_window = 2 * l + 1;
    y(l)       = arma::sum(x.head(adaptive_window)) / static_cast<double>(adaptive_window);
    y(255 - l) = arma::sum(x.tail(adaptive_window)) / static_cast<double>(adaptive_window);
  }

  // Average histogram core with input window size
  for (auto l = half_width; l != 256U - half_width; ++l) {
    y(l) = arma::sum(x.subvec(l - half_width, l + half_width)) / static_cast<double>(window_size);
  }

  // Update histogram with averaged value
  histogram.col(1) = y / arma::sum(y);
  histogram.col(2) = arma::cumsum(histogram.col(1));
}  // end of MovingAverageFilter function



double CalculateMeanLevel(const arma::Mat<double>::fixed<256, 3> &histogram,
                          const arma::uword l_start,
                          const arma::uword l_end) {
/**
 * This function calculates the mean greyscale level in the histogram within the provided boundaries.
 *
 * \f[
 *    \langle l \rangle (l_\text{start}, l_\text{end}) =
 *      \frac{\sum_{l = l_\text{start}}^{l_\text{end}} H_0(l) \cdot H_1(l)}
 *           {H_2(l_\text{end}) - H_2(l_\text{start} - 1)}
 * \f]
 *
 * \note Assuming \f$ H_2(-1) \equiv 0 \f$ is required when \f$ l_\text{start} = 0 \f$.
 *
 * \param[in] histogram   Greyscale level histogram.
 * \param[in] l_start     Left interval endpoint (optional).
 * \param[in] l_end       Right interval endpoint (optional).
 * \retval    l_mean      Mean greyscale level.
 */
  // Calculate weighted average
  double numerator = arma::dot(histogram(arma::span(l_start, l_end), 0),
                               histogram(arma::span(l_start, l_end), 1));

  // Calculate normalisation factor assuming H2(-1) = 0
  double denominator = l_start == 0U ?
                       histogram(l_end, 2) :
                       histogram(l_end, 2) - histogram(l_start - 1, 2);

  return numerator / denominator;
}  // end of CalculateMeanLevel function



arma::Col<arma::uword> FindLocalMaxima(const arma::Col<double>::fixed<256> &normalised_histogram,
                                       const arma::uword l_start,
                                       const arma::uword l_end) {
/**
 * This function takes the normalised histogram \f$ H_1(l) \f$, calculates the local maxima inside
 * \f$ \{ l \in \mathbb{N} \; | \; l_{\text{start}} \lt l \lt l_{\text{end}} \} \f$ and returns an
 * \c arma::uvec with their indexes.
 *
 * \note The endpoint levels \f$ l_{\text{start}} \f$ and \f$ l_{\text{end}} \f$ should \b never be
 * considered local maxima.
 *
 * \f[
 *    l \equiv l_{\text{max}} \iff H_1(l) \gt \max \left[ H_1(l \pm 1) \right] .
 * \f]
 *
 * \param[in] normalised_histogram  Normalised histogram \f$ H_1(l) \f$.
 * \param[in] l_start               Left interval endpoint (optional).
 * \param[in] l_end                 Right interval endpoint (optional).
 * \retval    index_of_maxima       Indexes \f$ l_{\text{max}} \f$ of maxima in \f$ H_1(l) \f$.
 */
  // Create empty vector to store local maxima
  std::vector<arma::uword> index_of_maxima;

  // Sweep normalised histogram and store indexes of local maxima
  for (auto l = l_start + 1U; l != l_end; ++l) {
    double largest = std::max(normalised_histogram(l - 1U), normalised_histogram(l + 1U));
    if (normalised_histogram(l) > largest) index_of_maxima.push_back(l);
  }

  return arma::conv_to<arma::uvec>::from(index_of_maxima);
}  // end of FindLocalMaxima function



arma::Col<arma::uword> FindLocalMinima(const arma::Col<double>::fixed<256> &normalised_histogram,
                                       const arma::uword l_start,
                                       const arma::uword l_end) {
/**
 * This function takes the normalised histogram \f$ H_1(l) \f$, calculates the local minima inside
 * \f$ \{ l \in \mathbb{N} \; | \; l_{\text{start}} \lt l \lt l_{\text{end}} \} \f$ and returns an
 * \c arma::uvec with their indexes.
 *
 * \note The endpoint levels \f$ l_{\text{start}} \f$ and \f$ l_{\text{end}} \f$ should \b never be
 * considered local minima.
 *
 * \f[
 *    l \equiv l_{\text{min}} \iff H_1(l) \lt \min \left[ H_1(l \pm 1) \right] .
 * \f]
 *
 * \param[in] normalised_histogram  Normalised histogram \f$ H_1(l) \f$.
 * \param[in] l_start               Left interval endpoint (optional).
 * \param[in] l_end                 Right interval endpoint (optional).
 * \retval    index_of_minima       Indexes \f$ l_{\text{min}} \f$ of minima in \f$ H_1(l) \f$.
 */
  // Create empty vector to store local minima
  std::vector<arma::uword> index_of_minima;

  // Sweep normalised histogram and store indexes of local minima
  for (auto l = l_start + 1U; l != l_end; ++l) {
    double smallest = std::min(normalised_histogram(l - 1U), normalised_histogram(l + 1U));
    if (normalised_histogram(l) < smallest) index_of_minima.push_back(l);
  }

  return arma::conv_to<arma::uvec>::from(index_of_minima);
}  // end of FindLocalMinima function



double EntropyLikeFunction(const double x) {
/**
 * The \c EntropyLikeFunction() function calculates an entropy-like quantity \f$ \mathcal{F}(x) \f$
 * to be used by some entropy-based segmentation methods. In this version, parameter \c x is a
 * \c double, and so is the return type.
 *
 * \f[
 *    \mathcal{F}(x) = -x \log{x}
 * \f]
 *
 * \param[in] x         Function parameter \f$ x \f$.
 * \retval    function  The result of \f$ \mathcal{F}(x) = -x \log{x} \f$.
 */
  return (x != 0.0 ? -x * std::log(x) : 0.0);
}  // end of EntropyLikeFunction function
