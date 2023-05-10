#pragma once

#include <string>
#include <zutano/Connection.h>

#include "Input.h"

namespace arango_import {

unsigned long importCSV(zutano::Collection collection, const std::string& file_name, const input::ImportInput& input);

void importFile(zutano::Database db, const std::string& file_name, const input::ImportInput& input);
int run(const input::ImportInput& input);

}  // namespace arango_import
