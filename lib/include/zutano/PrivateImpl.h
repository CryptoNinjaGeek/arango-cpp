#pragma once

#include <memory>
#include <string>

namespace zutano {
class PrivateImpl {
 public:
  ~PrivateImpl() = default;

  virtual auto ClassName() -> std::string {
	return "PrivateImpl";
  }
};

typedef std::shared_ptr<PrivateImpl> PrivateImplPtr;
}

