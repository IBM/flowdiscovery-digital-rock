/**
 * \file src/skeleton/centerline/centerline_set.h
 * \brief This file implements the data structure that represents a set of
 * centerlines.
 *
 * \author Alexandre Ashade Lassance Cunha \<aashade@br.ibm.com\>
 * \author Giulia Duncan Coutinho \<coutingi@br.ibm.com\>
 * \copyright © IBM Corp.
 * \date 2016
 */

#ifndef SRC_SKELETON_CENTERLINE_CENTERLINE_SET_H_
#define SRC_SKELETON_CENTERLINE_CENTERLINE_SET_H_

#include <cstdint>
#include <vector>
#include <stack>
#include <unordered_set>

#include "src/skeleton/utils/hasher.h"
#include "src/skeleton/utils/comparer.h"
#include "src/skeleton/centerline/centerline_builder.h"

namespace skeleton {

/**
 *  \brief Data structure to represent a set of centerlines.
 *
 *  @tparam Graph The graph type.
 *  @tparam QuadraticPathCalculator The distance calculator for computing
 *  statistics. Typically, an Euclidean distance calculator.
 */
template<class Graph, class QuadraticPathCalculator>
class CenterlineSet {
 public:
  /**
   * \brief Alias to the template parameter Graph.
   */
  using graph_type = Graph;

  /**
   * \brief Alias to the template parameter QuadraticPathCalculator.
   */
  using quadratic_calculator_type = QuadraticPathCalculator;

  /**
   * \brief Alias to the vertex type of graph_type.
   */
  using vertex_type = typename graph_type::vertex_type;

  /**
   * \brief Alias to the point type of vertex_type.
   */
  using point_type = typename vertex_type::key_type;

  /**
   * \brief Alias to the point property type of point_type.
   */
  using point_property_type = typename point_type::point_property_type;

  /**
   * \brief Alias to the annotation type of vertex_type.
   */
  using annotation_type = typename vertex_type::annotation_type;

  /**
   * \brief Alias to the distance type of annotation_type.
   */
  using distance_type = typename annotation_type::distance_type;

  class CenterLineNode {
   private:
    point_type point_;
    distance_type distance_;

   public:
    CenterLineNode(const point_type &point, const distance_type distance)
        : point_(point),
          distance_(distance) {
    }
    point_type Point() const {
      return point_;
    }

    distance_type Distance() const {
      return distance_;
    }
  };

  /**
   * \brief The type that represents a centerline node, which is a point of a
   * specific centerline.
   */
  using centerline_node_type = CenterLineNode;

  using centerline_builder_type = CenterlineBuilder<centerline_node_type>;

  /**
   * \brief The type that represents a set of centerlines.
   */
  using centerline_type = typename centerline_builder_type::centerline_type;


  using centerline_set_type = std::vector<centerline_type>;

  /**
   * \brief The iterator type to iterate through the set of centerlines.
   */
  using const_iterator = typename centerline_set_type::const_iterator;

  class Statistics {
   public:
    using real_type = double;

   private:
    const distance_type size_;
    const real_type tortuosity_;
    const real_type average_property_value_;

    real_type GetTortuosity(const centerline_type &centerline) const {
      quadratic_calculator_type calculator;
      auto ends_distance = calculator.GetWeight(
          centerline[0].Point().Point(),
          centerline[centerline.GetNumPoints() - 1].Point().Point());
      return size_ / ends_distance - 1.0;
    }

    real_type GetAveragePropertyValue(const centerline_type &centerline) const {
      real_type average = 0.0;
      real_type count = 0.0;
      for (const auto &node : centerline) {
        count += 1.0;
        average = average
            + (std::sqrt(node.Point().PropertyValue()) - average) / count;
      }
      return average;
    }

