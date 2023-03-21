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

#define __DEBUG__

namespace zutano {

class ConnectionPimpl : public PrivateImpl {
 public:
  std::string endpoint_;
  std::string user_name_;
  std::string password_;
  std::string jwt_token_;
  AuthType auth_type_{AuthType::NONE};

 public:
  static inline auto Pimpl(std::shared_ptr<PrivateImpl> p) {
	return std::dynamic_pointer_cast<ConnectionPimpl>(p);
  }
};

Connection::Connection() {
  p_ = std::make_shared<ConnectionPimpl>();
}

auto Connection::Create() -> ConnectionPtr {
  return ConnectionPtr(new Connection);
}

auto Connection::Endpoint(std::string endpoint) -> Connection & {
  auto p = ConnectionPimpl::Pimpl(p_);
  auto tmp_endpoint = tools::trim(endpoint);
  if (tmp_endpoint.length() > 1 && tmp_endpoint.find_last_of('/')==tmp_endpoint.length() - 1)
	tmp_endpoint = tmp_endpoint.substr(0, tmp_endpoint.length() - 1);
  p->endpoint_ = tmp_endpoint;
  return *this;
}

auto Connection::Auth(std::string userName, std::string password) -> Connection & {
  auto p = ConnectionPimpl::Pimpl(p_);
  p->user_name_ = userName;
  p->password_ = password;
  p->auth_type_ = AuthType::BASIC_AUTH;
  return *this;
}

auto Connection::Jwt(std::string jwtToken) -> Connection & {
  auto p = ConnectionPimpl::Pimpl(p_);
  p->jwt_token_ = jwtToken;
  p->auth_type_ = AuthType::JWT;
  return *this;
}

auto Connection::Database(std::string name) -> zutano::Database {
  return zutano::Database(*this, name);
}

auto Connection::SendRequest(Request request) -> Response {
  auto p = ConnectionPimpl::Pimpl(p_);
  if (request.endpoint().empty())
	throw RequestError(request.endpoint());
  cpr::Session session;
  std::string url_string = p->endpoint_;
  if (not request.database().empty())
	url_string += "/_db/" + request.database();
  if (not request.endpoint().empty())
	url_string += "/_api/" + request.endpoint();
  if (not request.collection().empty())
	url_string += "/" + request.collection();

  auto url = cpr::Url{url_string};
  switch (p->auth_type_) {
	case AuthType::BASIC_AUTH:
	  session.SetOption(cpr::Authentication{p->user_name_, p->password_, cpr::AuthMode::DIGEST});
	  break;
	case AuthType::JWT: session.SetOption(cpr::Bearer{p->jwt_token_});
	  break;
	case AuthType::NONE: break;
  }

  cpr::Parameters param;
  for (auto item : request.parameters()) {
	param.Add(cpr::Parameter{item.first, item.second});
  }
  session.SetParameters(param);

  auto request_headers = request.headers();
  cpr::Header headers;
  StandardHeaders(request_headers);
  for (auto item : request_headers) {
	headers[item.first] = item.second;
  }
  session.SetHeader(headers);

  if (not request.data().empty()) {
	session.SetBody(cpr::Body{request.data()});
  }

#ifdef __DEBUG__
  std::cout << "Data(" << request.data() << ")" << std::endl;
#endif

  session.SetUrl(url);
  session.SetHttpVersion(cpr::HttpVersion{cpr::HttpVersionCode::VERSION_1_1});

#ifdef __DEBUG__
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
  }

#ifdef __DEBUG__
  std::cout << r.status_code << std::endl;
  std::cout << r.text << std::endl;
  std::cout << r.raw_header << std::endl;
#endif

  nlohmann::json j = nlohmann::json::parse(r.text);

  if (j.contains("error") && j["error"].get<bool>()) {
	return Response()
		.Body(j)
		.HttpCode(r.status_code)
		.ErrorCode(j["errorNum"].get<int>())
		.Message(j["errorMessage"].get<std::string>());
  } else {
	return Response()
		.Body(j)
		.HttpCode(r.status_code);
  }
}

auto Connection::StandardHeaders(std::vector<StringPair> &header) -> void {
  auto driver_version = std::string("0.0.1");
  auto driver_header = std::string("zutano/") + driver_version;

  header.push_back(StringPair{"Content-Type", "application/json"});
  header.push_back(StringPair{"charset", "utf-8"});
  header.push_back(StringPair{"x-arango-driver", driver_header});
}

auto Connection::Ping() -> int {
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