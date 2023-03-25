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

struct ResolveRecord {
  std::string host;
  std::string ip;
  unsigned short port;
};

} // zutano

