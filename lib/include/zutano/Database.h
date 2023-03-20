#pragma once

#include <string>
#include <vector>
#include <zutano/PrivateImpl.h>
#include <zutano/Collection.h>
#include <nlohmann/json.hpp>

namespace zutano {

struct UserInput {
  std::string username;
  std::string password;
  bool active{true};
  nlohmann::json extra;
};

struct DatabaseCreateInput {
  std::string name;
  std::vector<UserInput> users;
  std::optional<int> replication_factor{3};
  std::optional<int> write_concern{0};
  std::optional<std::string> sharding;
  bool allowConflict{true};
};

struct CollectionCreateInput {
  std::string name;
  bool sync{false};
  bool system{false};
  bool edge{false};
  bool user_keys{true};
  std::optional<int> key_increment;
  std::optional<int> key_offset;
  std::string key_generator{"traditional"};
  std::optional<std::string> shard_fields;
  std::optional<int> shard_count;
  std::optional<int> replication_factor;
  std::optional<std::string> shard_like;
  std::optional<bool> sync_replication;
  std::optional<bool> enforce_replication_factor;
  std::optional<std::string> sharding_strategy;
  std::optional<std::string> smart_join_attribute;
  std::optional<int> write_concern;
  std::optional<nlohmann::json> schema;
  std::optional<nlohmann::json> computedValues;
  bool allowConflict{true};
};

class Connection;
class Database {
 public:
  Database(Connection, std::string);
  Database() = default;

  ~Database() = default;

  auto CreateDatatabase(DatabaseCreateInput) -> Database;

  auto CreateCollection(CollectionCreateInput) -> Collection;

  auto Collection(std::string) -> Collection;

  auto Execute(std::string) -> nlohmann::json;

  auto name() -> std::string;

 private:
  PrivateImplPtr p_;
};

typedef std::shared_ptr<Database> DatabasePtr;

} // zutano

