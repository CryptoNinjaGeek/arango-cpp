//
// Created by Carsten Tang on 19/03/2023.
//

#include <zutano/Response.h>
#include <algorithm>
#include <vector>
#include <iterator>

namespace zutano {

class ResponsePimpl : public PrivateImpl {
 public:
  int http_code_{0};
  int error_code_{0};
  std::string error_message_;
  nlohmann::json body_;

 public:
  static inline auto Pimpl(std::shared_ptr<PrivateImpl> p) {
	return std::dynamic_pointer_cast<ResponsePimpl>(p);
  }
};

Response::Response() {
  p_ = std::make_shared<ResponsePimpl>();
}

auto Response::http_code() -> int {
  auto p = ResponsePimpl::Pimpl(p_);
  return p->http_code_;
}

auto Response::body() -> nlohmann::json {
  auto p = ResponsePimpl::Pimpl(p_);
  return p->body_;
}

auto Response::error_code() -> int {
  auto p = ResponsePimpl::Pimpl(p_);
  return p->error_code_;
}

auto Response::error_message() -> std::string {
  auto p = ResponsePimpl::Pimpl(p_);
  return p->error_message_;
}

auto Response::contains(std::vector<int> list) -> bool {
  auto p = ResponsePimpl::Pimpl(p_);

  return std::find(begin(list), end(list), p->http_code_)!=std::end(list);
}

auto Response::is_success() -> bool {
  return contains({200, 201, 202});
}

auto Response::HttpCode(int code) -> Response {
  auto p = ResponsePimpl::Pimpl(p_);
  p->http_code_ = code;
  return *this;
}

auto Response::ErrorCode(int code) -> Response {
  auto p = ResponsePimpl::Pimpl(p_);
  p->error_code_ = code;
  return *this;
}

auto Response::Body(nlohmann::json body) -> Response {
  auto p = ResponsePimpl::Pimpl(p_);
  p->body_ = body;
  return *this;
}

auto Response::Message(std::string message) -> Response {
  auto p = ResponsePimpl::Pimpl(p_);
  p->error_message_ = message;
  return *this;
}
} // zutano