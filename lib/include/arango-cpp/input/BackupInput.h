#pragma once

#include <string>
#include <vector>
#include <optional>

namespace arangocpp::input {

struct BackupInput {
  std::optional<std::string> backupId{};
  std::optional<std::string> repository{};
  std::optional<bool> abort{};
  std::optional<jsoncons::json> config{};
  std::optional<std::string> downloadId{};
};

} // arango-cpp

