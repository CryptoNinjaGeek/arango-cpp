#include <arango-cpp/arango-cpp.h>
#include <cxxopts.hpp>
#include <fstream>

#include "Input.h"
#include "Tools.h"
#include "ArangoImport.h"

using namespace arangocpp::tools;

auto main(int argc, char* argv[]) -> int {
  arango_import::input::ImportInput input;

  cxxopts::Options options("arango_import", "Import of data into arangodb");
  options.allow_unrecognised_options();

  auto add = options.add_options();
  add("h,hosts", "Comma seperated list of hosts", cxxopts::value<std::string>());
  add("d,database", "Database", cxxopts::value<std::string>());
  add("n,name", "Name of entity", cxxopts::value<std::string>());
  add("t,type", "Type of entity to insert into", cxxopts::value<std::string>());
  add("u,user", "Username", cxxopts::value<std::string>());
  add("p,password", "Password", cxxopts::value<std::string>());
  add("b,batch-size", "Insert data in batches", cxxopts::value<int>());
  add("c,create", "Create missing collection and database");
  add("f,name-from-file", "Get entity name from filename");
  add("progress", "Show progress");
  add("o,overwrite", "Overwrite any existing records");
  add("no-sync", "Don't wait for write to be finished");
  add("truncate", "Truncate any existing data");
  add("v,verbose", "Verbose output");
  add("help", "Print usage");

  auto result = options.parse(argc, argv);

  if (result.count("help")) {
    std::cout << options.help() << std::endl;
    exit(0);
  }

  if (result.count("hosts")) input.hosts = split(result["hosts"].as<std::string>(), ',');
  if (result.count("database")) input.database = result["database"].as<std::string>();
  if (result.count("name")) input.name = result["name"].as<std::string>();
  if (result.count("type")) input.type = result["type"].as<std::string>();
  if (result.count("user")) input.user_name = result["user"].as<std::string>();
  if (result.count("password")) input.password = result["password"].as<std::string>();
  if (result.count("batch-size")) input.batch_size = result["batch-size"].as<int>();
  if (result.count("create")) input.create = true;
  if (result.count("progress")) input.progress = true;
  if (result.count("overwrite")) input.overwrite = true;
  if (result.count("no-sync")) input.no_sync = true;
  if (result.count("truncate")) input.truncate = true;
  if (result.count("verbose")) input.verbose = true;
  if (result.count("name-from-file")) input.name_from_file = true;

  input.files = result.unmatched();

  arango_import::run(input);

  return 0;
}
