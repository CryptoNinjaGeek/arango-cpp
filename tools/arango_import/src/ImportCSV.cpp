#include "ArangoImport.h"
#include <zutano/zutano.h>
#include <cxxopts.hpp>
#include <iostream>
#include <jsoncons/json.hpp>
#include <jsoncons_ext/csv/csv.hpp>
#include <fstream>

#include "ProgressBar.h"
#include "Input.h"
#include "Tools.h"

using namespace zutano;
using namespace zutano::tools;
using namespace jsoncons;
using namespace jsoncons::literals;

namespace arango_import {

auto importCSV(zutano::Collection collection, const std::string& file_name, const input::ImportInput& input) -> unsigned long {
  unsigned long rows_imported{0}, line_no{0};
  csv::csv_options options;
  std::shared_ptr<ProgressBar> bar;

  if (input.progress) {
    auto number_of_lines = tools::lineCount(file_name);
    long steps = number_of_lines / input.batch_size;
    bar = std::make_shared<ProgressBar>(steps);
    bar->set_todo_char(" ");
    bar->set_done_char("█");
    bar->set_opening_bracket_char("{");
    bar->set_closing_bracket_char("}");

    if (input.verbose) std::cout << "Number of lines: " << number_of_lines << std::endl;
  }

  options.assume_header(true).mapping_kind(csv::csv_mapping_kind::n_objects).field_delimiter('|');

  std::string line, header;
  std::ifstream infile(file_name);
  std::getline(infile, header);
  std::stringstream out;

  while (!infile.eof()) {
    out << header << std::endl;
    while (std::getline(infile, line) and line_no++ < input.batch_size) out << line << std::endl;
    json j = csv::decode_csv<json>(out.str(), options);

    collection.insert(j, {.sync = true});
    if (bar) bar->update();

    rows_imported += j.size();
    out.str(std::string());
    line_no = 0;
  }

  if (bar) std::cout << std::endl;
  return rows_imported;
}

}  // namespace arango_import
