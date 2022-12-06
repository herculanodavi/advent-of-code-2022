#include <algorithm>
#include <fstream>
#include <iostream>
#include <numeric>
#include <unordered_set>

/*
 * Returns the number of characters processed until a substring
 * with <n> different characters is found. Returns -1 if no such substring
 * is found.
 */
auto characters_to_n_different(const std::string &line, int n) -> int {
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
}

auto first_question(const std::string &line) -> int {
  return characters_to_n_different(line, 4);
}

auto second_question(const std::string &line) -> int {
  return characters_to_n_different(line, 14);
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

  std::string line;
  getline(infile, line);

  std::cout << "First question: " << first_question(line) << "\n";
  std::cout << "Second question: " << second_question(line) << "\n";

  return EXIT_SUCCESS;
}