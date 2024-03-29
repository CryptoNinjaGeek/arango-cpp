#include <arango-cpp/arango-cpp.h>
#include <iostream>

using namespace arangocpp;
using namespace arangocpp::tools;
using namespace jsoncons::literals;

auto main() -> int {
  try {
    auto conn = Connection()
                    .hosts({"http://db1.google.dk:8529/", "http://db2.google.dk:8529/"})
                    .auth("root", "openSesame")
                    .resolve({{"db1.google.dk", "127.0.0.1", 8529}, {"db2.google.dk", "127.0.0.1", 8529}});

    if (conn.ping()) std::cout << "ping'ed OK" << std::endl;

    auto sys_db = conn.database("_system");

    sys_db.createDatabase({.name = "demo", .sharding = "flexible", .allow_conflict = true});

    auto db = conn.database("demo");

    db.createCollection({.name = "students", .allow_conflict = true});

    auto students = db.collection("students");

    students.truncate();
    jsoncons::json array(jsoncons::json_array_arg);
    for (int no = 1; no < 10; no++) {
      array.push_back(to_json{{"name", std::string("josh-") + std::to_string(no)}, {"age", no}});
    }
    students.insert(array, {.sync = false});

    auto cursor = db.execute({.query = "FOR doc IN students RETURN doc", .count = true, .batch_size = 3});

    while (cursor.hasMore()) {
      auto row = cursor.next();
      std::cout << pretty_print(row) << "\n\n";
    }

    auto info = db.info();

    std::cout << pretty_print(info) << std::endl;

  } catch (std::exception& e) {
    std::cout << e.what() << std::endl;
  }

  return 0;
}