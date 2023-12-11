#pragma once

#include <arango-cpp/PrivateImpl.h>

#include <jsoncons/json.hpp>

namespace arangocpp {
class Database;
class Connection;

class Administration {
public:
  explicit Administration(const Connection&);
  Administration();

private:
  private_impl_ptr p_;
};

}  // namespace arangocpp
