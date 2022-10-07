/**
 * \file src/skeleton/network/node.h
 * \brief This file implements the class that stores the node definition for the capillary network.
 *
 * \authors Rodrigo Neumann Barros Ferreira \<rneumann@br.ibm.com\>
 * \copyright © IBM Corp.
 * \date 2018
 */

#ifndef SRC_SKELETON_NETWORK_NODE_H_
#define SRC_SKELETON_NETWORK_NODE_H_

#include "src/skeleton/points/voxel.h"
#include "src/skeleton/annotations/annotation.h"
#include "src/skeleton/neighbours/twenty_six_neighbour_calculator.h"

namespace skeleton {

template<class TPoint = Voxel,
         class TAnnotation = Annotation<TPoint>>
class Node {
 public:
  using point_type = TPoint;
  using id_type = std::size_t;
  using point_builder_type = PointBuilder;  // check parameter type: uint16_t vs. CenterlinesRawType
  using coordinate_type = typename point_type::coordinate;
  using annotation_type = typename TAnnotation::displacement_value;
  using coordinates_index_type = typename point_type::coordinates_index_type;
  using neighbours_calculator_type = TwentySixNeighbourCalculator<point_type, point_builder_type>;
  using neighbours_type = typename neighbours_calculator_type::neighbours_type;

  /// Parameterised constructor with full member initialisation
  Node(const id_type &id,
       const point_type &point,
       const annotation_type &annotation)
    : id_(id), point_(point), annotation_(annotation) { }

  /// Parameterised constructor with incomplete member initialisation (temporary node)
  explicit Node(const point_type &point)
    : point_(point) { }

  /// Getter for \c id_ private member
  id_type GetId(void) const { return id_; }

  /// Getter for \c point_ private member
  const point_type &GetPoint(void) const { return point_; }

  /// Getter for \c annotation_ private member
  annotation_type GetAnnotation(void) const { return annotation_; }

  /// Calculates point coordinates of potential neighbours of a node
  const neighbours_type CalculateNeighbourPoints(void) const {
    return calculator_.GetNeighbours(point_);
  }

  const coordinate_type CalculateSquaredDistance(const Node &other) const {
  /**
   * \brief Calculates squared distance to \c other node
   *
   * The \c CalculateSquaredDistance() calculates the sum of the squared differences in the point
   * coordinates \f$ \vec{P} \f$ of one node and the \c other node \f$ \vec{Q} \f$. Special care
   * is taken to avoid underflow since point coordinates often assume \c unsigned types.
   *
   * \f[
   *    d^2 = \sum_{i=0}^{c} ( \max{P_i, Q_i} - \min{P_i, Q_i} )^2
   * \f]
   *
   * \retval  distance  The square of the distance between \c this_node and \c other_node.
   */
    coordinate_type distance = 0U;
    const coordinates_index_type dimensions = point_.GetNumberOfCoordinates();

    // Accumulate differences across coordinate dimensions
    for (coordinates_index_type i = 0U; i != dimensions; ++i) {
      const coordinate_type node_i = this->GetPointCoordinate(i);
      const coordinate_type other_i = other.GetPointCoordinate(i);

      // Accumulate coordinate differences respecting their unsigned-ness
      const coordinate_type diff = (node_i > other_i) ? (node_i - other_i) : (other_i - node_i);
      distance += diff * diff;
    }
    return distance;
  }

  /// Custom \c < comparison operator required by \c std::set
  inline bool operator< (const Node &other) const noexcept {
    return (point_ < other.point_);
  }

  /// Custom \c == comparison operator required by \c EXPECT_EQ()
  inline bool operator== (const Node &other) const noexcept {
    return (point_ == other.point_);
  }

  const coordinate_type GetPointCoordinate(coordinates_index_type coordinate_index) const {
  /**
   * \brief Get \c coordinate_index -th coordinate of \c point_
   *
   * The \c GetPointCoordinate() retrieves the \c coordinate_index -th coordinate of the \c point_
   * member.
   *
   * \param[in] coordinate_index  Index of the requested coordinate of \c point_.
   * \retval    coordinate        Requested coordinate of \c point_.
   */
    return point_.GetCoordinate(coordinate_index);
  }

 private:
  /// Node id
  id_type id_;

  /// Node spatial location (in voxel units)
  point_type point_;

  /// Node squared radius (in voxel units)
  annotation_type annotation_;

  /// Neighbour calculator member object
  neighbours_calculator_type calculator_;
};

}  // namespace skeleton

#endif  // SRC_SKELETON_NETWORK_NODE_H_
