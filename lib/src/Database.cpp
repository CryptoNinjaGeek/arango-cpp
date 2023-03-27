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

auto Database::CreateDatabase(input::DatabaseCreateInput input) -> Database {
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
  else if (not response.is_success() && (response.http_code()!=409 || not input.allow_conflict))
	throw ServerError(response.error_message(), response.error_code());

  return {p->connection_, input.name};
}

auto Database::CreateCollection(input::CollectionCreateInput input) -> zutano::Collection {
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
  if (input.computed_values)
	data["computedValues"] = input.computed_values.value();

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
  else if (not response.is_success() && (response.http_code()!=409 || not input.allow_conflict))
	throw ServerError(response.error_message(), response.error_code());

  return {p->connection_, *this, input.name};
}

auto Database::Collection(std::string name) -> zutano::Collection {
  auto p = DatabasePimpl::Pimpl(p_);
  return {p->connection_, *this, std::move(name)};
}

auto Database::Execute(input::ExecuteInput input) -> Cursor {
  auto p = DatabasePimpl::Pimpl(p_);

  json data = to_json{{"query", input.query}, {"count", input.count}};

  if (input.batch_size)
	data["batchSize"] = input.batch_size.value();
  if (input.ttl)
	data["ttl"] = input.ttl.value();
  if (input.bind_vars)
	data["bindVars"] = input.bind_vars.value();
  if (input.cache)
	data["cache"] = tools::to_string(input.cache.value());
  if (input.memory_limit)
	data["memoryLimit"] = input.memory_limit;

  json options;
  if (input.full_count)
	options["fullCount"] = tools::to_string(input.full_count.value());
  if (input.fill_block_cache)
	options["fillBlockCache"] = tools::to_string(input.fill_block_cache.value());
  if (input.max_plans)
	options["maxNumberOfPlans"] = input.max_plans.value();
  if (input.optimizer_rules)
	options["optimizer"] = json{{"rules", input.optimizer_rules.value()}};
  if (input.fail_on_warning)
	options["failOnWarning"] = tools::to_string(input.fail_on_warning.value());
  if (input.profile)
	options["profile"] = tools::to_string(input.profile.value());
  if (input.max_transaction_size)
	options["maxTransactionSize"] = input.max_transaction_size.value();
  if (input.max_warning_count)
	options["maxWarningCount"] = input.max_warning_count.value();
  if (input.intermediate_commit_count)
	options["intermediateCommitCount"] = input.intermediate_commit_count.value();
  if (input.intermediate_commit_size)
	options["intermediateCommitSize"] = input.intermediate_commit_size.value();
  if (input.satellite_sync_wait)
	options["satelliteSyncWait"] = input.satellite_sync_wait.value();
  if (input.stream)
	options["stream"] = tools::to_string(input.stream.value());
  if (input.skip_inaccessible_cols)
	options["skipInaccessibleCollections"] = tools::to_string(input.skip_inaccessible_cols.value());
  if (input.max_runtime)
	options["maxRuntime"] = input.max_runtime.value();

  if (not options.empty())
	data["options"] = options;

  auto r = Request()
	  .Method(HttpMethod::POST)
	  .Database(p->name_)
	  .Data(data.to_string())
	  .Endpoint("/cursor");

  if (input.allow_dirty_read)
	r.Headers({{"x-arango-allow-dirty-read", "true"}});

  auto response = p->connection_.SendRequest(r);
  if (response.contains({401, 403}))
	throw AuthenticationError();
  else if (not response.is_success())
	throw ServerError(response.error_message(), response.error_code());

  return Cursor(p->name_, p->connection_, response.body());
}

auto Database::name() -> std::string {
  auto p = DatabasePimpl::Pimpl(p_);
  return p->name_;
}

auto Database::Explain(input::ExplainInput input) -> jsoncons::json {
  auto p = DatabasePimpl::Pimpl(p_);

  json options = to_json{{"allPlans", input.all_plans}};
  json data = to_json{{"query", input.query}};

  if (input.max_plans)
	options["maxNumberOfPlans"] = input.max_plans;
  if (input.opt_rules)
	options["optimizer"] = to_json{{"rules", input.opt_rules.value()}};

  if (input.bind_vars)
	data["bindVars"] = input.bind_vars.value();

  data["options"] = options;

  auto r = Request()
	  .Method(HttpMethod::POST)
	  .Database(p->name_)
	  .Data(data.to_string())
	  .Endpoint("/explain");

  auto response = p->connection_.SendRequest(r);
  if (response.contains({401, 403}))
	throw AuthenticationError();
  else if (not response.is_success())
	throw ServerError(response.error_message(), response.error_code());

  auto body = response.body();

  if (body.contains("plan"))
	return body["plan"];
  else if (body.contains("plans"))
	return body["plans"];
  return response.body();
}

auto Database::Validate(std::string query) -> jsoncons::json {
  auto p = DatabasePimpl::Pimpl(p_);

  json data = to_json{{"query", query}};

  auto r = Request()
	  .Method(HttpMethod::POST)
	  .Database(p->name_)
	  .Data(data.to_string())
	  .Endpoint("/query");

  auto response = p->connection_.SendRequest(r);
  if (response.contains({401, 403}))
	throw AuthenticationError();
  else if (not response.is_success())
	throw ServerError(response.error_message(), response.error_code());

  return response.is_success();
}

auto Database::Kill(std::string query_id) -> bool {
  auto p = DatabasePimpl::Pimpl(p_);

  auto r = Request()
	  .Method(HttpMethod::DELETE)
	  .Database(p->name_)
	  .Handle(std::move(query_id))
	  .Endpoint("/query/{handle}");

  auto response = p->connection_.SendRequest(r);
  if (response.contains({401, 403}))
	throw AuthenticationError();
  else if (not response.is_success())
	throw ServerError(response.error_message(), response.error_code());

  return response.is_success();
}

