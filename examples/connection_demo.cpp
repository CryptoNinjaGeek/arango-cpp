#include <zutano/zutano.h>
#include <iostream>

using namespace zutano;
using namespace zutano::tools;
using namespace jsoncons::literals;

auto main() -> int {

  try {
    auto conn = Connection()
        .Hosts({
                   "http://db1.google.dk:8529/",
                   "http://db2.google.dk:8529/"
               })
        .Auth("root", "EAS6wQuKbGH9kKxe")
        .Resolve({
                     {"db1.google.dk", "127.0.0.1", 8529},
                     {"db2.google.dk", "127.0.0.1", 8529}
                 }
        );

    if (conn.Ping())
      std::cout << "Ping'ed OK" << std::endl;

    auto sys_db = conn.Database("_system");

    sys_db.CreateDatabase({.name= "demo", .sharding="flexible", .allowConflict=true});

    auto db = conn.Database("demo");

    db.CreateCollection({.name="students", .allowConflict=true});

    auto students = db.Collection("students");

    students.Truncate();

    students.AddHashIndex({.fields= {"name"}, .unique= true});

    students.Insert(to_json{{"name", "jane5"}, {"age", 39}}, {.overwrite=true});
    students.Insert(to_json{{"name", "josh1"}, {"age", 18}}, {.overwrite=true});

    jsoncons::json array(jsoncons::json_array_arg);
    array.push_back(to_json{{"name", "josh2"}, {"age", 19}});
    array.push_back(to_json{{"name", "josh3"}, {"age", 20}});
    array.push_back(to_json{{"name", "josh4"}, {"age", 21}});
    array = students.Insert(array, {.overwrite=true});

    auto result = students.Insert(to_json{{"name", "judy1"}, {"age", 21}}, {.return_new=true, .overwrite=true});
    auto new_record = result["new"];

    new_record["name"] = "Hannah";

    students.Replace(new_record);

    new_record["name"] = "Bannah";

    students.Update(new_record, {.check_rev=false});
    students.Delete(new_record);
    students.Delete(array);

    auto cursor = db.Execute({.query="FOR doc IN students RETURN doc"});

    if (cursor.is_array()) {
      for (const auto &item : cursor.array_range()) {
        pretty_print(item);
      }
    }
  } catch (std::exception &e) {
    std::cout << e.what() << std::endl;
  }

  return 0;
}