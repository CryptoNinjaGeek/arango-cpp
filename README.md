
[![Tests](https://github.com/CryptoNinjaGeek/arango-cpp/actions/workflows/cmake.yml/badge.svg)](https://github.com/CryptoNinjaGeek/arango-cpp/actions/workflows/cmake.yml)

# Arango-Cpp

C++ driver for [ArangoDB](https://www.arangodb.com), a scalable multi-model
Database natively supporting documents, graphs and search.

## Requirements

## Installation

## Getting Started

Here is a simple usage example:

```cpp
// Initialize the client for ArangoDB.
auto conn = Connection()
	.Host("http://localhost:8529")
	.auth("root", "passwd");

// Connect to "_system" Database as root user.
auto sys_db = conn.Database("_system");

// Create a new Database named "demo".
sys_db.CreateDatatabase("demo");

// Connect to "demo" Database as root user.
auto db = conn.Database("demo");

// Create a new Collection named "students".
auto students = db.createCollection("students");

// Add a hash index to the Collection.
students.addHashIndex({ .fields= { "name" }, .unique= true });

// Insert new documents into the Collection.
students.Insert({
  {"name", "jane"},{"age", 39}
});
students.Insert({
  {"name", "josh"},{"age", 18}
});
students.Insert({
  {"name", "judy"},{"age", 21}
});

// Execute an AQL query and iterate through the result cursor.
auto cursor = db.Execute("FOR doc IN students RETURN doc");

for (auto doc: cursor) {
  doc.dump();
}
```