    real_type GetSize(const centerline_type &centerline) const {
      real_type size = 0.0;
      quadratic_calculator_type calculator;
      for (std::size_t index = 1; index != centerline.GetNumPoints(); ++index) {
        auto vertex = centerline[index].Point();
        auto previews_vertex = centerline[index - 1].Point();
        size += calculator.GetWeight(vertex.Point(), previews_vertex.Point());
      }
      return size;
    }

   public:
    explicit Statistics(const centerline_type &centerline)
        : size_(GetSize(centerline)),
          tortuosity_(GetTortuosity(centerline)),
          average_property_value_(GetAveragePropertyValue(centerline)) {
    }

    distance_type Size() const {
      return size_;
    }

    real_type Tortuosity() const {
      return tortuosity_;
    }

    real_type AveragePropertyValue() const {
      return average_property_value_;
    }
  };

  /**
   * \brief The type that represents the statistics of a centerline.
   */
  using statistics_type = Statistics;

  /**
   * \brief The type that represents a set of statistics, one for each centerline.
   */
  using statistics_set_type = std::vector<statistics_type>;

 private:
  centerline_set_type paths_;
  statistics_set_type statistics_;

  using used_nodes_type =
  std::unordered_set<point_type, Hasher<point_type>, Comparer<point_type>>;

  using branch_nodes_type =
  std::unordered_set<point_type, Hasher<point_type>, Comparer<point_type>>;

  used_nodes_type used_nodes_;
  branch_nodes_type branch_nodes_;

  bool IsNotUsed(const point_type &point) const {
    return used_nodes_.find(point) == used_nodes_.end();
  }

  void MarkUsed(const point_type &point) {
    used_nodes_.insert(point);
  }

  void MarkAsBranch(const point_type &point) {
    branch_nodes_.insert(point);
  }

  bool HandleNewNode(centerline_builder_type &builder,
                     centerline_set_type &centerlines,
                     const point_type &point,
                     const annotation_type &annotation,
                     const point_type &prev_point,
                     const annotation_type &prev_annotation,
                     bool &add_prev,
                     bool &is_building) const {
    add_prev = false;
    if (IsNotUsed(point)) {
      if (!is_building) {
        builder.AddNode(centerline_node_type {prev_point, prev_annotation.Distance()});
        add_prev = true;
      }
      is_building = true;
      builder.AddNode(centerline_node_type {point, annotation.Distance()});
      return true;
    } else if (builder.HasNodes()) {
      if (is_building) {
        builder.AddNode(centerline_node_type {point, annotation.Distance()});
        centerlines.push_back(builder.GetCenterline());
        builder.Clear();
      }
      is_building = false;
      return false;
    }
    is_building = false;
    return false;
  }

  centerline_set_type GetSegments(const graph_type &annotated_graph,
                                  const point_type &end_point) noexcept {
    centerline_builder_type builder;
    centerline_set_type centerlines;
    auto point = end_point;
    auto prev_point = end_point;
    auto annotation = annotated_graph[end_point];
    auto prev_annotation = annotation;
    bool is_end_point = true;
    bool is_building = true;
    bool add_prev;
    while (annotation.HasPredecessor()) {
      bool was_building = is_building;
      if (!HandleNewNode(builder, centerlines, point, annotation,
                         prev_point, prev_annotation, add_prev, is_building)) {
        if (was_building && !is_building && !is_end_point)
          MarkAsBranch(point);
      }
      if (add_prev)
         MarkAsBranch(prev_point);
      MarkUsed(point);

      prev_point = point;
      prev_annotation = annotation;
      point = annotation.Predecessor();
      annotation = annotated_graph[point];
      is_end_point = false;
    }
    // add source
    if (HandleNewNode(builder, centerlines, point, annotation,
                         prev_point, prev_annotation, add_prev, is_building)) {
      if (add_prev)
        MarkAsBranch(prev_point);

      centerlines.push_back(builder.GetCenterline());
      MarkUsed(point);
    }
    return centerlines;
  }

