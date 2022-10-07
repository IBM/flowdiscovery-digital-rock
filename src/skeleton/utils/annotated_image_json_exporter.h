/**
 * \file src/skeleton/utils/annotated_image_json_exporter.h
 * \brief This file implements a helper to export an annotated image to a JSON file.
 *
 * \authors Rodrigo Neumann Barros Ferreira \<rneumann@br.ibm.com\>
 * \authors Rodrigo Alves Prado da Silva \<rodrigo.alves@ibm.com\>
 * \copyright © IBM Corp.
 * \date 2018
 */

#ifndef SRC_SKELETON_UTILS_ANNOTATED_IMAGE_JSON_EXPORTER_H_
#define SRC_SKELETON_UTILS_ANNOTATED_IMAGE_JSON_EXPORTER_H_

#include <armadillo>  // TODO(rneuman): Remove when BuildMatrix() is gone
#include <rapidjson/writer.h>
#include <rapidjson/document.h>
#include <rapidjson/ostreamwrapper.h>
#include <string>
#include "src/skeleton/utils/default_reader.h"
#include "src/skeleton/network/network_builder.h"

namespace skeleton {

template<class AnnotatedImage, class AnnotationReader = DefaultReader>
class JSONExporter {
 public:
  using network_type = Network<>;
  using network_builder_type = NetworkBuilder<network_type, AnnotatedImage>;

  /// Constructor
  JSONExporter(void)
    : read_annotation_() { }

  void ExportAnnotation(std::string folder_path, const AnnotatedImage &image) const {
    auto matrix = BuildMatrix(image);
    matrix.save(folder_path + "/centerlines.raw", arma::raw_binary);

    // Define JSON file name
    std::string json_file_name = folder_path + "/centerlines.json";

    // Build network from image
    network_builder_type network_builder;
    network_type network = network_builder.Build(image);

    // Build JSON document from network
    auto centerlines_json_doc = BuildJSON(network);

    // Write JSON document to file
    WriteJSON(centerlines_json_doc, json_file_name);
  }

 private:
  AnnotationReader read_annotation_;

  rapidjson::Document BuildJSON(const network_type &network) const {
  /**
   * The \c BuildJSON() method takes a \c Network object and writes it to a JSON document using the
   * <a href="http://jsongraphformat.info">JSON Graph Format</a> specification.
   *
   * \param[in] network   A network object containing node and link information.
   * \retval    json_doc  JSON document to be written to disk.
   */
    // Initialise JSON document and allocator
    rapidjson::Document json_doc;
    json_doc.SetObject();
    rapidjson::Document::AllocatorType &allocator = json_doc.GetAllocator();

    // Create "metadata" object
    rapidjson::Value graph_metadata(rapidjson::kObjectType);
    auto node_set_size = static_cast<uint64_t>(network.GetNodeSetSize());
    auto link_set_size = static_cast<uint64_t>(network.GetLinkSetSize());
    graph_metadata.AddMember("number_of_nodes", node_set_size, allocator);
    graph_metadata.AddMember("number_of_links", link_set_size, allocator);

    // Create "nodes" array
    rapidjson::Value nodes(rapidjson::kArrayType);
    for (auto node = network.node_cbegin(); node != network.node_cend(); ++node) {
      rapidjson::Value node_coordinates(rapidjson::kObjectType);
      node_coordinates.AddMember("x", node->GetPointCoordinate(0), allocator);
      node_coordinates.AddMember("y", node->GetPointCoordinate(1), allocator);
      node_coordinates.AddMember("z", node->GetPointCoordinate(2), allocator);

      rapidjson::Value node_metadata(rapidjson::kObjectType);
      node_metadata.AddMember("node_squared_radius", node->GetAnnotation(), allocator);
      node_metadata.AddMember("node_coordinates", node_coordinates, allocator);

      rapidjson::Value node_object(rapidjson::kObjectType);
      rapidjson::Value id(std::to_string(node->GetId()).c_str(), allocator);
      node_object.AddMember("id", id, allocator);
      node_object.AddMember("metadata", node_metadata, allocator);

      nodes.PushBack(node_object, allocator);
    }

    // Create "edges" array
    rapidjson::Value edges(rapidjson::kArrayType);
    for (auto link = network.link_cbegin(); link != network.link_cend(); ++link) {
      rapidjson::Value edge_metadata(rapidjson::kObjectType);
      edge_metadata.AddMember("link_length", link->GetLength(), allocator);
      edge_metadata.AddMember("link_squared_radius", link->GetSquaredRadius(), allocator);

      rapidjson::Value edge_object(rapidjson::kObjectType);
      rapidjson::Value id(std::to_string(link->GetId()).c_str(), allocator);
      rapidjson::Value source(std::to_string(link->GetSourceId()).c_str(), allocator);
      rapidjson::Value target(std::to_string(link->GetTargetId()).c_str(), allocator);
      edge_object.AddMember("id", id, allocator);
      edge_object.AddMember("source", source, allocator);
      edge_object.AddMember("target", target, allocator);
      edge_object.AddMember("metadata", edge_metadata, allocator);

      edges.PushBack(edge_object, allocator);
    }

    // Create root "graph" object
    rapidjson::Value graph(rapidjson::kObjectType);
    graph.AddMember("metadata", graph_metadata, allocator);
    graph.AddMember("nodes", nodes, allocator);
    graph.AddMember("edges", edges, allocator);
    json_doc.AddMember("graph", graph, allocator);

    assert(!json_doc.ObjectEmpty());  // Make sure JSON (centerlines) is not empty
    return json_doc;
  }  // end of JSONExporter::BuildJSON() method

  bool WriteJSON(const rapidjson::Document &json_doc, const std::string &json_file_name) const {
  /**
   * The \c WriteJSON() method gets a RapidJSON document and writes it to a JSON file.
   *
   * \param[in] json_doc        RapidJSON document.
   * \param[in] json_file_name  Target file name.
   * \retval    bool_success    Whether the write operation worked.
   */
    std::ofstream output_file_stream(json_file_name);
    rapidjson::OStreamWrapper output_stream_wrapper(output_file_stream);
    rapidjson::Writer<rapidjson::OStreamWrapper> writer(output_stream_wrapper);
    bool success = json_doc.Accept(writer);
    assert(success);  // Make sure JSON Writer succeeded
    return success;
  }  // end of JSONExporter::WriteJSON() method

  using CenterlinesRawType = int32_t;
  arma::Mat<CenterlinesRawType> BuildMatrix(const AnnotatedImage &image) const {
    auto size = image.GetSize();
    arma::Mat<CenterlinesRawType> centerline_points(size, 4);  // 4 cols (x, y, z, R^2), "size" rows
    arma::uword counter = 0;
    for (const auto &point_and_annotation : image) {
      auto point = point_and_annotation.first;
      auto value = read_annotation_(point_and_annotation.second);

      arma::Row<CenterlinesRawType> voxel = {point.GetCoordinate(0),
                                             point.GetCoordinate(1),
                                             point.GetCoordinate(2),
                                             value};
      centerline_points.row(counter++) = voxel;
    }
    return centerline_points;
  }  // end of JSONExporter::BuildMatrix() method
};

}  // namespace skeleton

#endif  // SRC_SKELETON_UTILS_ANNOTATED_IMAGE_JSON_EXPORTER_H_
