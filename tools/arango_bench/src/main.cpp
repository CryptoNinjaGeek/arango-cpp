#include <zutano/zutano.h>
#include <cxxopts.hpp>
#include <fstream>

#include "Input.h"
#include "Tools.h"
#include "ArangoBench.h"

using namespace zutano::tools;

auto main(int argc, char* argv[]) -> int {
  arango_bench::Input input;

  cxxopts::Options options("arango_bench", "Run large-scale customer oriented tests");

  auto add = options.add_options();
  add("c,configuration", "Configuration file", cxxopts::value<std::string>());
  add("s,shutdown", "Shutdown and cleanup all tests");
  add("v,verbose", "Verbose output");
  add("help", "Print usage");

  auto result = options.parse(argc, argv);

  if (result.count("configuration")) input.configuration = result["configuration"].as<std::string>();
  if (result.count("shutdown")) input.shutdown = true;
  if (result.count("verbose")) input.verbose = true;

  if (result.count("help") || (!input.shutdown && input.configuration.empty())) {
    std::cout << options.help() << std::endl;
    exit(0);
  }

  arango_bench::ArangoBench bench(input);

  if (input.shutdown)
    bench.shutdown();
  else
    bench.run();

  return 0;
}