  void AddPathsAndStatistics(const centerline_set_type &new_paths) {
    for (const auto &path : new_paths) {
      paths_.push_back(path);
      statistics_.push_back(statistics_type {path});
    }
  }

  bool HasSkeletonNeighbour(const graph_type &graph,
                           const point_type &reference_vertex,
                           const point_type &predecessor,
                           const point_type &successor) {
    auto neighbours = graph.GetVertexNeighbours(reference_vertex);
    for (const auto &neighbour : neighbours) {
      if (neighbour == predecessor ||
          neighbour == successor ||
          !reference_vertex.Point().IsFaceNeighbour(neighbour.Point()))
        continue;
      if (!IsNotUsed(neighbour))
        return true;
    }
    return false;
  }

  bool HasGreaterNeighbour(const graph_type &graph,
                           const point_type &reference_vertex,
                           const point_type &predecessor,
                           const point_type &successor) {
    auto neighbours = graph.GetVertexNeighbours(reference_vertex);
    point_property_type reference_weight = reference_vertex.PropertyValue();
    for (const auto &neighbour : neighbours) {
      point_property_type weight = neighbour.PropertyValue();
      if (weight <= reference_weight )
        continue;
      if (!IsNotUsed(neighbour) || neighbour == successor || neighbour == predecessor)
        continue;

      if (neighbour.Point().IsFaceNeighbour(predecessor.Point()) ||
          neighbour.Point().IsEdgeNeighbour(predecessor.Point())) {
        std::cout << "Greater1: " << neighbour.Point().GetCoordinate(0) << ","
                  << neighbour.Point().GetCoordinate(1) << ","
                  << neighbour.Point().GetCoordinate(2) << std::endl;
        std::cout << "Error: " << weight << " - " << reference_weight << std::endl;
        return true;
      }

      if (neighbour.Point().IsNeighbour(predecessor.Point()) &&
          neighbour.Point().IsNeighbour(successor.Point())) {
        std::cout << "Greater2: "<< neighbour.Point().GetCoordinate(0) << ","
                                << neighbour.Point().GetCoordinate(1) << ","
                                << neighbour.Point().GetCoordinate(2) << std::endl;
        std::cout << "Error: "<< weight << " - " << reference_weight << std::endl;
        return true;
      }
    }
    return false;
  }

  bool HasSelfieNeighbour(const graph_type &graph,
                          const point_type &reference_vertex,
                          const point_type &predecessor,
                          const point_type &successor,
                          const used_nodes_type &new_centerline) {
    auto neighbours = graph.GetVertexNeighbours(reference_vertex);
    for (const auto &neighbour : neighbours) {
      if (neighbour == predecessor ||
          neighbour == successor)
        continue;
      if (!neighbour.Point().IsFaceNeighbour(reference_vertex.Point()))
        continue;
      if (new_centerline.find(neighbour) != new_centerline.end()) {
        std::cout << "SelfieNeighbours: " << reference_vertex.Point().GetCoordinate(0) << ","
                                          << reference_vertex.Point().GetCoordinate(1) << ","
                                          << reference_vertex.Point().GetCoordinate(2) << " and ";

        std::cout << neighbour.Point().GetCoordinate(0) << ","
                  << neighbour.Point().GetCoordinate(1) << ","
                  << neighbour.Point().GetCoordinate(2) << std::endl;

        return true;
      }
    }
    return false;
  }

