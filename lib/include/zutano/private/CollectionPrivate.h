#pragma one

#include <zutano/PrivateImpl.h>
#include <zutano/Connection.h>
#include <zutano/Database.h>

namespace zutano::pimp {
class CollectionPimpl : public PrivateImpl {
 public:
  Connection connection_;
  Database db_;
  std::string name_;

  static inline auto pimpl(const std::shared_ptr<PrivateImpl>& p) {
    return std::dynamic_pointer_cast<CollectionPimpl>(p);
  }
};
}  // namespace zutano::pimp