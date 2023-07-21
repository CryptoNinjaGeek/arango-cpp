#include "ArangoBench.h"
#include <zutano/zutano.h>
#include <cxxopts.hpp>
#include <iostream>
#include <zutano/zutano.h>
#include <jsoncons/json.hpp>
#include <fstream>
#include <stdarg.h>  // For va_start, etc.
#include <memory>    // For std::unique_ptr

#include "JsonGenerator.h"
#include "ProgressLine.h"
#include "Input.h"
#include "Tools.h"

#include "docker/Controller.h"

using namespace zutano;
using namespace zutano::tools;
using namespace jsoncons;
using namespace jsoncons::literals;

namespace arango_bench {

static int BATCH_SIZE = 1000;

ArangoBench::ArangoBench(Input input) {
  input_ = input;
  rand_source_ = std::mt19937(time(0));
}

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

  if (system.get_value_or<std::string>("deployment", "Local") == "ArangoGraph") {
    if (!setupArangoGraph(system)) return false;
  } else {
    if (!setupDocker(system)) return false;
  }

  auto database = config_["database"];
  auto data = config_["data"];
  auto test = config_["test"];

  if (createSchema(database) && createData(data)) return runTests(test);

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
  if (!containers.empty()) {
    ProgressLine bar("Stopping and removing controllers", containers.size());
    for (auto container_id : containers) {
      if (controller.stop_container({.id = container_id, .timeout = 5})) controller.remove_container(container_id);
      bar.update();
    }
  }

  auto networks = controller.list_networks({.filters = label});
  if (!networks.empty()) {
    ProgressLine bar("Removing networks", networks.size());
    for (auto networks_id : networks) {
      controller.remove_network(networks_id);
      bar.update();
    }
  }

  auto volumes = controller.list_volumes({.filters = label});
  if (!volumes.empty()) {
    ProgressLine bar("Removing volumes", volumes.size());
    for (auto volumes_id : volumes) {
      controller.remove_volume(volumes_id);
      bar.update();
    }
  }

  return true;
}

auto ArangoBench::setupDocker(jsoncons::json& system) -> bool {
  docker::Controller controller;
  unsigned short start_port = 5700;
  std::map<std::string, std::string> labels;
  std::vector<std::string> coordinators;

  labels["arango_bench"] = true;

  auto images = controller.get_image_list();
  if (!std::count(images.begin(), images.end(),
                  tools::string_format("arangodb/arangodb:%s", system.get_value_or<std::string_view>("version", "latest")))) {
    std::cout << "Pulling image arangodb/arangodb:" << system.get_value_or<std::string_view>("version", "latest") << std::endl;
    controller.pull_image(tools::string_format("arangodb/arangodb:%s", system.get_value_or<std::string_view>("version", "latest")));
  }

  auto network_id = controller.create_network({.name = "arango_graph", .check_duplicate = false, .labels = labels});
  auto agency_count = system.get_value_or<int>("agency", 1);
  auto dbserver_count = system.get_value_or<int>("dbservers", 1);
  auto coordinators_count = system.get_value_or<int>("coordinators", 1);

  if (network_id.empty()) {
    return false;
  }
  std::cout << "Created network => " << network_id << std::endl;

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
  std::cout << "Created agency => " << agency_count << std::endl;

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
  std::cout << "Created db servers => " << dbserver_count << std::endl;

  for (int n = 0; n < coordinators_count; n++) {
    std::vector<std::string> command;
    auto name = tools::string_format("coordinator%ld", n);

    command.push_back(tools::string_format("--cluster.my-address=tcp://%s", name.c_str()));
    command.push_back("--cluster.my-role=COORDINATOR");

    for (int no = 0; no < agency_count; no++) {
      command.push_back(tools::string_format("--cluster.agency-endpoint=tcp://agent%ld", no));
    }

    coordinators.push_back(tools::string_format("http://localhost:%ld/", start_port));

    startDockerContainer(
        {.name = name,
         .command = command,
         .port = start_port++,
         .network_id = network_id,
         .image = tools::string_format("arangodb/arangodb:%s", system.get_value_or<std::string_view>("version", "latest"))});
  }
  std::cout << "Created coordinators => " << coordinators_count << std::endl;

  connection_ = Connection().hosts(coordinators);

#ifdef DEBUG_OUTPUT
  for (auto host : coordinators) {
    std::cout << host << std::endl;
  }
#endif

  return true;
}

