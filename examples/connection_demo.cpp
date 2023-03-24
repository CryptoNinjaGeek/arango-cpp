#include <zutano/zutano.h>
#include <iostream>

using namespace zutano;
using namespace zutano::tools;
using namespace jsoncons::literals;

auto main() -> int {

  try {
	auto conn = Connection()
		.Hosts({
				   "http://10.211.55.4:8529/",
				   "http://10.211.55.4:8529/",
				   "http://10.211.55.4:8529/"
			   });
/*
 * Certificate(string)
 * Resolve(List<StringPair>)
 * */
	if (conn.Ping())
	  std::cout << "Ping'ed OK" << std::endl;

	std::cout << "XXX" << std::endl;
	auto sys_db = conn.Database("_system");
	std::cout << "XXX" << std::endl;

	sys_db.CreateDatatabase({"demo", .sharding="flexible", .allowConflict=true});
	std::cout << "XXX" << std::endl;

	auto db = conn.Database("demo");
	std::cout << "XXX" << std::endl;

	db.CreateCollection({"students", .allowConflict=true});

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

	auto result = students.Insert(to_json{{"name", "judy1"}, {"age", 21}}, {.overwrite=true, .return_new=true});
	auto new_record = result["new"];

	new_record["name"] = "Hannah";

	students.Replace(new_record);

	new_record["name"] = "Bannah";

	students.Update(new_record, {.check_rev=false});
	students.Delete(new_record);
	students.Delete(array);

	auto cursor = db.Execute("FOR doc IN students RETURN doc");

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