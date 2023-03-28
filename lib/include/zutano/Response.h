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

  auto httpCode() -> long;

  auto errorCode() -> int;

  auto body() -> jsoncons::json;

  auto errorMessage() -> std::string;

  auto isSuccess() -> bool;

  auto contains(std::vector<int>) -> bool;

  auto body(jsoncons::json) -> Response;

  auto httpCode(long) -> Response;

  auto errorCode(int) -> Response;

  auto message(std::string) -> Response;

 private:
  private_impl_ptr p_;
};

}  // namespace zutano
