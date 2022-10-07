/**
 * \file src/skeleton/graph/dijkstra.h
 * \brief Implements Dijkstra's single source shortest path algorithm.
 *
 * \author Alexandre Ashade Lassance Cunha \<aashade@br.ibm.com\>
 * \copyright © IBM Corp.
 * \date 2016
 */

#ifndef SRC_SKELETON_GRAPH_DIJKSTRA_H_
#define SRC_SKELETON_GRAPH_DIJKSTRA_H_

#include <inttypes.h>
#include <queue>
#include <vector>
#include <utility>
#include <algorithm>
#include <unordered_set>

namespace skeleton {

/**
 * \brief Standard implementation of the Dijkstra's algorithm using a
 * priority queue. See https://en.wikipedia.org/wiki/Dijkstra's_algorithm.
 *
 * @tparam Graph The implementation of a graph
 * @tparam AnnotatedGraph The implementation of an annotated graph.
 * @tparam WeightCalculator The entity that calculates the weight of moving
 * from node A to node B of the input graph.
 * @tparam WeightCalculator The priority queue implementation. Typically a
 * binary heap or a Fibonacci heap.
 *
 * */
template<class Graph,
         class AnnotatedGraph,
         class PathCalculator,
         class WeightCalculator,
         class GradientCalculator,
         class ClustersDiscoverer,
         template<class, class ...> class PriorityQueue>
class Dijkstra {
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
   * \brief Alias for the template type AnnotatedGraph.
   */
  using annotated_graph_type = AnnotatedGraph;

  /**
   * \brief Alias of the vertex key type.
   */
  using vertex_key_type = typename graph_type::vertex_key_type;

  /**
   * \brief Alias of the point type.
   */
  using point_type = typename vertex_key_type::point_type;

  /**
   * \brief Alias of the point coordinate type.
   */
  using coordinate_type = typename point_type::coordinate;

  /**
   * \brief Alias of the point index type.
   */
  using coordinates_index_type = typename point_type::coordinates_index_type;

  /**
   * \brief Alias for the template type WeightCalculator.
   */
  using weight_calculator_type = WeightCalculator;

  /**
   * \brief Alias for the template type PathCalculator.
   */
  using path_calculator_type = PathCalculator;

  /**
   * \brief Alias for the template type GradientCalculator.
   */
  using gradient_calculator_type = GradientCalculator;

  /**
   * \brief Alias for the template type ClusterDiscoverer.
   */
  using clusters_discoverer_type = ClustersDiscoverer;

  using gradient_point_type = typename gradient_calculator_type::gradient_point_type;
  using annotated_vertex_type = typename annotated_graph_type::vertex_type;
  using vertex_annotation_type = typename vertex_type::annotation_type;
  using distance_type = typename weight_calculator_type::distance_type;
  using queue_type = PriorityQueue<annotated_vertex_type>;
  using queue_handle_type = typename queue_type::handle;
  using leaf_list_type = std::vector<vertex_type>;

  using priority_queue_element_type = struct {
    vertex_type annotation;
    double weight;
    double penalties;
    int64_t v_id;
    double accumulated;
    double prev_accumulated;
  };

  using end_vertices_candidate_list_type = std::vector<std::pair<vertex_key_type, vertex_key_type>>;

 private:
  static constexpr double tolerance = -1.0e-5;
  const graph_type &graph_;
  annotated_graph_type annotated_graph_;
  annotated_graph_type annotated_graph_accumulated_;
  annotated_graph_type annotated_graph_acc_penalties_;
  annotated_graph_type annotated_graph_leaf_;
  weight_calculator_type weight_calculator_;
  path_calculator_type path_calculator_;
  leaf_list_type leaf_list_;
  double max_leaf_weight_;
  end_vertices_candidate_list_type end_vertices_candidate_list_;
  const clusters_discoverer_type &clusters_discoverer_;
  annotated_graph_type annotated_label_graph_;

