#pragma once

#include <jsoncons/json.hpp>
#include "Types.h"

namespace docker {

class Controller {
 public:
  Controller() = default;

  auto set_version(std::string);
  auto set_host(std::string);

  auto get_image_list() -> std::vector<std::string>;
  auto pull_image(std::string) -> bool;

  auto create_network(CreateNetworkInput) -> std::string;
  auto create_volume(CreateVolumeInput) -> std::string;
  auto create_port_mapping(PortMappingInput) -> bool;
  auto create_container(CreateContainerInput) -> std::string;
  auto connect_container_to_network(ConnectNetworkInput) -> bool;
  auto start_container(std::string) -> bool;

  auto list_containers(ListContainerInput) -> std::vector<std::string>;
  auto list_networks(ListNetworkInput) -> std::vector<std::string>;
  auto list_volumes(ListVolumeInput) -> std::vector<std::string>;

  auto remove_container(std::string) -> bool;
  auto remove_network(std::string) -> bool;
  auto remove_volume(std::string) -> bool;

  auto stop_container(StopContainerInput) -> bool;

 private:
  auto send_request(RequestType type, std::string path, jsoncons::json data = {},
                    std::vector<std::pair<std::string, std::string>> parameters = {}) -> jsoncons::json;

 private:
  std::string version_{"v1.43"};
  std::string host_{"localhost"};
};

}  // namespace docker
