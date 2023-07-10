#pragma once

#include <string>
#include <vector>
#include <zutano/PrivateImpl.h>
#include <zutano/Collection.h>
#include <zutano/Cursor.h>
#include <jsoncons/json.hpp>
#include <zutano/input/DatabaseInput.h>

namespace zutano {

class Connection;
class Database {
 public:
  Database(const Connection&, std::string);
  Database() = default;

  ~Database() = default;

  // Query functions
  auto execute(input::ExecuteInput) -> Cursor;
  auto explain(input::ExplainInput) -> jsoncons::json;
  auto validate(std::string) -> jsoncons::json;
  auto kill(std::string) -> bool;
  auto queries() -> jsoncons::json;
  auto slowQueries() -> jsoncons::json;
  auto clearSlowQueries() -> bool;
  auto tracking() -> jsoncons::json;
  auto setTracking(input::TrackingInput) -> jsoncons::json;
  auto functions() -> jsoncons::json;
  auto createFunction(const input::CreateFunctionInput&) -> jsoncons::json;
  auto deleteFunction(const input::DeleteFunctionInput&) -> jsoncons::json;
  auto queryRules() -> jsoncons::json;

  // Create functions
  auto createDatabase(input::DatabaseCreateInput) -> Database;
  auto createCollection(input::CollectionCreateInput) -> Collection;

  // Access functions
  auto collection(std::string) -> Collection;
  auto name() -> std::string;

 private:
  private_impl_ptr p_;
};

}  // namespace zutano
