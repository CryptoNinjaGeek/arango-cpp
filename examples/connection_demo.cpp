#include <zutano/zutano.h>
#include <iostream>

using namespace zutano;
using namespace zutano::tools;
using namespace jsoncons::literals;

auto main() -> int {
  try {
    auto conn = Connection()
                    .hosts({"http://db1.google.dk:8529/", "http://db2.google.dk:8529/"})
                    .auth("root", "EAS6wQuKbGH9kKxe")
                    .resolve({{"db1.google.dk", "10.211.55.4", 8529}, {"db2.google.dk", "10.211.55.4", 8529}});

    if (conn.ping()) std::cout << "ping'ed OK" << std::endl;
    
  } catch (std::exception& e) {
    std::cout << e.what() << std::endl;
  }

  return 0;
}