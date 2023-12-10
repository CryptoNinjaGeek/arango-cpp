#pragma once

#include <string>
#include <arango-cpp/PrivateImpl.h>
#include <arango-cpp/Types.h>
#include <arango-cpp/Connection.h>
#include <arango-cpp/input/BackupInput.h>

#include <vector>
#include <jsoncons/json.hpp>

namespace arangocpp {
class Database;
class Connection;

class Backup {
public:
  Backup(const Connection&);
  Backup();

  auto get(std::optional<std::string> backupId = {}) const -> jsoncons::json;
  auto remove(const std::string&) const -> bool;
  auto download(input::BackupInput = {}) const -> jsoncons::json;
  auto upload(input::BackupInput = {}) const -> jsoncons::json;
  auto restore(const std::string& backupId) const -> jsoncons::json;;

private:
  private_impl_ptr p_;
};

}  // namespace arangocpp
