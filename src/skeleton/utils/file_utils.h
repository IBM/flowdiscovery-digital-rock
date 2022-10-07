/**
 * \file src/skeleton/utils/file_utils.h
 * \brief This file implements helper classes to handle files.
 *
 * \author Alexandre Ashade Lassance Cunha \<aashade@br.ibm.com\>
 * \copyright © IBM Corp.
 * \date 2016
 */

#ifndef SRC_SKELETON_UTILS_FILE_UTILS_H_
#define SRC_SKELETON_UTILS_FILE_UTILS_H_

#include <fstream>
#include <string>
#include <vector>
#include <exception>

namespace skeleton {

class FileUtils {
 public:
  template<class T>
  static void WriteVectorToFile(const std::string full_path,
                                const std::vector<T> &vector) {
    std::ofstream stream{full_path, std::ios::binary | std::ios::out};
    if (!stream.is_open())
      throw std::ios::failure{"Impossible to create file at location " +
          full_path};
    stream.write(reinterpret_cast<const char *>(&vector[0]),
                 sizeof(T) * vector.size());
    stream.close();
  }

  static void WriteCsv(const std::string full_path,
                       const std::vector<std::vector<std::string>> &contents) {
    std::ofstream file;
    file.open(full_path);
    if (!file.is_open())
      throw std::ios::failure{"Impossible to create file at location " +
          full_path};
    for (const auto &line : contents) {
      for (const auto &value : line)
        file << value << ",";
      file << std::endl;
    }
    file.close();
  }

  static void AssertFolderExists(std::string folder) {
    std::ofstream file;
    auto temp_file = folder + "/test.temp";
    file.open(temp_file);
    if (!file.is_open())
      throw std::ios::failure{"Impossible to create test file at location " +
          folder};
    file.close();
    std::remove(temp_file.c_str());
  }
};

}  // namespace skeleton

#endif  // SRC_SKELETON_UTILS_FILE_UTILS_H_
