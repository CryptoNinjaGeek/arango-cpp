//
// Created by Carsten Tang on 19/03/2023.
//

#include <zutano/private/CursorPrivate.h>
#include <zutano/Exceptions.h>
#include "zutano/Cursor.h"

namespace zutano {

Cursor::Cursor(const std::string& db_name, const Connection& conn, const jsoncons::json& data) {
  auto p = std::make_shared<pimp::CursorPimpl>();
  p->connection_ = conn;
  p->db_name_ = db_name;
  p_ = p;

  update(data);
}

Cursor::~Cursor() { close(true); }

auto Cursor::id() -> std::string {
  auto p = pimp::CursorPimpl::pimpl(p_);
  return p->id_;
}

auto Cursor::type() -> std::string {
  auto p = pimp::CursorPimpl::pimpl(p_);
  return p->type_;
}

auto Cursor::update(jsoncons::json data) -> bool {
  auto p = pimp::CursorPimpl::pimpl(p_);

  if (data.contains("id")) p->id_ = data["id"].as<std::string>();

  if (data.contains("type")) p->type_ = data["type"].as<std::string>();

  if (data.contains("count")) p->count_ = data["count"].as<long>();

  if (data.contains("cached")) p->cached_ = data["cached"].as<std::string>();

  if (data.contains("hasMore")) p->has_more_ = data["hasMore"].as<bool>();

  if (data.contains("cached")) p->cached_ = data["cached"].as<std::string>();

  if (data.contains("extra")) {
    auto extra = data["extra"];

    if (extra.contains("profile")) p->profile_ = extra["profile"].as<std::string>();

    if (extra.contains("warnings")) p->warnings_ = extra["warnings"].as<std::string>();

    if (extra.contains("stats")) p->stats_ = extra["stats"].as<jsoncons::json>();
  }

  auto result = data["result"];

  if (result.is_array()) {
    for (const auto& row : result.array_range()) p->rows_.emplace_back(row);
  }

  return true;
}

auto Cursor::count() -> long {
  auto p = pimp::CursorPimpl::pimpl(p_);
  return p->count_;
}

auto Cursor::hasMore() -> bool {
  auto p = pimp::CursorPimpl::pimpl(p_);
  if (empty()) {
    if (p->has_more_)
      return fetch();
    else
      return false;
  }
  return true;
}

auto Cursor::cached() -> std::string {
  auto p = pimp::CursorPimpl::pimpl(p_);
  return p->cached_;
}

auto Cursor::statistics() -> jsoncons::json {
  auto p = pimp::CursorPimpl::pimpl(p_);
  return p->stats_;
}

auto Cursor::profile() -> std::string {
  auto p = pimp::CursorPimpl::pimpl(p_);
  return p->profile_;
}

auto Cursor::warnings() -> std::string {
  auto p = pimp::CursorPimpl::pimpl(p_);
  return p->warnings_;
}

auto Cursor::empty() -> bool {
  auto p = pimp::CursorPimpl::pimpl(p_);
  return p->rows_.empty();
}

auto Cursor::next() -> jsoncons::json {
  auto p = pimp::CursorPimpl::pimpl(p_);

  auto row = p->rows_.front();
  p->rows_.erase(p->rows_.begin());

  return row;
}

auto Cursor::fetch() -> bool {
  auto p = pimp::CursorPimpl::pimpl(p_);

  auto r =
      Request().method(HttpMethod::PUT).database(p->db_name_).handle(p->type_).id(p->id_).endpoint("/{handle}/{id}");

  auto response = p->connection_.sendRequest(r);

  if (response.contains({401, 403}))
    throw AuthenticationError();
  else if (not response.isSuccess())
    throw ServerError(response.errorMessage(), response.errorCode());

  return update(response.body());
}

auto Cursor::close(bool ignore_missing) -> bool {
  auto p = pimp::CursorPimpl::pimpl(p_);

  if (p->id_.empty()) return true;

  auto r =
      Request().method(HttpMethod::DELETE).database(p->db_name_).handle(p->type_).id(p->id_).endpoint("/{handle}/{id}");

  auto response = p->connection_.sendRequest(r);
  if (response.contains({401, 403}))
    throw AuthenticationError();
  else if (response.httpCode() == 404 and ignore_missing)
    return false;
  else if (not response.isSuccess())
    throw ServerError(response.errorMessage(), response.errorCode());

  return true;
}

}  // namespace zutano