  bool IsValidCenterline(const graph_type &annotated_graph,
                         const graph_type &graph,
                         const point_type &vertex) {
    bool reach_skeleton = false;
    point_type curr_vertex = vertex;
    point_type successor;
    auto annotation = annotated_graph[curr_vertex];
    used_nodes_type new_centerline;
    bool is_valid = true;
    while (!reach_skeleton && annotation.HasPredecessor()) {
      if (!IsNotUsed(curr_vertex)) {
        reach_skeleton = true;
        continue;
      }
      bool is_voxel_valid = true;
      if (HasGreaterNeighbour(graph,
                              curr_vertex,
                              annotation.Predecessor(),
                              successor)) {
        std::cout << "Find greater Neighbour." << std::endl;
        is_voxel_valid = false;
      }

      if (HasSelfieNeighbour(graph,
                             curr_vertex,
                             annotation.Predecessor(),
                             successor,
                             new_centerline)) {
        std::cout << "Selfie Neighbour." << std::endl;
        is_voxel_valid = false;
      }

      if (!is_voxel_valid) {
        std::cout << vertex.Point().GetCoordinate(0) << ","
                  << vertex.Point().GetCoordinate(1) << ","
                  << vertex.Point().GetCoordinate(2) << " is at.";
        std::cout << curr_vertex.Point().GetCoordinate(0) << ","
                  << curr_vertex.Point().GetCoordinate(1) << ","
                  << curr_vertex.Point().GetCoordinate(2) << std::endl;
        is_valid = false;
      }

      new_centerline.insert(curr_vertex);
      successor = curr_vertex;
      curr_vertex = annotation.Predecessor();
      annotation = annotated_graph[curr_vertex];
    }
    return is_valid;
  }

  std::vector<point_type> BuildLMpath(const graph_type &annotated_graph,
                                      const graph_type &graph,
                                      const point_type &vertex_a,
                                      const point_type &vertex_b) {
    std::vector<point_type> lm_path;
    std::stack<point_type> st;
    st.push(vertex_a);
    auto annotation = annotated_graph[vertex_a];
    while (annotation.HasPredecessor()) {
      if (graph.IsLocalMaximalVertex(st.top())) {
        break;
      }
      st.push(annotation.Predecessor());
      annotation = annotated_graph[st.top()];
    }
    while (!st.empty()) {
      lm_path.push_back(st.top());
      st.pop();
    }
    annotation = annotated_graph[vertex_b];
    lm_path.push_back(vertex_b);
    while (annotation.HasPredecessor()) {
      if (graph.IsLocalMaximalVertex(lm_path[lm_path.size()-1])) {
        break;
      }
      lm_path.push_back(annotation.Predecessor());
      annotation = annotated_graph[lm_path[lm_path.size()-1]];
    }
    return lm_path;
  }

  bool CheckProperty3(const point_type &vertex_i,
                      const point_type &vertex_i1,
                      const point_type &vertex_i2) {
    if (vertex_i.Point().IsEdgeNeighbour(vertex_i2.Point()) ||
        vertex_i.Point().IsFaceNeighbour(vertex_i2.Point())) {
      return false;
    }

    if (!(vertex_i.Point().IsEdgeNeighbour(vertex_i1.Point()) ||
          vertex_i.Point().IsFaceNeighbour(vertex_i1.Point())) ||
        !(vertex_i1.Point().IsEdgeNeighbour(vertex_i2.Point()) ||
          vertex_i1.Point().IsFaceNeighbour(vertex_i2.Point()))) {
      return false;
    }
    return true;
  }

  bool CheckProperty4(const point_type &vertex_i,
                      const point_type &vertex_i1,
                      const point_type &vertex_i2,
                      const point_type &neighbour,
                      const graph_type &graph) {
    if (neighbour.Point().IsFaceNeighbour(vertex_i1.Point()) &&
        !graph.IsLocalMaximalVertex(neighbour) &&
        neighbour.Point() != vertex_i.Point() &&
        neighbour.Point() != vertex_i2.Point() &&
        neighbour.PropertyValue() > vertex_i1.PropertyValue()) {
      return false;
    }
    return true;
  }

