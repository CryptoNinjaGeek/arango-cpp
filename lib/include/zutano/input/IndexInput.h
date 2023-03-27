#pragma once

#include <string>
#include <vector>
#include <optional>

namespace zutano::input {

struct IndexCreateInput {
  std::vector<std::string> fields;
  std::optional<std::string> name;
  std::optional<bool> in_background;
  std::optional<bool> unique;
  std::optional<bool> sparse;
  std::optional<bool> deduplicate;
};

struct GeoIndexCreateInput {
  std::vector<std::string> fields;
  std::optional<std::string> name;
  std::optional<bool> in_background;
  std::optional<bool> ordered;
  std::optional<bool> legacy_polygons{false};
};

struct FulltextIndexCreateInput {
  std::vector<std::string> fields;
  std::optional<std::string> name;
  std::optional<bool> in_background;
  std::optional<int> min_length;
};

struct PersistentIndexCreateInput {
  std::vector<std::string> fields;
  std::optional<std::string> name;
  std::optional<bool> in_background;
  std::optional<bool> unique;
  std::optional<bool> sparse;
  std::optional<bool> cache_enabled;
  std::optional<std::vector<std::string>> stored_values;
};

struct TTLIndexCreateInput {
  std::vector<std::string> fields;
  std::optional<std::string> name;
  std::optional<bool> in_background;
  int expiry_time;
};

struct InvertedIndexCreateInput {
  std::vector<std::string> fields;
  std::optional<std::string> name;
  std::optional<bool> in_background;
  std::optional<int> parallelism;
  std::optional<std::vector<std::string>> primary_sort;
  std::optional<std::vector<std::string>> stored_values;
  std::optional<std::string> analyzer;
  std::optional<std::vector<std::string>> features;
  std::optional<bool> include_all_fields;
  std::optional<bool> track_list_positions;
  std::optional<bool> search_field;
};

} // zutano
