/**
 * \file src/skeleton/network/network.h
 * \brief This file implements the class that stores the capillary network definition.
 *
 * \authors Rodrigo Neumann Barros Ferreira \<rneumann@br.ibm.com\>
 * \copyright © IBM Corp.
 * \date 2018
 */

#ifndef SRC_SKELETON_NETWORK_NETWORK_H_
#define SRC_SKELETON_NETWORK_NETWORK_H_

#include <set>
#include <vector>
#include <utility>
#include <algorithm>
#include "src/skeleton/network/node.h"
#include "src/skeleton/network/link.h"

namespace skeleton {

/**
 * \class Network network.h "src/skeleton/network/network.h"
 * \brief Class that stores the capillary network definition.
 *
 * The \c Network class carries the node and link information required by the capillary network.
 */

template<class TNode = Node<>,
         class TLink = Link>
class Network {
 public:
  // Custom type definition for Node objects
  using node_type = TNode;
  using node_id_type = typename node_type::id_type;
  using node_point_type = typename node_type::point_type;
  using node_annotation_type = typename node_type::annotation_type;
  using node_neighbours_point_type = typename node_type::neighbours_type;
  using node_neighbours_type = std::vector<node_type>;

  // Custom type definition for Node sets
  using node_set_type = std::set<node_type>;
  using node_set_index_type = typename node_set_type::size_type;
  using node_set_iterator_type = typename node_set_type::iterator;
  using node_set_const_iterator_type = typename node_set_type::const_iterator;
  using node_set_insert_return_type = std::pair<node_set_iterator_type, bool>;

  // Custom type definition for Link objects
  using link_type = TLink;
  using link_id_type = typename link_type::id_type;
  using link_geometry_type = typename link_type::link_geometry_type;

  // Custom type definition for Link sets
  using link_set_type = std::set<link_type>;
  using link_set_index_type = typename link_set_type::size_type;
  using link_set_iterator_type = typename link_set_type::iterator;
  using link_set_const_iterator_type = typename link_set_type::const_iterator;
  using link_set_insert_return_type = std::pair<link_set_iterator_type, bool>;

  /// Inserts a \c Node object to \c node_set_ based on its \c point and \c annotation
  const node_set_insert_return_type InsertNode(const node_point_type &point,
                                               const node_annotation_type &annotation) {
    const node_set_index_type index = node_set_.size();
    node_type node(index, point, annotation);
    return node_set_.insert(node);
  }

  /// Retrieves an \c std::vector with the neighbours of \c node that belong to \c node_set_
  const node_neighbours_type CalculateNeighbourNodes(const node_type &node) const {
    node_neighbours_type neighbour_nodes;
    const node_neighbours_point_type points = node.CalculateNeighbourPoints();
    for (const auto &point : points) {
      node_type temp_node(point);
      node_set_const_iterator_type node_iterator = node_set_.find(temp_node);
      if (node_iterator != node_set_.cend()) neighbour_nodes.push_back(*node_iterator);
    }
    return neighbour_nodes;
  }

  /// Checks whether there is already a link between \c node_A and \c node_B inside \c link_set_
  bool HasLink(const node_type &node_A, const node_type &node_B) const {
    const link_id_type source_id = std::min(node_A.GetId(), node_B.GetId());
    const link_id_type target_id = std::max(node_A.GetId(), node_B.GetId());
    link_type temp_link(source_id, target_id);
    return static_cast<bool>(link_set_.count(temp_link));
  }

  /// Inserts a \c Link object to \c link_set_ based on its IDs and geometrical dimensions
  const link_set_insert_return_type InsertLink(const node_type &node_A, const node_type &node_B) {
    const link_set_index_type index = link_set_.size();
    const link_id_type source_id = std::min(node_A.GetId(), node_B.GetId());
    const link_id_type target_id = std::max(node_A.GetId(), node_B.GetId());
    const link_geometry_type length = CalculateLength(node_A, node_B);
    const link_geometry_type squared_radius = CalculateSquaredRadius(node_A, node_B);
    link_type link(index, source_id, target_id, length, squared_radius);
    return link_set_.insert(link);
  }

  /// Getter for the size of \c node_set_
  node_id_type GetNodeSetSize(void) const { return node_set_.size(); }

  /// Getter for the size of \c link_set_
  link_id_type GetLinkSetSize(void) const { return link_set_.size(); }

  /// Custom \c cbegin() iterator for \c node_set_
  node_set_const_iterator_type node_cbegin() const noexcept {
    return node_set_const_iterator_type {node_set_.cbegin()};
  }

  /// Custom \c cend() iterator for \c node_set_
  node_set_const_iterator_type node_cend() const noexcept {
    return node_set_const_iterator_type {node_set_.cend()};
  }

  /// Custom \c cbegin() iterator for \c link_set_
  link_set_const_iterator_type link_cbegin() const noexcept {
    return link_set_const_iterator_type {link_set_.cbegin()};
  }

  /// Custom \c cend() iterator for \c link_set_
  link_set_const_iterator_type link_cend() const noexcept {
    return link_set_const_iterator_type {link_set_.cend()};
  }

 private:
  /// Calculates the length of the link between \c node_A and \c node_B
  link_geometry_type CalculateLength(const node_type &node_A, const node_type &node_B) {
    link_geometry_type distance_squared = node_A.CalculateSquaredDistance(node_B);
    return std::sqrt(distance_squared);
  }

  /// Calculates the squared radius of the link between \c node_A and \c node_B
  link_geometry_type CalculateSquaredRadius(const node_type &node_A, const node_type &node_B) {
    link_geometry_type R2_A = static_cast<link_geometry_type>(node_A.GetAnnotation());
    link_geometry_type R2_B = static_cast<link_geometry_type>(node_B.GetAnnotation());
    return (std::sqrt(2.0) * R2_A * R2_B) / std::sqrt(R2_A * R2_A + R2_B * R2_B);
  }

  /// STL set of \c Node objects
  node_set_type node_set_;

  /// STL set of \c Link objects
  link_set_type link_set_;
};

}  // namespace skeleton

#endif  // SRC_SKELETON_NETWORK_NETWORK_H_
