/**
 * \file src/skeleton/centerline/centerline_builder.h
 * \brief This file implements helper class to build a centerline from its
 * nodes.
 *
 * \author Alexandre Ashade Lassance Cunha \<aashade@br.ibm.com\>
 * \copyright © IBM Corp.
 * \date 2016
 */

#ifndef SRC_SKELETON_CENTERLINE_CENTERLINE_BUILDER_H_
#define SRC_SKELETON_CENTERLINE_CENTERLINE_BUILDER_H_

#include <vector>
#include "src/skeleton/centerline/centerline.h"

namespace skeleton {

template<class Node>
class CenterlineBuilder {
 public:
  using node_type = Node;
  using centerline_type = Centerline<node_type>;

 private:
  std::vector<node_type> nodes_;

 public:
  CenterlineBuilder() : nodes_() {
  }

  void Clear() {
    nodes_.clear();
  }

  bool HasNodes() const {
    return !nodes_.empty();
  }

  void AddNode(const node_type &node) {
    nodes_.push_back(node);
  }

  centerline_type GetCenterline() const {
    return centerline_type {nodes_};
  }
};

}  // namespace skeleton

#endif  // SRC_SKELETON_CENTERLINE_CENTERLINE_BUILDER_H_
