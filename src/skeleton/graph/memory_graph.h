/**
 * \file src/skeleton/graph/memory_graph.h
 * \brief Implements a graph data structure that favours memory-efficiency.
 *
 * \author Alexandre Ashade Lassance Cunha \<aashade@br.ibm.com\>
 * \authors Hugo de Oliveira Barbalho \<hugob@br.ibm.com\>
 * \copyright © IBM Corp.
 * \date 2016
 */

#ifndef SRC_SKELETON_GRAPH_MEMORY_GRAPH_H_
#define SRC_SKELETON_GRAPH_MEMORY_GRAPH_H_

#include <unordered_map>
#include <vector>
#include <exception>
#include <utility>
#include "src/skeleton/utils/comparer.h"
#include "src/skeleton/utils/hasher.h"

namespace skeleton {

/**
 * \brief Implementation of a Graph data structure.
 *
 * \tparam Vertex The vertex implementation should have two properties:
 * Vertex.Key() & Vertex.Annotation(). The Key() is an immutable unique
 * representation of the Vertex, whereas the Annotation() is a mutable
 * annotation object, having at least 2 properties: distance and predecessor.
 * The property "distance" should be a nonnegative number, while the property
 * predecessor should be of the same type of Vertex.Key().
 *
 * \tparam VertexNeighbourCalculator The adjacency relation for this graph.
 *
 */
template<class Vertex,
         class VertexNeighbourCalculator,
         class THasher = Hasher<typename Vertex::key_type>,
         class TComparer = Comparer<typename Vertex::key_type>>
class MemoryGraph {
 public:
  /**
   * \brief Alias of the template argument Vertex.
   */
  using vertex_type = Vertex;

  /**
   * \brief Alias of the key type, from the vertex type.
   */
  using vertex_key_type = typename vertex_type::key_type;

  /**
   * \brief Alias of the annotation type, from the vertex type.
   */
  using vertex_annotation_type = typename vertex_type::annotation_type;

  /**
   * \brief Alias of the vertex set type.
   */
  using vertex_set_type = std::unordered_map<vertex_key_type,
                                             vertex_annotation_type,
                                             THasher,
                                             TComparer>;

  /**
   * \brief Alias of the graph type, from the file memory_graph.h.
   */
  using graph_type = MemoryGraph<vertex_type,
                                 VertexNeighbourCalculator,
                                 THasher,
                                 TComparer>;

  /**
   * \brief Alias of the constant iterator.
   */
  using const_iterator = typename vertex_set_type::const_iterator;

  /**
   * \brief Alias of the neighbours list.
   */
  using neighbours_type = std::vector<vertex_key_type>;

  /**
   * \brief Alias of the VertexNeighbourCalculator.
   */
  using neighbour_calculator = VertexNeighbourCalculator;

 private:
  vertex_set_type vertex_set_;
  const neighbour_calculator calculator_;

 public:
  /**
   * \brief Constructs a new instance of \c MemoryGraph by receiving its points and
   * adjacency relation.
   *
   * \param vertex_set The set of all vertices.
   * \param calculator The adjacency relation.
   */
  MemoryGraph(vertex_set_type &&vertex_set, const neighbour_calculator &calculator)
    : vertex_set_(vertex_set),
      calculator_(calculator) {
  }

  /**
   * \brief Constructs a new instance of \c MemoryGraph by receiving another instance.
   *
   * \param graph A \c MemoryGraph instance.
   */
  MemoryGraph(const MemoryGraph &graph)
    : vertex_set_(graph.vertex_set_),
      calculator_(graph.calculator_) {
  }

  /**
   * \brief Constructs an empty new instance of \c MemoryGraph by receiving only an
   * adjacency relation. The user should call Insert to add new vertices.
   *
   * \param calculator The adjacency relation.
   */
  explicit MemoryGraph(const neighbour_calculator &calculator)
    : vertex_set_(),
      calculator_(calculator) {
  }

  /**
   * \brief Returns the annotation of a vertex, given its key. If there is no vertex
   * with the corresponding key, then the method throws an std::out_of_range
   * exception. The returned annotation is editable, so this method is useful
   * for modifying the graph.
   *
   * \param vertex_key The key of the vertex.
   */
  vertex_annotation_type &operator[](const vertex_key_type &vertex_key) {
    return vertex_set_.at(vertex_key);
  }

