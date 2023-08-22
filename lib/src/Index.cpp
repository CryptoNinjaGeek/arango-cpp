//
// Created by Carsten Tang on 19/03/2023.
//

#include <arango-cpp/Collection.h>
#include <arango-cpp/Connection.h>
#include <arango-cpp/Exceptions.h>
#include <arango-cpp/Tools.h>

#include <arango-cpp/private/CollectionPrivate.h>

namespace arangocpp {

auto Collection::addHashIndex(input::IndexCreateInput input) -> jsoncons::json {
  auto p = pimp::CollectionPimpl::pimpl(p_);

  jsoncons::json data = tools::to_json{
      {"type", "hash"},
      {"fields", input.fields},
  };

  if (input.unique) data["unique"] = input.unique.value();
  if (input.sparse) data["sparse"] = input.sparse.value();
  if (input.deduplicate) data["deduplicate"] = input.deduplicate.value();
  if (input.name) data["name"] = input.name.value();
  if (input.in_background) data["inBackground"] = input.in_background.value();

  return addIndex(data);
}

auto Collection::addSkiplistIndex(input::IndexCreateInput input) -> jsoncons::json {
  auto p = pimp::CollectionPimpl::pimpl(p_);

  jsoncons::json data = tools::to_json{
      {"type", "skiplist"},
      {"fields", input.fields},
  };

  if (input.unique) data["unique"] = input.unique.value();
  if (input.sparse) data["sparse"] = input.sparse.value();
  if (input.deduplicate) data["deduplicate"] = input.deduplicate.value();
  if (input.name) data["name"] = input.name.value();
  if (input.in_background) data["inBackground"] = input.in_background.value();

  return addIndex(data);
}

auto Collection::addGeoIndex(input::GeoIndexCreateInput input) -> jsoncons::json {
  auto p = pimp::CollectionPimpl::pimpl(p_);

  jsoncons::json data = tools::to_json{
      {"type", "geo"},
      {"fields", input.fields},
  };

  if (input.ordered) data["geoJson"] = input.ordered.value();
  if (input.legacy_polygons) data["legacyPolygons"] = input.legacy_polygons.value();
  if (input.name) data["name"] = input.name.value();
  if (input.in_background) data["inBackground"] = input.in_background.value();

  return addIndex(data);
}

auto Collection::addFulltextIndex(input::FulltextIndexCreateInput input) -> jsoncons::json {
  auto p = pimp::CollectionPimpl::pimpl(p_);

  jsoncons::json data = tools::to_json{
      {"type", "fulltext"},
      {"fields", input.fields},
  };

  if (input.min_length) data["minLength"] = input.min_length.value();
  if (input.name) data["name"] = input.name.value();
  if (input.in_background) data["inBackground"] = input.in_background.value();

  return addIndex(data);
}

auto Collection::addPersistentIndex(input::PersistentIndexCreateInput input) -> jsoncons::json {
  auto p = pimp::CollectionPimpl::pimpl(p_);

  jsoncons::json data = tools::to_json{
      {"type", "persistent"},
      {"fields", input.fields},
  };

  if (input.unique) data["unique"] = input.unique.value();
  if (input.sparse) data["sparse"] = input.sparse.value();
  if (input.stored_values) data["storedValues"] = input.stored_values.value();
  if (input.cache_enabled) data["cacheEnabled"] = input.cache_enabled.value();
  if (input.name) data["name"] = input.name.value();
  if (input.in_background) data["inBackground"] = input.in_background.value();

  return addIndex(data);
}

auto Collection::addTTLIndex(input::TTLIndexCreateInput input) -> jsoncons::json {
  auto p = pimp::CollectionPimpl::pimpl(p_);

  jsoncons::json data = tools::to_json{
      {"type", "ttl"},
      {"fields", input.fields},
      {"expireAfter", input.expiry_time},
  };

  if (input.name) data["name"] = input.name.value();
  if (input.in_background) data["inBackground"] = input.in_background.value();

  return addIndex(data);
}

auto Collection::addInvertedIndex(input::InvertedIndexCreateInput input) -> jsoncons::json {
  auto p = pimp::CollectionPimpl::pimpl(p_);

  jsoncons::json data = tools::to_json{
      {"type", "inverted"},
      {"fields", input.fields},
  };

  if (input.parallelism) data["parallelism"] = input.parallelism.value();
  if (input.primary_sort) data["primarySort"] = input.primary_sort.value();
  if (input.stored_values) data["storedValues"] = input.stored_values.value();
  if (input.analyzer) data["analyzer"] = input.analyzer.value();
  if (input.features) data["features"] = input.features.value();
  if (input.include_all_fields) data["includeAllFields"] = input.include_all_fields.value();
  if (input.track_list_positions) data["trackListPositions"] = input.track_list_positions.value();
  if (input.search_field) data["searchField"] = input.search_field.value();
  if (input.name) data["name"] = input.name.value();
  if (input.in_background) data["inBackground"] = input.in_background.value();

  return addIndex(data);
}

auto Collection::deleteIndex(const std::string& index_name, bool ignore_missing) -> bool {
  auto p = pimp::CollectionPimpl::pimpl(p_);
  auto r = Request()
               .method(HttpMethod::DELETE)
               .database(p->db_.name())
               .endpoint(std::string("/index/") + p->name_ + "/" + index_name);

  auto response = p->connection_.sendRequest(r);
  if (response.httpCode() == 1212 and ignore_missing)
    return false;
  else if (not response.isSuccess())
    throw ServerError(response.errorMessage(), response.errorCode());

  return true;
}

auto Collection::loadIndexesIntoMemory() -> bool {
  auto p = pimp::CollectionPimpl::pimpl(p_);
  auto r =
      Request().method(HttpMethod::PUT).database(p->db_.name()).collection(p->name_).endpoint("loadIndexesIntoMemory");

  auto response = p->connection_.sendRequest(r);
  if (not response.isSuccess()) throw ServerError(response.errorMessage(), response.errorCode());

  return true;
}

auto Collection::addIndex(const jsoncons::json& data) -> jsoncons::json {
  auto p = pimp::CollectionPimpl::pimpl(p_);
  auto r = Request()
               .method(HttpMethod::POST)
               .database(p->db_.name())
               .parameters({string_pair("collection", p->name_)})
               .endpoint("/index")
               .data(data.to_string());

  auto response = p->connection_.sendRequest(r);
  if (response.contains({401, 403}))
    throw AuthenticationError();
  else if (not response.isSuccess())
    throw ServerError(response.errorMessage(), response.errorCode());

  return response.body();
}

}  // namespace arango-cpp