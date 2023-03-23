#include <zutano/zutano.h>
#include <iostream>

using namespace zutano;

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

	auto sys_db = conn.Database("_system");

	sys_db.CreateDatatabase({"demo", .sharding="flexible", .allowConflict=true});

	auto db = conn.Database("demo");

	db.CreateCollection({"students", .allowConflict=true});

	auto students = db.Collection("students");

	students.Truncate();

	students.AddHashIndex({.fields= {"name"}, .unique= true});

	students.Insert({{"name", "jane5"},
					 {"age", 39}}, {.overwrite=true});
	students.Insert({{"name", "josh1"},
					 {"age", 18}}, {.overwrite=true});

	nlohmann::json array;
	array.push_back({{"name", "josh2"}, {"age", 19}});
	array.push_back({{"name", "josh3"}, {"age", 20}});
	array.push_back({{"name", "josh4"}, {"age", 21}});
	array = students.Insert(array, {.overwrite=true});

	auto result = students.Insert({{"name", "judy1"},
								   {"age", 21}}, {.overwrite=true, .return_new=true});
	auto new_record = result["new"];

	new_record["name"] = "Hannah";

	students.Replace(new_record);

	new_record["name"] = "Bannah";

	students.Update(new_record, {.check_rev=false});
	students.Delete(new_record);
	students.Delete(array);

	auto cursor = db.Execute("FOR doc IN students RETURN doc");

	for (auto doc : cursor) {
	  doc.dump();
	}
  } catch (std::exception &e) {
	std::cout << e.what() << std::endl;
  }

  return 0;
}