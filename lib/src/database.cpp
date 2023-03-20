//
// Created by Carsten Tang on 19/03/2023.
//

#include <zutano/database.h>

namespace zutano {

    class database_pimpl : public private_impl {
    public:
        static inline auto pimpl(std::shared_ptr<private_impl> p) {
            return std::dynamic_pointer_cast<database_pimpl>(p);
        }
    };

    database::database() {
        _p = std::make_shared<database_pimpl>();
    }

    auto database::create_datatabase(std::string) -> database {
        return database();
    }

    auto database::create_collection(std::string) -> zutano::collection {
        return zutano::collection();
    }

    auto database::collection(std::string) -> zutano::collection {
        return zutano::collection();
    }

    auto database::execute(std::string) -> nlohmann::json {
        nlohmann::json j;
        return j;
    }


} // zutano