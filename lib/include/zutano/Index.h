#pragma once

#include <string>
#include <vector>
#include <optional>

struct HashIndexCreateInput {
  std::vector<std::string> fields;
  std::optional<bool> unique;
  std::optional<bool> sparse;
  std::optional<bool> deduplicate;
  std::optional<std::string> name;
  std::optional<bool> in_background;
};
