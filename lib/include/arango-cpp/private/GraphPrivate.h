#pragma one

#include <arango-cpp/PrivateImpl.h>
#include <arango-cpp/Connection.h>
#include <arango-cpp/Database.h>

namespace arangocpp::pimp {
class GraphPimpl : public PrivateImpl {
 public:
  Connection connection_;
  Database db_;
  std::string name_;

  static inline auto pimpl(const std::shared_ptr<PrivateImpl>& p) { return std::dynamic_pointer_cast<GraphPimpl>(p); }
};
}  // namespace zutano::pimp