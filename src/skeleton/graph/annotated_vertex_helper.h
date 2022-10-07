/**
 * \file src/skeleton/graph/annotated_vertex_helper.h
 * \brief Implements a wrapper over a standard vertex to give it a Key and Annotation and handle all
 * the associated stuff.
 *
 * \author Alexandre Ashade Lassance Cunha \<aashade@br.ibm.com\>
 * \copyright © IBM Corp.
 * \date 2016
 */

#ifndef SRC_SKELETON_GRAPH_ANNOTATED_VERTEX_HELPER_H_
#define SRC_SKELETON_GRAPH_ANNOTATED_VERTEX_HELPER_H_

#include <limits>
#include <utility>
#include <cstdint>
#include <functional>
#include <vector>

namespace skeleton {

struct _annotated_vertex_helper_counter {
  static uint64_t counter;
};

/**
 * \brief Implementation of an annotated vertex structure useful for the Graph class.
 *
 * \tparam Vertex An unannotated vertex. This is the Key type.
 * \tparam DistanceType The type (usually double) of the distance measure.
 */
template<class Vertex, class DistanceType>
class AnnotatedVertexHelper {
 public:
  class Annotation {
   public:
    using distance_type = DistanceType;
    using predecessor_type = Vertex;
    using tag_type = void*;

   private:
    distance_type distance_;
    predecessor_type predecessor_;
    uint64_t counter_value_;
    tag_type tag_;
    bool has_predecessor_;
    bool is_removed_;
    bool is_local_maximal_set_;
    bool is_local_maximal_;
    int64_t cluster_id_;
    static constexpr distance_type max_distance_value_ = std::numeric_limits<distance_type>::max();
    static const uint64_t max_counter_value_ = std::numeric_limits<uint64_t>::max();

   public:
    Annotation(void)
        : distance_(max_distance_value_),
          predecessor_(),
          counter_value_(max_counter_value_),
          tag_(nullptr),
          has_predecessor_(false),
          is_removed_(false),
          is_local_maximal_set_(false),
          is_local_maximal_(false),
          cluster_id_(-1) { }

    explicit Annotation(const distance_type distance)
        : distance_(distance),
          predecessor_(),
          counter_value_(_annotated_vertex_helper_counter::counter++),
          tag_(nullptr),
          has_predecessor_(false),
          is_removed_(false),
          is_local_maximal_set_(false),
          is_local_maximal_(false),
          cluster_id_(-1) { }

    Annotation(const Annotation &other)
        : distance_(other.distance_),
          predecessor_(other.predecessor_),
          counter_value_(other.counter_value_),
          tag_(other.tag_),
          has_predecessor_(other.has_predecessor_),
          is_removed_(other.is_removed_),
          is_local_maximal_set_(false),
          is_local_maximal_(false),
          cluster_id_(-1) { }

    Annotation &operator=(const Annotation &other) {
      distance_ = other.distance_;
      predecessor_ = other.predecessor_;
      counter_value_ = other.counter_value_;
      tag_ = other.tag_;
      has_predecessor_ = other.has_predecessor_;
      is_removed_ = other.is_removed_;
      is_local_maximal_set_ = other.is_local_maximal_set_;
      is_local_maximal_ = other.is_local_maximal_;
      cluster_id_ = other.cluster_id_;
      return *this;
    }

    void SetDistance(const distance_type distance) {
      distance_ = distance;
    }

    void SetRemoved() {
      is_removed_ = true;
    }

    bool IsRemoved() const {
      return is_removed_;
    }

    template<class TagType>
    void SetTag(TagType tag) {
      tag_ = reinterpret_cast<tag_type>(tag);
    }

    template<class TagType>
    TagType Tag() {
      return reinterpret_cast<TagType>(tag_);
    }

    distance_type HasFiniteDistance() const noexcept {
      return distance_ < max_distance_value_;
    }

    distance_type Distance() const noexcept {
      return distance_;
    }

    void SetPredecessor(const predecessor_type &predecessor) {
      predecessor_ = predecessor;
      has_predecessor_ = true;
    }

    const predecessor_type &Predecessor() const noexcept {
      return predecessor_;
    }

    bool HasPredecessor() const noexcept {
      return has_predecessor_;
    }

    bool operator<(const Annotation &other) const noexcept {
      if (distance_ < other.distance_)
        return true;
      if (distance_ == other.distance_)
        return counter_value_ < other.counter_value_;
      return false;
    }

    std::size_t GetHash() const noexcept {
      const std::size_t prime = 823;
      std::size_t hash = std::hash<distance_type> {}(distance_);
      hash += hash * prime + std::hash<uint64_t> {}(counter_value_);
      return hash;
    }

    bool operator==(const Annotation &other) const noexcept {
      return distance_ == other.distance_
          && counter_value_ == other.counter_value_;
    }
    bool IsLocalMaximalSet() const {
      return is_local_maximal_set_;
    }

