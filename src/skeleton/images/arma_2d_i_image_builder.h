/**
 * \file src/skeleton/images/arma_2d_i_image_builder.h
 * \brief This file implements a helper to convert an armadillo matrix to a
 * space efficient 2D image.
 *
 * \author Alexandre Ashade Lassance Cunha \<aashade@br.ibm.com\>
 * \copyright © IBM Corp.
 * \date 2015
 */

#ifndef SRC_SKELETON_IMAGES_ARMA_2D_I_IMAGE_BUILDER_H_
#define SRC_SKELETON_IMAGES_ARMA_2D_I_IMAGE_BUILDER_H_

#include <armadillo>
#include <new>
#include <array>
#include <memory>
#include "src/skeleton/images/image.h"
#include "src/skeleton/points/pixel.h"
#include "src/skeleton/points/point_builder.h"

namespace skeleton {

template<typename T>
class Arma2DImageBuilder {
 public:
  Arma2DImageBuilder() {
    static_assert(std::is_convertible<T, typename Point<2>::coordinate>::value &&
                  sizeof(T) <= sizeof(typename Point<2>::coordinate),
                  "T should be non-narrowing convertible to Point<2>::coordinate");
  }
  std::unique_ptr<Image2D> BuildImage(const arma::Mat<T>& source) const noexcept {
    Image2D::ObjectPoints set;
    for (auto iterator = source.begin_row_col(); iterator != source.end_row_col(); ++iterator) {
      auto value = *iterator;
      if (value != 0)
        continue;
      set.insert(PointBuilder::Get(iterator.row(), iterator.col()));
    }
    std::array<Image2D::size, 2> sizes = {{static_cast<Image2D::size>(source.n_rows),
                                           static_cast<Image2D::size>(source.n_cols)}};
    return std::make_unique<Image2D>(set, sizes);
  }
};

} /* namespace skeleton */

#endif  // SRC_SKELETON_IMAGES_ARMA_2D_I_IMAGE_BUILDER_H_
