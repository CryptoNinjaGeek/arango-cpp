#pragma once

#include <string>
#include <arango-cpp/Database.h>
#include <arango-cpp/Backup.h>
#include <arango-cpp/Administration.h>
#include <arango-cpp/Request.h>
#include <arango-cpp/Response.h>
#include <arango-cpp/PrivateImpl.h>
#include <jsoncons/json.hpp>
#include <arango-cpp/input/DatabaseInput.h>

namespace arangocpp {
class Connection;
class Database;
class ResolveRecord;

class Connection {
 public:
  Connection();
  ~Connection() = default;

  // Interfaces
  auto backup() -> Backup;
  auto administration() -> Administration;

  // Setup
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

  // Access functions
  auto createDatabase(input::DatabaseCreateInput) -> Database;
  auto deleteDatabase(std::string, input::DatabaseDeleteInput = {}) -> bool;
  auto databases(bool onlyWithAccess = false) -> std::vector<std::string>;

 private:
  private_impl_ptr p_;
};

}  // namespace arangocpp
