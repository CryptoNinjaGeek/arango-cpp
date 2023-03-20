//
// Created by Carsten Tang on 19/03/2023.
//

#include <zutano/connection.h>
#include <zutano/database.h>

namespace zutano {

    enum class AuthType {
        BasicAuth,
        JWT
    };

    class connection_pimpl : public private_impl {
    public:
        std::string _endpoint;
        std::string _userName;
        std::string _password;
        std::string _jwtToken;
        AuthType _authType;

    public:
        static inline auto pimpl(std::shared_ptr<private_impl> p) {
            return std::dynamic_pointer_cast<connection_pimpl>(p);
        }
    };

    connection::connection() {
        _p = std::make_shared<connection_pimpl>();
    }

    auto connection::create() -> connection_ptr {
        return connection_ptr(new connection);
    }


    auto connection::endpoint(std::string endpoint) -> connection & {
        auto p = connection_pimpl::pimpl(_p);
        p->_endpoint = endpoint;
        return *this;
    }

    auto connection::auth(std::string userName, std::string password) -> connection & {
        auto p = connection_pimpl::pimpl(_p);
        p->_userName = userName;
        p->_password = password;
        p->_authType = AuthType::BasicAuth;
        return *this;
    }

    auto connection::jwt(std::string jwtToken) -> connection & {
        auto p = connection_pimpl::pimpl(_p);
        p->_jwtToken = jwtToken;
        p->_authType = AuthType::JWT;
        return *this;
    }

    auto connection::database(std::string) -> zutano::database {
        return zutano::database();
    }


} // zutano