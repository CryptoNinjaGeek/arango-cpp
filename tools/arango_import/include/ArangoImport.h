#pragma once

#include <string>
#include <arango-cpp/Connection.h>

#include "Input.h"

namespace arango_import {

unsigned long importCSV(arangocpp::Collection collection, const std::string& file_name, const input::ImportInput& input);

void importFile(arangocpp::Database db, const std::string& file_name, const input::ImportInput& input);
int run(const input::ImportInput& input);

}  // namespace arango_import
