#pragma once

#include <jsoncons/json.hpp>

namespace arango_bench {

auto build_array(jsoncons::json& config, int& document_count) -> jsoncons::json;

}  // namespace arango_bench
