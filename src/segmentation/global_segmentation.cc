/**
 * \file src/segmentation/global_segmentation.cc
 * \brief Implements several global segmentation algorithms.
 *
 * \authors Rodrigo Neumann Barros Ferreira \<rneumann@br.ibm.com\>
 * \copyright © IBM Corp.
 * \date 2014
 *
 * This source file implements several global segmentation algorithms. They take
 * the greyscale 3D geometry as input, perform the segmentation and return the
 * threshold value to the \c DigitalRock::PerformSegmentation() method.
 */

#include "src/segmentation/global_segmentation.h"
#include <algorithm>
#include <vector>
#include <cstdint>
#include <cassert>
#include "src/segmentation/segmentation_utils.h"

arma::sword GlobalManualSegmentation(const arma::sword user_t, arma::Cube<uint8_t> &xi_) {
/**
 * This function checks whether the \c threshold configuration parameter was
 * read at run-time and, if positive, the value is returned and the global
 * segmentation is performed at that level. If negative, an error is raised
 * telling the user to define the threshold level through the JSON file.
 *
 * \param[in]     user_t  Threshold passed by the JSON config file (\c threshold).
 * \param[in,out] xi_     3D greyscale geometry to be segmented.
 * \retval        t       Global threshold provided by the user, same as input.
 */
  // Check if the threshold level was defined by the user
  if (user_t < 0) {
    std::fprintf(stderr, "\nGLOBALMANUALSEGMENTATION SAYS: ");
    std::fprintf(stderr, "\nPlease provide the 'threshold' parameter!\n");
    std::exit(-1);
  }

  // Copy the user-provided threshold to t
  uint8_t t = static_cast<uint8_t>(user_t);

  // Perform segmentation at the user-defined threshold
  xi_ = arma::conv_to< arma::Cube<uint8_t> >::from(xi_ > t);

  return t;
}  // end of GlobalManualSegmentation function



arma::sword GlobalIsoDataSegmentation(const arma::mat::fixed<256, 3>  &hist_,
                                      arma::Cube<uint8_t>             &xi_) {
/**
 * This function reads the greyscale level histogram and performs the global IsoData algorithm
 * (Ridler & Calvard, 1978) to calculate the optimal threshold level \f$ t \f$.
 *
 * Starting from the lowest greyscale level in the image as an initial guess for the threshold
 * \f$ t_0 \f$, the algorithm calculates the mean background and mean foreground levels
 *
 * \f{eqnarray}{
 *    \langle l \rangle_\text{bg} = \frac{1}{H_2(t_n-1)} \sum_{l=0}^{t_n-1} l \cdot H_1(l) \\
 *    \langle l \rangle_\text{fg} = \frac{1}{1-H_2(t_n)} \sum_{l=t_n+1}^{255} l \cdot H_1(l) \\
 * \f}
 *
 * and iterates the threshold \f$ t_{n+1} = t_n + 1 \f$ until
 *
 * \f[
 *    t_n = \frac{\langle l \rangle_\text{bg} + \langle l \rangle_\text{fg}}{2} \, .
 * \f]
 *
 * \param[in]     hist_ Greyscale level histogram as calculated by \c CalculateHistogram().
 * \param[in,out] xi_   3D greyscale geometry to be segmented.
 * \retval        t     Optimal global threshold calculated by the algorithm.
 */
  arma::uword target_t, tentative_t = arma::as_scalar(arma::find(hist_.col(1) > 0, 1));

  // Iterate the algorithm until convergence
  do {
    // Increment tentative threshold
    ++tentative_t;

    // Calculate background and foreground mean levels
    arma::uword bg_mean = static_cast<arma::uword>(CalculateMeanLevel(hist_, 0, tentative_t - 1));
    arma::uword fg_mean = static_cast<arma::uword>(CalculateMeanLevel(hist_, tentative_t + 1, 255));

    // Calculate target level as midpoint between background and foreground mean levels
    target_t = std::round((bg_mean + fg_mean) / 2.0);
  } while (tentative_t != target_t);

  // Perform segmentation at the calculated threshold
  uint8_t t = static_cast<uint8_t>(tentative_t);
  xi_ = arma::conv_to< arma::Cube<uint8_t> >::from(xi_ > t);

  return t;
}  // end of GlobalIsoDataSegmentation function