    bool IsLocalMaximal() const {
      return is_local_maximal_;
    }

    void SetIsLocalMaximal(bool is_local_maximal) {
      is_local_maximal_ = is_local_maximal;
      is_local_maximal_set_ = true;
    }

    void SetClusterId(int64_t cluster_id) {
      cluster_id_ = cluster_id;
    }

    int64_t GetClusterId() const {
      return cluster_id_;
    }
  };

  /**
   * \brief annotation_type Alias to the internal annotation type.
   */
  using annotation_type = Annotation;

  /**
   * \brief key_type Alias to the template parameter Vertex.
   */
  using key_type = Vertex;

  using key_value_type = typename annotation_type::distance_type;

 private:
  key_type key_;
  annotation_type annotation_;
  bool is_empty_;

 public:
  /**
   * \brief Constructs and instance of the AnnotatedVertexHelper using default
   * internal values.
   */
  AnnotatedVertexHelper(void)
    : key_(),
      annotation_(),
      is_empty_(true) { }

  /**
   * \brief Constructs and instance of the AnnotatedVertexHelper using the
   * parameter as the Key.
   *
   * \param vertex The key of the annotated vertex to construct.
   */
  explicit AnnotatedVertexHelper(const Vertex &vertex)
    : key_(vertex),
      annotation_(),
      is_empty_(false) { }

  /**
   * \brief Constructs and instance of the AnnotatedVertexHelper using the
   * key and annotation provided as a std::pair instance.
   *
   * \param contents The key and the annotation.
   */
  explicit AnnotatedVertexHelper(const std::pair<key_type, annotation_type> &contents)
    : key_(contents.first),
      annotation_(contents.second),
      is_empty_(false) { }

  /**
   * \brief Constructs and instance of the AnnotatedVertexHelper using a
   * key and distance-initialised annotation.
   *
   * \param vertex The key.
   * \param distance The annotation has this distance.
   */
  AnnotatedVertexHelper(const Vertex &vertex, const DistanceType distance)
    : key_(vertex),
      annotation_(annotation_type {distance}),
      is_empty_(false) {
  }

  /**
   * \brief Constructs and instance of the AnnotatedVertexHelper using a
   * key and distance-initialised annotation.
   *
   * \param key The key.
   * \param annotation The annotation.
   */
  AnnotatedVertexHelper(const key_type &key, const annotation_type &annotation)
    : key_(key),
      annotation_(annotation),
      is_empty_(false) {
  }

  /**
   * \brief Use it to reset the annotation counter. If not sure, do not mess
   * with this function.
   */
  void ZeroAnnotationCounter() const noexcept {
    Annotation::counter_ = 0;
  }

  /**
   * \brief Returns the value of the key.
   */
  const key_type &Key() const {
    return key_;
  }

  /**
   * \brief Returns the value of the annotation.
   */
  const annotation_type &Annotation() const {
    return annotation_;
  }

  key_value_type GetValue() const {
    return annotation_.Distance();
  }

  /**
   * \brief Returns the value of the annotation.
   */
  annotation_type &ChangeAnnotation() {
    return annotation_;
  }

  /**
   * \brief This is the custom implementation of the \f$<\f$ operator for comparing
   * two instances of AnnotatedVertexHelper.
   *
   * \details Basically, \f$a < b\f$ if, and only if,
   * the distance annotated in \f$a\f$ is smaller than the distance annotated in
   * \f$b\f$, or the distances annotated in \f$a\f$ and \f$b\f$ equal and the
   * counter of \f$a\f$ is smaller than the counter of \f$b\f$.
   *
   * \param other The instance to compare to this.
   * \returns true if this < other; false otherwise.
   */
  bool operator<(const AnnotatedVertexHelper &other) const noexcept {
    if (is_empty_)
      return false;  // this instance has the maximum value.
    if (other.is_empty_)
      return true;  // other has maximum value.
    return annotation_ < other.annotation_;
  }

  /**
   * \brief This is the custom implementation of the \f$==\f$ operator for
   * comparing two instances of AnnotatedVertexHelper.
   *
   * \details Basically, \f$a == b\f$ if, and only if,
   * the distance annotated in \f$a\f$ equals the distance annotated in
   * \f$b\f$, and the counter of \f$a\f$ is smaller than the counter of
   * \f$b\f$.
   *
   * \param other The instance to compare to this.
   * \returns true if this == other; false otherwise.
   */
  bool operator==(const AnnotatedVertexHelper &other) const noexcept {
    if (is_empty_)
      return other.is_empty_;
    if (other.is_empty_)
      return false;
    return key_ == other.key_;
  }

  /**
   * \brief Hashcode implementation useful for std::unordered_map.
   *
   * \returns the hashcode.
   */
  std::size_t GetHash() const noexcept {
    if (is_empty_)
      return 0;
    return key_.GetHash();
  }
};

}  // namespace skeleton

#endif  // SRC_SKELETON_GRAPH_ANNOTATED_VERTEX_HELPER_H_
