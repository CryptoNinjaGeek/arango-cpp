#pragma once

#include <jsoncons/json.hpp>
#include <string>
#include <map>

namespace docker {
struct CreateNetworkInput {
  std::string name;
  std::optional<bool> check_duplicate;
};

struct CreateVolumeInput {
  std::string name;
};

struct PortMappingInput {
  std::string host_ip;
  std::string host_port;
};

struct CreateContainerInput {
  std::string name;
  std::string image;
  int port;
  std::string volume;
  std::optional<std::string> host_config;
  std::optional<std::vector<std::string>> command;
  std::optional<std::vector<std::string>> enviroment;
  std::optional<std::map<std::string, std::string>> labels;
};

struct ConnectNetworkInput {
  std::string network;
  std::string container;
};

struct ListContainerInput {
  bool all{false};
  int limit{1000};
  bool size{false};
  jsoncons::json filters;
};

enum class RequestType { POST, GET };

}  // namespace docker