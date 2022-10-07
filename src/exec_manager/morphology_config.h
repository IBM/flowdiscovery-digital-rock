/**
 * \file src/exec_manager/morphology_config.h
 * \brief Contains the declaration of the \c MorphologyConfig class.
 *
 * \authors Rodrigo Alves Prado da Silva \<rodrigo.alves@ibm.com\>
 * \copyright © IBM Corp.
 * \date 2019
 *
 * This header file contains the declaration of the \c MorphologyConfig class that stores the
 * configuration parameters related to morphology from the JSON file.
 */

#ifndef SRC_EXEC_MANAGER_MORPHOLOGY_CONFIG_H_
#define SRC_EXEC_MANAGER_MORPHOLOGY_CONFIG_H_

#include <armadillo>
#include <string>
#include "src/exec_manager/setup_config.h"

/**
 * \class MorphologyConfig morphology_config.h "src/exec_manager/morphology_config.h"
 * \brief Class that stores parameters from the "morphology" top-level JSON object.
 */
class MorphologyConfig : public SetupConfig {
 public:
  /**
   * \brief Performance tuning of the centerline extraction algorithm
   *
   * This string determines whether the centerline extraction algorithm will favor "memory" or
   * "speed" efficiency.
   */
  std::string performance;

  /**
   * \brief Shape of the counting box for the mass fractal dimension calculation
   *
   * This string defines whether spherical or cubic counting boxes will be used to calculate the
   * mass fractal dimension as part of the morphological analysis.
   */
  std::string counting_box;
};

#endif  // SRC_EXEC_MANAGER_MORPHOLOGY_CONFIG_H_
