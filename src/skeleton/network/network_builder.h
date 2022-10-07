/**
 * \file src/skeleton/network/network_builder.h
 * \brief This file implements the class that builds the capillary network.
 *
 * \authors Rodrigo Neumann Barros Ferreira \<rneumann@br.ibm.com\>
 * \authors Rodrigo Alves Prado da Silva \<rodrigo.alves@ibm.com\>
 * \copyright © IBM Corp.
 * \date 2018
 */

#ifndef SRC_SKELETON_NETWORK_NETWORK_BUILDER_H_
#define SRC_SKELETON_NETWORK_NETWORK_BUILDER_H_

#include "src/skeleton/network/network.h"
#include "src/skeleton/utils/default_reader.h"

namespace skeleton {

/**
 * \class NetworkBuilder network_builder.h "src/skeleton/network/network_builder.h"
 * \brief Class that builds the capillary network into a \c Network object.
 *
 * The \c NetworkBuilder contains algorithm that creates an instance of \c Network.
 */

template<class TNetwork,
         class TAnnotatedImage,
         class TAnnotationReader = DefaultReader>
class NetworkBuilder {
 public:
  using network_type = TNetwork;
  using annotated_image_type = TAnnotatedImage;
  using annotation_reader_type = TAnnotationReader;

  /// Constructor
  NetworkBuilder(void)
    : read_annotation_() { }

  /// Build \c Network object from \c image
  network_type Build(const annotated_image_type &image) {
    InsertNodesFrom(image);
    LinkNeighbouringNodes();
    return network_;
  }

 private:
  /// Private \c Network member
  network_type network_;

  /// Private \c AnnotationReader member
  annotation_reader_type read_annotation_;

  /// Insert all nodes from \c image into \c network_
  bool InsertNodesFrom(const annotated_image_type &image) {
    bool success = true;
    for (const auto &point_and_annotation : image) {
      auto point = point_and_annotation.first;
      auto annotation = read_annotation_(point_and_annotation.second);
      auto status = network_.InsertNode(point, annotation);
      success = (success && status.second);
    }
    assert(success);  // Two nodes with identical points were inserted
    return success;
  }

  /// Link all neighbouring nodes in \c network_
  bool LinkNeighbouringNodes(void) {
    bool success = true;
    for (auto node = network_.node_cbegin(); node != network_.node_cend(); ++node) {
      auto neighbours = network_.CalculateNeighbourNodes(*node);
      for (const auto &neighbour : neighbours) {
        if (!network_.HasLink(*node, neighbour)) {
          auto status = network_.InsertLink(*node, neighbour);
          success = (success && status.second);
        }
      }
    }
    assert(success);  // Two links with identical source/target nodes were inserted
    return success;
  }
};

}  // namespace skeleton

#endif  // SRC_SKELETON_NETWORK_NETWORK_BUILDER_H_
