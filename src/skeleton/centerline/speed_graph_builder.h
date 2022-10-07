/**
 * \file src/skeleton/centerline/speed_graph_builder.h
 * \brief This file implements helper class to build high-performance graphs from images.
 *
 * \author Alexandre Ashade Lassance Cunha \<aashade@br.ibm.com\>
 * \authors Hugo de Oliveira Barbalho \<hugob@br.ibm.com\>
 * \copyright © IBM Corp.
 * \date 2016
 */

#ifndef SRC_SKELETON_CENTERLINE_SPEED_GRAPH_BUILDER_H_
#define SRC_SKELETON_CENTERLINE_SPEED_GRAPH_BUILDER_H_

#include <utility>
#include <vector>

namespace skeleton {

template<class Vertex, class Image>
class Converter {
 private:
  using image_type = Image;
  using vertex_key_type = typename Vertex::key_type;

  const image_type &image_;

 public:
  explicit Converter(const image_type &image) : image_(image) {}
  std::size_t operator()(const vertex_key_type &key) const {
    return image_.ConvertToLinearIndex(key.Point());
  }
};

template<class Graph, class AnnotatedImage, class Image, class
GraphNeighbourCalculator>
class SpeedGraphBuilder {
 public:
  using graph_type = Graph;
  using neighbour_calculator = GraphNeighbourCalculator;
  using annotated_image_type = AnnotatedImage;
  using image_type = Image;
  using vertex_set_type = typename graph_type::vertex_set_type;
  using vertex_type = typename graph_type::vertex_type;
  using vertex_key_type = typename vertex_type::key_type;
  using vertex_annotation_type = typename graph_type::vertex_annotation_type;


  graph_type Build(const annotated_image_type &image,
                   const image_type &original_image) const noexcept {
    vertex_set_type vertex_set;
    std::vector<bool> vertex_is_annotated_;
    vertex_is_annotated_.resize(original_image.GetTotalSize(), false);
    vertex_set.resize(original_image.GetTotalSize());
    for (const auto &point_and_annotation : image) {
      auto index = original_image.ConvertToLinearIndex(point_and_annotation
                                                           .first);
      auto distance = point_and_annotation.second.distance;
      const auto &point = point_and_annotation.first;
      if (!original_image.IsObjectPoint(point))
        continue;  // we cannot add to the graph the non-object voxels.
      vertex_set.at(index) =
          std::make_pair(vertex_key_type {point,
                                          static_cast<double>(distance)},
                         vertex_annotation_type {});
      vertex_is_annotated_.at(index) = true;
    }
    neighbour_calculator calculator{image};
    return graph_type {std::move(vertex_set), std::move(vertex_is_annotated_),
                       calculator,
                       Converter<vertex_type, image_type>{original_image}};
  }
};

}  // namespace skeleton

#endif  // SRC_SKELETON_CENTERLINE_SPEED_GRAPH_BUILDER_H_
