/*
 * \file test/main_test.cc
 * \brief Contains the regression tests.
 *
 * \authors Alexandre Ashade Lassance Cunha \<aashade@br.ibm.com\>
 * \authors Rodrigo Neumann Barros Ferreira \<rneumann@br.ibm.com\>
 * \authors Hugo de Oliveira Barbalho \<hugob@br.ibm.com\>
 * \authors Rodrigo Alves Prado da Silva \<rodrigo.alves@ibm.com\>
 * \copyright © IBM Corp.
 * \date 2016
 */

#include <gtest/gtest.h>
#include <glog/logging.h>
#include "test/src/config_reader/config_reader_tests.h"
#include "test/src/segmentation/helper_segmentation_tests.h"
#include "test/src/segmentation/global_segmentation_tests.h"
#include "test/src/morphology/surface_to_volume_tests.h"
#include "test/src/morphology/fractal_dimension_tests.h"
#include "test/src/skeleton/point_test.h"
#include "test/src/skeleton/point_builder_test.h"
#include "test/src/skeleton/four_neighbour_calculator_test.h"
#include "test/src/skeleton/six_neighbour_calculator_test.h"
#include "test/src/skeleton/twenty_six_neighbour_calculator_test.h"
#include "test/src/skeleton/image_test.h"
#include "test/src/skeleton/image_iterator_test.h"
#include "test/src/skeleton/image__image_iterator_point_test.h"
#include "test/src/skeleton/arma_2d_image_builder_test.h"
#include "test/src/skeleton/contour_calculator_integrated_test.h"
#include "test/src/skeleton/annotated_image_test.h"
#include "test/src/skeleton/binary_heap_test.h"
#include "test/src/skeleton/quadratic_path_calculator_test.h"
#include "test/src/skeleton/binary_image_test.h"
#include "test/src/skeleton/centerline_test.h"
#include "test/src/skeleton/gradient_test.h"
#include "test/src/skeleton/dijkstra_test.h"
#include "test/src/skeleton/network_test.h"
#include "test/src/skeleton/network_node_test.h"
#include "test/src/skeleton/network_link_test.h"

int main(int argc, char** argv) {
  // Initialise Google's logging library
  ::google::InitGoogleLogging(argv[0]);

  // Initialise Google's testing library
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
