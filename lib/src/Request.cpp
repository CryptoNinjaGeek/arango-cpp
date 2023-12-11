//
// Created by Carsten Tang on 19/03/2023.
//

#include <arango-cpp/Request.h>
#include <arango-cpp/Tools.h>
#include <string>
#include <utility>

namespace arangocpp {

class RequestPimpl : public PrivateImpl {
 public:
  HttpMethod method_{HttpMethod::GET};
  bool is_admin_{false};
  std::string id_;
  std::string endpoint_;
  std::string data_;
  std::string database_name_;
  std::string collection_name_;
  std::string document_handle_;
  std::vector<string_pair> params_;
  std::vector<string_pair> headers_;

 public:
  static inline auto pimpl(const std::shared_ptr<PrivateImpl>& p) { return std::dynamic_pointer_cast<RequestPimpl>(p); }
};

Request::Request() { p_ = std::make_shared<RequestPimpl>(); }

auto Request::data(std::string data) -> Request {
  auto p = RequestPimpl::pimpl(p_);
  p->data_ = std::move(data);
  return *this;
}

auto Request::method(HttpMethod method) -> Request {
  auto p = RequestPimpl::pimpl(p_);
  p->method_ = method;
  return *this;
}

auto Request::database(std::string endpoint) -> Request {
  auto p = RequestPimpl::pimpl(p_);
  p->database_name_ = std::move(endpoint);
  return *this;
}

auto Request::handle(std::string name) -> Request {
  auto p = RequestPimpl::pimpl(p_);
  p->document_handle_ = std::move(name);
  return *this;
}

auto Request::id(std::string name) -> Request {
  auto p = RequestPimpl::pimpl(p_);
  p->id_ = std::move(name);
  return *this;
}

auto Request::collection(std::string endpoint) -> Request {
  auto p = RequestPimpl::pimpl(p_);
  p->collection_name_ = std::move(endpoint);
  return *this;
}

auto Request::parameters(std::vector<string_pair> param) -> Request {
  auto p = RequestPimpl::pimpl(p_);
  p->params_ = std::move(param);
  return *this;
}

auto Request::headers(std::vector<string_pair> headers) -> Request {
  auto p = RequestPimpl::pimpl(p_);
  p->headers_ = std::move(headers);
  return *this;
}

auto Request::isAdmin(bool is_admin) -> Request {
  auto p = RequestPimpl::pimpl(p_);
  p->is_admin_ = std::move(is_admin);
  return *this;
}

auto Request::endpoint(const std::string& endpoint) -> Request {
  auto p = RequestPimpl::pimpl(p_);
  auto tmp_endpoint = tools::trim(endpoint);

  if (tmp_endpoint.length() > 1 && tmp_endpoint.find_first_of('/') == 0)
    tmp_endpoint = tmp_endpoint.substr(1, tmp_endpoint.length() - 1);

  p->endpoint_ = tmp_endpoint;
  return *this;
}

auto Request::endpoint() -> std::string {
  auto p = RequestPimpl::pimpl(p_);
  return p->endpoint_;
}

auto Request::database() -> std::string {
  auto p = RequestPimpl::pimpl(p_);
  return p->database_name_;
}

auto Request::collection() -> std::string {
  auto p = RequestPimpl::pimpl(p_);
  return p->collection_name_;
}

auto Request::handle() -> std::string {
  auto p = RequestPimpl::pimpl(p_);
  return p->document_handle_;
}

auto Request::id() -> std::string {
  auto p = RequestPimpl::pimpl(p_);
  return p->id_;
}

auto Request::method() -> HttpMethod {
  auto p = RequestPimpl::pimpl(p_);
  return p->method_;
}

auto Request::data() -> std::string {
  auto p = RequestPimpl::pimpl(p_);
  return p->data_;
}

auto Request::parameters() -> std::vector<string_pair> {
  auto p = RequestPimpl::pimpl(p_);
  return p->params_;
}

auto Request::headers() -> std::vector<string_pair> {
  auto p = RequestPimpl::pimpl(p_);
  return p->headers_;
}

auto Request::isAdmin() ->  bool {
  auto p = RequestPimpl::pimpl(p_);
  return p->is_admin_;
}
}  // namespace arango-cpp