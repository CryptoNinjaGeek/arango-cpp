#pragma once

#include <string>
#include <vector>
#include <optional>

namespace zutano::input {

struct InsertInput {
  bool return_new{false};
  std::optional<bool> sync;
  bool silent{false};
  bool overwrite{false};
  bool return_old{false};
  std::optional<std::string> overwrite_mode;
  std::optional<bool> keep_none;
  std::optional<bool> merge;
};

struct DeleteInput {
  std::optional<std::string> rev;
  bool check_rev{true};
  bool ignore_missing{false};
  bool return_old{false};
  std::optional<bool> sync;
  bool silent{false};
};

struct GetInput {
  std::optional<std::string> rev;
  bool check_rev{true};
  bool allow_dirty_read{false};
};

struct UpdateInput {
  bool check_rev{true};
  bool merge{true};
  bool keep_none{true};
  bool return_new{false};
  bool return_old{false};
  std::optional<bool> sync;
  bool silent{false};
};

struct ReplaceInput {
  bool check_rev{true};
  bool return_new{false};
  bool return_old{false};
  std::optional<bool> sync;
  bool silent{false};
};

} // zutano

