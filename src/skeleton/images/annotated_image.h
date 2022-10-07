/**
 * \file src/skeleton/images/annotated_image.h
 * \brief This file implements the data structure that represents an
 * N-dimensional image with annotated points.
 *
 * \author Alexandre Ashade Lassance Cunha \<aashade@br.ibm.com\>
 * \copyright © IBM Corp.
 * \date 2015
 */

#ifndef SRC_SKELETON_IMAGES_ANNOTATED_IMAGE_H_
#define SRC_SKELETON_IMAGES_ANNOTATED_IMAGE_H_

#include <new>
#include <memory>
#include <type_traits>
#include <utility>
#include <unordered_map>
#include "src/skeleton/utils/hasher.h"
#include "src/skeleton/annotations/annotation.h"

namespace skeleton {

/**
 * \brief This class implements a memory efficient data structure to store a
 * image with its point annotations.
 */
template<class TPoint,
         class TAnnotation = Annotation<TPoint>,
         class THasher = Hasher<TPoint>,
         class TComparer = Comparer<TPoint>>
class AnnotatedImage {
 public:
  using Annotations = std::unordered_map<TPoint, TAnnotation, THasher, TComparer>;
  using point_type = TPoint;
  using annotation_type = TAnnotation;
  using pair_type = typename Annotations::value_type;
  using const_iterator = typename Annotations::const_iterator;

 private:
  Annotations annotations_;

  typename Annotations::iterator FindElement(const TPoint& point) noexcept {
    return annotations_.find(point);
  }

  bool HasAnnotation(const typename Annotations::iterator& element) const noexcept {
    return element != annotations_.cend();
  }

 public:
  AnnotatedImage()
    : annotations_() { }

  AnnotatedImage(const AnnotatedImage& other)
    : annotations_(other.annotations_) { }

  AnnotatedImage(AnnotatedImage && other)
    : annotations_(std::move(other.annotations_)) { }

  AnnotatedImage& operator =(AnnotatedImage&&) = delete;
  AnnotatedImage& operator =(const AnnotatedImage&) = delete;

  std::size_t GetSize() const {
    return annotations_.size();
  }

  /**
   * \brief This method checks whether a point has annotations or not.
   *
   * \param point A reference to a point.
   *
   * \return true if the point has annotations; false otherwise.
   */
  bool HasAnnotation(const TPoint& point) const noexcept {
    return annotations_.find(point) != annotations_.cend();
  }

  /**
   * \brief This method reads the annotation of the point given as the
   * parameter. If this point has no annotations, then the method raises an
   * std::out_of_range exception.
   *
   * \param point A reference to a point.
   *
   * \return The point's annotation.
   * \exception std::out_of_range Thrown whenever the point does not have
   * annotations.
   */
  TAnnotation ReadAnnotation(const TPoint& point) const {
    return annotations_.at(point);
  }

  /**
   * \brief Use this method to modify an existing annotation.
   *
   * \param point A reference to an annotated point.
   * \param new_value The new annotation value.
   *
   * \exception std::exception Thrown whenever the given point has no
   * annotation.
   */
  void ModifyAnnotation(const TPoint& point, const TAnnotation new_value) {
    auto element = FindElement(point);
    if (!HasAnnotation(element))
      throw std::exception { };
    element->second = new_value;
  }

  /**
   * \brief Use this method to annotate a point. This methos does not raise
   * exceptions.
   *
   * \param point A reference to the point to annotate.
   * \param new_value The new annotation value.
   *
   * \return true if the point gets successfully annotated; false otherwise.
   */
  bool TryAddAnnotation(const TPoint& point, const TAnnotation new_value)
      noexcept {
    auto result = annotations_.insert(std::make_pair(point, new_value));
    return result.second;
  }

  /**
   * \brief Use this method to annotate a point. This method raise an
   * exception in case of errors.
   *
   * \param point A reference to the point to annotate.
   * \param annotation The annotation to be added.
   *
   * \exception std::exception Thrown whenever the point has annotations
   * prior to calling this method.
   */
  void AddPointAnnotation(const TPoint& point, const TAnnotation annotation) {
    if (!TryAddAnnotation(point, annotation))
      throw std::exception { };
  }

  /**
   * \brief Use this method to remove the annotation of a annotated point. If
   * the point has no annotations, nothing happens.
   *
   * \param point A reference to the point.
   */
  void RemoveAnnotation(const TPoint& point) {
    annotations_.erase(point);
  }

  const_iterator begin() const {
    return annotations_.begin();
  }
  const_iterator end() const {
    return annotations_.end();
  }
};

} /* namespace skeleton */

#endif  // SRC_SKELETON_IMAGES_ANNOTATED_IMAGE_H_
