//
// Created by Carsten Tang on 19/03/2023.
//

#include <zutano/Database.h>
#include <zutano/Connection.h>
#include <zutano/Exceptions.h>
#include <zutano/Tools.h>

#include <jsoncons/json.hpp>

#include <iostream>
#include <utility>

using namespace jsoncons;
using namespace jsoncons::literals;
using namespace zutano::tools;

namespace zutano {

class DatabasePimpl : public PrivateImpl {
 public:
  Connection connection_;
  std::string name_;

 public:
  static inline auto Pimpl(const std::shared_ptr<PrivateImpl> &p) {
    return std::dynamic_pointer_cast<DatabasePimpl>(p);
  }
};

Database::Database(const Connection &conn, std::string name) {
  auto p = std::make_shared<DatabasePimpl>();
  p->connection_ = conn;
  p->name_ = std::move(name);
  p_ = p;
}

auto Database::CreateDatabase(DatabaseCreateInput input) -> Database {
  auto p = DatabasePimpl::Pimpl(p_);

  if (input.name.empty())
    throw ClientError("CreateDatabase => database name cannot be empty");

  jsoncons::json data = to_json{
      {"name", input.name}
  };

  jsoncons::json options;

  if (input.replication_factor)
    options["replicationFactor"] = input.replication_factor.value();
  if (input.write_concern)
    options["writeConcern"] = input.write_concern.value();
  if (input.sharding)
    options["sharding"] = input.sharding.value();

  if (!options.empty())
    data["options"] = options;

  jsoncons::json users(jsoncons::json_array_arg);
  for (auto user : input.users) {
    users.push_back(to_json{
        {"username", user.username},
        {"passwd", user.password},
        {"active", user.active},
    });
  }
  if (not users.empty())
    data["users"] = users;

  auto r = Request()
      .Method(HttpMethod::POST)
      .Database(p->name_)
      .Endpoint("/database")
      .Data(data.to_string());

  auto response = p->connection_.SendRequest(r);
  if (response.contains({401, 403}))
    throw AuthenticationError();
  else if (not response.is_success() && (response.http_code() != 409 || not input.allowConflict))
    throw ServerError(response.error_message(), response.error_code());

  return {p->connection_, input.name};
}

auto Database::CreateCollection(CollectionCreateInput input) -> zutano::Collection {
  auto p = DatabasePimpl::Pimpl(p_);

  if (input.name.empty())
    throw ClientError("CreateCollection => collection name cannot be empty");

  jsoncons::json data = to_json{
      {"name", input.name},
      {"waitForSync", input.sync},
      {"isSystem", input.system},
      {"type", input.edge ? 3 : 2},
  };

  jsoncons::json key_options = to_json{
      {"type", input.key_generator},
      {"allowUserKeys", input.user_keys}
  };

  if (input.key_increment)
    key_options["increment"] = input.key_increment.value();
  if (input.key_offset)
    key_options["offset"] = input.key_offset.value();

  data["keyOptions"] = key_options;

  if (input.shard_count)
    data["numberOfShards"] = input.shard_count.value();
  if (input.shard_fields)
    data["shardKeys"] = input.shard_fields.value();
  if (input.replication_factor)
    data["replicationFactor"] = input.replication_factor.value();
  if (input.shard_like)
    data["distributeShardsLike"] = input.shard_like.value();
  if (input.sharding_strategy)
    data["shardingStrategy"] = input.sharding_strategy.value();
  if (input.smart_join_attribute)
    data["smartJoinAttribute"] = input.smart_join_attribute.value();
  if (input.write_concern)
    data["writeConcern"] = input.write_concern.value();
  if (input.schema)
    data["schema"] = input.schema.value();
  if (input.computedValues)
    data["computedValues"] = input.computedValues.value();

  std::vector<StringPair> params;
  if (input.sync_replication)
    params.emplace_back("waitForSyncReplication", std::to_string(input.sync_replication.value()));
  if (input.enforce_replication_factor)
    params.emplace_back("enforceReplicationFactor", std::to_string(input.enforce_replication_factor.value()));

  auto r = Request()
      .Method(HttpMethod::POST)
      .Database(p->name_)
      .Endpoint("/collection")
      .Parameters(params)
      .Data(data.to_string());

  auto response = p->connection_.SendRequest(r);
  if (response.contains({401, 403}))
    throw AuthenticationError();
  else if (not response.is_success() && (response.http_code() != 409 || not input.allowConflict))
    throw ServerError(response.error_message(), response.error_code());

  return {p->connection_, *this, input.name};
}

auto Database::Collection(std::string name) -> zutano::Collection {
  auto p = DatabasePimpl::Pimpl(p_);
  return {p->connection_, *this, std::move(name)};
}

auto Database::Execute(const std::string &) -> jsoncons::json {
  jsoncons::json j;
  return j;
}

auto Database::name() -> std::string {
  auto p = DatabasePimpl::Pimpl(p_);
  return p->name_;
}

} // zutano