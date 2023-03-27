//
// Created by Carsten Tang on 19/03/2023.
//

#include <zutano/Collection.h>
#include <zutano/Connection.h>
#include <zutano/Exceptions.h>
#include <zutano/Tools.h>

#include <zutano/private/CollectionPrivate.h>

namespace zutano {

auto Collection::AddHashIndex(input::IndexCreateInput input) -> jsoncons::json {
  auto p = pimp::CollectionPimpl::Pimpl(p_);

  jsoncons::json data = tools::to_json{
	  {"type", "hash"},
	  {"fields", input.fields},
  };

  if (input.unique)
	data["unique"] = input.unique.value();
  if (input.sparse)
	data["sparse"] = input.sparse.value();
  if (input.deduplicate)
	data["deduplicate"] = input.deduplicate.value();
  if (input.name)
	data["name"] = input.name.value();
  if (input.in_background)
	data["inBackground"] = input.in_background.value();

  return AddIndex(data);
}

auto Collection::AddSkiplistIndex(input::IndexCreateInput input) -> jsoncons::json {
  auto p = pimp::CollectionPimpl::Pimpl(p_);

  jsoncons::json data = tools::to_json{
	  {"type", "skiplist"},
	  {"fields", input.fields},
  };

  if (input.unique)
	data["unique"] = input.unique.value();
  if (input.sparse)
	data["sparse"] = input.sparse.value();
  if (input.deduplicate)
	data["deduplicate"] = input.deduplicate.value();
  if (input.name)
	data["name"] = input.name.value();
  if (input.in_background)
	data["inBackground"] = input.in_background.value();

  return AddIndex(data);
}

auto Collection::AddGeoIndex(input::GeoIndexCreateInput input) -> jsoncons::json {
  auto p = pimp::CollectionPimpl::Pimpl(p_);

  jsoncons::json data = tools::to_json{
	  {"type", "geo"},
	  {"fields", input.fields},
  };

  if (input.ordered)
	data["geoJson"] = input.ordered.value();
  if (input.legacy_polygons)
	data["legacyPolygons"] = input.legacy_polygons.value();
  if (input.name)
	data["name"] = input.name.value();
  if (input.in_background)
	data["inBackground"] = input.in_background.value();

  return AddIndex(data);
}

auto Collection::AddFulltextIndex(input::FulltextIndexCreateInput input) -> jsoncons::json {
  auto p = pimp::CollectionPimpl::Pimpl(p_);

  jsoncons::json data = tools::to_json{
	  {"type", "fulltext"},
	  {"fields", input.fields},
  };

  if (input.min_length)
	data["minLength"] = input.min_length.value();
  if (input.name)
	data["name"] = input.name.value();
  if (input.in_background)
	data["inBackground"] = input.in_background.value();

  return AddIndex(data);
}

auto Collection::AddPersistentIndex(input::PersistentIndexCreateInput input) -> jsoncons::json {
  auto p = pimp::CollectionPimpl::Pimpl(p_);

  jsoncons::json data = tools::to_json{
	  {"type", "persistent"},
	  {"fields", input.fields},
  };

  if (input.unique)
	data["unique"] = input.unique.value();
  if (input.sparse)
	data["sparse"] = input.sparse.value();
  if (input.stored_values)
	data["storedValues"] = input.stored_values.value();
  if (input.cache_enabled)
	data["cacheEnabled"] = input.cache_enabled.value();
  if (input.name)
	data["name"] = input.name.value();
  if (input.in_background)
	data["inBackground"] = input.in_background.value();

  return AddIndex(data);
}

auto Collection::AddTTLIndex(input::TTLIndexCreateInput input) -> jsoncons::json {
  auto p = pimp::CollectionPimpl::Pimpl(p_);

  jsoncons::json data = tools::to_json{
	  {"type", "ttl"},
	  {"fields", input.fields},
	  {"expireAfter", input.expiry_time},
  };

  if (input.name)
	data["name"] = input.name.value();
  if (input.in_background)
	data["inBackground"] = input.in_background.value();

  return AddIndex(data);
}

auto Collection::AddInvertedIndex(input::InvertedIndexCreateInput input) -> jsoncons::json {
  auto p = pimp::CollectionPimpl::Pimpl(p_);

  jsoncons::json data = tools::to_json{
	  {"type", "inverted"},
	  {"fields", input.fields},
  };

  if (input.parallelism)
	data["parallelism"] = input.parallelism.value();
  if (input.primary_sort)
	data["primarySort"] = input.primary_sort.value();
  if (input.stored_values)
	data["storedValues"] = input.stored_values.value();
  if (input.analyzer)
	data["analyzer"] = input.analyzer.value();
  if (input.features)
	data["features"] = input.features.value();
  if (input.include_all_fields)
	data["includeAllFields"] = input.include_all_fields.value();
  if (input.track_list_positions)
	data["trackListPositions"] = input.track_list_positions.value();
  if (input.search_field)
	data["searchField"] = input.search_field.value();
  if (input.name)
	data["name"] = input.name.value();
  if (input.in_background)
	data["inBackground"] = input.in_background.value();

  return AddIndex(data);
}

auto Collection::DeleteIndex(const std::string &index_name, bool ignore_missing) -> bool {
  auto p = pimp::CollectionPimpl::Pimpl(p_);
  auto r = Request()
	  .Method(HttpMethod::DELETE)
	  .Database(p->db_.name())
	  .Endpoint(std::string("/index/") + p->name_ + "/" + index_name);

  auto response = p->connection_.SendRequest(r);
  if (response.http_code()==1212 and ignore_missing)
	return false;
  else if (not response.is_success())
	throw ServerError(response.error_message(), response.error_code());

  return true;
}

auto Collection::LoadIndexesIntoMemory() -> bool {
  auto p = pimp::CollectionPimpl::Pimpl(p_);
  auto r = Request()
	  .Method(HttpMethod::PUT)
	  .Database(p->db_.name())
	  .Collection(p->name_)
	  .Endpoint("loadIndexesIntoMemory");

  auto response = p->connection_.SendRequest(r);
  if (not response.is_success())
	throw ServerError(response.error_message(), response.error_code());

  return true;
}

auto Collection::AddIndex(const jsoncons::json &data) -> jsoncons::json {
  auto p = pimp::CollectionPimpl::Pimpl(p_);
  auto r = Request()
	  .Method(HttpMethod::POST)
	  .Database(p->db_.name())
	  .Parameters({StringPair("collection", p->name_)})
	  .Endpoint("/index")
	  .Data(data.to_string());

  auto response = p->connection_.SendRequest(r);
  if (response.contains({401, 403}))
	throw AuthenticationError();
  else if (not response.is_success())
	throw ServerError(response.error_message(), response.error_code());

  return response.body();
}

} // zutano