#pragma once

#include <string>
#include <arango-cpp/PrivateImpl.h>
#include <arango-cpp/Types.h>

#include <jsoncons/json.hpp>

namespace arangocpp {

class Request {
 public:
  Request();

  ~Request() = default;

  auto method(HttpMethod method) -> Request;
  auto database(std::string endpoint) -> Request;
  auto collection(std::string endpoint) -> Request;
  auto endpoint(const std::string& endpoint) -> Request;
  auto data(std::string) -> Request;
  auto handle(std::string) -> Request;
  auto id(std::string) -> Request;
  auto parameters(std::vector<string_pair>) -> Request;
  auto headers(std::vector<string_pair>) -> Request;
  auto isAdmin(bool) -> Request;
  auto database() -> std::string;
  auto collection() -> std::string;
  auto endpoint() -> std::string;
  auto method() -> HttpMethod;
  auto handle() -> std::string;
  auto id() -> std::string;
  auto data() -> std::string;
  auto parameters() -> std::vector<string_pair>;
  auto headers() -> std::vector<string_pair>;
  auto isAdmin() -> bool;

 private:
  private_impl_ptr p_;
};

}  // namespace arangocpp
