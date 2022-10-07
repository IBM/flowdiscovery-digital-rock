/**
 * \file src/skeleton/images/arma_2d_binary_image_builder.h
 * \brief This file implements a helper to convert an armadillo matrix to a
 * 2D image.
 *
 * \author Alexandre Ashade Lassance Cunha \<aashade@br.ibm.com\>
 * \copyright © IBM Corp.
 * \date 2015
 */

#ifndef SRC_SKELETON_IMAGES_ARMA_2D_BINARY_IMAGE_BUILDER_H_
#define SRC_SKELETON_IMAGES_ARMA_2D_BINARY_IMAGE_BUILDER_H_

#include <armadillo>
#include <array>
#include <vector>
#include <memory>

#include "src/skeleton/images/binary_image.h"
#include "src/skeleton/points/pixel.h"

namespace skeleton {

template<typename T>
class Arma2DBinaryImageBuilder {
 public:
  std::unique_ptr<BinaryImage < Pixel>> BuildImage(
  const arma::Mat<T> &source) const noexcept {
    BinaryImage<Pixel>::ObjectPoints set;
    set.resize(source.n_rows * source.n_cols, BinaryImage<Pixel>::Background);
    std::array<BinaryImage<Pixel>::size, 2> sizes = {
        static_cast<BinaryImage<Pixel>::size>(source.n_rows),
        static_cast<BinaryImage<Pixel>::size>(source.n_cols)};
    for (auto iterator = source.begin_row_col(); iterator != source.end_row_col(); ++iterator) {
      auto value = *iterator;
      if (value != 0)
        continue;
      auto point = PointBuilder::Get(iterator.row(), iterator.col());
      set[BinaryImage<Pixel>::ConvertToLinearIndex(point, sizes)] = BinaryImage<
          Pixel>::Foreground;
    }
    return std::make_unique<BinaryImage < Pixel>>(set, sizes);
  }
};

} /* namespace skeleton */

#endif  // SRC_SKELETON_IMAGES_ARMA_2D_BINARY_IMAGE_BUILDER_H_
