#pragma once

#include <string>
#include <zutano/PrivateImpl.h>
#include <zutano/input/IndexInput.h>
#include <zutano/input/DocumentInput.h>

#include <nlohmann/json.hpp>

namespace zutano {

class Collection;
class Connection;
class Database;

[[maybe_unused]] typedef std::shared_ptr<Collection> CollectionPtr;

class Collection {
 public:
  Collection(Connection, Database, std::string);

  ~Collection() = default;

  auto Truncate() -> bool;
  auto Head(nlohmann::json) -> nlohmann::json;

  // Document modifications
  auto Insert(nlohmann::json, input::InsertInput = {}) -> nlohmann::json;
  auto Delete(nlohmann::json, input::DeleteInput = {}) -> bool;
  auto Update(nlohmann::json, input::UpdateInput = {}) -> nlohmann::json;
  auto Replace(nlohmann::json, input::ReplaceInput = {}) -> nlohmann::json;
  auto Get(nlohmann::json, input::GetInput) -> nlohmann::json;

  // Indexes
  auto AddHashIndex(input::IndexCreateInput) -> nlohmann::json;
  auto AddSkiplistIndex(input::IndexCreateInput) -> nlohmann::json;
  auto AddGeoIndex(input::GeoIndexCreateInput) -> nlohmann::json;
  auto AddFulltextIndex(input::FulltextIndexCreateInput) -> nlohmann::json;
  auto AddPersistantIndex(input::PersistantIndexCreateInput) -> nlohmann::json;
  auto AddTTLIndex(input::TTLIndexCreateInput) -> nlohmann::json;
  auto AddInvertedIndex(input::InvertedIndexCreateInput) -> nlohmann::json;
  auto DeleteIndex(std::string, bool ignore_missing = false) -> bool;
  auto LoadIndexesIntoMemory() -> bool;

 protected:
  auto AddIndex(nlohmann::json) -> nlohmann::json;
  auto GetHandleFromDocument(nlohmann::json) -> std::string;

 private:
  PrivateImplPtr p_;
};

} // zutano

