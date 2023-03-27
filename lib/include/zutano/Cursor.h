#pragma once

#include <string>
#include <zutano/PrivateImpl.h>
#include <zutano/Types.h>
#include <zutano/Connection.h>

#include <vector>
#include <jsoncons/json.hpp>

namespace zutano {
class Database;
class Connection;

class Cursor {
 public:
  Cursor(const std::string &dbName, const Connection &conn, const jsoncons::json &);

  ~Cursor();

  auto Id() -> std::string;

  auto Type() -> std::string;

  auto Count() -> long;

  auto HasMore() -> bool;

  auto Next() -> jsoncons::json;

  auto Cached() -> std::string;

  auto Statistics() -> jsoncons::json;

  auto Profile() -> std::string;

  auto Warnings() -> std::string;

  auto Empty() -> bool;

 private:

  auto Update(jsoncons::json) -> bool;

  auto Fetch() -> bool;

  auto Close(bool ignore_missing = false) -> bool;

 private:
  PrivateImplPtr p_;
};

} // zutano