arma::sword GlobalOtsuSegmentation(const arma::mat::fixed<256, 3> &hist_,
                                   arma::Cube<uint8_t>            &xi_) {
/**
 * This function reads the greyscale level histogram and performs the global
 * Otsu's algorithm (Otsu, 1979) to calculate the optimal threshold level.
 *
 * \param[in]     hist_   Greyscale level histogram as calculated by \c CalculateHistogram().
 * \param[in,out] xi_     3D greyscale geometry to be segmented.
 * \retval        t       Optimal global threshold calculated by the algorithm.
 *
 * The Otsu's algorithm is based on an extensive search for the threshold level
 * that minimizes the intra-class variance or, conversely, maximizes the
 * inter-class variance defined as
 *
 * \f[
 *    \sigma_b^2(t) = H_2(t)\; \left( 1-H_2(t) \right)\;
 *    \left[\, \langle l \rangle_\text{bg}(t) - \langle l \rangle_\text{fg}(t)\, \right]^2
 * \f]
 *
 * where \f$ \langle l \rangle_\text{bg}(t) \f$ and \f$ \langle l \rangle_\text{fg}(t) \f$
 * are the background and foreground mean greyscale levels, respectively, for a
 * threshold set at \f$ t \f$.
 *
 * While \f$ \langle l \rangle_\text{bg}(t) \f$ can be easily calculated through
 *
 * \f[
 *    \langle l \rangle_\text{bg}(t) = \frac{1}{H_2(t)} \sum_{l=0}^{t} l \cdot H_1(l)
 * \f]
 *
 * \f$ \langle l \rangle_\text{fg}(t) \f$ is calculated faster by exploiting its
 * relationship with the global mean \f$ \langle l \rangle \f$
 *
 * \f[
 *    \langle l \rangle_\text{fg}(t) =
 *      \frac{\langle l \rangle - \langle l \rangle_\text{bg}(t)\, H_2(t)}{1-H_2(t)}.
 * \f]
 */
  // Calculate mean greyscale level
  double mean = CalculateMeanLevel(hist_);

  // Calculate background mean greyscale level
  arma::vec::fixed<256> bg_mean = arma::cumsum(hist_.col(0) % hist_.col(1))
                                / hist_.col(2);

  // Calculate foreground mean greyscale level
  arma::vec::fixed<256> fg_mean = (mean - bg_mean % hist_.col(2))
                                / (1 - hist_.col(2));

  // Calculate inter-class variance
  arma::vec::fixed<256> inter_class_var = hist_.col(2) % (1 - hist_.col(2))
                                        % arma::square(bg_mean - fg_mean);

  // Calculating threshold by maximizing the inter-class variance
  arma::uword idx = arma::index_max(inter_class_var);

  // Perform segmentation at the calculated threshold
  uint8_t t = static_cast<uint8_t>(idx);
  xi_ = arma::conv_to< arma::Cube<uint8_t> >::from(xi_ > t);

  return t;
}  // end of GlobalOtsuSegmentation function



arma::sword GlobalMeanSegmentation(const arma::mat::fixed<256, 3> &hist_,
                                   arma::Cube<uint8_t>            &xi_) {
/**
 * This function reads the image histogram and returns the mean greyscale level
 * as the global threshold (Glasbey, 1993).
 *
 * \param[in]     hist_ Greyscale level histogram as calculated by \c CalculateHistogram().
 * \param[in,out] xi_   3D greyscale geometry to be segmented.
 * \retval        t     Optimal global threshold calculated by the algorithm.
 *
 * The algorithm sets the global threshold as the mean greyscale level, as in
 *
 * \f[
 *    t = \langle l \rangle = \sum_{l=0}^{255} l \cdot H_1(l)\, .
 * \f]
 */
  // Calculate mean greyscale level
  double mean = CalculateMeanLevel(hist_);

  // Perform segmentation at the calculated threshold
  uint8_t t = static_cast<uint8_t>(mean);
  xi_ = arma::conv_to< arma::Cube<uint8_t> >::from(xi_ > t);

  return t;
}  // end of GlobalMeanSegmentation function



