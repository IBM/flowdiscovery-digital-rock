/**
 * \file test/src/utils/geometry_utils.h
 *
 * \author Alexandre Ashade Lassance Cunha \<aashade@br.ibm.com\>
 * \author Giulia Duncan Coutinho \<coutingi@br.ibm.com\>
 * \copyright © IBM Corp.
 * \date 2015
 */

#ifndef TEST_SRC_UTILS_GEOMETRY_UTILS_H_
#define TEST_SRC_UTILS_GEOMETRY_UTILS_H_

#include <cstdint>
#include <memory>
#include <vector>
#include <utility>
#include <exception>
#include <initializer_list>
#include <string>
#include <ios>
#include <fstream>
#include "src/skeleton/annotations/annotation.h"
#include "src/skeleton/utils/default_reader.h"

namespace geometry_utils {

template<class T>
class init {
 private:
  const std::vector<T> list_;
 public:
  init(std::initializer_list<T> list)
      : list_(std::vector<T> {list}) {
  }
  T operator[](std::size_t index) const {
    return list_[index];
  }
};

class Pixel {
 public:
  using coordinate = int16_t;
  static constexpr int Dimensions = 2;
  using coordinates_type = std::array<coordinate, Dimensions>;
  using coordinates_index_type = int_fast8_t;

 private:
  coordinate x_;
  coordinate y_;

 public:
  Pixel()
      : x_(0),
        y_(0) {
  }
  explicit Pixel(coordinates_type sizes)
      : x_(sizes[0]),
        y_(sizes[1]) {
  }
  Pixel(coordinate x, coordinate y)
      : x_(x),
        y_(y) {
  }
  explicit Pixel(const init<coordinate> &values)
      : x_(values[0]),
        y_(values[1]) {
  }
  Pixel(const Pixel &other)
      : x_(other.x()),
        y_(other.y()) {
  }
  coordinate x() const {
    return x_;
  }
  coordinate y() const {
    return y_;
  }
  bool operator==(const Pixel &other) const {
    return x() == other.x() && y() == other.y();
  }
  Pixel operator=(const Pixel &other) {
    x_ = other.x_;
    y_ = other.y_;
    return *this;
  }
  coordinates_index_type GetNumberOfCoordinates() const {
    return 2;
  }
  coordinate GetCoordinate(coordinates_index_type index) const {
    if (index == 0)
      return x();
    else if (index == 1)
      return y();
    else
      return -10000;
  }
};

class PixelBuilder {
  Pixel operator()(Pixel::coordinate x, Pixel::coordinate y) const {
    return Pixel {x, y};
  }
};

class Pixel_Hasher {
 public:
  std::size_t operator()(const Pixel &) const noexcept {
    return 1;
  }
};

class Pixel_Comparer {
 public:
  using T = Pixel;
  bool operator()(const T &x, const T &y) const noexcept {
    return x == y;
  }
};

class FourNeighbourCalculator {
 public:
  using Neighbours = std::vector<Pixel>;
  const std::vector<Pixel> GetNeighbours(const Pixel &pixel) const noexcept {
    std::vector<Pixel> neighbours;
    neighbours.push_back(Pixel {static_cast<Pixel::coordinate>(pixel.x() + 1),
                                pixel.y()});
    neighbours.push_back(Pixel {static_cast<Pixel::coordinate>(pixel.x() - 1),
                                pixel.y()});
    neighbours.push_back(
        Pixel {pixel.x(), static_cast<Pixel::coordinate>(pixel.y() + 1)});
    neighbours.push_back(
        Pixel {pixel.x(), static_cast<Pixel::coordinate>(pixel.y() - 1)});
    return neighbours;
  }
};

class AnnotatedImage {
 private:
  using element = std::pair<Pixel, skeleton::Annotation<Pixel>>;
  std::vector<element> annotations_;

 public:
  using point_type = Pixel;
  using annotation_type = skeleton::Annotation<point_type>;
  using Pointer = std::shared_ptr<annotation_type>;
  annotation_type ReadAnnotation(const Pixel &point) const {
    for (auto iterator = annotations_.cbegin(); iterator != annotations_.cend();
         ++iterator) {
      const element &current = *iterator;
      if (current.first == point)
        return current.second;
    }
    throw std::exception {};
  }
  bool HasAnnotation(const Pixel &point) const {
    for (auto iterator = annotations_.begin(); iterator != annotations_.end();
         ++iterator) {
      const element &current = *iterator;
      if (current.first == point)
        return true;
    }
    return false;
  }
  void ModifyAnnotation(const Pixel &point,
                        const annotation_type new_annotation) {
    for (auto iterator = annotations_.begin(); iterator != annotations_.end();
         ++iterator) {
      element &current = *iterator;
      if (current.first == point)
        current.second = new_annotation;
    }
  }
  void AddPointAnnotation(const Pixel &point,
                          const annotation_type annotation) {
    if (TryAddAnnotation(point, annotation) == false)
      throw std::exception {};
  }
  bool TryAddAnnotation(const Pixel &point, const annotation_type annotation) {
    if (HasAnnotation(point))
      return false;
    annotations_.push_back(
        std::pair<Pixel, annotation_type> {point, annotation});
    return true;
  }
};
class GeneralImage {
 public:
  using PointType = Pixel;
  using point_type = PointType;
  using size = Pixel::coordinate;
  using const_iterator = std::vector<PointType>::const_iterator;
  using coordinates_index_type = int_fast8_t;

