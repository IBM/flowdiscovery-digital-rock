/**
 * \file src/digital_rock/digital_rock.cc
 * \brief Contains the DigitalRock class methods.
 *
 * \authors Rodrigo Neumann Barros Ferreira \<rneumann@br.ibm.com\>
 * \author Giulia Duncan Coutinho \<coutingi@br.ibm.com\>
 * \copyright © IBM Corp.
 * \date 2014
 *
 * This source file contains the \c DigitalRock class  methods. The \c LoadFile()
 * method loads the input file specified by the JSON file to an \c arma::Cube,
 * checks if it was loaded properly and reshapes it. The \c CalculateHistogram()
 * method calculates the greyscale histogram of the input file, later to be
 * used for the image segmentation. The \c PerformSegmentation() method calculates
 * the segmentation threshold and performs the binarization of the input file.
 * The \c CalculateVolFrac() method counts the percentage of sites above and
 * below the threshold after the segmentation. The \c DetermineMorphology() method
 * identifies the connected part of the pore network, i.e. the "true" porosity,
 * and locates the voxels of the rock boundary.
 */

#include "src/digital_rock/digital_rock.h"
#include <cassert>
#include <vector>
#include <cstdint>
#include "src/morphology/morphology.h"
#include "src/segmentation/segmentation_utils.h"
#include "src/segmentation/global_segmentation.h"
#include "src/cluster_label/cluster_label.h"
#include "src/skeleton/centerline_includes.h"

void DigitalRock::LoadFile(const SetupConfig &setup_cfg) {
/**
 * The \c LoadFile() method loads the input file specified by the JSON file to an
 * \c arma::Cube. Afterwards, it checks if the provided shape matches the number
 * of elements in the \c arma::Cube. If negative, it could be necessary to
 * change the \c arma::Cube type or correct the shape provided. Finally, it
 * reshapes the greyscale \c arma::Cube \f$ \xi_{gs}(\vec{r}) \f$ to the correct
 * dimensions.
 *
 * \param[in] setup_cfg  Struct containing setup-related configuration parameters.
 */
  // Load input file as an arma::Cube
  xi_.load(setup_cfg.folder + "/" + setup_cfg.input_file);

  // Calculate the total number of elements defined by setup_cfg.shape
  arma::uword num_elem = arma::prod(setup_cfg.shape);

  // If 'shape' does not match the number of elements in the arma::Cube
  if (num_elem != xi_.n_elem) {
    std::fprintf(stderr, "\nDIGITALROCK::LOADFILE SAYS: ");
    std::fprintf(stderr, "Please check arma::Cube type and shape. ");
    std::fprintf(stderr, "The number of elements does not match the provided ");
    std::fprintf(stderr, "'shape' parameter.\n");
    std::exit(-1);
  }

  // Reshape the arma::Cube if its shape is consistent with its size
  xi_.reshape(setup_cfg.shape(0), setup_cfg.shape(1), setup_cfg.shape(2));

  // Print output messages
  std::printf("\nDIGITALROCK::LOADFILE SAYS: ");
  std::printf("The %s file was loaded correctly ", setup_cfg.input_file.c_str());
  std::printf("as a 3D geometry with the following dimensions: \n");
  std::printf("%llu rows\t along x\n", xi_.n_rows);
  std::printf("%llu columns\t along y\n", xi_.n_cols);
  std::printf("%llu slices\t along z\n", xi_.n_slices);
}  // end of DigitalRock::LoadFile() method



