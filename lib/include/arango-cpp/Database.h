#pragma once

#include <string>
#include <vector>
#include <arango-cpp/PrivateImpl.h>
#include <arango-cpp/Collection.h>
#include <arango-cpp/Graph.h>
#include <arango-cpp/Cursor.h>
#include <jsoncons/json.hpp>
#include <arango-cpp/input/DatabaseInput.h>

namespace arangocpp {

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
  auto createGraph(input::GraphCreateInput) -> Graph;

  // Delete functions

  auto deleteDatabase(std::string, input::DatabaseDeleteInput = {}) -> bool;
  auto deleteCollection(std::string, input::CollectionDeleteInput = {} ) -> bool;

  // Access functions
  auto collection(std::string) -> Collection;
  auto graph(std::string) -> Graph;
  auto name() -> std::string;
  auto info() -> jsoncons::json;

  auto hasCollection(std::string) -> bool;
  auto hasDatabase(std::string name) -> bool;

 private:
  private_impl_ptr p_;
};

}  // namespace arangocpp
