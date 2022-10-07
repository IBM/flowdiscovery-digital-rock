/**
 * \file src/skeleton/utils/default_reader.h
 * \brief This file implements the default annotation reader.
 *
 * \author Rodrigo Neumann Barros Ferreira \<rneumann@br.ibm.com\>
 * \copyright © IBM Corp.
 * \date 2018
 */

#ifndef SRC_SKELETON_UTILS_DEFAULT_READER_H_
#define SRC_SKELETON_UTILS_DEFAULT_READER_H_

namespace skeleton {

class DefaultReader {
 public:
  int32_t operator()(const int &annotation) const {
    return annotation;
  }
};

}  // namespace skeleton

#endif  // SRC_SKELETON_UTILS_DEFAULT_READER_H_
