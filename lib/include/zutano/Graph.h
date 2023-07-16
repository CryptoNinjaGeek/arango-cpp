#pragma once

#include <string>
#include <zutano/PrivateImpl.h>
#include <zutano/input/IndexInput.h>
#include <zutano/input/DocumentInput.h>

#include <jsoncons/json.hpp>

namespace zutano {

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

}  // namespace zutano
