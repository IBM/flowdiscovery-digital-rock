/**
 * \file src/exec_manager/config_reader.cc
 * \brief Contains the methods of the \c ConfigReader class.
 *
 * \authors Rodrigo Neumann Barros Ferreira \<rneumann@br.ibm.com\>
 * \authors Rodrigo Alves Prado da Silva \<rodrigo.alves@ibm.com\>
 * \copyright © IBM Corp.
 * \date 2018
 *
 * This source file contains the parse methods that handle each individual JSON file related to each
 * execution mode.
 */

#include <rapidjson/schema.h>
#include "src/exec_manager/config_reader.h"
#include "src/exec_manager/setup_config.h"
#include "src/exec_manager/segmentation_config.h"
#include "src/exec_manager/morphology_config.h"

void ConfigReader::PopulateSetupConfig(SetupConfig &setup_cfg,
                                       const std::string &json_file_name) const {
/**
 * This method parses and validates the JSON file containing setup-related configuration parameters.
 *
 * \param[out] setup_cfg       A object containing all setup-related configuration parameters.
 * \param[in]  json_file_name  The address of the JSON file with respect to project root.
 */
  // Parse file into rapidjson::Document object and validate JSON file with respect to JSON schema
  rapidjson::Document json_doc = ParseIntoJsonDocument(json_file_name);
  std::string schema_file_name = "util/config_schema.json";
  ValidateJsonDocument(json_doc, schema_file_name);

  // Check if document contains the required information
  if (json_doc.HasMember("setup")) {
    auto setup_obj = json_doc["setup"].GetObject();

    // Populate object
    setup_cfg.folder = setup_obj["folder"].GetString();
    setup_cfg.input_file = setup_obj["input_file"].GetString();
    setup_cfg.voxel_size = setup_obj["voxel_size"].GetDouble();
    setup_cfg.shape = {setup_obj["shape"]["x"].GetUint64(),
                       setup_obj["shape"]["y"].GetUint64(),
                       setup_obj["shape"]["z"].GetUint64()};
  } else {
    std::fprintf(stderr, "JSONPARSER::POPULATESETUPCONFIG says: ");
    std::fprintf(stderr, "The '%s' file lacks the 'setup' field.\n", json_file_name.c_str());
    std::exit(-1);
  }
}  // end of ConfigReader::PopulateSetupConfig method



void ConfigReader::PopulateSegmentationConfig(SegmentationConfig &segmentation_cfg,
                                                     const std::string &json_file_name) const {
/**
 * This method parses and validates the JSON file containing segmentation-related configuration
 * parameters.
 *
 * \param[out] segmentation_cfg A object containing all segmentation-related configuration
 *                              parameters.
 * \param[in]  json_file_name   The address of the JSON file with respect to project root.
 */
  // Parse file into rapidjson::Document object and validate JSON file with respect to JSON schema
  rapidjson::Document json_doc = ParseIntoJsonDocument(json_file_name);
  std::string schema_file_name = "util/config_schema.json";
  ValidateJsonDocument(json_doc, schema_file_name);

  // Check if document contains the required information
  if (json_doc.HasMember("segmentation")) {
    auto segmentation_obj = json_doc["segmentation"].GetObject();

    // Populate object
    segmentation_cfg.method = segmentation_obj["method"].GetString();
    segmentation_cfg.threshold = segmentation_obj["threshold"].GetInt64();
  } else {
    std::fprintf(stderr, "JSONPARSER::POPULATESEGMENTATIONCONFIG says: ");
    std::fprintf(stderr, "The '%s' file lacks the 'segmentation' field.\n", json_file_name.c_str());
    std::exit(-1);
  }
}  // end of ConfigReader::PopulateSegmentationConfig method



void ConfigReader::PopulateMorphologyConfig(MorphologyConfig &morphology_cfg,
                                                   const std::string &json_file_name) const {
/**
 * This method parses and validates the JSON file containing morphology-related configuration
 * parameters.
 *
 * \param[out] morphology_cfg  A object containing all morphology-related configuration parameters.
 * \param[in]  json_file_name  The address of the JSON file with respect to project root.
 */
  // Parse file into rapidjson::Document object and validate JSON file with respect to JSON schema
  rapidjson::Document json_doc = ParseIntoJsonDocument(json_file_name);
  std::string schema_file_name = "util/config_schema.json";
  ValidateJsonDocument(json_doc, schema_file_name);

  // Check if document contains the required information
  if (json_doc.HasMember("morphology")) {
    auto morphology_obj = json_doc["morphology"].GetObject();

    // Populate object
    morphology_cfg.performance = morphology_obj["centerlines_performance"].GetString();
    morphology_cfg.counting_box = morphology_obj["fractal_counting_box"].GetString();
  } else {
    std::fprintf(stderr, "JSONPARSER::POPULATEMORPHOLOGYCONFIG says: ");
    std::fprintf(stderr, "The '%s' file lacks the 'morphology' field.\n", json_file_name.c_str());
    std::exit(-1);
  }
}  // end of ConfigReader::PopulateMorphologyConfig method
