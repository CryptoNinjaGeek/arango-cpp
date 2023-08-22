#include <cxxopts.hpp>
#include <fstream>
#include <iostream>

#define MAX_LINES 200000

auto convert(std::string name, std::string from, std::string to, bool verbose) -> void {
  std::ifstream is(from);
  std::ofstream os(to);

  os << "#pragma once" << std::endl;
  os << "" << std::endl;
  os << "const std::vector<std::string> ";
  os << name << " = { ";

  std::string line;
  int cnt = 0;
  while (std::getline(is, line) && cnt < MAX_LINES) {
    if (cnt) os << ",";
    os << "\"" << line << "\"";
    cnt++;
  }
  os << "};" << std::endl;
}

auto main(int argc, char* argv[]) -> int {
  cxxopts::Options options("csv2code", "convert csv file to code");
  bool verbose = false;
  std::string from, to, name;

  auto add = options.add_options();
  add("n,name", "Name of struct", cxxopts::value<std::string>());
  add("f,from", "File with 1 string pr. line", cxxopts::value<std::string>());
  add("t,to", "Header file", cxxopts::value<std::string>());
  add("v,verbose", "Verbose output");
  add("help", "Print usage");

  auto result = options.parse(argc, argv);

  if (result.count("help")) {
    std::cout << options.help() << std::endl;
    exit(0);
  }

  if (result.count("name")) name = result["name"].as<std::string>();
  if (result.count("from")) from = result["from"].as<std::string>();
  if (result.count("to")) to = result["to"].as<std::string>();
  if (result.count("verbose")) verbose = true;

  convert(name, from, to, verbose);

  return 0;
}
