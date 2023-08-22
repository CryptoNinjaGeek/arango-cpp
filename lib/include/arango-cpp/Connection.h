#pragma once

#include <string>
#include <arango-cpp/Database.h>
#include <arango-cpp/Request.h>
#include <arango-cpp/Response.h>
#include <arango-cpp/PrivateImpl.h>

namespace arangocpp {
class Connection;
class Database;
class ResolveRecord;

class Connection {
 public:
  Connection();

  ~Connection() = default;

  auto host(const std::string&) -> Connection&;
  auto hosts(const std::vector<std::string>&) -> Connection&;
  auto auth(std::string, std::string) -> Connection&;
  auto certificate(std::string) -> Connection&;
  auto jwt(std::string) -> Connection&;
  auto database(std::string) -> Database;
  auto resolve(std::vector<ResolveRecord>) -> Connection&;
  auto ping() -> long;
  auto sendRequest(Request) -> Response;

  static auto standardHeaders(std::vector<string_pair>&) -> void;

 private:
  private_impl_ptr p_;
};

}  // namespace arangocpp