  class CompareAnnotatedVertexTypeByDistance {
    weight_calculator_type weight_calculator_;
    inline bool operator() (const priority_queue_element_type &vertex_a,
                            const priority_queue_element_type &vertex_b) {
      auto vertex_a_distance = vertex_a.weight;
      auto vertex_b_distance = vertex_b.weight;

      if (vertex_a_distance - vertex_b_distance < tolerance) {
        return false;
      } else if (vertex_b_distance - vertex_a_distance < tolerance) {
        return true;
      } else if (vertex_a.penalties - vertex_b.penalties < tolerance) {
        return false;
      } else if (vertex_b.penalties - vertex_a.penalties < tolerance) {
        return true;
      } else if (vertex_a.v_id < vertex_b.v_id) {
        return false;
      }
      return true;
    }
  };

  using priority_queue_type = std::priority_queue<priority_queue_element_type,
                                                  std::vector<priority_queue_element_type>,
                                                  CompareAnnotatedVertexTypeByDistance>;

  using label_type = typename clusters_discoverer_type::label_type;
  using set_pair_labels_type = std::unordered_set<label_type>;
  set_pair_labels_type pair_labels_;

  bool IsPairLabelsVisited(const label_type label_a, const label_type label_b) {
    if (label_a == label_b)
      return true;
    label_type pair_key = clusters_discoverer_.GetPairKey(label_a, label_b);
    return pair_labels_.find(pair_key) != pair_labels_.end();
  }

  bool SetPairLabelsVisited(const label_type label_a, const label_type label_b) {
    if (label_a == label_b)
      return true;
    label_type pair_key = clusters_discoverer_.GetPairKey(label_a, label_b);
    if (pair_labels_.find(pair_key) !=
        pair_labels_.end()) {
      return true;
    } else {
      pair_labels_.insert(pair_key);
      return false;
    }
  }

  void SetPathLabel(const vertex_key_type vertex_key,
                    const label_type label_value) {
    if (!annotated_label_graph_.HasVertex(vertex_key)) {
      annotated_vertex_type annotated_vertex = annotated_vertex_type{vertex_key, 0.0};
      annotated_label_graph_.Insert(annotated_vertex);
    }
    annotated_label_graph_[vertex_key].SetClusterId(label_value);
  }

  void Initialise(const annotated_vertex_type &source, queue_type &queue) {
    annotated_graph_.Clear();
    annotated_vertex_type copy{source};
    copy.ChangeAnnotation().SetDistance(0);
    auto handle = queue.Insert(copy);
    copy.ChangeAnnotation().SetTag(handle);
    annotated_graph_.Insert(copy);
    annotated_label_graph_.Clear();
    label_type source_label = clusters_discoverer_.GetClusterLabel(source.Key());
    SetPathLabel(source.Key(), source_label);
  }

  void Initialise(const annotated_vertex_type &source, priority_queue_type &queue, int64_t &v_id) {
    v_id = 0;
    annotated_graph_.Clear();
    annotated_vertex_type copy{source};
    copy.ChangeAnnotation().SetDistance(0);

    queue.push(priority_queue_element_type {copy, 0.0, 0.0, v_id++, 0.0, 0.0});
    annotated_graph_.Insert(copy);

    annotated_graph_[source.Key()].SetRemoved();
    max_leaf_weight_ = tolerance;
    leaf_list_.clear();
  }