arma::sword GlobalMedianSegmentation(const arma::mat::fixed<256, 3> &hist_,
                                     arma::Cube<uint8_t>            &xi_) {
/**
 * This function reads the image histogram and returns the median of the
 * greyscale level as the global threshold (Doyle, 1962 and Glasbey, 1993). It
 * is also known as the "Percentile" method.
 *
 * \param[in]     hist_ Greyscale level histogram as calculated by \c CalculateHistogram().
 * \param[in,out] xi_   3D greyscale geometry to be segmented.
 * \retval        t     Optimal global threshold calculated by the algorithm.
 *
 * The algorithm sets the global threshold as the median greyscale level, by
 * solving
 *
 * \f[
 *    H_2(t) = 0.5\, .
 * \f]
 */
  // Calculate the median of the greyscale level histogram
  arma::uword closest_to_half = arma::index_min(arma::abs(hist_.col(2) - 0.5));
  uint8_t t = static_cast<uint8_t>(closest_to_half);

  // Perform segmentation at the calculated threshold
  xi_ = arma::conv_to< arma::Cube<uint8_t> >::from(xi_ > t);

  return t;
}  // end of GlobalMedianSegmentation function



arma::sword GlobalLiSegmentation(const arma::mat::fixed<256, 3> &hist_,
                                 arma::Cube<uint8_t>            &xi_) {
/**
 * This function reads the image histogram and returns the threshold that minimises the cross
 * entropy (Li & Lee, 1993). This is actually the iterative version of the algorithm (Li & Tam, 1998).
 *
 * Starting from the mean greyscale level as an initial guess for the threshold
 *
 * \f[
 *    t_0 = \langle l \rangle = \sum_{l=0}^{255} l \cdot H_1(l)\, ,
 * \f]
 *
 * the algorithm calculates the next estimate for the threshold \f$ t_{n+1} \f$
 * depending on the previous estimate \f$ t_{n} \f$, according to
 *
 * \f{eqnarray}{
 *    \langle l \rangle_\text{bg} &=& \frac{1}{H_2(t_n)} \sum_{l=0}^{t_n} l \cdot H_1(l) \\
 *    \langle l \rangle_\text{fg} &=& \frac{1}{1-H_2(t_n)} \sum_{l=t_n+1}^{255} l \cdot H_1(l) \\
 *    t_{n+1} &=& \frac{\langle l \rangle_\text{fg} - \langle l \rangle_\text{bg}}
 *        {\ln{\langle l \rangle_\text{fg}} - \ln{\langle l \rangle_\text{bg}}}
 * \f}
 *
 * until \f$ | t_{n+1} - t_n | < 1 \f$. The expressions for
 * \f$ \langle l \rangle_\text{bg} \f$ and \f$ \langle l \rangle_\text{fg} \f$ represent
 * the average greyscale level at the background and foreground, respectively,
 * according to the previous threshold level set at \f$ t_n \f$.
 *
 * \param[in]     hist_ Greyscale level histogram as calculated by \c CalculateHistogram().
 * \param[in,out] xi_   3D greyscale geometry to be segmented.
 * \retval        t     Optimal global threshold calculated by the algorithm.
 */
  // Start with the average greyscale level as the threshold
  double old_t, new_t = std::ceil(CalculateMeanLevel(hist_));

  // Iterate the algorithm until it converges
  do {
    old_t = new_t;

    // Calculate background and foreground mean levels
    double bg_mean = CalculateMeanLevel(hist_, 0, old_t);
    double fg_mean = CalculateMeanLevel(hist_, old_t + 1, 255);

    // Calculate new threshold using the minimum cross entropy method
    new_t = std::round((fg_mean - bg_mean) / (std::log(fg_mean) - std::log(bg_mean)));
  } while (std::abs(new_t - old_t) > 0);

  // Perform segmentation at the calculated threshold
  uint8_t t = static_cast<uint8_t>(new_t);
  xi_ = arma::conv_to< arma::Cube<uint8_t> >::from(xi_ > t);

  return t;
}  // end of GlobalLiSegmentation function



