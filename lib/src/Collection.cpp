//
// Created by Carsten Tang on 19/03/2023.
//

#include <zutano/Collection.h>
#include <zutano/Connection.h>
#include <zutano/Database.h>
#include <zutano/Response.h>
#include <zutano/Exceptions.h>
#include <zutano/Tools.h>

#include <zutano/private/CollectionPrivate.h>

namespace zutano {

Collection::Collection(Connection conn, Database db, std::string name) {
  auto p = std::make_shared<pimp::CollectionPimpl>();
  p->connection_ = conn;
  p->name_ = name;
  p->db_ = db;
  p_ = p;
}

auto Collection::Insert(nlohmann::json doc, input::InsertInput input) -> nlohmann::json {
  auto p = pimp::CollectionPimpl::Pimpl(p_);
  std::vector<StringPair> params;

  params.push_back(StringPair("returnNew", tools::to_string(input.return_new)));
  params.push_back(StringPair("silent", tools::to_string(input.silent)));
  params.push_back(StringPair("overwrite", tools::to_string(input.overwrite)));
  params.push_back(StringPair("returnOld", tools::to_string(input.return_old)));

  if (input.sync)
	params.push_back(StringPair("waitForSync", tools::to_string(input.sync.value())));
  if (input.overwrite_mode)
	params.push_back(StringPair("overwriteMode", input.overwrite_mode.value()));
  if (input.keep_none)
	params.push_back(StringPair("keepNull", tools::to_string(input.keep_none.value())));
  if (input.merge)
	params.push_back(StringPair("mergeObjects", tools::to_string(input.merge.value())));

  auto r = Request()
	  .Method(HttpMethod::POST)
	  .Database(p->db_.name())
	  .Collection(p->name_)
	  .Parameters(params)
	  .Endpoint("/document")
	  .Data(doc.dump(2));

  auto response = p->connection_.SendRequest(r);
  if (response.contains({401, 403}))
	throw AuthenticationError();
  else if (not response.is_success())
	throw ServerError(response.error_message(), response.error_code());

  return response.body();
}

auto Collection::Delete(nlohmann::json doc, input::DeleteInput input) -> bool {
  auto p = pimp::CollectionPimpl::Pimpl(p_);

  std::vector<StringPair> params;

  auto handle = GetHandleFromDocument(doc);

  params.push_back(StringPair("returnOld", tools::to_string(input.return_old)));
  params.push_back(StringPair("ignoreRevs", tools::to_string(not input.check_rev)));
  params.push_back(StringPair("overwrite", tools::to_string(not input.check_rev)));
  params.push_back(StringPair("silent", tools::to_string(input.silent)));

  if (input.sync)
	params.push_back(StringPair("waitForSync", tools::to_string(input.sync.value())));

  auto r = Request()
	  .Method(HttpMethod::DELETE)
	  .Database(p->db_.name())
	  .Collection(p->name_)
	  .Parameters(params)
	  .Endpoint(std::string("/document'/") + handle);

  auto response = p->connection_.SendRequest(r);
  if (response.error_code()==1202 and input.ignore_missing)
	return false;
  if (response.error_code()==412)
	throw AuthenticationError();
  else if (not response.is_success())
	throw ServerError(response.error_message(), response.error_code());

  return true;
}

auto Collection::Update(nlohmann::json doc, input::UpdateInput input) -> nlohmann::json {
  auto p = pimp::CollectionPimpl::Pimpl(p_);

  std::vector<StringPair> params;

  auto handle = GetHandleFromDocument(doc);

  params.push_back(StringPair("keepNull", tools::to_string(input.keep_none)));
  params.push_back(StringPair("mergeObjects", tools::to_string(input.merge)));
  params.push_back(StringPair("returnNew", tools::to_string(input.return_new)));
  params.push_back(StringPair("returnOld", tools::to_string(input.return_old)));
  params.push_back(StringPair("ignoreRevs", tools::to_string(not input.check_rev)));
  params.push_back(StringPair("overwrite", tools::to_string(not input.check_rev)));
  params.push_back(StringPair("silent", tools::to_string(input.silent)));

  if (input.sync)
	params.push_back(StringPair("waitForSync", tools::to_string(input.sync.value())));

  auto r = Request()
	  .Method(HttpMethod::PATCH)
	  .Database(p->db_.name())
	  .Collection(p->name_)
	  .Parameters(params)
	  .Endpoint(std::string("/document'/") + handle);

  auto response = p->connection_.SendRequest(r);
  if (response.error_code()==412)
	throw AuthenticationError();
  else if (not response.is_success())
	throw ServerError(response.error_message(), response.error_code());
  else if (input.silent)
	return {};

  return response.body();
}

auto Collection::Replace(nlohmann::json doc, input::ReplaceInput input) -> nlohmann::json {
  auto p = pimp::CollectionPimpl::Pimpl(p_);

  std::vector<StringPair> params;

  auto handle = GetHandleFromDocument(doc);

  params.push_back(StringPair("returnNew", tools::to_string(input.return_new)));
  params.push_back(StringPair("returnOld", tools::to_string(input.return_old)));
  params.push_back(StringPair("ignoreRevs", tools::to_string(not input.check_rev)));
  params.push_back(StringPair("overwrite", tools::to_string(not input.check_rev)));
  params.push_back(StringPair("silent", tools::to_string(input.silent)));

  if (input.sync)
	params.push_back(StringPair("waitForSync", tools::to_string(input.sync.value())));

  auto r = Request()
	  .Method(HttpMethod::PUT)
	  .Database(p->db_.name())
	  .Collection(p->name_)
	  .Parameters(params)
	  .Endpoint(std::string("/document'/") + handle);

  auto response = p->connection_.SendRequest(r);
  if (response.error_code()==412)
	throw AuthenticationError();
  else if (not response.is_success())
	throw ServerError(response.error_message(), response.error_code());
  else if (input.silent)
	return {};

  return response.body();
}

auto Collection::Get(input::GetInput input) -> nlohmann::json {
  auto p = pimp::CollectionPimpl::Pimpl(p_);

  std::vector<StringPair> headers;
  if (input.allow_dirty_read)
	headers.push_back(StringPair("x-arango-allow-dirty-read", tools::to_string(true)));

  auto r = Request()
	  .Method(HttpMethod::GET)
	  .Headers(headers)
	  .Database(p->db_.name())
	  .Collection(p->name_)
	  .Endpoint(std::string("/document/") + input._id);

  auto response = p->connection_.SendRequest(r);
  if (response.error_code()==1202)
	return {};
  else if (response.error_code()==412)
	throw AuthenticationError();
  else if (not response.is_success())
	throw ServerError(response.error_message(), response.error_code());

  return response.body();
}

auto Collection::GetHandleFromDocument(nlohmann::json doc) -> std::string {
  if (doc.contains("_id"))
	return doc["_id"].get<std::string>();
  else if (doc.contains("_key"))
	return doc["_key"].get<std::string>();
  return {};
}

} // zutano