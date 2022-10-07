/**
 * \file src/exec_manager/exec_manager.cc
 * \brief Contains the \c ExecutionManager class methods.
 *
 * \authors Rodrigo Neumann Barros Ferreira \<rneumann@br.ibm.com\>
 * \copyright © IBM Corp.
 * \date 2016
 *
 * This source file contains the methods from the \c ExecutionManager class that run the mode
 * selected by command-line arguments.
 */

#include "src/exec_manager/exec_manager.h"
#include "src/digital_rock/digital_rock.h"

void ExecutionManager::RunSetup(const std::string &json_file_name) const {
/**
 * The \c RunSetup() method calls \c DigitalRock methods involved in loading the greyscale cube and
 * calculating the greyscale histogram for later segmentation.
 *
 * \param[in] json_file_name  Name of JSON file containing setup-related parameters.
 */
  // Parse JSON file into structs
  SetupConfig setup_cfg;
  config_reader_.PopulateSetupConfig(setup_cfg, json_file_name);

  // Perform setup steps
  DigitalRock sample;
  sample.LoadFile(setup_cfg);
  sample.CalculateHistogram(setup_cfg);
}  // end of ExecutionManager::RunSetup method



void ExecutionManager::RunSegmentation(const std::string &json_file_name) const {
/**
 * The \c RunSegmentation() method calls \c DigitalRock methods involved in segmenting the greyscale
 * cube into a binary cube.
 *
 * \param[in] json_file_name  Name of JSON file containing segmentation-related parameters.
 */
  // Parse JSON file into structs
  SegmentationConfig segmentation_cfg;
  config_reader_.PopulateSetupConfig(segmentation_cfg, json_file_name);
  config_reader_.PopulateSegmentationConfig(segmentation_cfg, json_file_name);

  // Perform segmentation steps
  DigitalRock sample;
  sample.LoadFile(segmentation_cfg);
  sample.CalculateHistogram(segmentation_cfg);
  sample.PerformSegmentation(segmentation_cfg);
  sample.CalculateVolFrac(segmentation_cfg);
}  // end of ExecutionManager::RunSegmentation method



void ExecutionManager::RunMorphology(const std::string &json_file_name) const {
/**
 * The \c RunMorphology() method calls \c DigitalRock methods involved in calculating morphological
 * figures-of-merit and extracting the centreline representation of the pore network.
 *
 * \param[in] json_file_name  Name of JSON file containing morphology-related parameters.
 */
  // Parse JSON file into structs
  MorphologyConfig morphology_cfg;
  config_reader_.PopulateSetupConfig(morphology_cfg, json_file_name);
  config_reader_.PopulateMorphologyConfig(morphology_cfg, json_file_name);

  // Perform morphology steps
  DigitalRock sample;
  sample.LoadBinaryFile(morphology_cfg);
  sample.DetermineMorphology(morphology_cfg);
  sample.CalculateCenterlines(morphology_cfg);
}  // end of ExecutionManager::RunMorphology method
