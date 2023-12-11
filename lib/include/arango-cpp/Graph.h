#pragma once

#include <string>
#include <arango-cpp/PrivateImpl.h>

namespace arangocpp {

class Connection;
class Database;

class Graph {
 public:
  Graph(const Connection&, const Database&, std::string);

  ~Graph() = default;

  auto name() -> std::string;
  
 private:
  private_impl_ptr p_;
};

}  // namespace arango-cpp
