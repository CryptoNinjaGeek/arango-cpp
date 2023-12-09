#include <gtest/gtest.h>

#include <arango-cpp/Collection.h>
#include <arango-cpp/Connection.h>
#include <arango-cpp/Database.h>

#include <cstdint>

using namespace arangocpp;

TEST(CollectionTest, Insert) {
  Connection conn;
  Database db(conn,"test");
  Collection col(conn, db, "test_collection");

  // Create a document to insert
  jsoncons::json doc;
  doc["foo"] = "bar";

  // Insert the document and verify the response
  auto response = col.insert(doc);
  ASSERT_EQ(response["_id"], "test_id");
  ASSERT_EQ(response["_revision"], "1");
}
