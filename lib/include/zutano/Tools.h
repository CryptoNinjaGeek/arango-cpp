#pragma once

#include <string>
#include <iostream>

namespace zutano::tools {

const std::string WHITESPACE = " \n\r\t\f\v";

inline std::string ltrim(const std::string& s) {
  size_t start = s.find_first_not_of(WHITESPACE);
  return (start == std::string::npos) ? "" : s.substr(start);
}

inline std::string rtrim(const std::string& s) {
  size_t end = s.find_last_not_of(WHITESPACE);
  return (end == std::string::npos) ? "" : s.substr(0, end + 1);
}

inline std::string trim(const std::string& s) { return rtrim(ltrim(s)); }

inline std::string toString(bool b) { return b ? "true" : "false"; }

inline bool replace(std::string& str, const std::string& from,
                    const std::string& to) {
  size_t start_pos = str.find(from);
  if (start_pos == std::string::npos) return false;
  str.replace(start_pos, from.length(), to);
  return true;
}

inline std::string toArray(jsoncons::json doc) {
  if (!doc.is_array()) {
    return std::string("[") + doc.to_string() + std::string("]");
  } else
    return doc.to_string();
}

inline std::string removeLastSlash(std::string str) {
  if (str.length() > 1 && str.find_last_of('/') == str.length() - 1)
    str = str.substr(0, str.length() - 1);
  return str;
}

typedef std::map<std::string, std::variant<std::string, bool, int, char, double,
                                           long, std::vector<std::string>>>
    to_json;
}  // namespace zutano::tools
