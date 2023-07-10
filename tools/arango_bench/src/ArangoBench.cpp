#include "ArangoBench.h"
#include <zutano/zutano.h>
#include <cxxopts.hpp>
#include <iostream>
#include <jsoncons/json.hpp>
#include <fstream>
#include <stdarg.h>  // For va_start, etc.
#include <memory>    // For std::unique_ptr

#include "ProgressBar.h"
#include "Input.h"
#include "Tools.h"

#include "docker/Controller.h"

using namespace zutano;
using namespace zutano::tools;
using namespace jsoncons;
using namespace jsoncons::literals;

namespace arango_bench {

ArangoBench::ArangoBench(Input input) { input_ = input; }

auto ArangoBench::parseConfigFile() -> bool {
  try {
    std::ifstream is(input_.configuration);

    config_ = json::parse(is);
  } catch (const std::exception& ex) {
    std::cout << ex.what() << std::endl;
    return false;
  }

  return true;
}

auto ArangoBench::run() -> bool {
  if (not parseConfigFile()) return false;

  auto system = config_["system"];

  if (system.get_value_or<std::string>("deployment", "Local") == "ArangoGraph")
    setupArangoGraph(system);
  else
    setupDocker(system);

  return true;
}

auto ArangoBench::shutdown() -> bool {
  docker::Controller controller;

  auto label = json::parse(R"({
    "label" : {
      "arango_bench" : true
    }
  }
  )");

  auto containers = controller.list_containers({.all = true, .filters = label});
  for (auto container_id : containers) {
    controller.remove_container(container_id);
  }

  auto networks = controller.list_networks({.all = true, .filters = label});
  for (auto networks_id : networks) {
    controller.remove_network(networks_id);
  }
  auto volumes = controller.list_volumes({.all = true, .filters = label});
  for (auto volumes_id : volumes) {
    controller.remove_volume(volumes_id);
  }

  return true;
}

auto ArangoBench::setupDocker(jsoncons::json& system) -> bool {
  docker::Controller controller;
  unsigned short start_port = 5700;
  std::map<std::string, std::string> labels;

  labels["arango_bench"] = true;

  auto network_id = controller.create_network({.name = "arango_graph", .check_duplicate = false, .labels = labels});
  auto agency_count = system.get_value_or<int>("agency", 1);
  auto dbserver_count = system.get_value_or<int>("dbservers", 1);
  auto coordinators_count = system.get_value_or<int>("coordinators", 1);

  if (network_id.empty()) return false;

  for (int n = 0; n < agency_count; n++) {
    std::vector<std::string> command;
    auto name = tools::string_format("agent%ld", n);

    command.push_back("--agency.activate=true");
    command.push_back("--agency.supervision=true");
    command.push_back(tools::string_format("--agency.my-address=tcp://%s", name.c_str()));
    command.push_back(tools::string_format("--agency.size=%ld", agency_count));
    command.push_back("--agency.endpoint=tcp://agent1");

    startDockerContainer(
        {.name = name,
         .command = command,
         .port = start_port++,
         .network_id = network_id,
         .image = tools::string_format("arangodb/arangodb:%s", system.get_value_or<std::string_view>("version", "latest"))});
  }

  for (int n = 0; n < dbserver_count; n++) {
    std::vector<std::string> command;
    auto name = tools::string_format("dbserver%ld", n);

    command.push_back(tools::string_format("--cluster.my-address=tcp://%s", name.c_str()));
    command.push_back("--cluster.my-role=DBSERVER");

    for (int no = 0; no < agency_count; no++) {
      command.push_back(tools::string_format("--cluster.agency-endpoint=tcp://agent%ld", no));
    }

    startDockerContainer(
        {.name = name,
         .command = command,
         .port = start_port++,
         .network_id = network_id,
         .image = tools::string_format("arangodb/arangodb:%s", system.get_value_or<std::string_view>("version", "latest"))});
  }

  for (int n = 0; n < coordinators_count; n++) {
    std::vector<std::string> command;
    auto name = tools::string_format("coordinator%ld", n);

    command.push_back(tools::string_format("--cluster.my-address=tcp://%s", name.c_str()));
    command.push_back("--cluster.my-role=COORDINATOR");

    for (int no = 0; no < agency_count; no++) {
      command.push_back(tools::string_format("--cluster.agency-endpoint=tcp://agent%ld", no));
    }

    startDockerContainer(
        {.name = name,
         .command = command,
         .port = start_port++,
         .network_id = network_id,
         .image = tools::string_format("arangodb/arangodb:%s", system.get_value_or<std::string_view>("version", "latest"))});
  }
  return false;
}

auto ArangoBench::startDockerContainer(StartDockerContainer input) -> bool {
  docker::Controller controller;

  std::vector<std::string> command;
  std::map<std::string, std::string> labels;
  std::vector<std::string> enviroment;

  labels["arango_bench"] = true;
  labels["name"] = input.name;

  command.push_back("--server.authentication=false");
  command.push_back("--server.endpoint=tcp://0.0.0.0");
  command.push_back("--database.directory=arangodb");

  command.insert(command.end(), input.command.begin(), input.command.end());

  enviroment.push_back("ARANGO_NO_AUTH=1");

  auto volume_id = controller.create_volume({.name = input.name, .labels = labels});
  auto container_id = controller.create_container({.name = input.name,
                                                   .image = input.image,
                                                   .labels = labels,
                                                   .command = command,
                                                   .port = input.port,
                                                   .enviroment = enviroment,
                                                   .volume = tools::string_format("%s:/var/lib/arangodb", volume_id.c_str())});

  controller.connect_container_to_network({.container = container_id, .network = input.network_id});
  controller.start_container(container_id);

  return true;
}

auto ArangoBench::setupArangoGraph(jsoncons::json&) -> bool { return false; }
auto ArangoBench::createSchema() -> bool { return false; }
auto ArangoBench::createData() -> bool { return false; }
auto ArangoBench::runTests() -> bool { return false; }

}  // namespace arango_bench