 private:
  const std::vector<PointType> object_pixels_;
  const size width_;
  const size height_;

 public:
  explicit GeneralImage(std::vector<PointType> object_pixels, size width,
                        size height)
      : object_pixels_(object_pixels),
        width_(width),
        height_(height) {
  }

  bool IsOffLimitsCoordinate() const noexcept {
    // not implemented, method not used in test
    return false;
  }

  bool IsOffLimitsPoint(const point_type &point) const noexcept {
    return point.x() < 0 || point.y() < 0 || point.x() >= width_
        || point.y() >= height_;
  }
  bool IsContourPoint() const noexcept {  // hard to eval
    // not implemented, method not used in test
    return false;
  }
  bool IsLimitPoint(const point_type &point) const noexcept {
    return point.x() == 0 || point.y() == 0 || point.x() == width_ - 1
        || point.y() == height_ - 1;
  }
  bool IsObjectPoint(const PointType &point) const noexcept {
    return std::find(object_pixels_.cbegin(), object_pixels_.cend(), point)
        != object_pixels_.cend();
  }
  int_fast8_t GetNumberOfDimensions() const {
    return 2;
  }
  size GetDimensionSize(int_fast8_t dimension_index) const noexcept {
    if (dimension_index == 0)
      return width_;
    else if (dimension_index == 1)
      return height_;
    return 0;
  }
  size GetAverageDimensionsSize() const noexcept {
    return static_cast<size>((width_ + height_) / 2);
  }
  const_iterator cbegin() const noexcept {
    return const_iterator {object_pixels_.cbegin()};
  }
  const_iterator cend() const noexcept {
    return const_iterator {object_pixels_.cend()};
  }
  const_iterator begin() const noexcept {
    return const_iterator {object_pixels_.cbegin()};
  }
  const_iterator end() const noexcept {
    return const_iterator {object_pixels_.cend()};
  }
};

class Rectangle2DImage {
 public:
  using PointType = Pixel;
  using point_type = Pixel;
  using size = Pixel::coordinate;
  using const_iterator = std::vector<PointType>::const_iterator;
  using coordinates_index_type = int_fast8_t;

 private:
  const size padding_;
  const size rectangle_width_;
  const size rectangle_height_;
  std::vector<PointType> points_;

 public:
  Rectangle2DImage(size padding, size rectangle_width, size rectangle_height)
      : padding_(padding),
        rectangle_width_(rectangle_width),
        rectangle_height_(rectangle_height) {
    for (size x = padding; x != rectangle_width - padding; ++x) {
      for (size y = padding; y != rectangle_height - padding; ++y)
        points_.push_back(PointType {x, y});
    }
  }
  bool IsOffLimitsCoordinate(const point_type &,
    coordinates_index_type) const noexcept {
    // not implemented, method not used in test
    return false;
  }
  bool IsContourPoint(const point_type &point) const noexcept {
    return (point.x() == padding_ || point.y() == padding_
    || point.x() == rectangle_width_ - padding_ || point.y() ==
        rectangle_height_ - padding_);
  }
  bool IsOffLimitsPoint(const point_type &point) const noexcept {
    return point.x() < 0 || point.y() < 0 || point.x() >= rectangle_width_
        || point.y() >= rectangle_height_;
  }
  bool IsLimitPoint(const point_type &point) const noexcept {
    return point.x() == 0 || point.y() == 0 || point.x() == rectangle_width_ - 1
        || point.y() == rectangle_height_ - 1;
  }

