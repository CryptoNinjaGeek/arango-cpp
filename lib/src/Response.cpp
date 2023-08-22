//
// Created by Carsten Tang on 19/03/2023.
//

#include <arango-cpp/Response.h>
#include <algorithm>
#include <utility>
#include <vector>
#include <iterator>

namespace arangocpp {

class ResponsePimpl : public PrivateImpl {
 public:
  long http_code_{0};
  int error_code_{0};
  std::string error_message_;
  jsoncons::json body_;

 public:
  static inline auto pimpl(const std::shared_ptr<PrivateImpl>& p) {
    return std::dynamic_pointer_cast<ResponsePimpl>(p);
  }
};

Response::Response() { p_ = std::make_shared<ResponsePimpl>(); }

auto Response::httpCode() -> long {
  auto p = ResponsePimpl::pimpl(p_);
  return p->http_code_;
}

auto Response::body() -> jsoncons::json {
  auto p = ResponsePimpl::pimpl(p_);
  return p->body_;
}

auto Response::errorCode() -> int {
  auto p = ResponsePimpl::pimpl(p_);
  return p->error_code_;
}

auto Response::errorMessage() -> std::string {
  auto p = ResponsePimpl::pimpl(p_);
  return p->error_message_;
}

auto Response::contains(std::vector<int> list) -> bool {
  auto p = ResponsePimpl::pimpl(p_);

  return std::find(begin(list), end(list), p->http_code_) != std::end(list);
}

auto Response::isSuccess() -> bool { return contains({200, 201, 202}); }

auto Response::httpCode(long code) -> Response {
  auto p = ResponsePimpl::pimpl(p_);
  p->http_code_ = code;
  return *this;
}

auto Response::errorCode(int code) -> Response {
  auto p = ResponsePimpl::pimpl(p_);
  p->error_code_ = code;
  return *this;
}

auto Response::body(jsoncons::json body) -> Response {
  auto p = ResponsePimpl::pimpl(p_);
  p->body_ = std::move(body);
  return *this;
}

auto Response::message(std::string message) -> Response {
  auto p = ResponsePimpl::pimpl(p_);
  p->error_message_ = std::move(message);
  return *this;
}
}  // namespace arango-cpp