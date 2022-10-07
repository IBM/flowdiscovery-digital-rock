/**
 * \file src/skeleton/centerline/maximal_clusters_discoverer.h
 * \brief Identify and label the set of local maximum voxels with the same distance.
 *
 * \author Hugo de Oliveira Barbalho \<hugob@br.ibm.com\>
 * \copyright © IBM Corp.
 * \date 2017
 */
#include <queue>
#include <algorithm>

#ifndef SRC_SKELETON_CENTERLINE_MAXIMAL_CLUSTERS_DISCOVERER_H_
#define SRC_SKELETON_CENTERLINE_MAXIMAL_CLUSTERS_DISCOVERER_H_

namespace skeleton {

/**
 * \brief Identify and label the set of local maximum voxels
 *        with the same distance.
 *
 * @tparam Graph The implementation of a graph
 * @tparam AnnotatedGraph The implementation of an annotated graph.
 *
 * */
template<class Graph>
class MaximalClustersDiscoverer {
 public:
  /**
  * \brief Alias for the template type Graph.
  */
  using graph_type = Graph;

  /**
  * \brief Alias of the vertex type.
  */
  using vertex_type = typename graph_type::vertex_type;

  /**
  * \brief Alias of the vertex key type.
  */
  using vertex_key_type = typename graph_type::vertex_key_type;

  /**
  * \brief Alias of the point type.
  */
  using point_type = typename vertex_key_type::point_type;

  using label_type = int64_t;

 private:
  using queue_type = std::queue<vertex_key_type>;

  graph_type &graph_;

  label_type num_of_labels;

  void SetLabel(const vertex_key_type &vertex_key, label_type label_value) {
        graph_[vertex_key].SetClusterId(label_value);
  }

  void Initialise() {
    num_of_labels = 0;
  }

  void LabelVertexCluster(const vertex_key_type &seed_key, const label_type label_value) {
    queue_type queue;
    queue.push(seed_key);
    while (!queue.empty()) {
      vertex_key_type ref_key = queue.front();
      queue.pop();
      SetLabel(ref_key, label_value);

      auto neighbours = graph_.GetVertexNeighbours(ref_key);
      for (const auto &vertex_key : neighbours) {
        if (vertex_key.Point().IsVertexNeighbour(ref_key.Point()))
          continue;
        if (graph_.IsLocalMaximalVertex(vertex_key)) {
          graph_[vertex_key].SetIsLocalMaximal(true);

          if (graph_[vertex_key].GetClusterId() == -1) {
            SetLabel(vertex_key, label_value);
            queue.push(vertex_key);
          }
        } else {
          graph_[vertex_key].SetIsLocalMaximal(false);
        }
      }
    }
  }

 public:
  /**
  *  \brief Constructs an instance of the class from an graph.
  *  \param graph The instance of a graph of type Graph.
  */
  explicit MaximalClustersDiscoverer(graph_type &graph)
    : graph_(graph) {
  }

  MaximalClustersDiscoverer(const MaximalClustersDiscoverer &discoverer)
      : graph_(discoverer.graph) {
  }

  /**
  *  \brief Set an unique label for each cluster of local maximal voxels.
  *         It also set the attribute IsLocalMaximal for each graph voxel.
  */
  void Execute() {
    Initialise();
    label_type next_label = 0;
    for (const auto &pair : graph_) {
      vertex_key_type key = pair.first;
      if (graph_.IsLocalMaximalVertex(key)) {
        graph_[key].SetIsLocalMaximal(true);
        if (graph_[key].GetClusterId() == -1)
          LabelVertexCluster(key, next_label++);
      } else {
        graph_[key].SetIsLocalMaximal(false);
      }
    }
    num_of_labels = next_label;
  }

  /**
  *  \brief Get an unique key for a pair of labels.
  *  \param label_a label from a cluster of type label_type.
  *  \param label_b label from a cluster of type label_type.
  */
  int64_t GetPairKey(const label_type label_a, const label_type label_b) const {
    return std::min<label_type>(label_a, label_b) * num_of_labels +
           std::max<label_type>(label_a, label_b);
  }

  /**
  *  \brief Get the label associated with the vertex_key .
  *  \param vertex_key label from a cluster of type label_type.
  */
  label_type GetClusterLabel(const vertex_key_type &vertex_key) const {
    if (graph_.HasVertex(vertex_key)) {
      return graph_[vertex_key].GetClusterId();
    } else {
      return -1;
    }
  }
};

}  // namespace skeleton

#endif  // SRC_SKELETON_CENTERLINE_MAXIMAL_CLUSTERS_DISCOVERER_H_
