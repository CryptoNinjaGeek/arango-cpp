#pragma once

#include <string>
#include "private_impl.h"
#include <nlohmann/json.hpp>
#include <zutano/index.h>

namespace zutano {
    class collection;

    typedef std::shared_ptr<collection> collection_ptr;

    class collection {
    public:
        collection();

        ~collection() = default;

        auto insert(nlohmann::json) -> nlohmann::json;

        auto add_hash_index(hash_index) -> bool;

    private:
        private_impl_ptr _p;
    };

} // zutano

