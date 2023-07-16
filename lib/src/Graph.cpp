//
// Created by Carsten Tang on 19/03/2023.
//

#include <zutano/Collection.h>
#include <zutano/Connection.h>
#include <zutano/Database.h>
#include <zutano/Response.h>
#include <zutano/Exceptions.h>
#include <zutano/Tools.h>

#include <zutano/private/GraphPrivate.h>

#include <utility>

namespace zutano {

Graph::Graph(const Connection& conn, const Database& db, std::string name) {
  auto p = std::make_shared<pimp::GraphPimpl>();
  p->connection_ = conn;
  p->name_ = std::move(name);
  p->db_ = db;
  p_ = p;
}

auto Graph::name() -> std::string {
  auto p = pimp::GraphPimpl::pimpl(p_);
  return p->name_;
}

}  // namespace zutano