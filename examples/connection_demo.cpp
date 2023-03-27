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
					 {"db1.google.dk", "10.211.55.4", 8529},
					 {"db2.google.dk", "10.211.55.4", 8529}
				 }
		);

	if (conn.Ping())
	  std::cout << "Ping'ed OK" << std::endl;

	auto sys_db = conn.Database("_system");

	sys_db.CreateDatabase({.name= "demo", .sharding="flexible", .allow_conflict=true});

	auto db = conn.Database("demo");

	db.CreateCollection({.name="students", .allow_conflict=true});

	auto students = db.Collection("students");

	students.Truncate();

	students.AddHashIndex({.fields= {"name"}, .unique= true});

	students.Insert(to_json{{"name", "jane5"}, {"age", 39}}, {.overwrite=true});
	students.Insert(to_json{{"name", "josh1"}, {"age", 18}}, {.overwrite=true});

	jsoncons::json array(jsoncons::json_array_arg);
	for (int no = 1; no < 100000; no++)
	  array.push_back(to_json{{"name", std::string("josh-") + std::to_string(no)}, {"age", no}});
	array = students.Insert(array, {.overwrite=true});

	auto result = students.Insert(to_json{{"name", "judy1"}, {"age", 21}}, {.overwrite=true, .sync=false});
	auto new_record = result["new"];

	return 0;
	new_record["name"] = "Hannah";

	students.Replace(new_record);

	new_record["name"] = "Bannah";

	students.Update(new_record, {.check_rev=false});
	students.Delete(new_record);

	auto cursor = db.Execute({.query="FOR doc IN students RETURN doc", .batch_size=100, .count=true});

	while (cursor.HasMore()) {
	  auto row = cursor.Next();
	  std::cout << pretty_print(row) << "\n\n";
	}
	students.Delete(array);

  } catch (std::exception &e) {
	std::cout << e.what() << std::endl;
  }

  return 0;
}