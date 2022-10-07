/**
 * \file src/cmdline_parser/cmdline_parser.cc
 * \brief Contains the \c Parse() method from the \c CmdLineParser class.
 *
 * \authors Rodrigo Neumann Barros Ferreira \<rneumann@br.ibm.com\>
 * \copyright © IBM Corp.
 * \date 2016
 *
 * This source file contains the \c Parse() method that reads from the command-line what needs to
 * be executed.
 */

#include "src/cmdline_parser/cmdline_parser.h"
#include <string>

void CmdLineParser::Parse(const int argc, const char **argv) {
/**
 * The \c Parse() method uses the TCLAP library \<http://tclap.sourceforge.net\>.
 *
 * \param[in] argc  Number of command-line arguments passed to \c main()
 * \param[in] argv  Strings containing command-line arguments passed to \c main()
 */
  try {                                       // Wrap everything in a try block.
    // Create CmdLine object
    TCLAP::CmdLine cmd(
      "Enhanced Pore Scale CO2 Separation, Conversion, Storage.",   // message
      '=',                                                          // delimiter
      "1.0",                                                        // version
      true);                                                        // help

    // Stores command-line flag that
    TCLAP::UnlabeledValueArg<std::string> json_file_name(
      "JSON_FILE_NAME",                                             // name
      "Name of the JSON configuration file to be loaded.",          // description
      true,                                                         // required
      "util/config_template.json",                                  // default
      "config.json",                                                // type
      cmd);                                                         // parser

    // Stores command-line flag that turns on "morphology" execution mode
    TCLAP::SwitchArg run_morphology(
      "",                                                           // flag
      "run_morphology",                                             // name
      "Extracts morphology and saves centreline representation.",   // description
      cmd,                                                          // parser
      false);                                                       // default

    // Stores command-line flag that turns on "segmentation" execution mode
    TCLAP::SwitchArg run_segmentation(
      "",                                                           // flag
      "run_segmentation",                                           // name
      "Segments greyscale tomography and saves binary cube.",       // description
      cmd,                                                          // parser
      false);                                                       // default

    // Stores command-line flag that turns on "setup" execution mode
    TCLAP::SwitchArg run_setup(
      "",                                                           // flag
      "run_setup",                                                  // name
      "Loads greyscale tomography and calculates histogram.",       // description
      cmd,                                                          // parser
      false);                                                       // default

    // Parse the argv array
    cmd.parse(argc, argv);

    // Get the value parsed by each arg and attribute it to a public variable
    json_file_name_   = json_file_name.getValue();
    run_setup_        = run_setup.getValue();
    run_segmentation_ = run_segmentation.getValue();
    run_morphology_   = run_morphology.getValue();
  }
  catch(const TCLAP::ArgException &e) {                  // catch any exceptions
    std::cerr << "Error: " << e.error() << "for arg" << e.argId() << std::endl;
  }
}  // end of CmdLineParser::Parse method
