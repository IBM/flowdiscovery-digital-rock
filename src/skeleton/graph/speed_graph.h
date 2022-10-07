/**
 * \file src/skeleton/graph/speed_graph.h
 * \brief Implements a graph data structure that favours high-performance.
 *
 * \author Alexandre Ashade Lassance Cunha \<aashade@br.ibm.com\>
 * \authors Hugo de Oliveira Barbalho \<hugob@br.ibm.com\>
 * \copyright © IBM Corp.
 * \date 2016
 */

#ifndef SRC_SKELETON_GRAPH_SPEED_GRAPH_H_
#define SRC_SKELETON_GRAPH_SPEED_GRAPH_H_

#include <vector>
#include <exception>
#include <utility>
#include <array>
#include <limits>

#include "src/skeleton/graph/speed_graph_iterator.h"

namespace skeleton {

/**
 * \brief Implementation of a Graph data structure with a "processing speed flavour".
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
 * \tparam KeyIndexConverter A helper class that is able to convert the
 * vertex key to a unique integer and vice versa. This class should implement
 * two overloads of operator(), one to convert from index to vertex key and
 * another one to convert from vertex key to index. Both overloads are
 * supposed to be a bijection and one is the inverse of the other.
 *
 */
template<class Vertex, class VertexNeighbourCalculator, class KeyIndexConverter>
class SpeedGraph {
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
  using vertex_set_type = std::vector<std::pair<vertex_key_type,
                                                vertex_annotation_type>>;

  /**
   * \brief Alias of the graph type, from the file graph.h.
   */
  using graph_type = SpeedGraph<vertex_type,
                                VertexNeighbourCalculator,
                                KeyIndexConverter>;

  /**
   * \brief Alias of the KeyIndexConverter.
   */
  using key_converter_type = KeyIndexConverter;

  /**
   * \brief Alias of the constant iterator.
   */
  using const_iterator = SpeedGraphIterator<graph_type, key_converter_type>;

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
  std::vector<bool> has_annotation_list_;
  const neighbour_calculator calculator_;
  const key_converter_type convert_key_;

  vertex_annotation_type &GetAnnotation(std::size_t index) {
    return vertex_set_.at(index).second;
  }

  vertex_annotation_type const &GetAnnotation(std::size_t index) const {
    return vertex_set_.at(index).second;
  }

 public:
  /**
   * \brief Constructs a new instance of the graph by receiving its points and
   * adjacency relation.
   *
   * \param vertex_set The set of all vertices.
   * \param has_annotation_list A boolean list defining which points are
   * annotated.
   * \param calculator The adjacency relation.
   * \param converter A functor to convert from the vertex key to a integer
   * index.
   */
  SpeedGraph(vertex_set_type &&vertex_set,
             std::vector<bool> &&has_annotation_list,
             const neighbour_calculator &calculator,
             const key_converter_type converter)
      : vertex_set_(vertex_set),
        has_annotation_list_(has_annotation_list),
        calculator_(calculator),
        convert_key_(converter) {
    if (vertex_set.size() != has_annotation_list.size())
      throw std::exception{};
  }

  /**
   * \brief Copy constructor. Useful for the Dijkstra's algorithm
   * implementation.
   */
  SpeedGraph(const SpeedGraph &graph)
      : vertex_set_(graph.vertex_set_),
        has_annotation_list_(graph.has_annotation_list_),
        calculator_(graph.calculator_),
        convert_key_(graph.convert_key_) {
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
    auto index = convert_key_(vertex_key);
    if (!has_annotation_list_[index])
      throw std::out_of_range("vertex does not exist in graph.");
    return GetAnnotation(index);
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
    auto index = convert_key_(vertex_key);
    if (!has_annotation_list_[index])
      throw std::out_of_range("vertex does not exist in graph.");
    return GetAnnotation(index);
  }

  /**
   * \brief Clears all vertices and corresponding annotations.
   */
  void Clear() {
    has_annotation_list_.assign(has_annotation_list_.size(), false);
  }

  /**
   * \brief Removes a vertex of the graph.
   *
   * \param vertex The vertex to remove.
   */
  void Remove(const vertex_type &vertex) {
    auto index = convert_key_(vertex.Key());
    has_annotation_list_.at(index) = false;
  }

  /**
   * \brief Inserts new vertices in the graph. If there is another vertex in the
   * graph having the same key, then this method throws an exception.
   *
   * \param vertex The vertex to insert.
   */
  void Insert(const vertex_type &vertex) {
    auto index = convert_key_(vertex.Key());
    vertex_set_.at(index) = std::make_pair(vertex.Key(), vertex.Annotation());
    has_annotation_list_.at(index) = true;
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
    auto index = convert_key_(vertex_key);
    return has_annotation_list_.at(index);
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
   * \brief Returns the list of neighbours of the vertex key. Internally, this method
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
   * \param vertex_key The vertex to check.
   * \return true if the vertex has not a neighbour vertex with a greater weight; false otherwise.
   */
  bool IsLocalMaximalVertex(const vertex_key_type &vertex_key) const {
    if (!HasVertex(vertex_key))
      throw std::exception{};
    auto index = convert_key_(vertex_key);
    if (GetAnnotation(index).IsLocalMaximalSet()) {
      return GetAnnotation(index).IsLocalMaximal();
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
    return const_iterator{convert_key_,
                          &vertex_set_,
                          &has_annotation_list_};
  }

  /**
   * \brief Returns the forward iterator to past-the-last vertex of the graph.
   * Useful for iterating through all the graph vertices.
   */
  const_iterator end() const noexcept {
    return const_iterator{vertex_set_.size(),
                          convert_key_,
                          &vertex_set_,
                          &has_annotation_list_};
  }

  /**
   * \brief Returns the neighbour calculator in use.
   */
  neighbour_calculator Calculator() const {
    return calculator_;
  }
};

}  // namespace skeleton

#endif  // SRC_SKELETON_GRAPH_SPEED_GRAPH_H_
