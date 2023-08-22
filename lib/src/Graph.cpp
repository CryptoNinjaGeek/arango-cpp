//
// Created by Carsten Tang on 19/03/2023.
//

#include <arango-cpp/Collection.h>
#include <arango-cpp/Connection.h>
#include <arango-cpp/Database.h>
#include <arango-cpp/Response.h>
#include <arango-cpp/Exceptions.h>
#include <arango-cpp/Tools.h>

#include <arango-cpp/private/GraphPrivate.h>

#include <utility>

namespace arangocpp {

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

}  // namespace arango-cpp