/**
 * \file src/skeleton/graph/speed_graph_iterator.h
 * \brief Helper class to iterate through a speed_graph implementation.
 *
 * \author Alexandre Ashade Lassance Cunha \<aashade@br.ibm.com\>
 * \author Giulia Duncan Coutinho \<coutingi@br.ibm.com\>
 * \authors Hugo de Oliveira Barbalho \<hugob@br.ibm.com\>
 * \copyright © IBM Corp.
 * \date 2016
 */

#ifndef SRC_SKELETON_GRAPH_SPEED_GRAPH_ITERATOR_H_
#define SRC_SKELETON_GRAPH_SPEED_GRAPH_ITERATOR_H_

#include <utility>
#include <vector>
#include <limits>

namespace skeleton {

template<class SpeedGraph, class IndexKeyConverter>
class SpeedGraphIterator {
 private:
  using graph_type = SpeedGraph;
  using vertex_set_type = typename graph_type::vertex_set_type;
  using vertex_key_type = typename graph_type::vertex_key_type;
  using vertex_annotation_type = typename graph_type::vertex_annotation_type;

  using const_iterator = typename vertex_set_type::const_iterator;
  using converter = IndexKeyConverter;

  const vertex_set_type *vertex_set_;
  const std::vector<bool> *has_annotation_list_;
  std::size_t current_index_;
  const converter convert_;

  static std::size_t MaxIndex() {
    return std::numeric_limits<std::size_t>::max();
  }

  bool IsInvalid() const noexcept {
    return current_index_ == MaxIndex();
  }

  bool IsValid() const noexcept {
    return !IsInvalid();
  }

  std::size_t LastIndex() const noexcept {
    return has_annotation_list_->size() - 1;
  }

  bool IsEnd() const noexcept {
    return IsValid() && current_index_ > LastIndex();
  }

  bool ContentEquals(const SpeedGraphIterator &other) const noexcept {
    return vertex_set_ == other.vertex_set_
        && has_annotation_list_ == other.has_annotation_list_
        && current_index_ == other.current_index_;
  }

  std::size_t GetFirstValidIndex() const {
    for (std::size_t index = 0; index != LastIndex(); ++index)
      if (has_annotation_list_->at(index) == true)
        return index;
    throw std::exception{};  // no valid index found. This should not happen.
  }

 public:
  using iterator_category = std::forward_iterator_tag;
  using value_type = std::pair<vertex_key_type, vertex_annotation_type>;
  using difference_type = std::ptrdiff_t;
  using pointer = value_type const *;
  using reference = value_type const;  // not a reference!

  SpeedGraphIterator(const std::size_t index,
                     const converter convert,
                     const vertex_set_type *vertex_set,
                     const std::vector<bool> *has_annotation_list)
      : vertex_set_(vertex_set),
        has_annotation_list_(has_annotation_list),
        current_index_(index),
        convert_(convert) {
  }

  SpeedGraphIterator(const converter convert,
                     const vertex_set_type *vertex_set,
                     const std::vector<bool> *has_annotation_list)
      : vertex_set_(vertex_set),
        has_annotation_list_(has_annotation_list),
        convert_(convert) {
    current_index_ = GetFirstValidIndex();
  }

  SpeedGraphIterator()
      : vertex_set_(nullptr),
        has_annotation_list_(nullptr),
        current_index_(MaxIndex()),
        convert_() {}

  SpeedGraphIterator(const SpeedGraphIterator &other)
      : vertex_set_(other.vertex_set_),
        has_annotation_list_(other.has_annotation_list_),
        current_index_(other.current_index_),
        convert_(other.convert_) {}

  SpeedGraphIterator &operator=(const SpeedGraphIterator &other) noexcept {
    vertex_set_ = other.vertex_set_;
    has_annotation_list_ = other.has_annotation_list_;
    current_index_ = other.current_index_;
    convert_ = other.convert_;
    return *this;
  }

  bool operator==(const SpeedGraphIterator &other) const noexcept {
    return IsValid() && (ContentEquals(other) || IsEnd() == other.IsEnd());
  }

  bool operator!=(const SpeedGraphIterator &other) const noexcept {
    return !(operator==(other));
  }

  reference operator*() const {
    return vertex_set_->at(current_index_);
  }

  SpeedGraphIterator &operator++() {  // prefix increment (++x)
    if (!IsEnd()) {
      do {
        current_index_ = current_index_ + 1;
      }
      while (current_index_ < has_annotation_list_->size() &&
          !has_annotation_list_->at(current_index_));  // do not add "{}"!
      // it's a do-while. cpplint is buggy.
    }
    return *this;
  }

  SpeedGraphIterator operator++(int) {  // postfix increment (x++)
    SpeedGraphIterator old{*this};
    operator++();
    return old;
  }
};

}  // namespace skeleton

#endif  // SRC_SKELETON_GRAPH_SPEED_GRAPH_ITERATOR_H_
