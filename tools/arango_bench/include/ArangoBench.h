#pragma once

#include <string>
#include <random>
#include <arango-cpp/Connection.h>
#include <jsoncons/json.hpp>

#include "Input.h"

class ProgressBar;

namespace arango_bench {

class ArangoBench {
 public:
  ArangoBench(Input);

  auto run() -> bool;
  auto shutdown() -> bool;

 private:
  auto parseConfigFile() -> bool;

  auto setupDocker(jsoncons::json&) -> bool;
  auto startDockerContainer(StartDockerContainer) -> bool;
  auto setupArangoGraph(jsoncons::json&) -> bool;
  auto createSchema(jsoncons::json&) -> bool;
  auto createData(jsoncons::json&) -> bool;
  auto runTests(jsoncons::json&) -> bool;
  auto waitOnPingOk() -> bool;

  inline auto randomInterval(std::pair<int, int>& interval) -> int;

 private:
  std::vector<arangocpp::Collection> document_collections_;
  std::map<std::string, arangocpp::Collection> edge_collections_;
  std::map<std::string, std::vector<std::string>> collection_ids_;
  arangocpp::Connection connection_;
  arangocpp::Database database_;
  Input input_;
  jsoncons::json config_;
  std::mt19937 rand_source_;
};

}  // namespace arango_bench
