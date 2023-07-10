#pragma once

#include <string>
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
  auto createSchema() -> bool;
  auto createData() -> bool;
  auto runTests() -> bool;

 private:
  Input input_;
  jsoncons::json config_;
};

}  // namespace arango_bench
