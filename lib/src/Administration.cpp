//
// Created by Carsten Tang on 19/03/2023.
//

#include <arango-cpp/Administration.h>
#include <arango-cpp/Connection.h>

namespace arangocpp {

class AdministrationPimpl : public PrivateImpl {
 public:
  Connection connection_;

  static auto pimpl(const std::shared_ptr<PrivateImpl>& p) { return std::dynamic_pointer_cast<AdministrationPimpl>(p); }
};

Administration::Administration(const Connection& conn) {
  auto p = std::make_shared<AdministrationPimpl>();
  p->connection_ = conn;
  p_ = p;
}

Administration::Administration() {
  auto p = std::make_shared<AdministrationPimpl>();
  p_ = p;
}

}  // namespace arangocpp