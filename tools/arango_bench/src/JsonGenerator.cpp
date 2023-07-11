#include "JsonGenerator.h"
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <random>
#include <ctime>
#include <cstdlib>
#include <cstring>
#include <sstream>

const std::string letterBytes = "abcde fghij klmn opqrs tuvwx yz";
const int letterIdxBits = 6;
const int letterIdxMask = (1 << letterIdxBits) - 1;
const int letterIdxMax = 63 / letterIdxBits;

struct GeneratorJSON {
  std::string name;
  std::string type;
  int length{0};
  std::pair<int, int> length_interval;
  std::optional<int> null_percentage;
  std::optional<int> min;
  std::optional<int> max;
  std::optional<int> size;
  std::vector<GeneratorJSON> object_content;
};

class Generator {
 public:
  virtual std::string Key() = 0;
  virtual void Value(std::mt19937& r, std::stringstream& ss) = 0;
  virtual bool Exists(std::mt19937& r) = 0;
};

class EmptyGenerator : public Generator {
 public:
  std::string key;
  int nullPercentage;

  EmptyGenerator(std::string k, int np) : key(k), nullPercentage(np) {}

  std::string Key() { return key; }

  bool Exists(std::mt19937& r) { return r() % 100 >= nullPercentage; }

  void Value(std::mt19937&, std::stringstream&) {}
};

static const char alphabet[] =
    "abcd efghi jklm nopqr stuvwx yz"
    "ABCDE FGHIJKLM NOP QRSTU VWXYZ"
    "012 345 6789";

class StringGenerator : public EmptyGenerator {
 public:
  int length;
  std::pair<int, int> length_interval;
  std::random_device rd;
  std::default_random_engine rng;
  std::uniform_int_distribution<> dist;

  StringGenerator(std::string k, int np, int len, std::pair<int, int> interval)
      : EmptyGenerator(k, np), length(len), length_interval(interval), rng(rd()), dist(0, sizeof(alphabet) / sizeof(*alphabet) - 2) {}

  void Value(std::mt19937& r, std::stringstream& ss) {
    auto real_length = length;
    if (!real_length) real_length = (r() % (length_interval.second - length_interval.first)) + length_interval.first;

    std::string str;
    str.reserve(real_length);
    std::generate_n(std::back_inserter(str), real_length, [&]() { return alphabet[dist(rng)]; });

    ss << '"';
    ss << str;
    ss << '"';

    /*    std::string by(real_length + 2, ' ');
        by[0] = '"';
        int last = 0;
        for (int i = real_length, cache, remain; i > 0;) {
          if (remain == 0) {
            cache = r();
            remain = letterIdxMax;
          }
          if (!(i % last)) {
            by[i] = ' ';
            i--;
          } else if (int idx = cache & letterIdxMask; idx < letterBytes.length()) {
            by[i] = letterBytes[idx];
            last = by[i];
            i--;
          }
          cache >>= letterIdxBits;
          remain--;
        }
        by[real_length] = '"';
ss << by;
        */
  }
};

class IntGenerator : public EmptyGenerator {
 public:
  int min;
  int max;

  IntGenerator(std::string k, int np, int mn, int mx) : EmptyGenerator(k, np), min(mn), max(mx) {}

  void Value(std::mt19937& r, std::stringstream& ss) { ss << r() % (max - min) + min; }
};

class BoolGenerator : public EmptyGenerator {
 public:
  BoolGenerator(std::string k, int np) : EmptyGenerator(k, np) {}

  void Value(std::mt19937& r, std::stringstream& ss) { ss << (r() % 2 == 0 ? "true" : "false"); }
};

class ArrayGenerator : public EmptyGenerator {
 public:
  int size;
  Generator* generator;

  ArrayGenerator(std::string k, int np, int sz, Generator* gen) : EmptyGenerator(k, np), size(sz), generator(gen) {}

  void Value(std::mt19937& r, std::stringstream& ss) {
    ss << "[";
    for (int i = 0; i < size; i++) {
      generator->Value(r, ss);
      if (i != size - 1) {
        ss << ",";
      }
    }
    ss << "]";
  }
};

class ObjectGenerator : public EmptyGenerator {
 public:
  std::vector<Generator*> generators;

  ObjectGenerator(std::string k, int np, std::vector<Generator*> gens) : EmptyGenerator(k, np), generators(gens) {}

  void Value(std::mt19937& r, std::stringstream& ss) {
    std::vector<Generator*> exists;
    for (int i = 0; i < generators.size(); i++) {
      if (generators[i]->Exists(r)) exists.push_back(generators[i]);
    }

    ss << "{";
    for (int i = 0; i < exists.size(); i++) {
      ss << "\"" << exists[i]->Key() << "\":";
      exists[i]->Value(r, ss);
      if (i != exists.size() - 1) {
        ss << ",";
      }
    }
    ss << "}";
  }
};

Generator* NewGenerator(std::string k, GeneratorJSON& v) {
  auto null_percentage = v.null_percentage.value_or(0);
  EmptyGenerator* eg = new EmptyGenerator(k, null_percentage);
  if (v.type == "string") {
    return new StringGenerator(k, null_percentage, v.length, v.length_interval);
  } else if (v.type == "int") {
    return new IntGenerator(k, null_percentage, 2, 4);
  } else if (v.type == "boolean") {
    return new BoolGenerator(k, null_percentage);
  } else if (v.type == "object") {
    std::vector<Generator*> gens;
    for (auto& it : v.object_content) {
      gens.push_back(NewGenerator(it.name, it));
    }
    return new ObjectGenerator(k, v.null_percentage.value_or(0), gens);
  } else {
    return new BoolGenerator(k, null_percentage);
  }
}

std::vector<Generator*> NewGeneratorsFromMap(std::vector<GeneratorJSON>& content) {
  std::vector<Generator*> genArr;
  for (auto& it : content) {
    genArr.push_back(NewGenerator(it.name, it));
  }
  return genArr;
}

ArrayGenerator baseGenerator(int count, std::vector<GeneratorJSON>& content) {
  return ArrayGenerator("", 0, count, new ObjectGenerator("", 0, NewGeneratorsFromMap(content)));
}

JSONCONS_N_MEMBER_TRAITS(GeneratorJSON, 2, name, type, null_percentage, length, length_interval, min, max, size, object_content)

namespace arango_bench {

jsoncons::json build_array(jsoncons::json& config, int& document_count) {
  std::mt19937 randSource(time(0));
  std::vector<GeneratorJSON> content;

  try {
    content = jsoncons::decode_json<std::vector<GeneratorJSON>>(config.to_string());
  } catch (const std::exception& e) {
    std::cout << e.what() << "";
  }
  ArrayGenerator generator = baseGenerator(document_count, content);

  std::stringstream ss;
  generator.Value(randSource, ss);
  auto result = jsoncons::json::parse(ss.str());

  return result;
}

}  // namespace arango_bench
