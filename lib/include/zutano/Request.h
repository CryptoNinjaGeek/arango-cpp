#pragma once

#include <string>
#include <zutano/PrivateImpl.h>
#include <zutano/Types.h>

#include <jsoncons/json.hpp>

namespace zutano {

class Request {
 public:
  Request();

  ~Request() = default;

  auto Method(HttpMethod method) -> Request;

  auto Database(std::string endpoint) -> Request;

  auto Collection(std::string endpoint) -> Request;

  auto Endpoint(const std::string &endpoint) -> Request;

  auto Data(std::string) -> Request;

  auto Handle(std::string) -> Request;

  auto Parameters(std::vector<StringPair>) -> Request;

  auto Headers(std::vector<StringPair>) -> Request;

  auto database() -> std::string;

  auto collection() -> std::string;

  auto endpoint() -> std::string;

  auto method() -> HttpMethod;

  auto handle() -> std::string;

  auto data() -> std::string;

  auto parameters() -> std::vector<StringPair>;

  auto headers() -> std::vector<StringPair>;
 private:
  PrivateImplPtr p_;
};

} // zutano