arma::sword GlobalMinimumSegmentation(const arma::mat::fixed<256, 3> &hist_,
                                      arma::Cube<uint8_t>            &xi_) {
/**
 * This function reads the image histogram and returns the local minimum between two local maxima as
 * the greyscale threshold according to Prewitt & Mendelsohn (1966). In case there are more than two
 * local maxima, the histogram is smoothed using \c MovingAverageFilter(hist_) until there are only
 * two local maxima
 *
 * \param[in]     hist_ Greyscale level histogram as calculated by \c CalculateHistogram().
 * \param[in,out] xi_   3D greyscale geometry to be segmented.
 * \retval        t     Optimal global threshold calculated by the algorithm.
 */
  // Copy histogram to temporary non-const arma::mat
  arma::mat::fixed<256, 3> hist = hist_;

  // Smooth histogram until there are only two local maxima
  while (FindLocalMaxima(hist.col(1)).n_elem > 2U) {
    MovingAverageFilter(hist);
  }

  // Make sure there are, precisely, two local maxima
  arma::uvec local_maxima = FindLocalMaxima(hist.col(1));
  assert(local_maxima.n_elem == 2U);

  // Calculate threshold as the local minimum between two local maxima
  arma::uvec local_minimum = FindLocalMinima(hist.col(1), local_maxima.min(), local_maxima.max());
  uint8_t t = static_cast<uint8_t>(arma::as_scalar(local_minimum));

  // Perform segmentation at the calculated threshold
  xi_ = arma::conv_to< arma::Cube<uint8_t> >::from(xi_ > t);

  return t;
}  // end of GlobalMinimumSegmentation function



arma::sword GlobalIntermodesSegmentation(const arma::mat::fixed<256, 3> &hist_,
                                         arma::Cube<uint8_t>            &xi_) {
/**
 * This function reads the image histogram and returns the position of the midpoint of the two
 * largest maxima as the greyscale threshold according to Prewitt & Mendelsohn (1966). In case there
 * are more than two local maxima, the histogram is smoothed using \c MovingAverageFilter(hist_)
 * until there are only two local maxima
 *
 * \param[in]     hist_ Greyscale level histogram as calculated by \c CalculateHistogram().
 * \param[in,out] xi_   3D greyscale geometry to be segmented.
 * \retval        t     Optimal global threshold calculated by the algorithm.
 */
  // Copy histogram to temporary non-const arma::mat
  arma::mat::fixed<256, 3> hist = hist_;

  // Smooth histogram until there are only two local maxima
  while (FindLocalMaxima(hist.col(1)).n_elem > 2U) {
    MovingAverageFilter(hist);
  }

  // Make sure there are, precisely, two local maxima
  arma::uvec local_maxima = FindLocalMaxima(hist.col(1));
  assert(local_maxima.n_elem == 2U);

  // Calculate threshold as the midpoint between two local maxima
  arma::uword midpoint = arma::mean(local_maxima);
  uint8_t t = static_cast<uint8_t>(midpoint);

  // Perform segmentation at the calculated threshold
  xi_ = arma::conv_to< arma::Cube<uint8_t> >::from(xi_ > t);

  return t;
}  // end of GlobalIntermodesSegmentation function