auto Database::Queries() -> jsoncons::json {
  auto p = DatabasePimpl::Pimpl(p_);

  auto r = Request()
	  .Method(HttpMethod::GET)
	  .Database(p->name_)
	  .Endpoint("/query/current");

  auto response = p->connection_.SendRequest(r);
  if (response.contains({401, 403}))
	throw AuthenticationError();
  else if (not response.is_success())
	throw ServerError(response.error_message(), response.error_code());

  return response.body();
}

auto Database::SlowQueries() -> jsoncons::json {
  auto p = DatabasePimpl::Pimpl(p_);

  auto r = Request()
	  .Method(HttpMethod::GET)
	  .Database(p->name_)
	  .Endpoint("/query/slow");

  auto response = p->connection_.SendRequest(r);
  if (response.contains({401, 403}))
	throw AuthenticationError();
  else if (not response.is_success())
	throw ServerError(response.error_message(), response.error_code());

  return response.body();
}

auto Database::ClearSlowQueries() -> bool {
  auto p = DatabasePimpl::Pimpl(p_);

  auto r = Request()
	  .Method(HttpMethod::DELETE)
	  .Database(p->name_)
	  .Endpoint("/query/slow");

  auto response = p->connection_.SendRequest(r);
  if (response.contains({401, 403}))
	throw AuthenticationError();
  else if (not response.is_success())
	throw ServerError(response.error_message(), response.error_code());

  return response.is_success();
}

auto Database::Tracking() -> jsoncons::json {
  auto p = DatabasePimpl::Pimpl(p_);

  auto r = Request()
	  .Method(HttpMethod::GET)
	  .Database(p->name_)
	  .Endpoint("/query/properties");

  auto response = p->connection_.SendRequest(r);
  if (response.contains({401, 403}))
	throw AuthenticationError();
  else if (not response.is_success())
	throw ServerError(response.error_message(), response.error_code());

  return response.body();
}

auto Database::SetTracking(input::TrackingInput input) -> jsoncons::json {
  auto p = DatabasePimpl::Pimpl(p_);

  json data;
  if (input.enabled)
	data["enabled"] = tools::to_string(input.enabled.value());
  if (input.max_slow_queries)
	data["maxSlowQueries"] = input.max_slow_queries.value();
  if (input.max_query_string_length)
	data["maxQueryStringLength"] = input.max_query_string_length.value();
  if (input.slow_query_threshold)
	data["slowQueryThreshold"] = input.slow_query_threshold.value();
  if (input.track_bind_vars)
	data["trackBindVars"] = tools::to_string(input.track_bind_vars.value());
  if (input.track_slow_queries)
	data["trackSlowQueries"] = tools::to_string(input.track_slow_queries.value());

  auto r = Request()
	  .Method(HttpMethod::PUT)
	  .Database(p->name_)
	  .Data(data.to_string())
	  .Endpoint("/query/properties");

  auto response = p->connection_.SendRequest(r);
  if (response.contains({401, 403}))
	throw AuthenticationError();
  else if (not response.is_success())
	throw ServerError(response.error_message(), response.error_code());

  return response.body();
}

auto Database::Functions() -> jsoncons::json {
  auto p = DatabasePimpl::Pimpl(p_);

  auto r = Request()
	  .Method(HttpMethod::GET)
	  .Database(p->name_)
	  .Endpoint("/aqlfunction");

  auto response = p->connection_.SendRequest(r);
  if (response.contains({401, 403}))
	throw AuthenticationError();
  else if (not response.is_success())
	throw ServerError(response.error_message(), response.error_code());

  return response.body();
}

auto Database::CreateFunction(const input::CreateFunctionInput &input) -> jsoncons::json {
  auto p = DatabasePimpl::Pimpl(p_);

  json data = {{"name", input.name, "code", input.code}};

  auto r = Request()
	  .Method(HttpMethod::POST)
	  .Database(p->name_)
	  .Data(data.to_string())
	  .Endpoint("/aqlfunction");

  auto response = p->connection_.SendRequest(r);
  if (response.contains({401, 403}))
	throw AuthenticationError();
  else if (not response.is_success())
	throw ServerError(response.error_message(), response.error_code());

  auto body = response.body();

  return {{"is_new", body["isNewlyCreated"]}};
}

auto Database::DeleteFunction(const input::DeleteFunctionInput &input) -> jsoncons::json {
  auto p = DatabasePimpl::Pimpl(p_);
  std::vector<StringPair> params;

  params.emplace_back("group", tools::to_string(input.group));

  auto r = Request()
	  .Method(HttpMethod::DELETE)
	  .Database(p->name_)
	  .Handle(input.name)
	  .Parameters(params)
	  .Endpoint("/aqlfunction/{handle}");

  auto response = p->connection_.SendRequest(r);
  if (response.contains({401, 403}))
	throw AuthenticationError();
  else if (response.error_code()==1582 and input.ignore_missing)
	return {};
  else if (not response.is_success())
	throw ServerError(response.error_message(), response.error_code());

  auto body = response.body();

  return to_json{{"deleted", body["deletedCount"].as<std::string>()}};
}

auto Database::QueryRules() -> jsoncons::json {
  auto p = DatabasePimpl::Pimpl(p_);

  auto r = Request()
	  .Method(HttpMethod::GET)
	  .Database(p->name_)
	  .Endpoint("/query/rules");

  auto response = p->connection_.SendRequest(r);
  if (response.contains({401, 403}))
	throw AuthenticationError();
  else if (not response.is_success())
	throw ServerError(response.error_message(), response.error_code());

  return response.body();
}

} // zutano