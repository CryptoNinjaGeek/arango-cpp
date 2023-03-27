//
// Created by Carsten Tang on 19/03/2023.
//

#include <zutano/Request.h>
#include <zutano/Tools.h>
#include <string>
#include <utility>

namespace zutano {

class RequestPimpl : public PrivateImpl {
 public:
  HttpMethod method_{HttpMethod::GET};
  std::string id_;
  std::string endpoint_;
  std::string data_;
  std::string database_name_;
  std::string collection_name_;
  std::string document_handle_;
  std::vector<StringPair> params_;
  std::vector<StringPair> headers_;

 public:
  static inline auto Pimpl(const std::shared_ptr<PrivateImpl> &p) {
	return std::dynamic_pointer_cast<RequestPimpl>(p);
  }
};

Request::Request() {
  p_ = std::make_shared<RequestPimpl>();
}

auto Request::Data(std::string data) -> Request {
  auto p = RequestPimpl::Pimpl(p_);
  p->data_ = std::move(data);
  return *this;
}

auto Request::Method(HttpMethod method) -> Request {
  auto p = RequestPimpl::Pimpl(p_);
  p->method_ = method;
  return *this;
}

auto Request::Database(std::string name) -> Request {
  auto p = RequestPimpl::Pimpl(p_);
  p->database_name_ = std::move(name);
  return *this;
}

auto Request::Handle(std::string name) -> Request {
  auto p = RequestPimpl::Pimpl(p_);
  p->document_handle_ = std::move(name);
  return *this;
}

auto Request::Id(std::string name) -> Request {
  auto p = RequestPimpl::Pimpl(p_);
  p->id_ = std::move(name);
  return *this;
}

auto Request::Collection(std::string name) -> Request {
  auto p = RequestPimpl::Pimpl(p_);
  p->collection_name_ = std::move(name);
  return *this;
}

auto Request::Parameters(std::vector<StringPair> param) -> Request {
  auto p = RequestPimpl::Pimpl(p_);
  p->params_ = std::move(param);
  return *this;
}

auto Request::Headers(std::vector<StringPair> headers) -> Request {
  auto p = RequestPimpl::Pimpl(p_);
  p->headers_ = std::move(headers);
  return *this;
}

auto Request::Endpoint(const std::string &endpoint) -> Request {
  auto p = RequestPimpl::Pimpl(p_);
  auto tmp_endpoint = tools::trim(endpoint);

  if (tmp_endpoint.length() > 1 && tmp_endpoint.find_first_of('/')==0)
	tmp_endpoint = tmp_endpoint.substr(1, tmp_endpoint.length() - 1);

  p->endpoint_ = tmp_endpoint;
  return *this;
}

auto Request::endpoint() -> std::string {
  auto p = RequestPimpl::Pimpl(p_);
  return p->endpoint_;
}

auto Request::database() -> std::string {
  auto p = RequestPimpl::Pimpl(p_);
  return p->database_name_;
}

auto Request::collection() -> std::string {
  auto p = RequestPimpl::Pimpl(p_);
  return p->collection_name_;
}

auto Request::handle() -> std::string {
  auto p = RequestPimpl::Pimpl(p_);
  return p->document_handle_;
}

auto Request::id() -> std::string {
  auto p = RequestPimpl::Pimpl(p_);
  return p->id_;
}

auto Request::method() -> HttpMethod {
  auto p = RequestPimpl::Pimpl(p_);
  return p->method_;
}

auto Request::data() -> std::string {
  auto p = RequestPimpl::Pimpl(p_);
  return p->data_;
}

auto Request::parameters() -> std::vector<StringPair> {
  auto p = RequestPimpl::Pimpl(p_);
  return p->params_;
}

auto Request::headers() -> std::vector<StringPair> {
  auto p = RequestPimpl::Pimpl(p_);
  return p->headers_;
}

} // zutano