  bool Relax(const annotated_vertex_type &reference,
             const vertex_key_type &candidate_key,
             vertex_annotation_type &cand_annotation) {
    auto weight = weight_calculator_.GetWeight(reference.Key(), candidate_key);
    auto &ref_annotation = reference.Annotation();
    auto vertices_distance = ref_annotation.Distance() + weight;
    if (cand_annotation.Distance() > vertices_distance) {
      cand_annotation.SetDistance(vertices_distance);
      cand_annotation.SetPredecessor(reference.Key());
      return true;
    }
    return false;
  }
  void RelaxNeighbours(const annotated_vertex_type &reference,
                       queue_type &queue) {
    auto neighbours = graph_.GetVertexNeighbours(reference);
    for (const auto &vertex_key : neighbours) {
      bool new_vertex = false;
      if (!annotated_graph_.HasVertex(vertex_key)) {
        auto annotated_vertex = annotated_vertex_type{vertex_key,
                                                      weight_calculator_type::InfiniteDistance()};
        annotated_graph_.Insert(annotated_vertex);
        new_vertex = true;
      }
      auto &vertex_annotation = annotated_graph_[vertex_key];
      if (vertex_annotation.IsRemoved())
        continue;
      auto have_relaxed = Relax(reference, vertex_key, vertex_annotation);
      if (have_relaxed) {
        if (new_vertex) {
          auto handle = queue.Insert(annotated_vertex_type {vertex_key, vertex_annotation});
          vertex_annotation.SetTag(handle);
        } else {
          queue.DecreasePriority(
              vertex_annotation.template Tag<queue_handle_type>(),  // handle
              annotated_vertex_type {vertex_key, vertex_annotation});
        }
      }
    }
  }

  distance_type ComputeVerticeDistance(gradient_calculator_type &gradient_calculator,
                                       const gradient_point_type &gradient,
                                       const vertex_key_type &ref_key,
                                       const vertex_key_type &cand_key,
                                       const distance_type &ref_accumulated_distance,
                                       const distance_type &ref_accumulated_penalties,
                                       distance_type &cand_accumulated_distance,
                                       distance_type &cand_accumulated_penalties) {
    double penalties = 0.0;
    double penalty_direction = 0.0;
    double path_distance = 0.0;
    auto weight = weight_calculator_.GetWeight(ref_key, cand_key);
    auto annotated_ref = annotated_graph_[ref_key];

    if (!graph_.IsLocalMaximalVertex(ref_key)) {
      double penalty = gradient_calculator.ComputeStepPenalty(ref_key.Point(),
                                                              cand_key.Point(),
                                                              gradient);
      double next_step_penalty = 0.0;
      if (annotated_ref.HasPredecessor()) {
        gradient_point_type step_gradient = gradient_calculator.ComputeOnDemand(cand_key.Point(),
                                                                                ref_key.Point());
        next_step_penalty = gradient_calculator.ComputeStepPenalty(ref_key.Point(),
                                                                   cand_key.Point(),
                                                                   step_gradient);
      }

      penalties = penalty + next_step_penalty;

      cand_accumulated_penalties = 0.5 + penalty * weight + weight;
      /* 1.0E3 value garantees that the vertex weight is prioritized and
         penalties are only used as a tie breaker*/
      path_distance = 1 + ref_accumulated_penalties + cand_accumulated_penalties + 1.0e3 * weight;
    } else {
      if (annotated_ref.HasPredecessor()) {
        gradient_point_type vector_a =
            gradient_calculator.CreateGradientVector(annotated_ref.Predecessor().Point(),
                                                    ref_key.Point());
        gradient_point_type vector_b =
            gradient_calculator.CreateGradientVector(cand_key.Point(),
                                                     ref_key.Point());

        penalty_direction = gradient_calculator.IsGradientSumZero(vector_a, vector_b)?0:0.5;

        if (!graph_.IsLocalMaximalVertex(cand_key)) {
          gradient_point_type step_gradient = gradient_calculator.ComputeOnDemand(cand_key.Point(),
                                                                                  ref_key.Point());
          penalty_direction += gradient_calculator.ComputeStepPenalty(ref_key.Point(),
                                                                      cand_key.Point(),
                                                                      step_gradient);
        }
      }
      penalty_direction += path_calculator_.GetWeight(ref_key.Point(), cand_key.Point());

      cand_accumulated_penalties = penalty_direction * weight + penalties * weight +
                                   weight;
      /* 1.0E3 value garantees that the vertex weight is prioritized and
         penalties are only used as a tie breaker*/
      path_distance = ref_accumulated_penalties + cand_accumulated_penalties +
                      1.0e3 * weight;
    }
    cand_accumulated_distance = ref_accumulated_distance + weight;

    return path_distance;
  }

