#pragma once

#include <string>
#include <vector>
#include <map>
#include <optional>

namespace arangocpp::input {

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
  bool allow_conflict{true};
};

struct edgeDefinitions {
  std::string collection;
  std::vector<std::string> from;
  std::vector<std::string> to;
};

struct GraphCreateInput {
  std::string name;
  bool wait_for_sync{true};
  std::optional<bool> is_smart;
  std::optional<bool> is_disjoint;
  std::optional<std::string> smart_graph_attribute;
  std::optional<std::string> satellites;
  std::optional<int> number_of_shards;
  std::optional<int> replication_factor;
  std::optional<int> write_concern;
  std::optional<std::vector<std::string>> orphan_collections;
  std::vector<edgeDefinitions> edge_definitions;
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
  std::optional<jsoncons::json> computed_values;
  bool allow_conflict{true};
};

struct ExplainInput {
  std::string query;
  bool all_plans{false};
  std::optional<int> max_plans;
  std::optional<std::vector<std::string>> opt_rules;
  std::optional<std::map<std::string, std::string>> bind_vars;
};

struct ExecuteInput {
  std::string query;
  bool count{false};
  std::optional<int> batch_size;
  std::optional<int> ttl;
  std::optional<std::map<std::string, std::string>> bind_vars;
  std::optional<bool> full_count;
  std::optional<int> max_plans;
  std::optional<std::vector<std::string>> optimizer_rules;
  std::optional<bool> cache;
  int memory_limit{0};
  std::optional<bool> fail_on_warning;
  std::optional<bool> profile;
  std::optional<int> max_transaction_size;
  std::optional<int> max_warning_count;
  std::optional<int> intermediate_commit_count;
  std::optional<int> intermediate_commit_size;
  std::optional<int> satellite_sync_wait;
  std::optional<bool> stream;
  std::optional<bool> skip_inaccessible_cols;
  std::optional<double> max_runtime;
  std::optional<bool> fill_block_cache;
  bool allow_dirty_read{false};
};

struct TrackingInput {
  std::optional<bool> enabled;
  std::optional<int> max_slow_queries;
  std::optional<int> slow_query_threshold;
  std::optional<int> max_query_string_length;
  std::optional<bool> track_bind_vars;
  std::optional<bool> track_slow_queries;
};

struct CreateFunctionInput {
  std::string name;
  std::string code;
};

struct DeleteFunctionInput {
  std::string name;
  bool group{false};
  bool ignore_missing{false};
};

struct DatabaseDeleteInput {
  bool ignore_missing{false};
};

struct CollectionDeleteInput {
  bool ignore_missing{false};
  std::optional<bool> system;
};
} // arango-cpp