auto ArangoBench::startDockerContainer(StartDockerContainer input) -> bool {
  docker::Controller controller;

  std::vector<std::string> command;
  std::map<std::string, std::string> labels;
  std::vector<std::string> environment;

  labels["arango_bench"] = true;
  labels["name"] = input.name;

  command.push_back("--server.authentication=false");
  command.push_back("--server.endpoint=tcp://0.0.0.0");
  command.push_back("--database.directory=arangodb");

  command.insert(command.end(), input.command.begin(), input.command.end());

  environment.push_back("ARANGO_NO_AUTH=1");

  auto volume_id = controller.create_volume({.name = input.name, .labels = labels});
  auto container_id = controller.create_container({.name = input.name,
                                                   .group = "arango_bench",
                                                   .image = input.image,
                                                   .labels = labels,
                                                   .command = command,
                                                   .port = input.port,
                                                   .environment = environment,
                                                   .volume = tools::string_format("%s:/var/lib/arangodb", volume_id.c_str())});

  controller.connect_container_to_network({.container = container_id, .network = input.network_id});
  controller.start_container(container_id);

  return true;
}

auto ArangoBench::setupArangoGraph(jsoncons::json&) -> bool { return false; }

auto ArangoBench::createSchema(jsoncons::json& json) -> bool {
  if (!connection_.ping()) {
    std::cout << "ping'ed failed" << std::endl;
    return false;
  } else
    std::cout << "ping'ed ok" << std::endl;

  auto sys_db = connection_.database("_system");
  auto db_name = json.get_value_or<std::string>("name", "demo");

  auto db = sys_db.createDatabase({.name = db_name,
                                   .replication_factor = json.get_value_or<int>("replication_factor", 1),
                                   .sharding = "flexible",
                                   .write_concern = json.get_value_or<int>("write_concern", 1),
                                   .allow_conflict = true});

  database_ = connection_.database(db_name);

  auto collections = json.get_value_or<jsoncons::json>("documents", jsoncons::json());

  if (collections.is_object()) {
    auto count = collections.get_value_or<int>("count", 1);
    auto naming_schema = collections.get_value_or<std::string>("naming_schema", "collection{id}");
    auto sharding_interval = collections.get_value_or<std::pair<int, int>>("sharding", std::pair<int, int>(1, 1));

    for (int no = 1; no <= count; no++) {
      auto name = naming_schema;
      name = std::regex_replace(name, std::regex("\\{id\\}"), std::to_string(no));
      auto sharding = randomInterval(sharding_interval);
      auto collection = database_.createCollection({.name = name, .shard_count = sharding});
      document_collections_.push_back(collection);
    }
  }

  auto graphs = json.get_value_or<jsoncons::json>("graphs", jsoncons::json());

  if (graphs.is_object()) {
    auto count = graphs.get_value_or<int>("count", 1);
    auto naming_schema = graphs.get_value_or<std::string>("naming_schema", "collection{id}");
    auto sharding_interval = graphs.get_value_or<std::pair<int, int>>("sharding", std::pair<int, int>(1, 1));

    for (int no = 1; no <= count; no++) {
      auto name = naming_schema;
      name = std::regex_replace(name, std::regex("\\{id\\}"), std::to_string(no));
      auto sharding = randomInterval(sharding_interval);
      auto collection = database_.createCollection({.name = name, .shard_count = sharding, .edge = true});
      std::cout << "Created edge collection: " << name << std::endl;
      edge_collections_.insert_or_assign(name, collection);
    }
  }

  return true;
}

