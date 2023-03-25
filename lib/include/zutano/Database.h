#pragma once

#include <string>
#include <vector>
#include <zutano/PrivateImpl.h>
#include <zutano/Collection.h>
#include <jsoncons/json.hpp>

namespace zutano {

struct UserInput {
  std::string username;
  std::string password;
  bool active{true};
  jsoncons::json extra;
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
  std::optional<jsoncons::json> schema;
  std::optional<jsoncons::json> computedValues;
  bool allowConflict{true};
};

class Connection;
class Database {
 public:
  Database(const Connection &, std::string);
  Database() = default;

  ~Database() = default;

  auto CreateDatabase(DatabaseCreateInput) -> Database;

  auto CreateCollection(CollectionCreateInput) -> Collection;

  auto Collection(std::string) -> Collection;

  auto Execute(const std::string &) -> jsoncons::json;

  auto name() -> std::string;

 private:
  PrivateImplPtr p_;
};

} // zutano

