//
// Created by Carsten Tang on 19/03/2023.
//

#include <zutano/Collection.h>
#include <zutano/Connection.h>
#include <zutano/Exceptions.h>
#include <zutano/Tools.h>

#include <zutano/private/CollectionPrivate.h>

namespace zutano {

auto Collection::AddHashIndex(input::IndexCreateInput input) -> nlohmann::json {
  auto p = pimp::CollectionPimpl::Pimpl(p_);

  nlohmann::json data = {
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

auto Collection::AddSkiplistIndex(input::IndexCreateInput input) -> nlohmann::json {
  auto p = pimp::CollectionPimpl::Pimpl(p_);

  nlohmann::json data = {
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

auto Collection::AddGeoIndex(input::GeoIndexCreateInput input) -> nlohmann::json {
  auto p = pimp::CollectionPimpl::Pimpl(p_);

  nlohmann::json data = {
	  {"type", "geo"},
	  {"fields", input.fields},
  };

  if (input.ordered)
	data["geoJson"] = input.ordered.value();
  if (input.legacyPolygons)
	data["legacyPolygons"] = input.legacyPolygons.value();
  if (input.name)
	data["name"] = input.name.value();
  if (input.in_background)
	data["inBackground"] = input.in_background.value();

  return AddIndex(data);
}

auto Collection::AddFulltextIndex(input::FulltextIndexCreateInput input) -> nlohmann::json {
  auto p = pimp::CollectionPimpl::Pimpl(p_);

  nlohmann::json data = {
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

auto Collection::AddPersistantIndex(input::PersistantIndexCreateInput input) -> nlohmann::json {
  auto p = pimp::CollectionPimpl::Pimpl(p_);

  nlohmann::json data = {
	  {"type", "persistent"},
	  {"fields", input.fields},
  };

  if (input.unique)
	data["unique"] = input.unique.value();
  if (input.sparse)
	data["sparse"] = input.sparse.value();
  if (input.storedValues)
	data["storedValues"] = input.storedValues.value();
  if (input.cacheEnabled)
	data["cacheEnabled"] = input.cacheEnabled.value();
  if (input.name)
	data["name"] = input.name.value();
  if (input.in_background)
	data["inBackground"] = input.in_background.value();

  return AddIndex(data);
}

auto Collection::AddTTLIndex(input::TTLIndexCreateInput input) -> nlohmann::json {
  auto p = pimp::CollectionPimpl::Pimpl(p_);

  nlohmann::json data = {
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

auto Collection::AddInvertedIndex(input::InvertedIndexCreateInput input) -> nlohmann::json {
  auto p = pimp::CollectionPimpl::Pimpl(p_);

  nlohmann::json data = {
	  {"type", "inverted"},
	  {"fields", input.fields},
  };

  if (input.parallelism)
	data["parallelism"] = input.parallelism.value();
  if (input.primarySort)
	data["primarySort"] = input.primarySort.value();
  if (input.storedValues)
	data["storedValues"] = input.storedValues.value();
  if (input.analyzer)
	data["analyzer"] = input.analyzer.value();
  if (input.features)
	data["features"] = input.features.value();
  if (input.includeAllFields)
	data["includeAllFields"] = input.includeAllFields.value();
  if (input.trackListPositions)
	data["trackListPositions"] = input.trackListPositions.value();
  if (input.searchField)
	data["searchField"] = input.searchField.value();
  if (input.name)
	data["name"] = input.name.value();
  if (input.in_background)
	data["inBackground"] = input.in_background.value();

  return AddIndex(data);
}

auto Collection::DeleteIndex(std::string index_name, bool ignore_missing) -> bool {
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

auto Collection::AddIndex(nlohmann::json data) -> nlohmann::json {
  auto p = pimp::CollectionPimpl::Pimpl(p_);
  auto r = Request()
	  .Method(HttpMethod::POST)
	  .Database(p->db_.name())
	  .Parameters({StringPair("collection", p->name_)})
	  .Endpoint("/index")
	  .Data(data.dump(2));

  auto response = p->connection_.SendRequest(r);
  if (response.contains({401, 403}))
	throw AuthenticationError();
  else if (not response.is_success())
	throw ServerError(response.error_message(), response.error_code());

  return response.body();
}

} // zutano