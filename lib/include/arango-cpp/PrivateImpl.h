#pragma once

#include <memory>
#include <string>

namespace arangocpp {
class PrivateImpl {
 public:
  ~PrivateImpl() = default;

  virtual auto className() -> std::string { return "PrivateImpl"; }
};

typedef std::shared_ptr<PrivateImpl> private_impl_ptr;
}  // namespace arangocpp
