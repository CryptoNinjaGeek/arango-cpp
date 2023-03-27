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

#include <utility>

namespace zutano {

Collection::Collection(const Connection& conn, const Database& db, std::string name) {
  auto p = std::make_shared<pimp::CollectionPimpl>();
  p->connection_ = conn;
  p->name_ = std::move(name);
  p->db_ = db;
  p_ = p;
}

auto Collection::head(jsoncons::json doc) -> jsoncons::json {
  auto p = pimp::CollectionPimpl::pimpl(p_);

  auto r = Request()
               .method(HttpMethod::HEAD)
               .database(p->db_.name())
               .collection(p->name_)
               .endpoint(std::string("/document/{handle}"))
               .handle(getHandleFromDocument(std::move(doc)));

  auto response = p->connection_.sendRequest(r);
  if (response.errorCode() == 412)
    throw AuthenticationError();
  else if (not response.isSuccess())
    throw ServerError(response.errorMessage(), response.errorCode());

  return response.body();
}

auto Collection::insert(const jsoncons::json& doc, input::InsertInput input) -> jsoncons::json {
  auto p = pimp::CollectionPimpl::pimpl(p_);
  std::vector<string_pair> params;

  params.emplace_back("returnNew", tools::toString(input.return_new));
  params.emplace_back("silent", tools::toString(input.silent));
  params.emplace_back("overwrite", tools::toString(input.overwrite));
  params.emplace_back("returnOld", tools::toString(input.return_old));

  if (input.sync) params.emplace_back("waitForSync", tools::toString(input.sync.value()));
  if (input.overwrite_mode) params.emplace_back("overwriteMode", input.overwrite_mode.value());
  if (input.keep_none) params.emplace_back("keepNull", tools::toString(input.keep_none.value()));
  if (input.merge) params.emplace_back("mergeObjects", tools::toString(input.merge.value()));

  auto r = Request()
               .method(HttpMethod::POST)
               .database(p->db_.name())
               .collection(p->name_)
               .parameters(params)
               .endpoint("/document/{collection}")
               .data(doc.to_string());

  auto response = p->connection_.sendRequest(r);
  if (response.contains({401, 403}))
    throw AuthenticationError();
  else if (not response.isSuccess())
    throw ServerError(response.errorMessage(), response.errorCode());

  return response.body();
}

auto Collection::remove(const jsoncons::json& doc, input::DeleteInput input) -> bool {
  auto p = pimp::CollectionPimpl::pimpl(p_);

  std::vector<string_pair> params;

  params.emplace_back("returnOld", tools::toString(input.return_old));
  params.emplace_back("ignoreRevs", tools::toString(not input.check_rev));
  params.emplace_back("overwrite", tools::toString(not input.check_rev));
  params.emplace_back("silent", tools::toString(input.silent));

  if (input.sync) params.emplace_back("waitForSync", tools::toString(input.sync.value()));

  auto r = Request().method(HttpMethod::DELETE).database(p->db_.name()).collection(p->name_).parameters(params);

  if (doc.is_array())
    r = r.endpoint(std::string("/document/{collection}")).data(doc.to_string());
  else
    r = r.endpoint(std::string("/document/{handle}")).handle(getHandleFromDocument(doc));

  auto response = p->connection_.sendRequest(r);
  if (response.errorCode() == 1202 and input.ignore_missing) return false;
  if (response.errorCode() == 412)
    throw AuthenticationError();
  else if (not response.isSuccess())
    throw ServerError(response.errorMessage(), response.errorCode());

  return true;
}

auto Collection::update(const jsoncons::json& doc, input::UpdateInput input) -> jsoncons::json {
  auto p = pimp::CollectionPimpl::pimpl(p_);

  std::vector<string_pair> params;

  params.emplace_back("keepNull", tools::toString(input.keep_none));
  params.emplace_back("mergeObjects", tools::toString(input.merge));
  params.emplace_back("returnNew", tools::toString(input.return_new));
  params.emplace_back("returnOld", tools::toString(input.return_old));
  params.emplace_back("ignoreRevs", tools::toString(not input.check_rev));
  params.emplace_back("overwrite", tools::toString(not input.check_rev));
  params.emplace_back("silent", tools::toString(input.silent));

  if (input.sync) params.emplace_back("waitForSync", tools::toString(input.sync.value()));

  auto r = Request()
               .method(HttpMethod::PATCH)
               .database(p->db_.name())
               .collection(p->name_)
               .parameters(params)
               .data(doc.to_string());

  if (doc.is_array())
    r = r.endpoint(std::string("/document/{collection}/"));
  else
    r = r.endpoint(std::string("/document/{handle}")).handle(getHandleFromDocument(doc));

  auto response = p->connection_.sendRequest(r);
  if (response.errorCode() == 412)
    throw AuthenticationError();
  else if (not response.isSuccess())
    throw ServerError(response.errorMessage(), response.errorCode());
  else if (input.silent)
    return {};

  return response.body();
}

auto Collection::replace(const jsoncons::json& doc, input::ReplaceInput input) -> jsoncons::json {
  auto p = pimp::CollectionPimpl::pimpl(p_);

  std::vector<string_pair> params;

  params.emplace_back("returnNew", tools::toString(input.return_new));
  params.emplace_back("returnOld", tools::toString(input.return_old));
  params.emplace_back("ignoreRevs", tools::toString(not input.check_rev));
  params.emplace_back("overwrite", tools::toString(not input.check_rev));
  params.emplace_back("silent", tools::toString(input.silent));

  if (input.sync) params.emplace_back("waitForSync", tools::toString(input.sync.value()));

  auto r = Request()
               .method(HttpMethod::PUT)
               .database(p->db_.name())
               .collection(p->name_)
               .parameters(params)
               .data(doc.to_string());

  if (doc.is_array())
    r = r.endpoint(std::string("/document/{collection}/"));
  else
    r = r.endpoint(std::string("/document/{handle}")).handle(getHandleFromDocument(doc));

  auto response = p->connection_.sendRequest(r);
  if (response.errorCode() == 412)
    throw AuthenticationError();
  else if (not response.isSuccess())
    throw ServerError(response.errorMessage(), response.errorCode());
  else if (input.silent)
    return {};

  return response.body();
}

auto Collection::get(const jsoncons::json& doc, const input::GetInput& input) -> jsoncons::json {
  auto p = pimp::CollectionPimpl::pimpl(p_);

  std::vector<string_pair> headers;
  if (input.allow_dirty_read) headers.emplace_back("x-arango-allow-dirty-read", tools::toString(true));

  auto r = Request().method(HttpMethod::GET).headers(headers).database(p->db_.name()).collection(p->name_);

  if (doc.is_array())
    r = r.endpoint(std::string("/document/{collection}/"));
  else
    r = r.endpoint(std::string("/document/{handle}")).handle(getHandleFromDocument(doc));

  auto response = p->connection_.sendRequest(r);
  if (response.errorCode() == 1202)
    return {};
  else if (response.errorCode() == 412)
    throw AuthenticationError();
  else if (not response.isSuccess())
    throw ServerError(response.errorMessage(), response.errorCode());

  return response.body();
}

auto Collection::getHandleFromDocument(jsoncons::json doc) -> std::string {
  if (doc.contains("_id"))
    return doc["_id"].as<std::string>();
  else if (doc.contains("_key"))
    return doc["_key"].as<std::string>();
  return {};
}

auto Collection::truncate() -> bool {
  auto p = pimp::CollectionPimpl::pimpl(p_);

  auto r = Request()
               .method(HttpMethod::PUT)
               .database(p->db_.name())
               .collection(p->name_)
               .endpoint("/collection/{collection}/truncate");

  auto response = p->connection_.sendRequest(r);
  if (not response.isSuccess()) throw ServerError(response.errorMessage(), response.errorCode());

  return true;
}

}  // namespace zutano