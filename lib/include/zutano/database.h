#pragma once

#include <string>
#include "private_impl.h"
#include "database.h"
#include "collection.h"

namespace zutano {

    class database {
    public:
        database();

        ~database() = default;

        auto create_datatabase(std::string) -> database;

        auto create_collection(std::string) -> collection;

        auto collection(std::string) -> collection;

        auto execute(std::string) -> nlohmann::json;

    private:
        private_impl_ptr _p;
    };

    typedef std::shared_ptr<database> database_ptr;

} // zutano