void DigitalRock::LoadBinaryFile(const SetupConfig &setup_cfg) {
/**
 * The \c LoadBinaryFile() method loads the \c binary_image.raw file to an
 * \c arma::Cube. Afterwards, it checks if the provided shape matches the number
 * of elements in the \c arma::Cube. If negative, it could be necessary to
 * change the \c arma::Cube type or correct the shape provided. Finally, it
 * reshapes the binary \c arma::Cube \f$ \xi_{bin}(\vec{r}) \f$ to the correct
 * dimensions.
 *
 * \param[in] setup_cfg  Struct containing setup-related configuration parameters.
 */
  // Load input file as an arma::Cube
  xi_.load(setup_cfg.folder + "/binary_image.raw");

  // Calculate the total number of elements defined by setup_cfg.shape
  arma::uword num_elem = arma::prod(setup_cfg.shape);

  // If 'shape' does not match the number of elements in the arma::Cube
  if (num_elem != xi_.n_elem) {
    std::fprintf(stderr, "\nDIGITALROCK::LOADBINARYFILE SAYS: ");
    std::fprintf(stderr, "Please check arma::Cube type and shape. ");
    std::fprintf(stderr, "The number of elements does not match the provided ");
    std::fprintf(stderr, "'shape' parameter.\n");
    std::exit(-1);
  }

  // Reshape the arma::Cube if its shape is consistent with its size
  xi_.reshape(setup_cfg.shape(0), setup_cfg.shape(1), setup_cfg.shape(2));

  // Print output messages
  std::printf("\nDIGITALROCK::LOADBINARYFILE SAYS: ");
  std::printf("The binary_image.raw file was loaded correctly ");
  std::printf("as a 3D geometry with the following dimensions: \n");
  std::printf("%llu rows\t along x\n", xi_.n_rows);
  std::printf("%llu columns\t along y\n", xi_.n_cols);
  std::printf("%llu slices\t along z\n", xi_.n_slices);
}  // end of DigitalRock::LoadBinaryFile() method



void DigitalRock::CalculateHistogram(const SetupConfig &setup_cfg) {
/**
 * The \c CalculateHistogram() method calculates the normalised and accumulated
 * histograms of greyscale levels in the \f$ \xi_{gs}(\vec{r}) \f$ \c arma::Cube
 * according to \c CalculateGreyscaleStatistics().
 *
 * \param[in] setup_cfg  Struct containing setup-related configuration parameters.
 */
  // Calculate the normalised and accumulated histograms
  hist_ = CalculateGreyscaleStatistics(xi_);

  // Print output messages if necessary
  std::printf("\nDIGITALROCK::CALCULATEHISTOGRAM SAYS: ");
  std::printf("Greyscale histogram calculated.\n");

  // Write histogram output file to disk
  hist_.save(setup_cfg.folder + "/histogram.dat", arma::raw_ascii);
}  // end of DigitalRock::CalculateHistogram() method



