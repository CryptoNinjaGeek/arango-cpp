//
// Created by Carsten Tang on 19/03/2023.
//

#include <zutano/collection.h>

namespace zutano {

    class collection_pimpl : public private_impl {
    public:
        static inline auto pimpl(std::shared_ptr<private_impl> p) {
            return std::dynamic_pointer_cast<collection_pimpl>(p);
        }
    };

    collection::collection() {
        _p = std::make_shared<collection_pimpl>();
    }

    auto collection::insert(nlohmann::json doc) -> nlohmann::json {
        return doc;
    }

    auto collection::add_hash_index(hash_index) -> bool {
        return true;
    }

} // zutano