/**
 * \file src/skeleton/utils/annotated_image_binary_exporter.h
 * \brief This file implements a helper to export an annotated image to a binary file.
 *
 * \author Alexandre Ashade Lassance Cunha \<aashade@br.ibm.com\>
 * \author Giulia Duncan Coutinho \<coutingi@br.ibm.com\>
 * \author Rodrigo Neumann Barros Ferreira \<rneumann@br.ibm.com\>
 * \copyright © IBM Corp.
 * \date 2016
 */

#ifndef SRC_SKELETON_UTILS_ANNOTATED_IMAGE_BINARY_EXPORTER_H_
#define SRC_SKELETON_UTILS_ANNOTATED_IMAGE_BINARY_EXPORTER_H_

#include <armadillo>
#include <vector>
#include <string>
#include "src/skeleton/utils/file_utils.h"
#include "src/skeleton/utils/default_reader.h"

namespace skeleton {

using CenterlinesRawType = int32_t;

template<class AnnotatedImage, class Image, class AnnotationReader = DefaultReader>
class BinaryExporter {
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

  std::size_t GetLinearIndex(const point_type &point, const Image &image) const {
    std::size_t accumulated_size = 1;
    std::size_t index = point.GetCoordinate(0);
    for (typename point_type::coordinates_index_type coord_index = 1;
         coord_index != image.GetNumberOfDimensions(); ++coord_index) {
      accumulated_size *= image.GetDimensionSize(coord_index - 1);
      index += point.GetCoordinate(coord_index) * accumulated_size;
    }
    return index;
  }

  std::vector<CenterlinesRawType> BuildLinearImage(const AnnotatedImage &image,
                                                   const Image &original_image) const noexcept {
    std::vector<CenterlinesRawType> all_points;
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

  arma::Mat<CenterlinesRawType> BuildMatrix(const AnnotatedImage &image) const {
    auto size = image.GetSize();
    arma::Mat<CenterlinesRawType> centerline_points(size, 4);  // 4 cols (x, y, z, R^2), "size" rows
    arma::uword counter = 0;
    for (const auto &point_and_annotation : image) {
      auto point = point_and_annotation.first;
      auto value = read_annotation_(point_and_annotation.second);

      arma::Row<CenterlinesRawType> voxel = {point.GetCoordinate(0),
                                             point.GetCoordinate(1),
                                             point.GetCoordinate(2),
                                             value};
      centerline_points.row(counter++) = voxel;
    }
    return centerline_points;
  }

  arma::Mat<CenterlinesRawType> BuildCube(const Image &original_image) const {
    arma::uword size = 0;

    for (const auto &point : original_image)
      if (!original_image.IsObjectPoint(point))
        size += 1;

    arma::Mat<CenterlinesRawType> centerline_points(size, 4);  // 4 cols (x, y, z, R^2), "size" rows
    arma::uword counter = 0;
    for (const auto &point : original_image) {
      if (original_image.IsObjectPoint(point))
        continue;

      arma::Row<CenterlinesRawType> voxel = {point.GetCoordinate(0),
                                             point.GetCoordinate(1),
                                             point.GetCoordinate(2),
                                             2};
      centerline_points.row(counter++) = voxel;
    }
    return centerline_points;
  }

  arma::Cube<CenterlinesRawType> BuildCube(const AnnotatedImage &image,
                                           const Image &original_image) const {
    auto sizes = original_image.Sizes();
    arma::Cube<CenterlinesRawType> all_points{sizes[0], sizes[1], sizes[2]};
    all_points.fill(-2);

    for (const auto &point : original_image) {
      if (original_image.IsObjectPoint(point)) {
        all_points(point.GetCoordinate(0),
                   point.GetCoordinate(1),
                   point.GetCoordinate(2)) = -1;
      }
    }

    for (const auto &point_and_annotation : image) {
      auto point = point_and_annotation.first;
      auto value = read_annotation_(point_and_annotation.second);
      all_points(point.GetCoordinate(0),
                 point.GetCoordinate(1),
                 point.GetCoordinate(2)) = value;
    }

    return all_points;
  }

 public:
  BinaryExporter()
    : read_annotation_() { }

  void ExportAnnotation(std::string folder_path, const AnnotatedImage &image, const Image &) const {
    auto matrix = BuildMatrix(image);
    matrix.save(folder_path + "/centerlines.raw", arma::raw_binary);
  }
  void ExportBinaryAnnotation(std::string full_path,
                              const AnnotatedImage &image,
                              const Image &original_image) const {
    auto all_points = BuildLinearImage(image, original_image);
    FileUtils::WriteVectorToFile(full_path, all_points);
  }
};

}  // namespace skeleton

#endif  // SRC_SKELETON_UTILS_ANNOTATED_IMAGE_BINARY_EXPORTER_H_