  bool IsValidCandidate(const annotated_vertex_type &reference,
                        const vertex_key_type &candidate_key) const {
    auto annotation = annotated_graph_[reference.Key()];
    if (!annotation.HasPredecessor())
      return true;

    vertex_key_type predecessor = annotation.Predecessor();


    return !predecessor.Point().IsFaceNeighbour(candidate_key.Point());
  }

  bool RelaxGradient(const annotated_vertex_type &reference,
                     const vertex_key_type &candidate_key,
                     vertex_annotation_type &cand_annotation,
                     gradient_calculator_type &gradient_calculator,
                     const gradient_point_type &gradient) {
    auto ref_annotation_acc = annotated_graph_accumulated_[reference.Key()];
    auto ref_accumulated = 0.0;
    if (ref_annotation_acc.HasPredecessor()) {
      ref_accumulated = ref_annotation_acc.Distance();
    }

    auto ref_annotation_acc_pen = annotated_graph_acc_penalties_[reference.Key()];
    auto ref_accumulated_pen = 0.0;
    if (ref_annotation_acc_pen.HasPredecessor()) {
      ref_accumulated_pen = ref_annotation_acc_pen.Distance();
    }

    distance_type cand_accumulated_distance;
    distance_type cand_accumulated_penalties;
    distance_type vertices_distance = ComputeVerticeDistance(gradient_calculator,
                                                             gradient,
                                                             reference.Key(),
                                                             candidate_key,
                                                             ref_accumulated,
                                                             ref_accumulated_pen,
                                                             cand_accumulated_distance,
                                                             cand_accumulated_penalties);

    if (cand_annotation.Distance() > vertices_distance) {
      cand_annotation.SetDistance(vertices_distance);
      cand_annotation.SetPredecessor(reference.Key());

      annotated_graph_acc_penalties_[candidate_key].SetDistance(cand_accumulated_penalties);
      annotated_graph_acc_penalties_[candidate_key].SetPredecessor(reference.Key());

      annotated_graph_accumulated_[candidate_key].SetDistance(cand_accumulated_distance);
      annotated_graph_accumulated_[candidate_key].SetPredecessor(reference.Key());

      label_type new_label = clusters_discoverer_.GetClusterLabel(candidate_key);
      if (new_label >= 0)
        SetPathLabel(candidate_key, new_label);
      else
        SetPathLabel(candidate_key, GetLabelPath(reference.Key()));
      return true;
    }

    return false;
  }

  std::vector<vertex_key_type> BuildValidList(const vertex_key_type &predecessor,
                                              const vertex_key_type &reference) {
    std::vector<vertex_key_type> valid_list;
    std::vector<vertex_key_type> relaxed_valid_list;
    auto neighbours = graph_.GetVertexNeighbours(reference);
    bool ref_maximal = graph_.IsLocalMaximalVertex(reference);
    if (ref_maximal) {
      for (const auto &candidate : neighbours) {
        if (candidate.Point().IsVertexNeighbour(reference.Point()))
          continue;
        if (!candidate.Point().IsFaceNeighbour(predecessor.Point()))
          valid_list.push_back(candidate);
      }
    } else {
      for (const auto &candidate : neighbours) {
        if (candidate.Point().IsVertexNeighbour(reference.Point()))
          continue;
        if (candidate.Point().IsFaceNeighbour(predecessor.Point()))
          continue;
        bool valid = true;
        for (const auto &vertex_key : neighbours) {
          if (vertex_key.Point().IsVertexNeighbour(reference.Point()))
            continue;
          if (vertex_key.Point() == predecessor.Point() ||
              vertex_key.Point() == candidate.Point())
              continue;
          if (vertex_key.Point().IsFaceNeighbour(reference.Point()) &&
              vertex_key.PropertyValue() > reference.PropertyValue()) {
            valid = false;
            break;
          }

          if (vertex_key.PropertyValue() > reference.PropertyValue() &&
              (vertex_key.Point().IsEdgeNeighbour(predecessor.Point()) ||
              vertex_key.Point().IsFaceNeighbour(predecessor.Point())) &&
              (vertex_key.Point().IsEdgeNeighbour(candidate.Point()) ||
              vertex_key.Point().IsFaceNeighbour(candidate.Point()))) {
            valid = false;
            break;
          }
        }
        if (valid) {
          valid_list.push_back(candidate);
        }
        relaxed_valid_list.push_back(candidate);
      }
    }

    if (!valid_list.empty())
      return valid_list;
    else if (!relaxed_valid_list.empty())
      return relaxed_valid_list;
    else
      return neighbours;
  }

