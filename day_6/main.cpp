#include <algorithm>
#include <charconv>
#include <fstream>
#include <iostream>
#include <numeric>
#include <unordered_set>
#include <vector>

auto handle_string(std::ifstream &stream, auto &&handler) {
  std::string line;
  getline(stream, line);
  auto result = handler(line);
  stream.clear();
  stream.seekg(0, std::ios::beg);
  return result;
}

auto characters_to_n_different(std::ifstream &stream, int n) {
  return handle_string(stream, [&](std::string &line) -> int {
    auto p = line.begin();
    auto q = line.begin();
    std::unordered_set<char> set{line[0]};

    while (q != line.end()) {
      std::advance(q, 1);
      if (set.contains(*q)) {
        while (*p != *q) {
          set.erase(*p);
          std::advance(p, 1);
        }
        std::advance(p, 1);
      } else {
        set.insert(*q);

        if (set.size() == n) {
          return std::distance(line.begin(), q) + 1;
        }
      }
    }
    return -1;
  });
}

auto first_question(std::ifstream &stream) {
  return characters_to_n_different(stream, 4);
}

auto second_question(std::ifstream &stream) {
  return characters_to_n_different(stream, 14);
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
  std::cout << "Second question: " << second_question(infile) << "\n";

  return EXIT_SUCCESS;
}