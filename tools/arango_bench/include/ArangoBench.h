#pragma once

#include <string>
#include <random>
#include <zutano/Connection.h>
#include <jsoncons/json.hpp>

#include "Input.h"

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

  inline auto random_interval(std::pair<int, int>&) -> int;

 private:
  std::vector<zutano::Collection> collections_;
  zutano::Connection connection_;
  zutano::Database database_;
  Input input_;
  jsoncons::json config_;
  std::mt19937 rand_source_;
};

}  // namespace arango_bench
