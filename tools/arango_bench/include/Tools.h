#pragma once

#include <string>

namespace arango_bench::tools {

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

// requires at least C++11
inline const std::string string_format(const char* const zcFormat, ...) {
  // initialize use of the variable argument array
  va_list vaArgs;
  va_start(vaArgs, zcFormat);

  // reliably acquire the size
  // from a copy of the variable argument array
  // and a functionally reliable call to mock the formatting
  va_list vaArgsCopy;
  va_copy(vaArgsCopy, vaArgs);
  const int iLen = std::vsnprintf(NULL, 0, zcFormat, vaArgsCopy);
  va_end(vaArgsCopy);

  // return a formatted string without risking memory mismanagement
  // and without assuming any compiler or platform specific behavior
  std::vector<char> zc(iLen + 1);
  std::vsnprintf(zc.data(), zc.size(), zcFormat, vaArgs);
  va_end(vaArgs);
  return std::string(zc.data(), iLen);
}

}  // namespace arango_bench::tools