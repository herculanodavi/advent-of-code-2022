#include <algorithm>
#include <fstream>
#include <functional>
#include <iostream>
#include <numeric>
#include <span>
#include <unordered_set>
#include <vector>

int value(char c) {
  char origin;
  if (c <= 'Z') {
    return static_cast<int>(c - 'A') + 27;
  } else {
    return static_cast<int>(c - 'a') + 1;
  }
}

template <typename... T> auto unique_common(T... Sequences) {
  (std::sort(Sequences.begin(), Sequences.end()), ...);
  std::array pointers{Sequences.begin()...};
  std::array ends{Sequences.end()...};

  auto has_any_ended = [&]() {
    for (int i = 0; i < sizeof...(Sequences); i++) {
      if (pointers.at(i) == ends.at(i))
        return true;
    }
    return false;
  };

  auto is_same = [&]() {
    return std::all_of(pointers.begin(), pointers.end(),
                       [&](auto p) { return *p == *pointers.at(0); });
  };

  while (!has_any_ended()) {
    if (is_same()) {
      return *pointers.at(0);
    } else {
      auto el =
          std::min_element(pointers.begin(), pointers.end(),
                           [](auto lhs, auto rhs) { return *lhs < *rhs; });
      std::advance(*el, 1);
    }
  }

  throw;
}

auto first_question(std::ifstream &stream) {
  std::string line;
  int result{};

  while (!stream.eof()) {
    getline(stream, line);
    auto half_size = line.size() / 2;
    std::span first{&line.at(0), half_size};
    std::span second{&line[half_size], half_size};
    result += value(unique_common(first, second));
  }
  return result;
}

auto second_question(std::ifstream &stream) {
  std::string first, second, third;
  int result{};

  while (!stream.eof()) {
    getline(stream, first);
    getline(stream, second);
    getline(stream, third);
    result += value(unique_common(first, second, third));
  }
  return result;
}

int main(int argc, char *argv[]) {
  if (argc != 2) {
    std::cout << "Invalid arguments! Usage is ./main <input_file>.\n";
  }

  // Check input file stream
  std::ifstream infile(argv[1]);
  if (!infile) {
    std::cout << "Invalid input file!\n";
    return EXIT_FAILURE;
  }

  std::cout << "First question: " << first_question(infile) << "\n";

  infile.clear();
  infile.seekg(0, std::ios::beg);

  std::cout << "Second question: " << second_question(infile) << "\n";

  return EXIT_SUCCESS;
}