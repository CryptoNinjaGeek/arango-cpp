#include <arango-cpp/arango-cpp.h>
#include <cxxopts.hpp>
#include <iostream>
#include <jsoncons/json.hpp>
#include <jsoncons_ext/csv/csv.hpp>
#include <fstream>

#include "progressbar.h"

using namespace arangocpp;
using namespace arangocpp::tools;
using namespace jsoncons;
using namespace jsoncons::literals;

namespace arango_import {
struct ImportInput {
  std::string user_name{"root"};
  std::string database{"demo"};
  std::string name{"demo"};
  std::string type{"collection"};
  std::string password;
  std::vector<std::string> hosts{"localhost"};

  long batch_size{1000};
  bool create{false};
  bool progress{true};
  bool overwrite{false};
  bool truncate{false};
  bool no_sync{false};
  bool verbose{false};
  bool name_from_file{false};

  std::vector<std::string> files;
};

typedef std::chrono::high_resolution_clock time;
typedef std::chrono::milliseconds ms;
typedef std::chrono::duration<float> fsec;

std::string basename(const std::string& path) {
  std::string base_filename = path.substr(path.find_last_of("/\\") + 1);
  std::string::size_type const p(base_filename.find_last_of('.'));
  std::string file_without_extension = base_filename.substr(0, p);

  return file_without_extension;
}

long lineCount(const std::string& file) {
  long number_of_lines{0};
  std::string line;
  std::ifstream fstream(file);

  while (std::getline(fstream, line)) ++number_of_lines;

  return number_of_lines;
}

unsigned long importData(arangocpp::Collection collection, const std::string& file_name, const ImportInput& input) {
  unsigned long rows_imported{0}, line_no{0};
  csv::csv_options options;
  std::shared_ptr<progressbar> bar;

  if (input.progress) {
    auto number_of_lines = lineCount(file_name);
    long steps = number_of_lines / input.batch_size;
    bar = std::make_shared<progressbar>(steps);
    bar->set_todo_char(" ");
    bar->set_done_char("█");
    bar->set_opening_bracket_char("{");
    bar->set_closing_bracket_char("}");

    if (input.verbose) std::cout << "Number of lines: " << number_of_lines << std::endl;
  }

  options.assume_header(true).mapping_kind(csv::csv_mapping_kind::n_objects).field_delimiter('|');

  std::string line, header;
  std::ifstream infile(file_name);
  std::getline(infile, header);
  std::stringstream out;

  while (!infile.eof()) {
    out << header << std::endl;
    while (std::getline(infile, line) and line_no++ < input.batch_size) out << line << std::endl;
    json j = csv::decode_csv<json>(out.str(), options);

    collection.insert(j, {.sync = true});
    if (bar) bar->update();

    rows_imported += j.size();
    out.str(std::string());
    line_no = 0;
  }

  if (bar) std::cout << std::endl;
  return rows_imported;
}

void importFile(arangocpp::Database db, const std::string& file_name, const ImportInput& input) {
  auto entity_name = input.name;
  auto t0 = time::now();

  if (input.name_from_file) entity_name = basename(file_name);

  if (input.create) {
    db.createCollection({.name = entity_name, .allow_conflict = true});
    if (input.verbose) std::cout << input.type << " => " << entity_name << " Created" << std::endl;
  }

  auto collection = db.collection(entity_name);

  if (input.truncate) {
    collection.truncate();
    if (input.verbose) std::cout << "Truncated => " << entity_name << std::endl;
  }

  auto rows_imported = importData(collection, file_name, input);

  if (input.verbose) {
    auto t1 = time::now();
    fsec fs = t1 - t0;
    ms d = std::chrono::duration_cast<ms>(fs);

    std::cout << "Import done " << rows_imported << " rows in " << fs.count() << "s" << std::endl;
  }
}

void run(const ImportInput& input) {
  auto t0 = time::now();

  try {
    auto conn = Connection().hosts(input.hosts).auth(input.user_name, input.password);

    if (!conn.ping()) {
      std::cout << "Connection failed." << std::endl;
      return;
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
    fsec fs = t1 - t0;
    ms d = std::chrono::duration_cast<ms>(fs);

    std::cout << "Total Import done " << input.files.size() << " files in " << fs.count() << "s" << std::endl;
  }
}  // namespace arango_import
}  // namespace arango_import

auto main(int argc, char* argv[]) -> int {
  arango_import::ImportInput input;

  cxxopts::Options options("arango_import", "Import of data into arangodb");

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
  options.allow_unrecognised_options();
  auto result = options.parse(argc, argv);

  if (result.count("help")) {
    std::cout << options.help() << std::endl;
    exit(0);
  }

  if (result.count("hosts")) input.hosts = tools::split(result["hosts"].as<std::string>(), ',');
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