arma::sword GlobalMomentsSegmentation(const arma::mat::fixed<256, 3> &hist_,
                                      arma::Cube<uint8_t>            &xi_) {
/**
 * This function reads the image histogram, calculates the three first moments
 * of the greyscale distribution and determines the threshold level necessary in
 * order to preserve the moments in the thresholded image (Tsai, 1985).
 *
 * \param[in]     hist_ Greyscale level histogram as calculated by \c CalculateHistogram().
 * \param[in,out] xi_   3D greyscale geometry to be segmented.
 * \retval        t     Optimal global threshold calculated by the algorithm.
 *
 * The algorithm starts by calculating the three first moments of the greyscale
 * distribution, as in
 *
 * \f{eqnarray}{
 *    m_1 &=& \langle l   \rangle = \sum_{l=0}^{255} l   \cdot H_1(l)\, , \\
 *    m_2 &=& \langle l^2 \rangle = \sum_{l=0}^{255} l^2 \cdot H_1(l)\, , \\
 *    m_3 &=& \langle l^3 \rangle = \sum_{l=0}^{255} l^3 \cdot H_1(l)\, .
 * \f}
 *
 * Then, the moments of the original image are equated to those of the
 * thresholded image and the system of equations are solved for the threshold
 * level. The following auxiliary variables are calculated:
 *
 * \f{eqnarray}{
 *    x &=& \frac{m_1 m_3 - m_2^2}{m_2 - m_1^2}\, , \\
 *    y &=& \frac{m_1 m_2 - m_3}{m_2 - m_1^2}\, , \\
 *    z &=& \frac{1}{2} - \frac{m_1 + y/2}{\sqrt{y^2 - 4 x}}\, .
 * \f}
 *
 * The threshold level \f$ t \f$ is, finally, chosen as the \f$ z \f$-tile of
 * the distribution by solving the equation
 *
 * \f[
 *   H_2(t) = z\, .
 * \f]
 */
  // Calculate three first moments of the histogram
  double m1 = arma::dot(hist_.col(0), hist_.col(1));
  double m2 = arma::dot(arma::square(hist_.col(0)), hist_.col(1));
  double m3 = arma::dot(arma::pow(hist_.col(0), 3), hist_.col(1));

  // Calculate auxiliary variables
  double x = (m1 * m3 - m2 * m2) / (m2 - m1 * m1);
  double y = (m1 * m2 - m3) / (m2 - m1 * m1);
  double z = 0.5 - (m1 + 0.5 * y) / std::sqrt(y * y - 4 * x);

  // Perform segmentation at the calculated threshold
  uint8_t t = static_cast<uint8_t>(arma::as_scalar(arma::find(hist_.col(2) >= z, 1)));
  xi_ = arma::conv_to< arma::Cube<uint8_t> >::from(xi_ > t);

  return t;
}  // end of GlobalMomentsSegmentation function



arma::sword GlobalMaxShannonEntropySegmentation(const arma::mat::fixed<256, 3> &hist_,
                                                arma::Cube<uint8_t>            &xi_) {
/**
 * This function reads the greyscale level histogram and returns the threshold
 * that maximises the total Shannon entropy of the segmented image
 * (Kapur et al., 1985).
 *
 * \param[in]     hist_   Greyscale level histogram as calculated by \c CalculateHistogram().
 * \param[in,out] xi_     3D greyscale geometry to be segmented.
 * \retval        t       Optimal global threshold calculated by the algorithm.
 *
 * The MaxShannonEntropy algorithm uses the helper function \c EntropyLikeFunction() defined as
 * \f$ \mathcal{F}(x) = -x \log{x} \f$. The threshold \f$ t \f$ is defined as the greyscale level
 * that maximises the total Shannon entropy of the thresholded image
 *
 * \f[
 *    S(t) = \sum_{l=0}^{t} \mathcal{F} \left[ \frac{H_1(l)}{H_2(t)} \right] +
 *           \sum_{l=t + 1}^{255} \mathcal{F} \left[ \frac{H_1(l)}{1 - H_2(t)} \right] \, .
 * \f]
 */
  // Copy histogram matrix columns to vectors to make expressions easier to read
  arma::vec::fixed<256> H1 = hist_.col(1);
  arma::vec::fixed<256> H2 = hist_.col(2);

  // Identify first and last bins with non-zero values
  arma::uvec threshold_range = arma::find(H2 > arma::datum::eps && 1.0 - H2 > arma::datum::eps);

  // Initialise values to be optimised
  double max_entropy = -arma::datum::inf;
  uint8_t t = 0U;

  // Sweep valid threshold values
  for (const auto &threshold : threshold_range) {
    // Calculate total entropy for given threshold
    double total_entropy = 0.0;
    for (arma::uword l = 0U; l != 256U; ++l) {
      double argument = l <= threshold ? H1(l) / H2(threshold) :         // background
                                         H1(l) / (1.0 - H2(threshold));  // foreground
      total_entropy += EntropyLikeFunction(argument);
    }

    // Check if current threshold maximises the entropy
    if (max_entropy < total_entropy) {
      max_entropy = total_entropy;
      t = static_cast<uint8_t>(threshold);
    }
  }

  // Perform segmentation at the optimised threshold
  xi_ = arma::conv_to< arma::Cube<uint8_t> >::from(xi_ > t);

  return t;
}  // end of GlobalMaxShannonEntropySegmentation function