  /**
   * \brief Returns the (constant) annotation of a vertex, given its key.
   * If there is no vertex with the corresponding key, then the method throws
   * an std::out_of_range exception. The returned annotation is immutable.
   *
   * \param vertex_key The key of the vertex.
   */
  vertex_annotation_type const &operator[](
      const vertex_key_type &vertex_key) const {
    return vertex_set_.at(vertex_key);
  }

  /**
   * \brief Clears all vertices and corresponding annotations.
   */
  void Clear() {
    vertex_set_.clear();
  }

  /**
   * \brief Removes a vertex in the graph.
   *
   * \param vertex The vertex to remove.
   */
  void Remove(const vertex_type &vertex) {
    vertex_set_.erase(vertex.Key());
  }

  /**
   * \brief Inserts new vertices in the graph. If there is another vertex in the
   * graph having the same key, then this method throws an exception.
   *
   * \param vertex The vertex to insert.
   */
  void Insert(const vertex_type &vertex) {
    vertex_set_.insert(std::make_pair(vertex.Key(), vertex.Annotation()));
  }

  /**
   * \brief Checks whether a vertex with a given key exists in the graph.
   *
   * \param vertex The vertex to check.
   * \return true if the graph has a vertex with the same key; false otherwise.
   */
  bool HasVertex(const vertex_type &vertex) const noexcept {
    return HasVertex(vertex.Key());
  }

  /**
   * \brief Checks whether a vertex with a given key exists in the graph.
   *
   * \param vertex_key The vertex key to look for.
   * \return true if the graph has a vertex with the same key; false otherwise.
   */
  bool HasVertex(const vertex_key_type &vertex_key) const noexcept {
    return vertex_set_.find(vertex_key) != vertex_set_.end();
  }

  /**
   * \brief Returns the list of neighbours of the vertex. Internally, this method
   * utilises the adjacency relation passed to the constructor of this class.
   *
   * \param vertex This method returns the neighbours of this parameter.
   * \return The list of neighbours.
   */
  neighbours_type GetVertexNeighbours(const vertex_type &vertex) const {
    return GetVertexNeighbours(vertex.Key());
  }

  /**
   * \brief Returns the list of neighbours of the vertex. Internally, this method
   * utilises the adjacency relation passed to the constructor of this class.
   *
   * \param vertex_key This method returns the neighbours of this parameter.
   * \return The list of neighbours.
   */
  neighbours_type GetVertexNeighbours(const vertex_key_type &vertex_key) const {
    if (!HasVertex(vertex_key))
      throw std::exception{};
    auto neighbours = calculator_.GetNeighbours(vertex_key);
    neighbours_type output;
    for (const auto &neighbour : neighbours)
      if (HasVertex(neighbour))
        output.push_back(neighbour);
    return output;
  }

  /**
   * \brief Checks whether a vertex is local maximum in the graph by its key.
   *
   * \param vertex_key The vertex key to check.
   * \return true if the vertex_key has not a neighbour vertex with a greater weight; false otherwise.
   */
  bool IsLocalMaximalVertex(const vertex_key_type &vertex_key) const {
    if (!HasVertex(vertex_key))
      throw std::exception{};
    if (vertex_set_.at(vertex_key).IsLocalMaximalSet()) {
      return vertex_set_.at(vertex_key).IsLocalMaximal();
    }

    neighbours_type neighbours = GetVertexNeighbours(vertex_key);
    for (const auto &neighbour : neighbours) {
      if (neighbour.Point().IsVertexNeighbour(vertex_key.Point()))
        continue;
      if (neighbour.PropertyValue() > vertex_key.PropertyValue())
        return false;
    }
    return true;
  }

  /**
   * \brief Returns the forward iterator to the first vertex of the graph. Useful for
   * iterating through all the graph vertices.
   */
  const_iterator begin() const noexcept {
    return vertex_set_.begin();
  }

  /**
   * \brief Returns the forward iterator to past-the-last vertex of the graph.
   * Useful for iterating through all the graph vertices.
   */
  const_iterator end() const noexcept {
    return vertex_set_.end();
  }

  /**
   * \brief Returns the neighbour calculator in use.
   */
  neighbour_calculator Calculator() const {
    return calculator_;
  }
};

}  // namespace skeleton

#endif  // SRC_SKELETON_GRAPH_MEMORY_GRAPH_H_
