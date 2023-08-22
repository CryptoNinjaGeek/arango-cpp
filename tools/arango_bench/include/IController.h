#pragma once

namespace arango_bench {

class IController {
  virtual auto create_network() -> bool = 0;
};

}  // namespace arango_bench
