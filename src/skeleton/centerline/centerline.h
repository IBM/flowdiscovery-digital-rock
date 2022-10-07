/**
 * \file src/skeleton/centerline/centerline.h
 * \brief This file implements the data structure that represents a single
 * centerline.
 *
 * \author Alexandre Ashade Lassance Cunha \<aashade@br.ibm.com\>
 * \copyright © IBM Corp.
 * \date 2016
 */

#ifndef SRC_SKELETON_CENTERLINE_CENTERLINE_H_
#define SRC_SKELETON_CENTERLINE_CENTERLINE_H_

#include <vector>

namespace skeleton {

template<class Node>
class Centerline {
 public:
  using node_type = Node;

 private:
  std::vector<node_type> nodes_;

 public:
  using const_iterator = typename std::vector<node_type>::const_iterator;

  explicit Centerline(std::vector<node_type> nodes) : nodes_(nodes) { }

  const node_type &GetStart() const {
    if (nodes_.empty())
      throw std::exception{};
    return nodes_.back();
  }

  const node_type &GetEnd() const {
    if (nodes_.empty())
      throw std::exception{};
    return nodes_.front();
  }

  std::size_t GetNumPoints() const {
    return nodes_.size();
  }

  const node_type &operator[](const std::size_t index) const {
    return nodes_.at(index);
  }

  const_iterator begin() const {
    return nodes_.begin();
  }

  const_iterator end() const {
    return nodes_.end();
  }

  Centerline split(const std::size_t index){
    if (index == 0 || index == GetNumPoints() - 1) {
      std::vector<node_type> new_nodes;
      Centerline new_centerline(new_nodes);
      return new_centerline;
    }

    std::vector<node_type> new_nodes;
    new_nodes.assign(nodes_.begin() + static_cast<int>(index), nodes_.end());
    std::size_t original_size = GetNumPoints();
    for (std::size_t i = index + 1; i != original_size; ++i)
      nodes_.pop_back();

    Centerline new_centerline(new_nodes);
    return new_centerline;
  }
};

}  // namespace skeleton

#endif  // SRC_SKELETON_CENTERLINE_CENTERLINE_H_
