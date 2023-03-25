#pragma once

#include <string>
#include <vector>
#include <zutano/PrivateImpl.h>
#include <zutano/Collection.h>
#include <jsoncons/json.hpp>
#include <zutano/input/DatabaseInput.h>

namespace zutano {

class Connection;
class Database {
 public:
  Database(const Connection &, std::string);
  Database() = default;

  ~Database() = default;

  // Query functions
  auto Execute(input::ExecuteInput) -> jsoncons::json;

  auto Explain(input::ExplainInput) -> jsoncons::json;

  auto Validate(std::string) -> jsoncons::json;

  auto Kill(std::string) -> bool;

  auto Queries() -> jsoncons::json;

  auto SlowQueries() -> jsoncons::json;

  auto ClearSlowQueries() -> bool;

  auto Tracking() -> jsoncons::json;

  auto SetTracking(input::TrackingInput) -> jsoncons::json;

  auto Functions() -> jsoncons::json;

  auto CreateFunction(input::CreateFunctionInput) -> jsoncons::json;

  auto DeleteFunction(input::DeleteFunctionInput) -> jsoncons::json;

  auto QueryRules() -> jsoncons::json;

  // Create functions
  auto CreateDatabase(input::DatabaseCreateInput) -> Database;

  auto CreateCollection(input::CollectionCreateInput) -> Collection;

  // Access functions
  auto Collection(std::string) -> Collection;

  auto name() -> std::string;

 private:
  PrivateImplPtr p_;
};

} // zutano

