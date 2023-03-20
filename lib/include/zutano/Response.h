#pragma once

#include <string>
#include <zutano/PrivateImpl.h>
#include <zutano/Types.h>

#include <vector>
#include <nlohmann/json.hpp>

namespace zutano {

class Response {
 public:
  Response();

  ~Response() = default;

  auto http_code() -> int;

  auto error_code() -> int;

  auto body() -> nlohmann::json;

  auto error_message() -> std::string;

  auto is_success() -> bool;

  auto contains(std::vector<int>) -> bool;

  auto Body(nlohmann::json) -> Response;

  auto HttpCode(int) -> Response;

  auto ErrorCode(int) -> Response;

  auto Message(std::string) -> Response;

 private:
  PrivateImplPtr p_;
};

typedef std::shared_ptr<Response> ResponsePtr;

} // zutano

