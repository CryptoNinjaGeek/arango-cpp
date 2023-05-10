#pragma once

#include <string>

namespace arango_import::tools {

inline static std::string basename(const std::string& path) {
  std::string base_filename = path.substr(path.find_last_of("/\\") + 1);
  std::string::size_type const p(base_filename.find_last_of('.'));
  std::string file_without_extension = base_filename.substr(0, p);

  return file_without_extension;
}

inline static long lineCount(const std::string& file) {
  long number_of_lines{0};
  std::string line;
  std::ifstream file_stream(file);

  while (std::getline(file_stream, line)) ++number_of_lines;

  return number_of_lines;
}

}  // namespace arango_import::tools