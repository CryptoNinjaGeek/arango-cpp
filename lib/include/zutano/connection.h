#pragma once

#include <string>
#include "private_impl.h"
#include "database.h"

namespace zutano {
    class connection;

    typedef std::shared_ptr<connection> connection_ptr;

    class connection {
    public:
        connection();

        ~connection() = default;

        auto endpoint(std::string) -> connection &;

        auto auth(std::string, std::string) -> connection &;

        auto jwt(std::string) -> connection &;

        auto database(std::string) -> database;

    public:
        static auto create() -> connection_ptr;

    private:
        private_impl_ptr _p;
    };

} // zutano

