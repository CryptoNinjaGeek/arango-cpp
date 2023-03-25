//
// Created by Carsten Tang on 19/03/2023.
//

#include <zutano/Connection.h>
#include <zutano/Database.h>
#include <zutano/Types.h>
#include <zutano/Exceptions.h>
#include <zutano/Tools.h>

#include <cpr/cpr.h>
#include <iostream>
#include <memory>
#include <utility>

#define DEBUG_OUTPUT

namespace zutano {

class ConnectionPimpl : public PrivateImpl {
 public:
  std::vector<std::string> endpoints_;
  std::string user_name_;
  std::string password_;
  std::string jwt_token_;
  AuthType auth_type_{AuthType::NONE};

 public:
  static inline auto Pimpl(const std::shared_ptr<PrivateImpl> &p) {
    return std::dynamic_pointer_cast<ConnectionPimpl>(p);
  }

  auto host() -> std::string {
    return this->endpoints_.at(last_++ % this->endpoints_.size());
  }

 private:
  unsigned long last_ = 0;
};

Connection::Connection() {
  p_ = std::make_shared<ConnectionPimpl>();
}

auto Connection::Create() -> ConnectionPtr {
  return std::make_shared<Connection>();
}

auto Connection::Host(const std::string &endpoint) -> Connection & {
  auto p = ConnectionPimpl::Pimpl(p_);
  p->endpoints_.push_back(tools::remove_last_slash(tools::trim(endpoint)));
  return *this;
}

auto Connection::Hosts(const std::vector<std::string> &endpoints) -> Connection & {
  auto p = ConnectionPimpl::Pimpl(p_);
  for (const auto &host : endpoints)
    p->endpoints_.push_back(tools::remove_last_slash(tools::trim(host)));
  return *this;
}

auto Connection::Auth(std::string userName, std::string password) -> Connection & {
  auto p = ConnectionPimpl::Pimpl(p_);
  p->user_name_ = std::move(userName);
  p->password_ = std::move(password);
  p->auth_type_ = AuthType::BASIC_AUTH;
  return *this;
}

auto Connection::Jwt(std::string jwtToken) -> Connection & {
  auto p = ConnectionPimpl::Pimpl(p_);
  p->jwt_token_ = std::move(jwtToken);
  p->auth_type_ = AuthType::JWT;
  return *this;
}

auto Connection::Database(std::string name) -> zutano::Database {
  return {*this, std::move(name)};
}

auto Connection::SendRequest(Request request) -> Response {
  auto p = ConnectionPimpl::Pimpl(p_);
  if (request.endpoint().empty())
    throw RequestError(request.endpoint());
  cpr::Session session;
  std::string url_string = p->host();
  if (not request.database().empty())
    url_string += "/_db/" + request.database();
  if (not request.endpoint().empty())
    url_string += "/_api/" + request.endpoint();
  if (not request.collection().empty())
    tools::replace(url_string, "{collection}", request.collection());
  if (not request.handle().empty())
    tools::replace(url_string, "{handle}", request.handle());

  auto url = cpr::Url{url_string};
  switch (p->auth_type_) {
    case AuthType::BASIC_AUTH:session.SetOption(cpr::Authentication{p->user_name_, p->password_, cpr::AuthMode::BASIC});
      break;
    case AuthType::JWT: session.SetOption(cpr::Bearer{p->jwt_token_});
      break;
    case AuthType::NONE: break;
  }

  cpr::Parameters param;
  for (const auto &item : request.parameters()) {
    param.Add(cpr::Parameter{item.first, item.second});
  }
  session.SetParameters(param);

  auto request_headers = request.headers();
  cpr::Header headers;
  StandardHeaders(request_headers);
  for (const auto &item : request_headers) {
    headers[item.first] = item.second;
  }
  session.SetHeader(headers);

  if (not request.data().empty()) {
    session.SetBody(cpr::Body{request.data()});
  }

#ifdef DEBUG_OUTPUT
  std::cout << "Data(" << request.data() << ")" << std::endl;
#endif

  session.SetUrl(url);
  session.SetHttpVersion(cpr::HttpVersion{cpr::HttpVersionCode::VERSION_1_1});

#ifdef DEBUG_OUTPUT
  std::cout << session.GetFullRequestUrl() << std::endl;
#endif

  cpr::Response r;
  switch (request.method()) {
    case HttpMethod::GET: r = session.Get();
      break;
    case HttpMethod::POST: r = session.Post();
      break;
    case HttpMethod::PUT: r = session.Put();
      break;
    case HttpMethod::DELETE: r = session.Delete();
      break;
    case HttpMethod::PATCH: r = session.Patch();
      break;
    case HttpMethod::HEAD: r = session.Head();
      break;
  }

#ifdef DEBUG_OUTPUT
  std::cout << r.raw_header << std::endl;
  std::cout << r.status_code << std::endl;
  std::cout << r.text << std::endl;
#endif

  jsoncons::json j;
  if (not r.text.empty())
    j = jsoncons::json::parse(r.text);

  if (j.contains("error") && j["error"].as<bool>()) {
    return Response()
        .Body(j)
        .HttpCode(r.status_code)
        .ErrorCode(j["errorNum"].as<int>())
        .Message(j["errorMessage"].as<std::string>());
  } else {
    return Response()
        .Body(j)
        .HttpCode(r.status_code);
  }
}

auto Connection::StandardHeaders(std::vector<StringPair> &header) -> void {
  auto driver_version = std::string("0.0.1");
  auto driver_header = std::string("zutano/") + driver_version;

  header.emplace_back("Content-Type", "application/json");
  header.emplace_back("charset", "utf-8");
  header.emplace_back("x-arango-driver", driver_header);
}

auto Connection::Ping() -> long {
  auto r = Request()
      .Method(HttpMethod::GET)
      .Endpoint("/collection");

  auto response = SendRequest(r);
  if (response.contains({401, 403}))
    throw AuthenticationError();
  else if (not response.is_success())
    throw ServerError(response.error_message(), response.error_code());
  return response.http_code();
}

} // zutano