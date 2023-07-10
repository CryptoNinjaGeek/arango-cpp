#pragma once

#include <jsoncons/json.hpp>
#include <string>
#include <map>

namespace docker {
struct CreateNetworkInput {
  std::string name;
  std::optional<bool> check_duplicate;
  std::optional<std::map<std::string, std::string>> labels;
};

struct CreateVolumeInput {
  std::string name;
  std::optional<std::map<std::string, std::string>> labels;
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
  std::optional<std::string> group;
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

struct ListNetworkInput {
  jsoncons::json filters;
};

struct ListVolumeInput {
  jsoncons::json filters;
};

struct StopContainerInput {
  std::string id;
  int timeout{10};
};

enum class RequestType { POST, GET, DELETE };

}  // namespace docker
