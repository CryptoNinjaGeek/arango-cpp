#pragma once

#include <string>
#include <zutano/PrivateImpl.h>
#include <zutano/Types.h>

#include <vector>
#include <jsoncons/json.hpp>

namespace zutano {

class Response {
 public:
  Response();

  ~Response() = default;

  auto http_code() -> long;

  auto error_code() -> int;

  auto body() -> jsoncons::json;

  auto error_message() -> std::string;

  auto is_success() -> bool;

  auto contains(std::vector<int>) -> bool;

  auto Body(jsoncons::json) -> Response;

  auto HttpCode(long) -> Response;

  auto ErrorCode(int) -> Response;

  auto Message(std::string) -> Response;

 private:
  PrivateImplPtr p_;
};

} // zutano

