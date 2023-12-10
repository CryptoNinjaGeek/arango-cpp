#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <arango-cpp/Collection.h>
#include <arango-cpp/Connection.h>
#include <arango-cpp/Database.h>
#include <fmt/core.h>

#include <cstdint>

using namespace arangocpp;
using ::testing::StartsWith;

class DatabaseTest : public testing::Test {
protected:
  // Create a connection to the ArangoDB database
  Connection connection;

  // Create the database object
  Database database;

  void SetUp() override {
    connection.host(fmt::format("http://localhost:{}/", 8529));
    connection.auth("root","openSesame");
    connection.ping();

    auto sys_db = connection.database("_system");

    sys_db.createDatabase({
        .name = "test_database",
        .sharding = "flexible",
        .allow_conflict = true
    });

    database = connection.database("test_database");

    database.createCollection(input::CollectionCreateInput{
      .name = "test_collection"
    });
  }

  void TearDown() override {
    // Delete the database
    auto sys_db = connection.database("_system");

    sys_db.deleteDatabase("test_database",{.ignore_missing = true});
  }
};

TEST_F(DatabaseTest, CreateDatabase) {
  // Verify that the database was created successfully
  ASSERT_EQ(database.name(), "test_database");
}

TEST_F(DatabaseTest, CreateCollection) {
  // Create a collection
  input::CollectionCreateInput input;
  input.name = "test_collection";
  database.createCollection(input);

  // Verify that the collection was created successfully
  ASSERT_TRUE(database.hasCollection("test_collection"));
}

TEST_F(DatabaseTest, ExecuteQuery) {
  jsoncons::json doc;
  doc["name"] = "John Doe";

  // Insert the document into the collection
  auto response = database.collection("test_collection")
                      .insert(doc);

  // Execute a query
  Cursor cursor = database.execute({
    .query = "FOR doc IN test_collection RETURN doc",
    .count = true
  });

  // Verify that the query executed successfully
  ASSERT_TRUE(cursor.count() != 0);

  // Iterate over the cursor and verify the results
  while (cursor.hasMore()) {
    auto doc = cursor.next();
    ASSERT_EQ(doc["name"], "John Doe");
  }
}

TEST_F(DatabaseTest, ExplainQuery) {
  // Explain a query
  input::ExplainInput input;
  input.query = "RETURN 1";
  auto explain = database.explain(input);

  // Verify that the query was explained successfully
  ASSERT_TRUE(not explain.is_null());

  // Verify the execution plan
//  ASSERT_EQ(explain.plan.size(), 1);
//  ASSERT_EQ(explain.plan[0].type, "RETURN");
}

TEST_F(DatabaseTest, DeleteDatabase) {
  // Verify that the database was deleted successfully
  auto sys_db = connection.database("_system");

  sys_db.deleteDatabase("test_database");

  ASSERT_FALSE(database.hasDatabase("test_database"));
}

TEST_F(DatabaseTest, DeleteCollection) {
  // Delete the collection
  database.deleteCollection("test_collection");

  // Verify that the collection was deleted successfully
  ASSERT_FALSE(database.hasCollection("test_collection"));
}
