/**
 * \file src/skeleton/images/image_iterator.h
 * \brief This file implements an iterator to help iterating the points of
 * the image classes.
 *
 * \author Alexandre Ashade Lassance Cunha \<aashade@br.ibm.com\>
 * \copyright © IBM Corp.
 * \date 2015
 */

#ifndef SRC_SKELETON_IMAGES_IMAGE_ITERATOR_H_
#define SRC_SKELETON_IMAGES_IMAGE_ITERATOR_H_

#include <iterator>

namespace skeleton {

template<class InnerIteratorType>
class ImageIterator {
 private:
  InnerIteratorType points_iterator_;

  using traits_ = std::iterator_traits<InnerIteratorType>;
  using my_type_ = ImageIterator<InnerIteratorType>;

 public:
  using value_type = typename traits_::value_type;
  using reference = typename traits_::value_type const&;
  using pointer = typename traits_::value_type const*;
  using iterator_category = std::forward_iterator_tag;
  using difference_type = typename traits_::difference_type;
  explicit ImageIterator(const InnerIteratorType iterator) {
    points_iterator_ = iterator;
  }
  ImageIterator() { /* default constructible */
  }
  ImageIterator(const ImageIterator<InnerIteratorType>& other)
      : points_iterator_(other.points_iterator_) {
  }
  my_type_& operator =(const my_type_& other) noexcept { /* assignable */
    points_iterator_ = other.points_iterator_;
    return *this;
  }
  bool operator ==(const my_type_& other) const noexcept {  // equity comparable
    return points_iterator_ == other.points_iterator_;
  }
  bool operator !=(const my_type_& other) const noexcept {  // equity comparable
    return points_iterator_ != other.points_iterator_;
  }
  pointer operator ->() const { /* de-referenciable as rvalue */
    return points_iterator_.operator->();
  }
  reference operator *() const { /* de-referenciable as rvalue */
    return *points_iterator_;
  }
  my_type_& operator ++() {  // prefix increment (++x)
    ++points_iterator_;
    return *this;
  }
  my_type_ operator ++(int) {  // postfix increment (x++)
    my_type_ old { *this };
    operator ++();
    return old;
  }
};

}  // namespace skeleton

#endif  // SRC_SKELETON_IMAGES_IMAGE_ITERATOR_H_
