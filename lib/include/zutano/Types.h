#pragma once

#include <string>

namespace zutano {

enum class AuthType {
  NONE,
  BASIC_AUTH,
  JWT
};

enum class HttpMethod {
  GET,
  POST,
  PUT,
  DELETE,
  PATCH,
  HEAD
};

typedef std::pair<std::string, std::string> StringPair;

} // zutano

