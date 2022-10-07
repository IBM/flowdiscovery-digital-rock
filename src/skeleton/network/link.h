/**
 * \file src/skeleton/network/link.h
 * \brief This file implements the class that stores the link definition for the capillary network.
 *
 * \authors Rodrigo Neumann Barros Ferreira \<rneumann@br.ibm.com\>
 * \copyright © IBM Corp.
 * \date 2018
 */

#ifndef SRC_SKELETON_NETWORK_LINK_H_
#define SRC_SKELETON_NETWORK_LINK_H_

namespace skeleton {

class Link {
 public:
  using id_type = std::size_t;
  using link_geometry_type = double;

  /// Parameterised constructor with full member initialisation
  Link(const id_type &id,
       const id_type &source_id,
       const id_type &target_id,
       const link_geometry_type &length,
       const link_geometry_type &squared_radius)
    : id_(id),
      source_id_(source_id),
      target_id_(target_id),
      length_(length),
      squared_radius_(squared_radius) { }

  /// Parameterised constructor with incomplete member initialisation (temporary link)
  Link(const id_type &source_id, const id_type &target_id)
    : id_(), source_id_(source_id), target_id_(target_id) { }

  /// Getter for \c id_ private member
  id_type GetId(void) const { return id_; }

  /// Getter for \c source_id_ private member
  id_type GetSourceId(void) const { return source_id_; }

  /// Getter for \c target_id_ private member
  id_type GetTargetId(void) const { return target_id_; }

  /// Getter for \c length_ private member
  link_geometry_type GetLength(void) const { return length_; }

  /// Getter for \c squared_radius_ private member
  link_geometry_type GetSquaredRadius(void) const { return squared_radius_; }

  /// Custom comparison operator required by \c std::set
  inline bool operator< (const Link &other) const noexcept {
    return (source_id_ < other.source_id_) ||
           (source_id_ == other.source_id_ && target_id_ < other.target_id_);
  }

 private:
  /// Link identification
  id_type id_;

  /// Source node identification
  id_type source_id_;

  /// Target node identification
  id_type target_id_;

  /// Link length (in voxel units)
  link_geometry_type length_;

  /// Link squared radius (in voxel units)
  link_geometry_type squared_radius_;
};

}  // namespace skeleton

#endif  // SRC_SKELETON_NETWORK_LINK_H_
