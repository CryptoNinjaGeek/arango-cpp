//
// Created by Carsten Tang on 19/03/2023.
//

#include <arango-cpp/Backup.h>
#include <arango-cpp/Connection.h>
#include <arango-cpp/Response.h>
#include <arango-cpp/Exceptions.h>
#include <arango-cpp/Tools.h>

namespace arangocpp {

class BackupPimpl : public PrivateImpl {
 public:
  Connection connection_;

  static auto pimpl(const std::shared_ptr<PrivateImpl>& p) { return std::dynamic_pointer_cast<BackupPimpl>(p); }
};

Backup::Backup(const Connection& conn) {
  auto p = std::make_shared<BackupPimpl>();
  p->connection_ = conn;
  p_ = p;
}

Backup::Backup() {
  auto p = std::make_shared<BackupPimpl>();
  p_ = p;
}

auto Backup::get(std::optional<std::string> backupId) const -> jsoncons::json {
  auto p = BackupPimpl::pimpl(p_);

  jsoncons::json data;

  if (backupId) data = tools::to_json{{"id", backupId.value()}};

  Request request = Request()
    .isAdmin(true)
    .method(HttpMethod::POST)
    .endpoint("/backup/list")
    .data(data.to_string());

  auto response = p->connection_.sendRequest(request);

  if (response.contains({401, 403})) {
    throw AuthenticationError();
  }
  if (!response.isSuccess()) {
    throw ServerError(response.errorMessage(), response.errorCode());
  }
  return response.body();
}

auto Backup::remove(const std::string& backupId) const -> bool {
  auto p = BackupPimpl::pimpl(p_);

  jsoncons::json data = tools::to_json{{"id", backupId}};

  Request request = Request().method(HttpMethod::POST).isAdmin(true).endpoint("/backup/delete").data(data.to_string());

  auto response = p->connection_.sendRequest(request);
  if (response.contains({401, 403})) {
    throw AuthenticationError();
  }
  if (!response.isSuccess()) {
    throw ServerError(response.errorMessage(), response.errorCode());
  }
  return true;
}

auto Backup::download(input::BackupInput input) const -> jsoncons::json {
  auto p = BackupPimpl::pimpl(p_);

  jsoncons::json data;

  if (input.backupId) data["id"] = input.backupId.value();
  if (input.repository) data["remoteRepository"] = input.repository.value();
  if (input.abort) data["abort"] = input.abort.value();
  if (input.downloadId) data["downloadId"] = input.downloadId.value();
  if (input.config) data["config"] = input.config.value();

  Request request = Request().method(HttpMethod::POST).isAdmin(true).endpoint("/backup/download").data(data.to_string());

  auto response = p->connection_.sendRequest(request);

  if (response.contains({401, 403})) {
    throw AuthenticationError();
  }
  if (!response.isSuccess()) {
    throw ServerError(response.errorMessage(), response.errorCode());
  }
  return response.body();
}

auto Backup::upload(input::BackupInput input) const -> jsoncons::json {
  auto p = BackupPimpl::pimpl(p_);

  jsoncons::json data;

  if (input.backupId) data["id"] = input.backupId.value();
  if (input.repository) data["remoteRepository"] = input.repository.value();
  if (input.abort) data["abort"] = input.abort.value();
  if (input.downloadId) data["downloadId"] = input.downloadId.value();
  if (input.config) data["config"] = input.config.value();

  Request request = Request().method(HttpMethod::POST).isAdmin(true).endpoint("/backup/upload").data(data.to_string());

  auto response = p->connection_.sendRequest(request);

  if (response.contains({401, 403})) {
    throw AuthenticationError();
  }
  if (!response.isSuccess()) {
    throw ServerError(response.errorMessage(), response.errorCode());
  }
  return response.body();
}

auto Backup::restore(const std::string& backupId) const -> jsoncons::json {
  auto p = BackupPimpl::pimpl(p_);

  jsoncons::json data = tools::to_json{{"id", backupId}};

  Request request = Request().method(HttpMethod::POST).isAdmin(true).endpoint("/backup/restore").data(data.to_string());

  auto response = p->connection_.sendRequest(request);

  if (response.contains({401, 403})) {
    throw AuthenticationError();
  }
  if (!response.isSuccess()) {
    throw ServerError(response.errorMessage(), response.errorCode());
  }
  return response.body();
}
}  // namespace arangocpp