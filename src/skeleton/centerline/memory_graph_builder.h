/**
 * \file src/skeleton/centerline/memory_graph_builder.h
 * \brief This file implements helper class to build memory-efficient graphs from images.
 *
 * \author Alexandre Ashade Lassance Cunha \<aashade@br.ibm.com\>
 * \authors Hugo de Oliveira Barbalho \<hugob@br.ibm.com\>
 * \copyright © IBM Corp.
 * \date 2016
 */

#ifndef SRC_SKELETON_CENTERLINE_MEMORY_GRAPH_BUILDER_H_
#define SRC_SKELETON_CENTERLINE_MEMORY_GRAPH_BUILDER_H_

#include <utility>

namespace skeleton {

template<class Graph, class AnnotatedImage, class GraphNeighbourCalculator>
class MemoryGraphBuilder {
 public:
  using graph_type = Graph;
  using neighbour_calculator = GraphNeighbourCalculator;
  using annotated_image_type = AnnotatedImage;
  using vertex_set_type = typename graph_type::vertex_set_type;
  using vertex_type = typename graph_type::vertex_type;
  using vertex_key_type = typename vertex_type::key_type;
  using vertex_key_prop_type = typename vertex_key_type::point_property_type;
  using vertex_annotation_type = typename graph_type::vertex_annotation_type;

  template<class TImage>
  graph_type Build(const annotated_image_type &image, const TImage &original)
  const
  noexcept {
    vertex_set_type vertex_set;
    for (const auto &point_and_annotation : image) {
      auto distance = point_and_annotation.second.distance;
      const auto &point = point_and_annotation.first;
      if (!original.IsObjectPoint(point))
        continue;
      vertex_set.insert(
        std::make_pair(vertex_key_type {point, static_cast<vertex_key_prop_type>(distance)},
                       vertex_annotation_type {}));
    }
    neighbour_calculator calculator{image};
    return graph_type {std::move(vertex_set), calculator};
  }
};

}  // namespace skeleton

#endif  // SRC_SKELETON_CENTERLINE_MEMORY_GRAPH_BUILDER_H_
