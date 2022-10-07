/**
 * \file src/exec_manager/segmentation_config.h
 * \brief Contains the declaration of the \c SegmentationConfig class.
 *
 * \authors Rodrigo Alves Prado da Silva \<rodrigo.alves@ibm.com\>
 * \copyright © IBM Corp.
 * \date 2019
 *
 * This header file contains the declaration of the \c SegmentationConfig class that stores the
 * configuration parameters related to segmentation from the JSON file.
 */

#ifndef SRC_EXEC_MANAGER_SEGMENTATION_CONFIG_H_
#define SRC_EXEC_MANAGER_SEGMENTATION_CONFIG_H_

#include <armadillo>
#include <string>
#include "src/exec_manager/setup_config.h"

/**
 * \class SegmentationConfig segmentation_config.h "src/exec_manager/segmentation_config.h"
 * \brief Class that stores parameters from the "segmentation" top-level JSON object.
 */
class SegmentationConfig : public SetupConfig {
 public:
  /**
   * \brief Greyscale threshold for manual image segmentation
   *
   * When the segmentation \c method is set to "global_manual", this option is mandatory since it
   * defines the greyscale threshold at which the image will be segmented, otherwise it is not used.
   */
  arma::sword threshold;

  /**
   * \brief Name of the image segmentation method
   *
   * Defines which segmentation method in \c src/segmentation/ will be called at run-time.
   * The "global_manual" option requires setting the \c threshold parameter.
   */
  std::string method;
};

#endif  // SRC_EXEC_MANAGER_SEGMENTATION_CONFIG_H_
