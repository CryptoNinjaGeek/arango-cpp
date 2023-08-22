#pragma once

#include <string>

namespace arangocpp {

enum class AuthType { NONE, BASIC_AUTH, JWT };

enum class HttpMethod { GET, POST, PUT, DELETE, PATCH, HEAD };

typedef std::pair<std::string, std::string> string_pair;

struct ResolveRecord {
  std::string host;
  std::string ip;
  unsigned short port;
};

}  // namespace arangocpp
