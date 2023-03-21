#pragma once

#include <string>
#include <zutano/Database.h>
#include <zutano/Request.h>
#include <zutano/Response.h>
#include <zutano/PrivateImpl.h>

namespace zutano {
class Connection;
class Database;

typedef std::shared_ptr<Connection> ConnectionPtr;

class Connection {
 public:
  Connection();

  ~Connection() = default;

  auto Endpoint(std::string) -> Connection &;

  auto Auth(std::string, std::string) -> Connection &;

  auto Jwt(std::string) -> Connection &;

  auto Database(std::string) -> Database;

  auto Ping() -> int;

  auto SendRequest(Request) -> Response;

  auto StandardHeaders(std::vector<StringPair> &) -> void;

 public:
  static auto Create() -> ConnectionPtr;

 private:
  PrivateImplPtr p_;
};

} // zutano

