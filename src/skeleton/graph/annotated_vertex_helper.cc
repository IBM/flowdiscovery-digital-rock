/**
 * \file src/skeleton/graph/annotated_vertex_helper.cc
 * \brief This file implements a helper to initiate a counter useful for the
 * class Vertex.
 *
 * \author Alexandre Ashade Lassance Cunha \<aashade@br.ibm.com\>
 * \copyright © IBM Corp.
 * \date 2016
 */

#include "src/skeleton/graph/annotated_vertex_helper.h"

namespace skeleton {

uint64_t _annotated_vertex_helper_counter::counter = std::numeric_limits<uint64_t>::min();

}  // namespace skeleton
