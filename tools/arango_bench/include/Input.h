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
  std::string image;
  std::string network_id;
  int port;
  std::vector<std::string> command;
};
}  // namespace arango_bench