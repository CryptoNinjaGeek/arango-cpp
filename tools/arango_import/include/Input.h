#pragma once

#include <string>

namespace arango_import::input {

enum class FileType { UNKNOWN, CSV, JSON, XLSX };

struct ImportInput {
  std::string user_name{"root"};
  std::string database{"demo"};
  std::string name{"demo"};
  std::string type{"collection"};
  std::string password;
  std::vector<std::string> hosts{"localhost"};

  long batch_size{1000};
  bool create{false};
  bool progress{true};
  bool overwrite{false};
  bool truncate{false};
  bool no_sync{false};
  bool verbose{false};
  bool name_from_file{false};

  std::vector<std::string> files;
};

}  // namespace arango_import::input