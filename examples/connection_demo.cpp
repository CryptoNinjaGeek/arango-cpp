#include <zutano/zutano.h>

using namespace zutano;

auto main() -> int {

    // Initialize the client for ArangoDB.
    auto conn = connection()
            .endpoint("http://localhost:8529")
            .auth("root", "passwd");

    // Connect to "_system" database as root user.
    auto sys_db = conn.database("_system");

    // Create a new database named "demo".
    sys_db.create_datatabase("demo");

    // Connect to "demo" database as root user.
    auto db = conn.database("demo");

    // Create a new collection named "students".
    auto students = db.create_collection("students");

    // Add a hash index to the collection.
    students.add_hash_index({.fields= {"name"}, .unique= true});

    // Insert new documents into the collection.
    students.insert({{"name", "jane"},
                     {"age",  39}});
    students.insert({{"name", "josh"},
                     {"age",  18}});
    students.insert({{"name", "judy"},
                     {"age",  21}});

    // Execute an AQL query and iterate through the result cursor.
    auto cursor = db.execute("FOR doc IN students RETURN doc");

    for (auto doc: cursor) {
        doc.dump();
    }

    return 0;
}