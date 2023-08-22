#pragma once

#include <string>
#include <arango-cpp/PrivateImpl.h>
#include <arango-cpp/input/IndexInput.h>
#include <arango-cpp/input/DocumentInput.h>

#include <jsoncons/json.hpp>

namespace arangocpp {

class Collection;
class Connection;
class Database;

class Collection {
 public:
  Collection(const Connection&, const Database&, std::string);

  ~Collection() = default;

  auto truncate() -> bool;
  auto head(jsoncons::json) -> jsoncons::json;

  // Document modifications
  auto insert(const jsoncons::json& doc, input::InsertInput) -> jsoncons::json;
  auto remove(const jsoncons::json&, input::DeleteInput = {}) -> bool;
  auto update(const jsoncons::json& doc, input::UpdateInput) -> jsoncons::json;
  auto replace(const jsoncons::json& doc, input::ReplaceInput input = {}) -> jsoncons::json;
  auto get(const jsoncons::json& doc, const input::GetInput&) -> jsoncons::json;

  // Indexes
  auto addHashIndex(input::IndexCreateInput) -> jsoncons::json;
  auto addSkiplistIndex(input::IndexCreateInput) -> jsoncons::json;
  auto addGeoIndex(input::GeoIndexCreateInput) -> jsoncons::json;
  auto addFulltextIndex(input::FulltextIndexCreateInput) -> jsoncons::json;
  auto addPersistentIndex(input::PersistentIndexCreateInput) -> jsoncons::json;
  auto addTTLIndex(input::TTLIndexCreateInput) -> jsoncons::json;
  auto addInvertedIndex(input::InvertedIndexCreateInput) -> jsoncons::json;
  auto deleteIndex(const std::string&, bool ignore_missing = false) -> bool;
  auto loadIndexesIntoMemory() -> bool;

 protected:
  auto addIndex(const jsoncons::json&) -> jsoncons::json;
  static auto getHandleFromDocument(jsoncons::json) -> std::string;

 private:
  private_impl_ptr p_;
};

}  // namespace arangocpp
