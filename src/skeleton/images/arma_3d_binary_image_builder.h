/**
 * \file src/skeleton/images/arma_3d_binary_image_builder.h
 * \brief This file implements a helper to convert an armadillo cube to a
 * 3D image.
 *
 * \author Alexandre Ashade Lassance Cunha \<aashade@br.ibm.com\>
 * \author Giulia Duncan Coutinho \<coutingi@br.ibm.com\>
 * \copyright © IBM Corp.
 * \date 2016
 */

#ifndef SRC_SKELETON_IMAGES_ARMA_3D_BINARY_IMAGE_BUILDER_H_
#define SRC_SKELETON_IMAGES_ARMA_3D_BINARY_IMAGE_BUILDER_H_

#include <armadillo>
#include <array>
#include <vector>
#include <memory>

#include "src/skeleton/images/binary_image.h"
#include "src/skeleton/points/voxel.h"

namespace skeleton {

template<typename T>
class Arma3DBinaryImageBuilder {
 public:
  using image_type = BinaryImage<Voxel>;

  std::unique_ptr<const image_type> BuildImage(const arma::Cube<T> &source) const noexcept {
    image_type::ObjectPoints set;
    set.resize(source.n_elem, image_type::Background);
    std::array<image_type::size, 3> sizes = {{static_cast<image_type::size>(source.n_rows),
                                              static_cast<image_type::size>(source.n_cols),
                                              static_cast<image_type::size>(source.n_slices)}};

    for (Voxel::coordinate z = 0U; z != sizes[2]; ++z)
      for (Voxel::coordinate y = 0U; y != sizes[1]; ++y)
        for (Voxel::coordinate x = 0U; x != sizes[0]; ++x) {
          if (source(x, y, z) != image_type::Foreground)
            continue;
          auto point = PointBuilder::Get(x, y, z);
          set[image_type::ConvertToLinearIndex(point, sizes)] = image_type::Foreground;
        }
    return std::make_unique<image_type>(set, sizes);
  }
};

} /* namespace skeleton */

#endif  // SRC_SKELETON_IMAGES_ARMA_3D_BINARY_IMAGE_BUILDER_H_