arma::sword GlobalShanbhagSegmentation(const arma::mat::fixed<256, 3> &hist_,
                                       arma::Cube<uint8_t>            &xi_) {
/**
 * This function reads the greyscale level histogram and returns the threshold that minimises the
 * image information measure (Shanbhag, 1994).
 *
 * The Shanbhag algorithm calculates the information measure defined as
 *
 * \f[
 *    I(t) =  -\sum_{l=0}^{t} \frac{H_1(l) \cdot \log{ \mu^t_{\text{b}}(l) }}{H_2(t)}
 *            +\sum_{l=t+1}^{255} \frac{H_1(l) \cdot \log{ \mu^t_{\text{f}}(l) }}{1-H_2(t)}\, ,
 * \f]
 *
 * where \f$ \mu^t_{\text{b}}(l) \f$ and \f$ \mu^t_{\text{f}}(l) \f$ are the fuzzy membership
 * coefficients of greyscale level \f$ l \f$ with respect the background and foreground when the
 * threshold is set at \f$ t \f$.
 *
 * Those fuzzy membership coefficients are calculated as
 *
 * \f{eqnarray}{
 *    \mu^t_{\text{b}}(l) &=& 1 - \frac{1}{2} \left[ \frac{H_2(l) - H_1(l)}{H_2(t)} \right]\, , \\
 *    \mu^t_{\text{f}}(l) &=& 1 - \frac{1}{2} \left[ \frac{1 - H_2(l)}{1 - H_2(t)} \right]\, .
 * \f}
 *
 * The threshold level is, then, chosen as to minimise \f$ | I(t) | \f$.
 *
 * \param[in]     hist_   Greyscale level histogram as calculated by \c CalculateHistogram().
 * \param[in,out] xi_     3D greyscale geometry to be segmented.
 * \retval        t       Optimal global threshold calculated by the algorithm.
 */
  // Copy histogram matrix columns to vectors to make expressions easier to read
  arma::vec::fixed<256> H1 = hist_.col(1);
  arma::vec::fixed<256> H2 = hist_.col(2);

  // Create image information vector as defined by Shanbhag
  arma::vec I(256, arma::fill::zeros);

  // Sweep all possible threshold and calculate corresponding image information
  for (arma::uword t = 0U; t != 256U; ++t) {
    // Calculate membership coefficients of background and foreground classes at threshold t
    arma::vec::fixed<256> bg_mu = 1.0 - (H2 - H1) / (2.0 * H2(t));
    arma::vec::fixed<256> fg_mu = 1.0 - (1.0 - H2) / 2.0 / (1.0 - H2(t));

    // Calculate corresponding image information at threshold t
    for (arma::uword l = 0U; l != 256U; ++l) {
      I(t) += l <= t ? -H1(l) * std::log(bg_mu(l)) / H2(t)
                     : +H1(l) * std::log(fg_mu(l)) / (1.0 - H2(t));
    }
  }

  // Obtain threshold by minimising the information measure
  arma::uword threshold = arma::index_min(arma::abs(I));

  // Perform segmentation at the calculated threshold
  uint8_t t = static_cast<uint8_t>(threshold);
  xi_ = arma::conv_to< arma::Cube<uint8_t> >::from(xi_ > t);

  return t;
}  // end of GlobalShanbhagSegmentation function
