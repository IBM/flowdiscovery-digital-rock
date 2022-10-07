/**
 * \file src/skeleton/points/voxel.h
 * \brief This file implements a data structure to represent a voxel.
 *
 * \author Alexandre Ashade Lassance Cunha \<aashade@br.ibm.com\>
 * \copyright © IBM Corp.
 * \date 2016
 */

#ifndef SRC_SKELETON_POINTS_VOXEL_H_
#define SRC_SKELETON_POINTS_VOXEL_H_

#include <vector>
#include <memory>
#include "src/skeleton/points/point.h"
#include "src/skeleton/points/point_builder.h"

namespace skeleton {

using Voxel = Point<3>;

}  // namespace skeleton

#endif  // SRC_SKELETON_POINTS_VOXEL_H_
