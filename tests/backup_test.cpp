#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <arango-cpp/Connection.h>
#include <arango-cpp/Database.h>
#include <arango-cpp/Backup.h>
#include <fmt/core.h>

#include <cstdint>

using namespace arangocpp;
using ::testing::StartsWith;

class BackupTest : public testing::Test {
 public:
  Connection connection;
  Database database;
  Backup backup;

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

      backup = Backup(connection);
    } catch (std::exception& e) {
      std::cout << e.what() << std::endl;
    }
  }

  void TearDown() override {
    try {
      auto sys_db = connection.database("_system");

      sys_db.deleteDatabase("test_database");
    } catch (std::exception& e) {
      std::cout << e.what() << std::endl;
    }
  }
};


TEST_F(BackupTest, Get) {
  auto response = backup.get();
  std::cout << response << std::endl;
  ASSERT_EQ(response, jsoncons::json::array());
}

TEST_F(BackupTest, Remove) {
  auto backupId = "backup-id";
  auto result = backup.remove(backupId);
  std::cout << result << std::endl;

  ASSERT_TRUE(result);
}

TEST_F(BackupTest, Download) {
  auto input = input::BackupInput{};
  auto response = backup.download(input);
  std::cout << response << std::endl;
  //ASSERT_EQ(response, jsoncons::json::object());
}

TEST_F(BackupTest, Upload) {
  auto input = input::BackupInput{};
  auto response = backup.upload(input);
  std::cout << response << std::endl;
//  ASSERT_EQ(response, jsoncons::json::object());
}

TEST_F(BackupTest, Restore) {
  auto backupId = "backup-id";
  auto response = backup.restore(backupId);
  std::cout << response << std::endl;
//  ASSERT_EQ(response, jsoncons::json::object());
}
