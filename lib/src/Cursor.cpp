//
// Created by Carsten Tang on 19/03/2023.
//

#include <zutano/private/CursorPrivate.h>
#include <zutano/Exceptions.h>
#include "zutano/Cursor.h"

namespace zutano {

Cursor::Cursor(const std::string &dbName, const Connection &conn, const jsoncons::json &data) {
  auto p = std::make_shared<pimp::CursorPimpl>();
  p->connection_ = conn;
  p->db_name_ = dbName;
  p_ = p;

  Update(data);
}

Cursor::~Cursor() {
  Close(true);
}

auto Cursor::Id() -> std::string {
  auto p = pimp::CursorPimpl::Pimpl(p_);
  return p->id_;
}

auto Cursor::Type() -> std::string {
  auto p = pimp::CursorPimpl::Pimpl(p_);
  return p->type_;
}

auto Cursor::Update(jsoncons::json data) -> bool {
  auto p = pimp::CursorPimpl::Pimpl(p_);

  if (data.contains("id"))
	p->id_ = data["id"].as<std::string>();

  if (data.contains("type"))
	p->type_ = data["type"].as<std::string>();

  if (data.contains("count"))
	p->count_ = data["count"].as<long>();

  if (data.contains("cached"))
	p->cached_ = data["cached"].as<std::string>();

  if (data.contains("hasMore"))
	p->has_more_ = data["hasMore"].as<bool>();

  if (data.contains("cached"))
	p->cached_ = data["cached"].as<std::string>();

  if (data.contains("extra")) {
	auto extra = data["extra"];

	if (extra.contains("profile"))
	  p->profile_ = extra["profile"].as<std::string>();

	if (extra.contains("warnings"))
	  p->warnings_ = extra["warnings"].as<std::string>();

	if (extra.contains("stats"))
	  p->stats_ = extra["stats"].as<jsoncons::json>();
  }

  auto result = data["result"];

  if (result.is_array()) {
	for (const auto &row : result.array_range())
	  p->rows_.emplace_back(row);
  }

  return true;
}

auto Cursor::Count() -> long {
  auto p = pimp::CursorPimpl::Pimpl(p_);
  return p->count_;
}

auto Cursor::HasMore() -> bool {
  auto p = pimp::CursorPimpl::Pimpl(p_);
  if (Empty()) {
	if (p->has_more_)
	  return Fetch();
	else
	  return false;
  }
  return true;
}

auto Cursor::Cached() -> std::string {
  auto p = pimp::CursorPimpl::Pimpl(p_);
  return p->cached_;
}

auto Cursor::Statistics() -> jsoncons::json {
  auto p = pimp::CursorPimpl::Pimpl(p_);
  return p->stats_;
}

auto Cursor::Profile() -> std::string {
  auto p = pimp::CursorPimpl::Pimpl(p_);
  return p->profile_;
}

auto Cursor::Warnings() -> std::string {
  auto p = pimp::CursorPimpl::Pimpl(p_);
  return p->warnings_;
}

auto Cursor::Empty() -> bool {
  auto p = pimp::CursorPimpl::Pimpl(p_);
  return p->rows_.empty();
}

auto Cursor::Next() -> jsoncons::json {
  auto p = pimp::CursorPimpl::Pimpl(p_);

  auto row = p->rows_.front();
  p->rows_.erase(p->rows_.begin());

  return row;
}

auto Cursor::Fetch() -> bool {
  auto p = pimp::CursorPimpl::Pimpl(p_);
  
  auto r = Request()
	  .Method(HttpMethod::PUT)
	  .Database(p->db_name_)
	  .Handle(p->type_)
	  .Id(p->id_)
	  .Endpoint("/{handle}/{id}");

  auto response = p->connection_.SendRequest(r);

  if (response.contains({401, 403}))
	throw AuthenticationError();
  else if (not response.is_success())
	throw ServerError(response.error_message(), response.error_code());

  return Update(response.body());
}

auto Cursor::Close(bool ignore_missing) -> bool {
  auto p = pimp::CursorPimpl::Pimpl(p_);

  if (p->id_.empty())
	return true;

  auto r = Request()
	  .Method(HttpMethod::DELETE)
	  .Database(p->db_name_)
	  .Handle(p->type_)
	  .Id(p->id_)
	  .Endpoint("/{handle}/{id}");

  auto response = p->connection_.SendRequest(r);
  if (response.contains({401, 403}))
	throw AuthenticationError();
  else if (response.http_code()==404 and ignore_missing)
	return false;
  else if (not response.is_success())
	throw ServerError(response.error_message(), response.error_code());

  return true;
}

} // zutano