  bool IsObjectPoint(const PointType &point) const noexcept {
    return (point.x() >= padding_ && point.y() >= padding_
        && point.y() < rectangle_height_ - padding_
        && point.x() < rectangle_width_ - padding_);
  }
  int_fast8_t GetNumberOfDimensions() const {
    return 2;
  }
  size GetDimensionSize(int_fast8_t dimension_index) const noexcept {
    if (dimension_index == 0)
      return rectangle_width_;
    else if (dimension_index == 1)
      return rectangle_height_;
    return 0;
  }
  size GetAverageDimensionsSize() const noexcept {
    return static_cast<size>((rectangle_width_ + rectangle_height_) / 2);
  }
  const_iterator cbegin() const noexcept {
    return const_iterator {points_.cbegin()};
  }
  const_iterator cend() const noexcept {
    return const_iterator {points_.cend()};
  }
  const_iterator begin() const noexcept {
    return const_iterator {points_.cbegin()};
  }
  const_iterator end() const noexcept {
    return const_iterator {points_.cend()};
  }
};
class FileUtils {
 public:
  template<class T>
  static void WriteVectorToFile(const std::string full_path,
                                const std::vector<T> &vector) {
    std::ofstream stream{full_path, std::ios::binary | std::ios::out};
    if (!stream.is_open())
      throw new std::exception{};
    stream.write(reinterpret_cast<const char *>(&vector[0]),
                 sizeof(T) * vector.size());
    stream.close();
  }
  template<class T>
  static void WriteVectorToASCIIFile(const std::string full_path,
                                const std::vector<T> &vector) {
    std::ofstream stream{full_path, std::ios::out};
    if (!stream.is_open())
      throw new std::exception{};
    for (const auto v : vector) {
      stream << v << " ";
    }
    stream.close();
  }
  static std::vector<bool> ReadRawImage(std::string file, uint32_t width,
                                        uint32_t height, uint32_t,
                                        uint32_t slice,
                                        int8_t object_point_threshold) {
    std::ifstream input{file, std::ios::binary};
    if (!input.is_open())
      throw std::exception {};
    constexpr int32_t kBufferSize = 4 * 1024;  // 4kbytes
    char buffer[kBufferSize];
    const std::size_t slice_size = width * height;
    input.seekg(static_cast<std::streamoff>(slice_size * slice));
    std::vector<bool> output;
    output.resize(slice_size, true);
    std::size_t counter = 0;
    while (counter < width * height && !input.eof()) {
      input.read(buffer, kBufferSize);
      for (int32_t index = 0;
           index < input.gcount() && index < kBufferSize
               && counter < slice_size;
           ++index) {
        if (buffer[index] <= object_point_threshold)
          output.at(counter) = false;
        counter = counter + 1;
      }
    }
    return output;
  }

  static std::vector<bool> ReadRawImage(std::string file, uint32_t width,
                                        uint32_t height, uint32_t depth,
                                        int8_t object_point_threshold) {
    const bool foreground = false;
    std::ifstream input{file, std::ios::binary};
    if (!input.is_open())
      throw std::exception {};
    constexpr int32_t kBufferSize = 4 * 1024;  // 4kbytes
    char buffer[kBufferSize];
    const std::size_t total_size = width * height * depth;
    std::vector<bool> output;
    output.resize(total_size, !foreground);
    std::size_t counter = 0;
    while (counter < total_size && !input.eof()) {
      input.read(buffer, kBufferSize);
      for (int32_t index = 0;
           index < input.gcount() && index < kBufferSize
               && counter < total_size;
           ++index) {
        if (buffer[index] <= object_point_threshold)
          output.at(counter) = foreground;
        counter = counter + 1;
      }
    }
    return output;
  }
};

template<class AnnotatedImage, class Image, class AnnotationReader = skeleton::DefaultReader>
class Exporter {
 private:
  AnnotationReader read_annotation_;
  using point_type = typename Image::point_type;
  std::size_t GetImageTotalSize(const Image &image) const {
    std::size_t total_size = 1;
    for (int_fast8_t i = 0; i != image.GetNumberOfDimensions(); ++i) {
      auto size = image.GetDimensionSize(i);
      total_size *= size;
    }
    return total_size;
  }
  std::size_t GetLinearIndex(const point_type &point,
                             const Image &image) const {
    std::size_t accumulated_size = 1;
    std::size_t index = point.GetCoordinate(0);
    for (typename point_type::coordinates_index_type coord_index = 1;
         coord_index != image.GetNumberOfDimensions(); ++coord_index) {
      accumulated_size *= image.GetDimensionSize(coord_index - 1);
      index += point.GetCoordinate(coord_index) * accumulated_size;
    }
    return index;
  }

  std::vector<int32_t> BuildLinearImage(const AnnotatedImage &image,
                                        const Image &original_image) const
  noexcept {
    std::vector<int32_t> all_points;
    all_points.resize(GetImageTotalSize(original_image), -2);
    for (const auto &point : original_image) {
      if (original_image.IsObjectPoint(point)) {
        auto index = GetLinearIndex(point, original_image);
        all_points[index] = -1;
      }
    }
    for (const auto &point_and_annotation : image) {
      auto point = point_and_annotation.first;
      auto index = GetLinearIndex(point, original_image);
      auto value = read_annotation_(point_and_annotation.second);
      all_points[index] = value;
    }
    return all_points;
  }

 public:
  Exporter()
      : read_annotation_() {
  }
  void ExportAnnotationASCII(std::string full_path, const AnnotatedImage &image,
                        const Image &original_image) const {
    auto all_points = BuildLinearImage(image, original_image);
    FileUtils::WriteVectorToASCIIFile(full_path, all_points);
  }
  void ExportAnnotation(std::string full_path, const AnnotatedImage &image,
                        const Image &original_image) const {
    auto all_points = BuildLinearImage(image, original_image);
    FileUtils::WriteVectorToFile(full_path, all_points);
  }
  void ExportBinaryAnnotation(std::string full_path,
                              const AnnotatedImage &image,
                              const Image &original_image) const {
    auto all_points = BuildLinearImage(image, original_image);
    FileUtils::WriteVectorToFile(full_path, all_points);
  }
};

}  // namespace geometry_utils

#endif  // TEST_SRC_UTILS_GEOMETRY_UTILS_H_