  std::vector<vertex_key_type> BuildValidNeighbours(const annotated_vertex_type &reference) {
    auto annotation = annotated_graph_[reference.Key()];
    if (annotation.HasPredecessor()) {
      vertex_key_type predecessor = annotation.Predecessor();
      return BuildValidList(predecessor, reference.Key());
    } else {
      return graph_.GetVertexNeighbours(reference);
    }
  }

  void RelaxNeighboursGradient(const annotated_vertex_type &reference,
                               queue_type &queue,
                               gradient_calculator_type &gradient_calculator) {
    auto annotated_ref = annotated_graph_[reference.Key()];
    std::vector<vertex_key_type> valid_neighbours = BuildValidNeighbours(reference);

    gradient_point_type gradient;
    if (!annotated_ref.HasPredecessor()) {
      gradient = gradient_calculator.ComputeOnDemand(reference.Key().Point(),
                                                     reference.Key().Point());
    } else {
      gradient = gradient_calculator.ComputeOnDemand(reference.Key().Point(),
                                                     annotated_ref.Predecessor().Point());

      label_type ref_label = GetLabelPath(reference.Key());
      label_type pred_label = GetLabelPath(annotated_ref.Predecessor());

      SetPairLabelsVisited(ref_label, pred_label);
    }

    for (const auto &vertex_key : valid_neighbours) {
      bool new_vertex = false;
      if (!annotated_graph_.HasVertex(vertex_key)) {
        auto infinite_distance = weight_calculator_type::InfiniteDistance();
        auto annotated_vertex = annotated_vertex_type{vertex_key,
                                                      infinite_distance};
        annotated_graph_.Insert(annotated_vertex);

        auto annotated_vertex_acc = annotated_vertex_type{vertex_key,
                                                          infinite_distance};
        annotated_graph_accumulated_.Insert(annotated_vertex_acc);

        auto annotated_vertex_acc_pen = annotated_vertex_type{vertex_key,
                                                              infinite_distance};
        annotated_graph_acc_penalties_.Insert(annotated_vertex_acc_pen);
        new_vertex = true;
      }
      auto &vertex_annotation = annotated_graph_[vertex_key];

      if (vertex_annotation.IsRemoved()) {
        label_type ref_label = GetLabelPath(reference.Key());
        label_type vertex_label = GetLabelPath(vertex_key);
        if (!IsPairLabelsVisited(ref_label, vertex_label)) {
            gradient_point_type local_gradient =
                gradient_calculator.ComputeOnDemand(vertex_key.Point(),
                                                    reference.Key().Point());
            double penalty = gradient_calculator.ComputeStepPenalty(reference.Key().Point(),
                                                                    vertex_key.Point(),
                                                                    local_gradient);

            if (penalty < 0.1) {
              SetPairLabelsVisited(ref_label, vertex_label);
              end_vertices_candidate_list_.push_back(std::make_pair(reference.Key(), vertex_key));
            }
        }
        continue;
      }

      auto have_relaxed = RelaxGradient(reference, vertex_key, vertex_annotation,
                                        gradient_calculator, gradient);
      if (have_relaxed) {
        if (new_vertex) {
          auto handle = queue.Insert(annotated_vertex_type {vertex_key, vertex_annotation});
          vertex_annotation.SetTag(handle);
        } else {
          queue.DecreasePriority(
              vertex_annotation.template Tag<queue_handle_type>(),  // handle
              annotated_vertex_type {vertex_key, vertex_annotation});
        }
      }
    }
  }

