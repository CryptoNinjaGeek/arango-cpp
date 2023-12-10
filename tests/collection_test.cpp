#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <arango-cpp/Collection.h>
#include <arango-cpp/Connection.h>
#include <arango-cpp/Database.h>
#include <fmt/core.h>

#include <cstdint>

using namespace arangocpp;
using ::testing::StartsWith;

class CollectionTest : public testing::Test {
 public:
  Connection connection;
  Database database;

  void SetUp() override {
    try {
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
    } catch (std::exception& e) {
      std::cout << e.what() << std::endl;
    }
  }

  void TearDown() override {
    try {

//    database.deleteDatabase();
    } catch (std::exception& e) {
      std::cout << e.what() << std::endl;
    }
  }

};

TEST_F(CollectionTest, Insert) {
  // Create a document to insert
  jsoncons::json doc;
  doc["foo"] = "bar";

  // Insert the document into the collection
  auto response = this->database.collection("test_collection")
                      .insert(doc);

  // Verify the response
  EXPECT_THAT(response["_id"].as_string(), StartsWith("test_collection/"));
  EXPECT_THAT(response["_rev"].as_string().length(), 11);
}

TEST_F(CollectionTest, Update) {
  // Create a document to update
  jsoncons::json doc;
  doc["foo"] = "bar";

  // Insert the document into the collection
  auto col = this->database.collection("test_collection");

  // Insert the document and get its handle
  auto response = col.insert(doc);
  std::string handle = response["_id"].as<std::string>();

  doc["lem"] = "bar";
  doc["_id"] = response["_id"].as_string();

  // Update the document and verify the response
  response = col.update(doc, input::UpdateInput{
                                 .return_old = false,
                                 .return_new = false,
                                 .merge = false,
                                 .check_rev = true
                             });

  EXPECT_THAT(response["_id"].as_string(), StartsWith("test_collection/"));
  EXPECT_THAT(response["_rev"].as_string().length(), 11);
  EXPECT_THAT(response["_oldRev"].as_string().length(), 11);
}

TEST_F(CollectionTest, Replace) {
  // Insert the document into the collection
  auto col = this->database.collection("test_collection");

  // Create a document to replace
  jsoncons::json doc;
  doc["foo"] = "bar";

  // Insert the document and get its handle
  auto response = col.insert(doc);
  std::string handle = response["_id"].as<std::string>();

  doc["foo"] = "bar";
  doc["_id"] = handle;

  // Replace the document and verify the response
  response = col.replace(doc, input::ReplaceInput{
                                  .return_old = false,
                                  .return_new = false,
                                  .check_rev = true
                              });

  EXPECT_THAT(response["_id"].as_string(), StartsWith("test_collection/"));
  EXPECT_THAT(response["_rev"].as_string().length(), 11);
  EXPECT_THAT(response["_oldRev"].as_string().length(), 11);
}

TEST_F(CollectionTest, Remove) {
  // Insert the document into the collection
  auto col = this->database.collection("test_collection");

  // Create a document to remove
  jsoncons::json doc,remove;
  doc["foo"] = "bar";

  // Insert the document and get its handle
  auto response = col.insert(doc);
  std::string handle = response["_id"].as<std::string>();

  remove["_id"] = handle;

  // Remove the document and verify the response
  bool deleted = col.remove(remove, input::DeleteInput{
                                     .return_old = false,
                                     .check_rev = true
                                 });

  ASSERT_TRUE(deleted);
}

TEST_F(CollectionTest, Truncate) {
  // Insert the document into the collection
  auto col = this->database.collection("test_collection");

  // Truncate the collection and verify the response
  bool truncated = col.truncate();

  ASSERT_TRUE(truncated);
}
