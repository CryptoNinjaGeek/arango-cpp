#include "ArangoImport.h"
#include <arango-cpp/arango-cpp.h>
#include <cxxopts.hpp>
#include <iostream>
#include <jsoncons/json.hpp>
#include <jsoncons_ext/csv/csv.hpp>
#include <fstream>

#include "ProgressBar.h"
#include "Input.h"
#include "Tools.h"

using namespace arangocpp;
using namespace arangocpp::tools;
using namespace jsoncons;
using namespace jsoncons::literals;

namespace arango_import {

typedef std::chrono::high_resolution_clock time;
typedef std::chrono::milliseconds ms;
typedef std::chrono::duration<float> seconds;

auto file_type(std::string name) -> input::FileType {
  std::transform(name.begin(), name.end(), name.begin(), ::toupper);
  if (name.ends_with(".CSV")) return input::FileType::CSV;

  return input::FileType::UNKNOWN;
}

auto importFile(arangocpp::Database db, const std::string& file_name, const input::ImportInput& input) -> void {
  auto entity_name = input.name;
  auto t0 = time::now();
  auto rows_imported{0};

  if (input.name_from_file) entity_name = tools::basename(file_name);

  if (input.create) {
    db.createCollection({.name = entity_name, .allow_conflict = true});
    if (input.verbose) std::cout << input.type << " => " << entity_name << " Created" << std::endl;
  }

  auto collection = db.collection(entity_name);

  if (input.truncate) {
    collection.truncate();
    if (input.verbose) std::cout << "Truncated => " << entity_name << std::endl;
  }

  auto type = file_type(file_name);

  switch (type) {
    case input::FileType::CSV:
      rows_imported = importCSV(collection, file_name, input);
      break;
    default:
      std::cout << "Unsupported file format => " << file_name << std::endl;
  }

  if (input.verbose) {
    auto t1 = time::now();
    seconds fs = t1 - t0;
    ms d = std::chrono::duration_cast<ms>(fs);

    std::cout << "Import done " << rows_imported << " rows in " << fs.count() << "s" << std::endl;
  }
}

auto run(const input::ImportInput& input) -> int {
  auto t0 = time::now();

  try {
    auto conn = Connection().hosts(input.hosts).auth(input.user_name, input.password);

    if (!conn.ping()) {
      std::cout << "Connection failed." << std::endl;
      return -1;
    } else if (input.verbose)
      std::cout << "Connected ok" << std::endl;

    if (input.create) {
      auto sys_db = conn.database("_system");

      sys_db.createDatabase({.name = input.database, .sharding = "flexible", .allow_conflict = true});
      if (input.verbose) std::cout << "Database => " << input.database << " Created" << std::endl;
    }

    auto db = conn.database(input.database);

    for (const auto& file : input.files) importFile(db, file, input);
  } catch (std::exception& e) {
    std::cout << e.what() << std::endl;
  }
  if (input.verbose) {
    auto t1 = time::now();
    seconds fs = t1 - t0;
    ms d = std::chrono::duration_cast<ms>(fs);

    std::cout << "Total Import done " << input.files.size() << " files in " << fs.count() << "s" << std::endl;
  }
  return 0;
}

}  // namespace arango_import