  void EnqueueNeighbours(const priority_queue_element_type &element,
                       priority_queue_type &queue,
                       int64_t &v_id) {
    // Initialising variables
    const annotated_vertex_type &reference = element.annotation;
    auto &ref_annotation = reference.Annotation();
    distance_type ref_distance = ref_annotation.Distance();
    vertex_key_type ref_key = reference.Key();
    auto neighbours = graph_.GetVertexNeighbours(reference);
    std::size_t prev_size = queue.size();

    // Enqueuing not visited neighbours
    for (const vertex_key_type &vertex_key : neighbours) {
      if (annotated_graph_.HasVertex(vertex_key) || annotated_graph_leaf_.HasVertex(vertex_key))
        continue;
      distance_type weight = weight_calculator_.GetWeight(ref_key, vertex_key);
      distance_type vertices_distance = ref_distance + weight;

      annotated_vertex_type annotated_vertex = annotated_vertex_type{vertex_key,
                                                      vertices_distance};
      annotated_graph_.Insert(annotated_vertex);
      auto &vertex_annotation = annotated_graph_[vertex_key];

      vertex_annotation.SetDistance(vertices_distance);
      vertex_annotation.SetPredecessor(ref_key);

      queue.push(priority_queue_element_type{ annotated_vertex_type {vertex_key, vertex_annotation},
                                              vertices_distance,
                                              0.0,
                                              v_id,
                                              0.0, 0.0});
      v_id+=1;
    }
    if (prev_size == queue.size()) {
     leaf_list_.push_back(reference);
     distance_type ref_weight = weight_calculator_.GetWeight(ref_key, ref_key);
     if (max_leaf_weight_ - ref_weight < tolerance )
      max_leaf_weight_ = ref_weight;
    }
  }

  void EnqueueNeighboursGradient(const priority_queue_element_type &element,
                                 gradient_calculator_type &gradient_calculator,
                                 priority_queue_type &queue,
                                 int64_t &v_id) {
    // Initialising variables
    const annotated_vertex_type &reference = element.annotation;
    auto &ref_annotation = reference.Annotation();
    distance_type ref_distance = ref_annotation.Distance();
    vertex_key_type ref_key = reference.Key();
    auto neighbours = graph_.GetVertexNeighbours(reference);
    gradient_calculator.MarkAsVisited(ref_key.Point());
    auto gradient = gradient_calculator.ComputeOnDemand(ref_key.Point());

    // Enqueuing not visited neighbours
    for (const vertex_key_type &vertex_key : neighbours) {
      if (annotated_graph_.HasVertex(vertex_key))
        continue;

      distance_type cand_accumulated_distance;
      distance_type cand_accumulated_penalties;
      distance_type path_distance = ComputeVerticeDistance(gradient_calculator,
                                                           gradient,
                                                           ref_key,
                                                           vertex_key,
                                                           ref_distance,
                                                           element.accumulated,
                                                           cand_accumulated_distance,
                                                           cand_accumulated_penalties);

      annotated_vertex_type annotated_vertex = annotated_vertex_type{vertex_key,
                                                                     cand_accumulated_distance};
      annotated_graph_.Insert(annotated_vertex);
      auto &vertex_annotation = annotated_graph_[vertex_key];

      vertex_annotation.SetDistance(cand_accumulated_distance);
      vertex_annotation.SetPredecessor(ref_key);

      queue.push(priority_queue_element_type{ annotated_vertex_type {vertex_key, vertex_annotation},
                                              path_distance,
                                              cand_accumulated_distance,
                                              v_id,
                                              cand_accumulated_penalties,
                                              0.0});
      v_id+=1;
    }
  }

