//
// Created by Carsten Tang on 19/03/2023.
//

#include <arango-cpp/Connection.h>
#include <arango-cpp/Database.h>
#include <arango-cpp/Types.h>
#include <arango-cpp/Exceptions.h>
#include <arango-cpp/Tools.h>

#include <cpr/cpr.h>
#include <iostream>
#include <memory>
#include <utility>

// #define DEBUG_OUTPUT
//  #define DEBUG_OUTPUT_URL

namespace arangocpp {

class ConnectionPimpl : public PrivateImpl {
 public:
  std::vector<std::string> endpoints_;
  std::string user_name_;
  std::string password_;
  std::string jwt_token_;
  std::string certificate_;
  std::vector<ResolveRecord> resolvers_;
  AuthType auth_type_{AuthType::NONE};

 public:
  static inline auto pimpl(const std::shared_ptr<PrivateImpl>& p) { return std::dynamic_pointer_cast<ConnectionPimpl>(p); }

  auto host() -> std::string {
    if (!endpoints_.empty())
      return this->endpoints_.at(last_++ % this->endpoints_.size());
    else
      return {};
  }

  auto debug() -> void {
    for (const auto& host : endpoints_) {
      std::cout << host << std::endl;
    }
  }

 private:
  unsigned long last_ = 0;
};

Connection::Connection() { p_ = std::make_shared<ConnectionPimpl>(); }

auto Connection::host(const std::string& endpoint) -> Connection& {
  auto p = ConnectionPimpl::pimpl(p_);
  p->endpoints_.push_back(tools::removeLastSlash(tools::trim(endpoint)));
  return *this;
}

auto Connection::resolve(std::vector<ResolveRecord> list) -> Connection& {
  auto p = ConnectionPimpl::pimpl(p_);
  p->resolvers_ = std::move(list);
  return *this;
}

auto Connection::hosts(const std::vector<std::string>& endpoints) -> Connection& {
  auto p = ConnectionPimpl::pimpl(p_);
  for (const auto& host : endpoints) p->endpoints_.push_back(tools::removeLastSlash(tools::trim(host)));
  return *this;
}

auto Connection::auth(std::string user_name, std::string password) -> Connection& {
  auto p = ConnectionPimpl::pimpl(p_);
  p->user_name_ = std::move(user_name);
  p->password_ = std::move(password);
  p->auth_type_ = AuthType::BASIC_AUTH;
  return *this;
}

auto Connection::jwt(std::string jwt_token) -> Connection& {
  auto p = ConnectionPimpl::pimpl(p_);
  p->jwt_token_ = std::move(jwt_token);
  p->auth_type_ = AuthType::JWT;
  return *this;
}

auto Connection::database(std::string name) -> arangocpp::Database { return {*this, std::move(name)}; }

auto Connection::sendRequest(Request request) -> Response {
  auto p = ConnectionPimpl::pimpl(p_);
  if (request.endpoint().empty()) throw RequestError(request.endpoint());
  cpr::Session session;
  std::string url_string = p->host();
  if (not request.database().empty()) url_string += "/_db/" + request.database();
  if (request.isAdmin() && not request.endpoint().empty()) url_string += "/_admin/" + request.endpoint();
  else if (not request.endpoint().empty()) url_string += "/_api/" + request.endpoint();
  if (not request.collection().empty()) tools::replace(url_string, "{collection}", request.collection());
  if (not request.handle().empty()) tools::replace(url_string, "{handle}", request.handle());
  if (not request.id().empty()) tools::replace(url_string, "{id}", request.id());

  auto url = cpr::Url{url_string};
  switch (p->auth_type_) {
    case AuthType::BASIC_AUTH:
      session.SetOption(cpr::Authentication{p->user_name_, p->password_, cpr::AuthMode::BASIC});
      break;
    case AuthType::JWT:
      session.SetOption(cpr::Bearer{p->jwt_token_});
      break;
    case AuthType::NONE:
      break;
  }

  cpr::Parameters param;
  for (const auto& item : request.parameters()) {
    param.Add(cpr::Parameter{item.first, item.second});
  }
  session.SetParameters(param);

  auto request_headers = request.headers();
  cpr::Header headers;
  standardHeaders(request_headers);
  for (const auto& item : request_headers) {
    headers[item.first] = item.second;
  }
  session.SetHeader(headers);

  if (!p->resolvers_.empty()) {
    std::vector<cpr::Resolve> resolvers;
    for (const auto& item : p->resolvers_) {
      resolvers.push_back(cpr::Resolve{item.host, item.ip, {item.port}});
    }
    session.SetResolves(resolvers);
  }

  if (not request.data().empty()) {
    session.SetBody(cpr::Body{request.data()});
  }

#ifdef DEBUG_OUTPUT
  std::cout << "Data(" << request.data() << ")" << std::endl;
#endif

  session.SetUrl(url);
  session.SetHttpVersion(cpr::HttpVersion{cpr::HttpVersionCode::VERSION_1_1});

#ifdef DEBUG_OUTPUT_URL
  std::cout << session.GetFullRequestUrl() << std::endl;
#endif

  if (!p->certificate_.empty()) session.SetSslOptions(cpr::Ssl(cpr::ssl::PinnedPublicKey{p->certificate_.c_str()}));

  cpr::Response r;
  bool retry;
  int retry_cnt{0};
  std::vector<int> retry_codes{0, 503};

  do {
    switch (request.method()) {
      case HttpMethod::GET:
        r = session.Get();
        break;
      case HttpMethod::POST:
        r = session.Post();
        break;
      case HttpMethod::PUT:
        r = session.Put();
        break;
      case HttpMethod::DELETE:
        r = session.Delete();
        break;
      case HttpMethod::PATCH:
        r = session.Patch();
        break;
      case HttpMethod::HEAD:
        r = session.Head();
        break;
    }

#ifdef DEBUG_OUTPUT
    if (r.status_code) {
      std::cout << r.raw_header << std::endl;
      std::cout << r.status_code << std::endl;
      std::cout << r.text << std::endl;
    }
#endif
    retry = std::count(retry_codes.begin(), retry_codes.end(), r.status_code) && retry_cnt++ < 10;
    if (retry) {
      std::cout << "Unable to connect(" << retry_cnt << "), retry in 5s" << std::endl;
      std::this_thread::sleep_for(std::chrono::milliseconds(5000));
    }
  } while (retry);

  jsoncons::json j;
  if (not r.text.empty()) j = jsoncons::json::parse(r.text);

  if (j.contains("error") && j["error"].as<bool>()) {
    return Response().body(j).httpCode(r.status_code).errorCode(j["errorNum"].as<int>()).message(j["errorMessage"].as<std::string>());
  } else {
    return Response().body(j).httpCode(r.status_code);
  }
}

auto Connection::standardHeaders(std::vector<string_pair>& header) -> void {
  auto driver_version = std::string("0.0.1");
  auto driver_header = std::string("arango-cpp/") + driver_version;

  header.emplace_back("Content-Type", "application/json");
  header.emplace_back("charset", "utf-8");
  header.emplace_back("x-arango-driver", driver_header);
}

auto Connection::ping() -> long {
  auto r = Request().method(HttpMethod::GET).endpoint("/collection");

  auto response = sendRequest(r);

  if (response.contains({401, 403}))
    throw AuthenticationError();
  else if (not response.isSuccess())
    throw ServerError(response.errorMessage(), response.errorCode());
  return response.httpCode();
}

auto Connection::certificate(std::string certificate) -> Connection& {
  auto p = ConnectionPimpl::pimpl(p_);
  p->certificate_ = std::move(certificate);
  return *this;
}

auto Connection::databases(bool onlyWithAccess) -> std::vector<std::string> {
  auto p = ConnectionPimpl::pimpl(p_);

  Request request = Request().method(HttpMethod::GET);
  if(onlyWithAccess)
    request = request.endpoint("/database/user");
  else
    request = request.endpoint("/database");

  auto response = sendRequest(request);

  if (response.contains({401, 403})) {
    throw AuthenticationError();
  }
  if (!response.isSuccess()) {
    throw ServerError(response.errorMessage(), response.errorCode());
  }

  auto body = response.body();
  auto results = body["result"];

  if (results.is_array()) {
    std::vector<std::string> list;
    for (const auto& row : results.array_range())
      list.emplace_back(row.as_string());
    return list;
  }
  return {};
}

auto Connection::createDatabase(input::DatabaseCreateInput input) -> Database {
  if (input.name.empty()) throw ClientError("createDatabase => database name cannot be empty");

  jsoncons::json data = tools::to_json{{"name", input.name}};

  jsoncons::json options;

  if (input.replication_factor) options["replicationFactor"] = input.replication_factor.value();
  if (input.write_concern) options["writeConcern"] = input.write_concern.value();
  if (input.sharding) options["sharding"] = input.sharding.value();

  if (!options.empty()) data["options"] = options;

  jsoncons::json users(jsoncons::json_array_arg);
  for (auto user : input.users) {
    users.push_back(tools::to_json{
        {"username", user.username},
        {"passwd", user.password},
        {"active", user.active},
    });
  }
  if (not users.empty()) data["users"] = users;

  auto r = Request().method(HttpMethod::POST).endpoint("/database").data(data.to_string());

  auto response = sendRequest(r);
  if (response.contains({401, 403}))
    throw AuthenticationError();
  else if (not response.isSuccess() && (response.httpCode() != 409 || not input.allow_conflict))
    throw ServerError(response.errorMessage(), response.errorCode());

  return {*this, input.name};
}

auto Connection::deleteDatabase(std::string name, input::DatabaseDeleteInput input) -> bool {
  Request request = Request().method(HttpMethod::DELETE).handle(std::move(name)).endpoint("/database/{handle}");

  auto response = sendRequest(request);

  if (response.contains({401, 403}))
    throw AuthenticationError();
  if (response.errorCode() == 1228 and input.ignore_missing) return false;
  if (not response.isSuccess())
    throw ServerError(response.errorMessage(), response.errorCode());

  return true;
}

}  // namespace arangocpp