void DigitalRock::PerformSegmentation(const SegmentationConfig &segmentation_cfg) {
/**
 * The \c PerformSegmentation() method performs the segmentation of the original 3D geometry from
 * greyscale to a binary format (given a segmentation algorithm) and save it to disk.
 *
 * \param[in] segmentation_cfg  Struct containing segmentation-related configuration parameters.
 *
 * The segmentation algorithms, responsible for calculating the threshold
 * level, are implemented in the \c src/segmentation/ source files. Once the
 * threshold level \f$ t \f$ is set, the binary geometry
 * \f$ \xi_{bin}(\vec{r}) \f$ is created from the greyscale geometry
 * \f$ \xi_{gs}(\vec{r}) \f$ according to
 *
 * \f{equation*}
 * \xi_{bin}(\vec{r}) =
 *    \begin{cases}
 *      0~\text{(pore)},  & \text{if } \xi_{gs}(\vec{r}) \leq t \\
 *      1~\text{(solid)}, & \text{if } \xi_{gs}(\vec{r})   >  t
 *    \end{cases}
 * \f}
 *
 * \image html  docs/xi_gs2bin.png "Greyscale (left) and binary (right) image"
 * \image latex docs/xi_gs2bin.png "Greyscale (left) and binary (right) image" width=10cm
 */
  // No image segmentation required
  if (segmentation_cfg.method == "none") {
    // Print output messages if necessary
    std::printf("\nDIGITALROCK::PERFORMSEGMENTATION SAYS: ");
    std::printf("Segmentation was not performed.\n");

  // Global segmentation methods
  } else if (segmentation_cfg.method.at(0) == 'g') {
    arma::sword global_threshold = 0;

    if (segmentation_cfg.method == "global_manual")
      global_threshold = GlobalManualSegmentation(segmentation_cfg.threshold, xi_);

    else if (segmentation_cfg.method == "global_isodata")
      global_threshold = GlobalIsoDataSegmentation(hist_, xi_);

    else if (segmentation_cfg.method == "global_otsu")
      global_threshold = GlobalOtsuSegmentation(hist_, xi_);

    else if (segmentation_cfg.method == "global_mean")
      global_threshold = GlobalMeanSegmentation(hist_, xi_);

    else if (segmentation_cfg.method == "global_median")
      global_threshold = GlobalMedianSegmentation(hist_, xi_);

    else if (segmentation_cfg.method == "global_li")
      global_threshold = GlobalLiSegmentation(hist_, xi_);

    else if (segmentation_cfg.method == "global_minimum")
      global_threshold = GlobalMinimumSegmentation(hist_, xi_);

    else if (segmentation_cfg.method == "global_intermodes")
      global_threshold = GlobalIntermodesSegmentation(hist_, xi_);

    else if (segmentation_cfg.method == "global_moments")
      global_threshold = GlobalMomentsSegmentation(hist_, xi_);

    else if (segmentation_cfg.method == "global_maxshannonentropy")
      global_threshold = GlobalMaxShannonEntropySegmentation(hist_, xi_);

    else if (segmentation_cfg.method == "global_shanbhag")
      global_threshold = GlobalShanbhagSegmentation(hist_, xi_);

    // Make sure one of the methods ran successfully
    assert(global_threshold);

    // Print output messages
    std::printf("\nDIGITALROCK::PERFORMSEGMENTATION SAYS: ");
    std::printf("Segmentation performed at level %lld ", global_threshold);
    std::printf("using the %s algorithm.\n", segmentation_cfg.method.c_str());

  // Local segmentation methods
  } else if (segmentation_cfg.method.at(0) == 'l') {
    arma::sword avg_threshold = 0;

    // Make sure one of the methods ran successfully
    assert(avg_threshold);

    // Print output messages if necessary
    std::printf("\nDIGITALROCK::PERFORMSEGMENTATION SAYS: ");
    std::printf("Segmentation performed locally at an average level ");
    std::printf("%lld using the ", avg_threshold);
    std::printf("%s algorithm.\n", segmentation_cfg.method.c_str());
  }

  // Save binary image to disk
  xi_.save(segmentation_cfg.folder + "/binary_image.raw", arma::raw_binary);
}  // end of DigitalRock::PerformSegmentation() method



void DigitalRock::CalculateVolFrac(const SetupConfig &setup_cfg) const {
/**
 * The \c CalculateVolFrac() method determines the fraction \f$ \phi \f$ of sites
 * in the binary \c arma::Cube \f$ \xi_{bin}(\vec{r}) \f$ that corresponds to 0.
 *
 * \f[
 *    \phi = \frac{1}{N_{site}}
 *      \int_V \delta \left[ \xi_{bin}(\vec{r}) \right] \,\mathrm{d}\vec{r}\,,
 * \f]
 *
 * \param[in] setup_cfg         Struct containing setup-related configuration parameters.
 */
  // Calculate volume fraction as a percentage of total sites
  double vol_frac = 100 * static_cast<double>(arma::accu(xi_== 0)) / xi_.n_elem;

  // Print output messages if necessary
    std::printf("\nDIGITALROCK::CALCULATEVOLFRAC SAYS: ");
    std::printf("The %s file has ", setup_cfg.input_file.c_str());
    std::printf("%lf%% of its sites flagged as 0.\n", vol_frac);
}  // end of DigitalRock::CalculateVolFrac() method



