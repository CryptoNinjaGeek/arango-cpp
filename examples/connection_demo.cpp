#include <zutano/zutano.h>
#include <iostream>

using namespace zutano;

auto main() -> int {

  try {
	auto conn = Connection()
		.Endpoint("http://10.211.55.4:8529/");

	if (conn.Ping())
	  std::cout << "Ping'ed OK" << std::endl;

	auto sys_db = conn.Database("_system");

	sys_db.CreateDatatabase({"demo", .sharding="flexible", .allowConflict=true});

	auto db = conn.Database("demo");

	db.CreateCollection({"students", .allowConflict=true});

	auto students = db.Collection("students");
	
	students.AddHashIndex({.fields= {"name"}, .unique= true});

	students.Insert({{"name", "jane5"},
					 {"age", 39}}, {.overwrite=true});
	students.Insert({{"name", "josh1"},
					 {"age", 18}}, {.overwrite=true});
	students.Insert({{"name", "judy1"},
					 {"age", 21}}, {.overwrite=true});

	auto cursor = db.Execute("FOR doc IN students RETURN doc");

	for (auto doc : cursor) {
	  doc.dump();
	}
  } catch (std::exception &e) {
	std::cout << e.what() << std::endl;
  }

  return 0;
}