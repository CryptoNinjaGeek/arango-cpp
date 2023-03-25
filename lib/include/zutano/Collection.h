#pragma once

#include <string>
#include <zutano/PrivateImpl.h>
#include <zutano/input/IndexInput.h>
#include <zutano/input/DocumentInput.h>

#include <jsoncons/json.hpp>

namespace zutano {

class Collection;
class Connection;
class Database;

[[maybe_unused]] typedef std::shared_ptr<Collection> CollectionPtr;

class Collection {
 public:
  Collection(const Connection &, const Database &, std::string);

  ~Collection() = default;

  auto Truncate() -> bool;
  auto Head(jsoncons::json) -> jsoncons::json;

  // Document modifications
  auto Insert(const jsoncons::json &, input::InsertInput = {}) -> jsoncons::json;
  auto Delete(const jsoncons::json &, input::DeleteInput = {}) -> bool;
  auto Update(const jsoncons::json &, input::UpdateInput = {}) -> jsoncons::json;
  auto Replace(const jsoncons::json &, input::ReplaceInput = {}) -> jsoncons::json;
  auto Get(const jsoncons::json &, const input::GetInput &) -> jsoncons::json;

  // Indexes
  auto AddHashIndex(input::IndexCreateInput) -> jsoncons::json;
  auto AddSkiplistIndex(input::IndexCreateInput) -> jsoncons::json;
  auto AddGeoIndex(input::GeoIndexCreateInput) -> jsoncons::json;
  auto AddFulltextIndex(input::FulltextIndexCreateInput) -> jsoncons::json;
  auto AddPersistentIndex(input::PersistentIndexCreateInput) -> jsoncons::json;
  auto AddTTLIndex(input::TTLIndexCreateInput) -> jsoncons::json;
  auto AddInvertedIndex(input::InvertedIndexCreateInput) -> jsoncons::json;
  auto DeleteIndex(const std::string &, bool ignore_missing = false) -> bool;
  auto LoadIndexesIntoMemory() -> bool;

 protected:
  auto AddIndex(const jsoncons::json &) -> jsoncons::json;
  static auto GetHandleFromDocument(jsoncons::json) -> std::string;

 private:
  PrivateImplPtr p_;
};

} // zutano

