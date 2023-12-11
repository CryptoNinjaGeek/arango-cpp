//
// Created by Carsten Tang on 19/03/2023.
//

#include <arango-cpp/Database.h>
#include <arango-cpp/Connection.h>
#include <arango-cpp/Exceptions.h>
#include <arango-cpp/Tools.h>

#include <jsoncons/json.hpp>

#include <iostream>
#include <utility>

using namespace jsoncons;
using namespace jsoncons::literals;
using namespace arangocpp::tools;

namespace arangocpp {

class DatabasePimpl : public PrivateImpl {
 public:
  Connection connection_;
  std::string name_;

 public:
  static inline auto pimpl(const std::shared_ptr<PrivateImpl>& p) { return std::dynamic_pointer_cast<DatabasePimpl>(p); }
};

Database::Database(const Connection& conn, std::string name) {
  auto p = std::make_shared<DatabasePimpl>();
  p->connection_ = conn;
  p->name_ = std::move(name);
  p_ = p;
}

auto Database::createDatabase(input::DatabaseCreateInput input) -> Database {
  auto p = DatabasePimpl::pimpl(p_);

  if (input.name.empty()) throw ClientError("createDatabase => database name cannot be empty");

  jsoncons::json data = to_json{{"name", input.name}};

  jsoncons::json options;

  if (input.replication_factor) options["replicationFactor"] = input.replication_factor.value();
  if (input.write_concern) options["writeConcern"] = input.write_concern.value();
  if (input.sharding) options["sharding"] = input.sharding.value();

  if (!options.empty()) data["options"] = options;

  jsoncons::json users(jsoncons::json_array_arg);
  for (auto user : input.users) {
    users.push_back(to_json{
        {"username", user.username},
        {"passwd", user.password},
        {"active", user.active},
    });
  }
  if (not users.empty()) data["users"] = users;

  auto r = Request().method(HttpMethod::POST).database(p->name_).endpoint("/database").data(data.to_string());

  auto response = p->connection_.sendRequest(r);
  if (response.contains({401, 403}))
    throw AuthenticationError();
  else if (not response.isSuccess() && (response.httpCode() != 409 || not input.allow_conflict))
    throw ServerError(response.errorMessage(), response.errorCode());

  return {p->connection_, input.name};
}

auto Database::createCollection(input::CollectionCreateInput input) -> arangocpp::Collection {
  auto p = DatabasePimpl::pimpl(p_);

  if (input.name.empty()) throw ClientError("createCollection => collection name cannot be empty");

  jsoncons::json data = to_json{
      {"name", input.name},
      {"waitForSync", input.sync},
      {"isSystem", input.system},
      {"type", input.edge ? 3 : 2},
  };

  jsoncons::json key_options = to_json{{"type", input.key_generator}, {"allowUserKeys", input.user_keys}};

  if (input.key_increment) key_options["increment"] = input.key_increment.value();
  if (input.key_offset) key_options["offset"] = input.key_offset.value();

  data["keyOptions"] = key_options;

  if (input.shard_count) data["numberOfShards"] = input.shard_count.value();
  if (input.shard_fields) data["shardKeys"] = input.shard_fields.value();
  if (input.replication_factor) data["replicationFactor"] = input.replication_factor.value();
  if (input.shard_like) data["distributeShardsLike"] = input.shard_like.value();
  if (input.sharding_strategy) data["shardingStrategy"] = input.sharding_strategy.value();
  if (input.smart_join_attribute) data["smartJoinAttribute"] = input.smart_join_attribute.value();
  if (input.write_concern) data["writeConcern"] = input.write_concern.value();
  if (input.schema) data["schema"] = input.schema.value();
  if (input.computed_values) data["computedValues"] = input.computed_values.value();

  std::vector<string_pair> params;
  if (input.sync_replication) params.emplace_back("waitForSyncReplication", std::to_string(input.sync_replication.value()));
  if (input.enforce_replication_factor)
    params.emplace_back("enforceReplicationFactor", std::to_string(input.enforce_replication_factor.value()));

  auto r = Request().method(HttpMethod::POST).database(p->name_).endpoint("/collection").parameters(params).data(data.to_string());

  auto response = p->connection_.sendRequest(r);
  if (response.contains({401, 403}))
    throw AuthenticationError();
  else if (not response.isSuccess() && (response.httpCode() != 409 || not input.allow_conflict))
    throw ServerError(response.errorMessage(), response.errorCode());

  return {p->connection_, *this, input.name};
}

auto Database::collection(std::string name) -> arangocpp::Collection {
  auto p = DatabasePimpl::pimpl(p_);
  return {p->connection_, *this, std::move(name)};
}

auto Database::execute(input::ExecuteInput input) -> Cursor {
  auto p = DatabasePimpl::pimpl(p_);

  json data = to_json{{"query", input.query}, {"count", input.count}};

  if (input.batch_size) data["batchSize"] = input.batch_size.value();
  if (input.ttl) data["ttl"] = input.ttl.value();
  if (input.bind_vars) data["bindVars"] = input.bind_vars.value();
  if (input.cache) data["cache"] = tools::toString(input.cache.value());
  if (input.memory_limit) data["memoryLimit"] = input.memory_limit;

  json options;
  if (input.full_count) options["fullCount"] = tools::toString(input.full_count.value());
  if (input.fill_block_cache) options["fillBlockCache"] = tools::toString(input.fill_block_cache.value());
  if (input.max_plans) options["maxNumberOfPlans"] = input.max_plans.value();
  if (input.optimizer_rules) options["optimizer"] = json{{"rules", input.optimizer_rules.value()}};
  if (input.fail_on_warning) options["failOnWarning"] = tools::toString(input.fail_on_warning.value());
  if (input.profile) options["profile"] = tools::toString(input.profile.value());
  if (input.max_transaction_size) options["maxTransactionSize"] = input.max_transaction_size.value();
  if (input.max_warning_count) options["maxWarningCount"] = input.max_warning_count.value();
  if (input.intermediate_commit_count) options["intermediateCommitCount"] = input.intermediate_commit_count.value();
  if (input.intermediate_commit_size) options["intermediateCommitSize"] = input.intermediate_commit_size.value();
  if (input.satellite_sync_wait) options["satelliteSyncWait"] = input.satellite_sync_wait.value();
  if (input.stream) options["stream"] = tools::toString(input.stream.value());
  if (input.skip_inaccessible_cols) options["skipInaccessibleCollections"] = tools::toString(input.skip_inaccessible_cols.value());
  if (input.max_runtime) options["maxRuntime"] = input.max_runtime.value();

  if (not options.empty()) data["options"] = options;

  auto r = Request().method(HttpMethod::POST).database(p->name_).data(data.to_string()).endpoint("/cursor");

  if (input.allow_dirty_read) r.headers({{"x-arango-allow-dirty-read", "true"}});

  auto response = p->connection_.sendRequest(r);
  if (response.contains({401, 403}))
    throw AuthenticationError();
  else if (not response.isSuccess())
    throw ServerError(response.errorMessage(), response.errorCode());

  return {p->name_, p->connection_, response.body()};
}

auto Database::name() -> std::string {
  auto p = DatabasePimpl::pimpl(p_);
  return p->name_;
}

auto Database::explain(input::ExplainInput input) -> jsoncons::json {
  auto p = DatabasePimpl::pimpl(p_);

  json options = to_json{{"allPlans", input.all_plans}};
  json data = to_json{{"query", input.query}};

  if (input.max_plans) options["maxNumberOfPlans"] = input.max_plans;
  if (input.opt_rules) options["optimizer"] = to_json{{"rules", input.opt_rules.value()}};

  if (input.bind_vars) data["bindVars"] = input.bind_vars.value();

  data["options"] = options;

  auto r = Request().method(HttpMethod::POST).database(p->name_).data(data.to_string()).endpoint("/explain");

  auto response = p->connection_.sendRequest(r);
  if (response.contains({401, 403}))
    throw AuthenticationError();
  else if (not response.isSuccess())
    throw ServerError(response.errorMessage(), response.errorCode());

  auto body = response.body();

  if (body.contains("plan"))
    return {body["plan"]};
  else if (body.contains("plans"))
    return {body["plans"]};
  return response.body();
}

auto Database::validate(std::string query) -> jsoncons::json {
  auto p = DatabasePimpl::pimpl(p_);

  json data = to_json{{"query", query}};

  auto r = Request().method(HttpMethod::POST).database(p->name_).data(data.to_string()).endpoint("/query");

  auto response = p->connection_.sendRequest(r);
  if (response.contains({401, 403}))
    throw AuthenticationError();
  else if (not response.isSuccess())
    throw ServerError(response.errorMessage(), response.errorCode());

  return response.isSuccess();
}

auto Database::kill(std::string query_id) -> bool {
  auto p = DatabasePimpl::pimpl(p_);

  auto r = Request().method(HttpMethod::DELETE).database(p->name_).handle(std::move(query_id)).endpoint("/query/{handle}");

  auto response = p->connection_.sendRequest(r);
  if (response.contains({401, 403}))
    throw AuthenticationError();
  else if (not response.isSuccess())
    throw ServerError(response.errorMessage(), response.errorCode());

  return response.isSuccess();
}

auto Database::queries() -> jsoncons::json {
  auto p = DatabasePimpl::pimpl(p_);

  auto r = Request().method(HttpMethod::GET).database(p->name_).endpoint("/query/current");

  auto response = p->connection_.sendRequest(r);
  if (response.contains({401, 403}))
    throw AuthenticationError();
  else if (not response.isSuccess())
    throw ServerError(response.errorMessage(), response.errorCode());

  return response.body();
}

auto Database::slowQueries() -> jsoncons::json {
  auto p = DatabasePimpl::pimpl(p_);

  auto r = Request().method(HttpMethod::GET).database(p->name_).endpoint("/query/slow");

  auto response = p->connection_.sendRequest(r);
  if (response.contains({401, 403}))
    throw AuthenticationError();
  else if (not response.isSuccess())
    throw ServerError(response.errorMessage(), response.errorCode());

  return response.body();
}

auto Database::clearSlowQueries() -> bool {
  auto p = DatabasePimpl::pimpl(p_);

  auto r = Request().method(HttpMethod::DELETE).database(p->name_).endpoint("/query/slow");

  auto response = p->connection_.sendRequest(r);
  if (response.contains({401, 403}))
    throw AuthenticationError();
  else if (not response.isSuccess())
    throw ServerError(response.errorMessage(), response.errorCode());

  return response.isSuccess();
}

auto Database::tracking() -> jsoncons::json {
  auto p = DatabasePimpl::pimpl(p_);

  auto r = Request().method(HttpMethod::GET).database(p->name_).endpoint("/query/properties");

  auto response = p->connection_.sendRequest(r);
  if (response.contains({401, 403}))
    throw AuthenticationError();
  else if (not response.isSuccess())
    throw ServerError(response.errorMessage(), response.errorCode());

  return response.body();
}

auto Database::setTracking(input::TrackingInput input) -> jsoncons::json {
  auto p = DatabasePimpl::pimpl(p_);

  json data;
  if (input.enabled) data["enabled"] = tools::toString(input.enabled.value());
  if (input.max_slow_queries) data["maxSlowQueries"] = input.max_slow_queries.value();
  if (input.max_query_string_length) data["maxQueryStringLength"] = input.max_query_string_length.value();
  if (input.slow_query_threshold) data["slowQueryThreshold"] = input.slow_query_threshold.value();
  if (input.track_bind_vars) data["trackBindVars"] = tools::toString(input.track_bind_vars.value());
  if (input.track_slow_queries) data["trackSlowQueries"] = tools::toString(input.track_slow_queries.value());

  auto r = Request().method(HttpMethod::PUT).database(p->name_).data(data.to_string()).endpoint("/query/properties");

  auto response = p->connection_.sendRequest(r);
  if (response.contains({401, 403}))
    throw AuthenticationError();
  else if (not response.isSuccess())
    throw ServerError(response.errorMessage(), response.errorCode());

  return response.body();
}

auto Database::functions() -> jsoncons::json {
  auto p = DatabasePimpl::pimpl(p_);

  auto r = Request().method(HttpMethod::GET).database(p->name_).endpoint("/aqlfunction");

  auto response = p->connection_.sendRequest(r);
  if (response.contains({401, 403}))
    throw AuthenticationError();
  else if (not response.isSuccess())
    throw ServerError(response.errorMessage(), response.errorCode());

  return response.body();
}

auto Database::createFunction(const input::CreateFunctionInput& input) -> jsoncons::json {
  auto p = DatabasePimpl::pimpl(p_);

  json data = {{"name", input.name, "code", input.code}};

  auto r = Request().method(HttpMethod::POST).database(p->name_).data(data.to_string()).endpoint("/aqlfunction");

  auto response = p->connection_.sendRequest(r);
  if (response.contains({401, 403}))
    throw AuthenticationError();
  else if (not response.isSuccess())
    throw ServerError(response.errorMessage(), response.errorCode());

  auto body = response.body();

  return {{"is_new", body["isNewlyCreated"]}};
}

auto Database::deleteFunction(const input::DeleteFunctionInput& input) -> jsoncons::json {
  auto p = DatabasePimpl::pimpl(p_);
  std::vector<string_pair> params;

  params.emplace_back("group", tools::toString(input.group));

  auto r = Request().method(HttpMethod::DELETE).database(p->name_).handle(input.name).parameters(params).endpoint("/aqlfunction/{handle}");

  auto response = p->connection_.sendRequest(r);
  if (response.contains({401, 403}))
    throw AuthenticationError();
  else if (response.errorCode() == 1582 and input.ignore_missing)
    return {};
  else if (not response.isSuccess())
    throw ServerError(response.errorMessage(), response.errorCode());

  auto body = response.body();

  return to_json{{"deleted", body["deletedCount"].as<std::string>()}};
}

auto Database::queryRules() -> jsoncons::json {
  auto p = DatabasePimpl::pimpl(p_);

  auto r = Request().method(HttpMethod::GET).database(p->name_).endpoint("/query/rules");

  auto response = p->connection_.sendRequest(r);
  if (response.contains({401, 403}))
    throw AuthenticationError();
  else if (not response.isSuccess())
    throw ServerError(response.errorMessage(), response.errorCode());

  return response.body();
}

auto Database::createGraph(input::GraphCreateInput input) -> Graph {
  auto p = DatabasePimpl::pimpl(p_);

  if (input.name.empty()) throw ClientError("createGraph => graph name cannot be empty");

  jsoncons::json data = to_json{
      {"name", input.name},
  };

  jsoncons::json array(jsoncons::json_array_arg);

  for (const auto& orphan : input.orphan_collections.value_or(std::vector<std::string>())) {
    array.push_back(orphan);
  }
  if (!array.empty()) data["orphanCollections"] = array;

  jsoncons::json edges(jsoncons::json_array_arg);
  for (auto edgeDefinition : input.edge_definitions) {
    jsoncons::json edge_definition =
        to_json{{"collection", edgeDefinition.collection}, {"from", edgeDefinition.from}, {"to", edgeDefinition.to}};
    edges.push_back(edge_definition);
  }
  if (!edges.empty()) data["edgeDefinitions"] = edges;

  if (input.is_disjoint) data["isDisjoint"] = input.is_disjoint.value();
  if (input.is_smart) data["isSmart"] = input.is_smart.value();
  if (input.smart_graph_attribute) data["smartGraphAttribute"] = input.smart_graph_attribute.value();
  if (input.satellites) data["satellites"] = input.satellites.value();
  if (input.replication_factor) data["replicationFactor"] = input.replication_factor.value();
  if (input.write_concern) data["writeConcern"] = input.write_concern.value();
  if (input.number_of_shards) data["numberOfShards"] = input.number_of_shards.value();

  std::vector<string_pair> params;
  params.emplace_back("waitForSync", std::to_string(input.wait_for_sync));

  auto r = Request().method(HttpMethod::POST).database(p->name_).endpoint("/gharial").parameters(params).data(data.to_string());

  auto response = p->connection_.sendRequest(r);
  if (response.contains({401, 403}))
    throw AuthenticationError();
  else if (not response.isSuccess() && (response.httpCode() != 409))
    throw ServerError(response.errorMessage(), response.errorCode());

  return {p->connection_, *this, input.name};
}

auto Database::graph(std::string name) -> Graph {
  auto p = DatabasePimpl::pimpl(p_);
  return {p->connection_, *this, std::move(name)};
}

bool Database::deleteDatabase(std::string name, input::DatabaseDeleteInput input) {
  auto p = DatabasePimpl::pimpl(p_);

  Request request = Request().method(HttpMethod::DELETE).handle(std::move(name)).endpoint("/database/{handle}");

  auto response = p->connection_.sendRequest(request);

  if (response.contains({401, 403}))
    throw AuthenticationError();
  if (response.errorCode() == 1228 and input.ignore_missing) return false;
  if (not response.isSuccess())
    throw ServerError(response.errorMessage(), response.errorCode());

  return true;
}

bool Database::deleteCollection(std::string name, input::CollectionDeleteInput input) {
  auto p = DatabasePimpl::pimpl(p_);

  std::vector<string_pair> params;
  if (input.system)
    params.emplace_back("isSystem", std::to_string(input.system.value()));

  Request request = Request()
                        .method(HttpMethod::DELETE)
                        .database(p->name_)
                        .parameters(params)
                        .handle(std::move(name))
                        .endpoint("/collection/{handle}");

  auto response = p->connection_.sendRequest(request);

  if (response.errorCode() == 1202 and input.ignore_missing) return false;
  else if (not response.isSuccess())
    throw ServerError(response.errorMessage(), response.errorCode());

  return true;
}

auto Database::hasCollection(std::string name) -> bool {
  auto p = DatabasePimpl::pimpl(p_);

  Request request = Request().method(HttpMethod::GET).database(p->name_).endpoint("/collection");

  auto response = p->connection_.sendRequest(request);

  if (response.contains({401, 403})) {
    throw AuthenticationError();
  } else if (!response.isSuccess()) {
    throw ServerError(response.errorMessage(), response.errorCode());
  }
  auto body = response.body();
  auto results = body["result"];

  if (results.is_array()) {
    for (const auto& row : results.array_range()) {
      if( row["name"] == name )
        return true;
    }
  }
  return false;
}

auto Database::hasDatabase(std::string name) -> bool{
  auto p = DatabasePimpl::pimpl(p_);

  Request request = Request().method(HttpMethod::GET).endpoint("/database");

  auto response = p->connection_.sendRequest(request);

  if (response.contains({401, 403})) {
    throw AuthenticationError();
  }
  if (!response.isSuccess()) {
    throw ServerError(response.errorMessage(), response.errorCode());
  }

  auto body = response.body();
  auto results = body["result"];

  if (results.is_array()) {
    for (const auto& row : results.array_range())
      if( row == name )
        return true;
  }
  return false;
}

auto Database::info() -> jsoncons::json {
  auto p = DatabasePimpl::pimpl(p_);

  Request request = Request().method(HttpMethod::GET).database(p->name_).endpoint("/database/current");

  auto response = p->connection_.sendRequest(request);

  if (response.contains({401, 403})) {
    throw AuthenticationError();
  }
  if (!response.isSuccess())
    throw ServerError(response.errorMessage(), response.errorCode());
  auto body = response.body();
  return body["result"];
}


}  // namespace arango-cpp