 public:
   /**
   * \brief Constructs an instance of the class from an (immutable) graph.
   * \param graph The instance of a read-only graph of type Graph.
   * \param clusters_discoverer The instance of a read-only object of type clusters_discoverer_type.
   */
  explicit Dijkstra(const graph_type &graph,
                    const clusters_discoverer_type &clusters_discoverer)
       : graph_(graph),
         annotated_graph_(graph),
         annotated_graph_accumulated_(graph),
         annotated_graph_acc_penalties_(graph),
         annotated_graph_leaf_(graph),
         clusters_discoverer_(clusters_discoverer),
         annotated_label_graph_(graph) {
     annotated_graph_leaf_.Clear();
  }

  Dijkstra(const Dijkstra &) = delete;
  /* no copy */
  Dijkstra &operator=(const Dijkstra &) = delete; /* no assignment */

  /**
   * \brief Executes the Dijkstra's algorithm using the vertex argument.
   * This method cleans previous results before its execution.
   *
   * \param source The instance of a read-only vertex of the graph to behave
   * as a source.
   *
   * \return true if the algorithm executed; false otherwise (possibly the
   * source is not in the graph).
   */
  bool Execute(const vertex_type &source) {
    if (!graph_.HasVertex(source))
      return false;
    queue_type queue{};
    Initialise(source, queue);
    while (!queue.IsEmpty()) {
      auto current_vertex = queue.Remove();
      annotated_graph_[current_vertex.Key()].SetRemoved();
      RelaxNeighbours(current_vertex, queue);
    }
    return true;
  }

  bool ExecutePriorityQueue(const vertex_type &source) {
    if (!graph_.HasVertex(source))
      return false;
    priority_queue_type queue;
    int64_t v_id;
    Initialise(source, queue, v_id);
    while (!queue.empty()) {
      auto current_vertex = queue.top();
      queue.pop();
      EnqueueNeighbours(current_vertex, queue, v_id);
    }
    for (const auto &leaf : leaf_list_) {
      auto leaf_weight = weight_calculator_.GetWeight(leaf.Key(), leaf.Key());
      if (leaf_weight - max_leaf_weight_ > tolerance)
        annotated_graph_leaf_.Insert(leaf);
    }
    return true;
  }

  bool ExecuteGradient(const vertex_type &source,
                       gradient_calculator_type &gradient_calculator) {
    if (!graph_.HasVertex(source))
      return false;
    queue_type queue{};
    Initialise(source, queue);
    while (!queue.IsEmpty()) {
      auto current_vertex = queue.Remove();
      annotated_graph_[current_vertex.Key()].SetRemoved();

      RelaxNeighboursGradient(current_vertex,
                              queue,
                              gradient_calculator);
    }
    return true;
  }

  void RemoveFromLeaf(const vertex_type &vertex) {
    annotated_graph_leaf_.Remove(vertex);
  }

  /**
   * \brief Returns a list of vertices candidates to be in the centerline.
   */
  end_vertices_candidate_list_type const &GetEndCandidatesList() const {
    return end_vertices_candidate_list_;
  }

  /**
   * \brief Returns the result as an annotated graph. Each vertex has an predecessor
   * and the total accumulated weight from the source to it.
   */
  annotated_graph_type const &GetResult() const {
    return annotated_graph_;
  }

  label_type GetLabelPath(const vertex_key_type &vertex_key) const {
    return annotated_label_graph_[vertex_key].GetClusterId();
  }
};

}  // namespace skeleton

#endif  // SRC_SKELETON_GRAPH_DIJKSTRA_H_