auto ArangoBench::createData(jsoncons::json& json) -> bool {
  auto documents = json.get_value_or<jsoncons::json>("documents", jsoncons::json());
  auto graphs = json.get_value_or<jsoncons::json>("graphs", jsoncons::json());
  auto count_interval = documents.get_value_or<std::pair<int, int>>("count", std::pair<int, int>(1, 1));
  auto content = documents.get_value_or<jsoncons::json>("content", jsoncons::json());

  for (auto collection : document_collections_) {
    auto count = randomInterval(count_interval);

    std::cout << "Collection: " << collection.name() << " , Size: " << count << std::endl;

    std::vector<std::string> ids;
    while (count > 0) {
      auto request = 0;
      if (count > BATCH_SIZE)
        request = BATCH_SIZE;
      else
        request = count;

      auto array = build_array(content, request);
      auto result = collection.insert(array, {.sync = false});

      for (auto row : result.array_range()) {
        auto id = row.get_value_or<std::string>("_id", "");
        if (!id.empty()) ids.push_back(id);
      }
      count -= request;
    }
    collection_ids_.insert_or_assign(collection.name(), ids);
  }

  for (auto& graph : graphs.array_range()) {
    auto name = graph.get_value_or<std::string>("name", "");
    auto from = graph.get_value_or<std::string>("from", "");
    auto to = graph.get_value_or<std::string>("to", "");
    auto allow_collisions = graph.get_value_or<bool>("allow_collisions", true);

    if (!edge_collections_.contains(name)) {
      std::cout << "Edge collection not found: " << name << std::endl;
      continue;
    }
    if (!collection_ids_.contains(from)) {
      std::cout << "From collection not found: " << from << std::endl;
      continue;
    }
    if (!collection_ids_.contains(to)) {
      std::cout << "To collection not found: " << to << std::endl;
      continue;
    }

    std::cout << "Creating edge collection: " << name << " with data from " << from << " to " << to << std::endl;

    auto collection = edge_collections_.at(name);
    auto count_interval = graph.get_value_or<std::pair<int, int>>("count", std::pair<int, int>(1, 1));
    auto count = randomInterval(count_interval);
    auto org_count = count;
    std::unordered_set<std::string> edges;

    while (count > 0) {
      auto request = 0;
      if (count > BATCH_SIZE)
        request = BATCH_SIZE;
      else
        request = count;

      jsoncons::json array(jsoncons::json_array_arg);
      for (int no = 0; no < request; no++) {
        auto from_interval = std::pair<int, int>(0, collection_ids_.at(from).size() - 1);
        auto to_interval = std::pair<int, int>(0, collection_ids_.at(to).size() - 1);
        auto from_index = randomInterval(from_interval);
        auto to_index = randomInterval(to_interval);
        auto key = tools::string_format("%ld-%ld", from_index, to_index);

        if (allow_collisions || edges.find(key) == edges.end()) {
          array.push_back(to_json{{"_from", collection_ids_.at(from).at(from_index)}, {"_to", collection_ids_.at(to).at(to_index)}});
          if (!allow_collisions) edges.insert(key);
        }
      }
      collection.insert(array, {.sync = false});
      count -= request;
    }
    if (!allow_collisions) {
      auto conflicts = edges.size();
      std::cout << "Edges created: " << conflicts << ", Conflicts " << org_count - conflicts << std::endl;
    } else
      std::cout << "Edges created: " << org_count << std::endl;

    database_.createGraph({.name = name, .edge_definitions = {{.collection = name, .from = {from}, .to = {to}}}});
  }

  return true;
}

auto ArangoBench::runTests(jsoncons::json&) -> bool { return false; }

auto ArangoBench::randomInterval(std::pair<int, int>& interval) -> int {
  return (rand_source_() % (interval.second - interval.first + 1)) + interval.first;
}

}  // namespace arango_bench
