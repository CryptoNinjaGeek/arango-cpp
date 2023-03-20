#pragma once

#include <memory>

namespace zutano {
    class private_impl {
    public:
        ~private_impl() = default;

        virtual auto className() -> std::string {
            return "private_impl";
        }
    };

    typedef std::shared_ptr<private_impl> private_impl_ptr;
}