  bool CheckProperty5(const point_type &vertex_i,
                      const point_type &vertex_i1,
                      const point_type &vertex_i2,
                      const point_type &vertex_key) {
    if ((vertex_key.Point().IsFaceNeighbour(vertex_i1.Point()) ||
        vertex_key.Point().IsEdgeNeighbour(vertex_i1.Point())) &&
        !vertex_key.Point().IsFaceNeighbour(vertex_i.Point()) &&
        !vertex_key.Point().IsEdgeNeighbour(vertex_i.Point()) &&
        vertex_key.Point() != vertex_i1.Point() &&
        vertex_key.Point() != vertex_i.Point() &&
        vertex_key.PropertyValue() > vertex_i2.PropertyValue()) {
      return false;
    }
    return true;
  }

  bool CheckPropertyThin(const point_type &vertex_0,
                         const point_type &vertex_i1,
                         const point_type &vertex_m,
                         const point_type &neighbour) {
    if (neighbour.Point().IsFaceNeighbour(vertex_i1.Point()) &&
        neighbour.Point() != vertex_0.Point() &&
        neighbour.Point() != vertex_m.Point() &&
        !IsNotUsed(vertex_i1)) {
      return false;
    }
    return true;
  }

  bool ValidateLMpath(const graph_type &graph,
                      const std::vector<point_type> &lm_path) {
    if (lm_path[0].Point() == lm_path[lm_path.size() - 1].Point()) {
      // std::cout << "Failed: p0 and pm are the same." << std::endl;
      return false;
    }

    if (lm_path.size() == 3) {
      auto neighbours = graph.GetVertexNeighbours(lm_path[1]);
      if (!CheckProperty3(lm_path[0], lm_path[1], lm_path[2]))
        return false;

      for (const auto &vertex_key : neighbours) {
        if (!CheckPropertyThin(lm_path[0],
                               lm_path[1],
                               lm_path[2],
                               vertex_key))
          return false;

        if (vertex_key.Point().IsFaceNeighbour(lm_path[1].Point()) &&
            vertex_key.Point() != lm_path[0].Point() &&
            vertex_key.Point() != lm_path[2].Point() &&
            vertex_key.PropertyValue() > lm_path[1].PropertyValue()) {
          return false;
        }
        if (!vertex_key.Point().IsVertexNeighbour(lm_path[1].Point()) &&
            vertex_key.Point() != lm_path[1].Point() &&
            vertex_key.Point() != lm_path[0].Point() &&
            vertex_key.PropertyValue() > lm_path[2].PropertyValue()) {
          return false;
        }
      }
      return true;
    } else if (lm_path.size() > 3) {
      for (std::size_t i = 0; i != lm_path.size() - 3; ++i) {
        if (!CheckProperty3(lm_path[i], lm_path[i + 1], lm_path[i + 2]))
          return false;
        auto neighbours = graph.GetVertexNeighbours(lm_path[i+1]);

        for (const auto &vertex_key : neighbours) {
          if (!CheckPropertyThin(lm_path[0],
                                 lm_path[i + 1],
                                 lm_path[lm_path.size() - 1],
                                 vertex_key))
            return false;

          if (!CheckProperty4(lm_path[i],
                              lm_path[i + 1],
                              lm_path[i + 2],
                              vertex_key,
                              graph)) {
            return false;
          }

          bool property5_valid = true;
          if (lm_path[i].PropertyValue() < lm_path[i+1].PropertyValue() ||
              lm_path[i+1].PropertyValue() < lm_path[i+2].PropertyValue())
            property5_valid = CheckProperty5(lm_path[i], lm_path[i+1], lm_path[i+2], vertex_key);
          else
            property5_valid = CheckProperty5(lm_path[i+2], lm_path[i+1], lm_path[i], vertex_key);
          if (!property5_valid) {
            return false;
          }
        }
      }
      return true;
    }
    return false;
  }

 public:
  /**
   * \brief Constructs an empty set of centerlines.
   */
  CenterlineSet()
      : paths_(),
        statistics_(),
        used_nodes_(),
        branch_nodes_() {
  }

