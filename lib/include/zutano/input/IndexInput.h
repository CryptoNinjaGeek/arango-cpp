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
  std::optional<bool> legacyPolygons{false};
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
  std::optional<bool> cacheEnabled;
  std::optional<std::vector<std::string>> storedValues;
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
  std::optional<std::vector<std::string>> primarySort;
  std::optional<std::vector<std::string>> storedValues;
  std::optional<std::string> analyzer;
  std::optional<std::vector<std::string>> features;
  std::optional<bool> includeAllFields;
  std::optional<bool> trackListPositions;
  std::optional<bool> searchField;
};

} // zutano
