#pragma once

struct hash_index {
    std::vector<std::string> fields;
    bool unique{false};
};