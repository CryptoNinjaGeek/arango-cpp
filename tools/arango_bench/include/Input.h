#pragma once

#include <string>

namespace arango_bench {

struct Input {
  std::string configuration;

  bool shutdown{false};
  bool verbose{false};
};

struct StartDockerContainer {
  std::string name;
  std::vector<std::string> command;
  int port;
  std::string network_id;
  std::string image;
};
}  // namespace arango_bench