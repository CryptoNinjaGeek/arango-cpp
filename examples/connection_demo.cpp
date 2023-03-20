#include <zutano/zutano.h>

using namespace zutano;

auto main() -> int {

    auto conn = connection()
            .endpoint("http://192.168.1.10")
            .auth("root", "123");

    auto sys_db = conn.database("_system");

    sys_db.create_datatabase("demo");

    auto db = conn.database("demo");

    db.create_collection("students");

    auto students = db.collection("students");

    students.add_hash_index({.fields= {"name"}, .unique= true});

    students.insert({{"name", "jane"},
                     {"age",  39}});
    students.insert({{"name", "josh"},
                     {"age",  18}});
    students.insert({{"name", "judy"},
                     {"age",  21}});

    auto cursor = db.execute("FOR doc IN students RETURN doc");

    for (auto doc: cursor) {
        doc.dump();
    }

    return 0;
}