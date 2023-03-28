#include <zutano/zutano.h>
#include <iostream>

using namespace zutano;
using namespace zutano::tools;
using namespace jsoncons::literals;

auto main() -> int {
  try {
    auto conn = Connection()
                    .hosts({"http://db1.google.dk:8529/", "http://db2.google.dk:8529/"})
                    .auth("root", "EAS6wQuKbGH9kKxe")
                    .resolve({{"db1.google.dk", "10.211.55.4", 8529}, {"db2.google.dk", "10.211.55.4", 8529}});

    if (conn.ping()) std::cout << "ping'ed OK" << std::endl;

    auto sys_db = conn.database("_system");

    sys_db.createDatabase({.name = "demo", .sharding = "flexible", .allow_conflict = true});

    auto db = conn.database("demo");

    db.createCollection({.name = "students", .allow_conflict = true});

    auto students = db.collection("students");

    /*    students.truncate();
        students.addHashIndex({.fields = {"name"}, .unique = true});
        students.insert(to_json{{"name", "jane5"}, {"age", 39}}, {.overwrite = true});
        students.insert(to_json{{"name", "josh1"}, {"age", 18}}, {.overwrite = true});

        jsoncons::json array(jsoncons::json_array_arg);
        for (int no = 1; no < 100000; no++) {
          array.push_back(to_json{{"name", std::string("josh-") + std::to_string(no)}, {"age", no}});
        }
        array = students.insert(array, {.sync = false});

        auto result = students.insert(to_json{{"name", "judy1"}, {"age", 21}}, {.overwrite = true, .return_new = true});
        auto new_record = result["new"];

        new_record["name"] = "Hannah";

        students.replace(new_record);

        new_record["name"] = "Bannah";

        students.update(new_record, {.check_rev = false});
        students.remove(new_record);
    */
    auto cursor = db.execute({.query = "FOR doc IN students RETURN doc", .count = true, .batch_size = 100});

    while (cursor.hasMore()) {
      auto row = cursor.next();
      std::cout << pretty_print(row) << "\n\n";
    }
    //    students.remove(array);

  } catch (std::exception& e) {
    std::cout << e.what() << std::endl;
  }

  return 0;
}