#pragma once

#include <string>
#include <zutano/PrivateImpl.h>
#include <zutano/Index.h>

#include <nlohmann/json.hpp>

namespace zutano {

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

class Collection;
class Connection;
class Database;

[[maybe_unused]] typedef std::shared_ptr<Collection> CollectionPtr;

class Collection {
 public:
  Collection(Connection, Database, std::string);

  ~Collection() = default;

  auto Insert(nlohmann::json, InsertInput = {}) -> nlohmann::json;

  auto AddHashIndex(HashIndexCreateInput) -> nlohmann::json;

 protected:
  auto AddIndex(nlohmann::json) -> nlohmann::json;

 private:
  PrivateImplPtr p_;
};

} // zutano

