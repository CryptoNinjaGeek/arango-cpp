#pragma once

#include <string>
#include <arango-cpp/PrivateImpl.h>

#include <jsoncons/json.hpp>

namespace arangocpp {
class Database;
class Connection;

class Cursor {
 public:
  Cursor(const std::string& db_name, const Connection& conn, const jsoncons::json&);

  ~Cursor();

  auto id() -> std::string;
  auto type() -> std::string;
  auto count() -> long;
  auto hasMore() -> bool;
  auto next() -> jsoncons::json;
  auto cached() -> std::string;
  auto statistics() -> jsoncons::json;
  auto profile() -> std::string;
  auto warnings() -> std::string;
  auto empty() -> bool;

 private:
  auto update(jsoncons::json) -> bool;
  auto fetch() -> bool;
  auto close(bool ignore_missing = false) -> bool;

 private:
  private_impl_ptr p_;
};

}  // namespace arangocpp