void DigitalRock::DetermineMorphology(const SetupConfig &setup_cfg) {
/**
 * The \c DetermineMorphology() method finds the connected part of the pore
 * structure, the one that allows fluid flow across the system. Using the
 * terminology of Percolation Theory, it determines the "infinite cluster".
 *
 * \param[in] setup_cfg       Struct containing setup-related configuration parameters.
 *
 * It starts by locating and labelling clusters of sites flagged as "0".
 * The \c arma::ucube \f$ c(\vec{r}) \f$ with the cluster labels is initialised
 * as a collection of 0's and 1's indicating the flagged sites, as a result of
 * the operation below.
 *
 * \f{equation*}
 * c(\vec{r}) =
 *    \begin{cases}
 *      1~\text{(pore)},  & \text{if } \xi_{bin}(\vec{r}) = 0 \\
 *      0~\text{(solid)}, & \text{if } \xi_{bin}(\vec{r}) = 1
 *    \end{cases}
 * \f}
 *
 * As it leaves the \c EnhancedHoshenKopelman() cluster labelling algorithm,
 * the \f$ c(\vec{r}) \f$ \c arma::ucube contains flagged sites corresponding to
 * the infinite cluster.
 * The volume fraction of the infinite cluster is a direct measure of the
 * "true" or "connected" porosity of the sample, that which allows fluid to flow.
 * Additional properties of the infinite cluster, such as its surface-to-volume
 * ratio and fractal dimension are also calculated, since they give valuable
 * insights regarding the system. The binary geometry is finally updated by
 * removing the non-connected pores and by differentiating surface and bulk
 * solid voxels.
 *
 * The cluster labelling and related algorithms, responsible for updating
 * \f$ c(\vec{r}) \f$, are implemented in the \c cluster_label.cc source file.
 * The morphology analysis algorithms, responsible for calculating the fractal
 * dimension and the surface-to-volume are in the \c morphology.cc source file.
 */
  // Print output messages if necessary
  std::printf("\nDIGITALROCK::DETERMINEMORPHOLOGY SAYS: ");
  std::printf("Cluster labelling performed by the Enhanced Hoshen-Kopelman algorithm with");
  std::printf(" 26-neighbourhood scheme. ");
  std::printf("Labelling clusters of sites flagged as 0...\n");

  // Create arma::ucube for clusters of pore voxels (xi_ == 0 --> clusters == 1)
  arma::ucube clusters = (xi_ == 0);

  // Call Enhanced Hoshen & Kopelman cluster labelling algorithm on pore space
  EnhancedHoshenKopelman(clusters);

  // Revert cluster flag (clusters == 0 --> clusters == 1)
  clusters = (clusters == 0);

  // Calculated "true" or "connected" porosity, that which allows fluid flow
  porosity_ = 1.0 - static_cast<double>(arma::accu(clusters)) / clusters.n_elem;

  // Distinguish surface from bulk voxels and return surface-to-volume ratio
  surf2vol_ = GetSurfaceToVolume(clusters);

  // Calculate the box-counting fractal dimension of the infinite pore cluster
  GetFractalDimension(setup_cfg.folder, clusters);

  // Update the binary geometry by removing the isolated pore clusters
  xi_ = arma::conv_to<arma::Cube<uint8_t> >::from(clusters);

  // Print output messages
  std::printf("The porosity takes %lf%% of the sample.\n", 100.0 * porosity_);
  std::printf("The surface-to-volume ratio of the pore network is %lf.\n", surf2vol_(0));
  std::printf("The surface-to-volume ratio of the rock geometry is %lf.\n", surf2vol_(1));
}  // end of DigitalRock::DetermineMorphology() method



void DigitalRock::CalculateCenterlines(const MorphologyConfig &morphology_cfg) {
/**
 * The \c CalculateCenterlines() method computes the centerlines and outputs the
 * results to the folder specified in the JSON file. See skeleton::CenterlineManager for details.
 *
 * \param[in] morphology_cfg  Struct containing morphology-related configuration parameters.
 */
  // Print output messages if necessary
  std::printf("\nDIGITALROCK::CALCULATECENTERLINES SAYS: ");

  // Configure the algorithm
  skeleton::Settings settings;
  settings.folder = morphology_cfg.folder;
  settings.flavor = (morphology_cfg.performance == "speed" ? 0 : 1);

  // Call the centerline implementation
  skeleton::CenterlineManager manager;
  manager.ComputeCenterlines(xi_, settings);
}  // end of DigitalRock::CalculateCenterlines() method