  /**
   * \brief Extracts and adds another centerline to the set.
   *
   * \param annotated_graph The annotated graph that has the centerline, after the
   * centerline algorithm execution.
   *
   * \param end_point The sink point (last / end point) of the centerline.
   */
  void AddSet(const graph_type &annotated_graph, const point_type &end_point) {
    if (!annotated_graph.HasVertex(end_point))
      return;
    auto annotation = annotated_graph[end_point];
    if (!annotation.HasFiniteDistance())
      return;

    auto paths = GetSegments(annotated_graph, end_point);
    AddPathsAndStatistics(paths);
  }

  /**
   * \brief Extracts and adds another centerline to the set.
   *        Also checks centerline quality
   *
   * \param annotated_graph The annotated graph that has the centerline, after the
   * centerline algorithm execution.
   *
   * \param graph The graph that has the image properties.
   *
   * \param end_point The sink point (last / end point) of the centerline.
   */
  void AddSet(const graph_type &annotated_graph,
              const graph_type &graph,
              const point_type &end_point) {
    if (!annotated_graph.HasVertex(end_point))
      return;
    auto annotation = annotated_graph[end_point];
    if (!annotation.HasFiniteDistance())
      return;

#if defined(_VALIDATION)
    if (!IsValidCenterline(annotated_graph, graph, end_point))
      std::cout << "INVALID CENTERLINE" << std::endl;
    else
      std::cout << "valid centerline" << std::endl;
#else
    // Avoid warnning during compilation
    (void)(&(graph));
#endif

    auto paths = GetSegments(annotated_graph, end_point);
    AddPathsAndStatistics(paths);
  }

  /**
  * \brief Validate and add a path to create centerline cycle.
  *        Validation properties enumeration are associated to
  *        the article https://doi.org/10.1007/s003710050142
  * \param annotated_graph The annotated graph that has the centerline, after the
  *        centerline algorithm execution.
  *
  * \param graph The graph that has the image properties.
  *
  * \param vertex_a First vertex of a pair of vertices to be connected if valid.
  * \param vertex_b Second vertex of a pair of vertices to be connected if valid.
  */
  void AddSet(const graph_type &annotated_graph,
              const graph_type &graph,
              const point_type &vertex_a,
              const point_type &vertex_b) {
    auto lm_path = BuildLMpath(annotated_graph, graph, vertex_a, vertex_b);
    if (ValidateLMpath(graph, lm_path)) {
      auto paths_a = GetSegments(annotated_graph, vertex_a);
      auto paths_b = GetSegments(annotated_graph, vertex_b);
      AddPathsAndStatistics(paths_a);
      AddPathsAndStatistics(paths_b);
    }
  }

  /**
   * \brief Returns an immutable reference to the statistics set.
   */
  const statistics_set_type &Statistics() const {
    return statistics_;
  }

  /**
   * \brief Returns an iterator pointing to the first centerline in the set.
   */
  const_iterator begin() const noexcept {
    return paths_.begin();
  }

  /**
   * \brief Returns an iterator pointing to the past-the-last centerline in the set.
   */
  const_iterator end() const noexcept {
    return paths_.end();
  }

  size_t size() {
    return paths_.size();
  }

  const centerline_type &operator[](const std::size_t index) const {
    return paths_[index];
  }

  bool IsBranch(const point_type &point) const {
    return branch_nodes_.find(point) != branch_nodes_.end();
  }

  void SplitByBranchPoints(){
    statistics_.clear();
    for (std::size_t j = 0; j != paths_.size(); ++j) {
      auto &path = paths_[j];
      for (std::size_t i = 0 ; i != path.GetNumPoints() ; ++i) {
        if (IsBranch(path[i].Point())) {
          auto new_path = path.split(i);
          if (new_path.GetNumPoints() > 0) {
            paths_.push_back(new_path);
            break;
          }
        }
      }
    }

    for (const auto &path : paths_)
      statistics_.push_back(statistics_type {path});
  }
};

}  // namespace skeleton

#endif  // SRC_SKELETON_CENTERLINE_CENTERLINE_SET_H_
