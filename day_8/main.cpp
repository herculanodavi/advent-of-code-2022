#include <algorithm>
#include <charconv>
#include <fstream>
#include <iostream>
#include <numeric>
#include <unordered_set>
#include <variant>
#include <vector>

struct mdview {
  std::vector<int> data;
  size_t rows;
  size_t columns;

  int &operator()(size_t x, size_t y) { return data[x + columns * y]; }

  int operator()(size_t x, size_t y) const { return data[x + columns * y]; }
};

mdview make_forest(std::ifstream &input) {
  std::vector<int> result{};
  std::string line;
  size_t num_lines = 0;
  size_t num_columns = 0;

  while (!input.eof()) {
    getline(input, line);
    num_lines = line.size();
    num_columns++;

    for (char c : line) {
      result.push_back(static_cast<int>(c - '0'));
    }
  }

  return {result, num_lines, num_columns};
}

auto first_question(const mdview &map) -> int {
  mdview visibilities{std::vector<int>(map.rows * map.columns, 0), map.rows,
                      map.columns};

  // se mapa é maior, ele bloqueia todo mundo à esquerda
  for (int j = 0; j < map.rows; j++) {
    int max_row = std::numeric_limits<int>::min();
    for (int i = 0; i < map.columns; i++) {
      if (map(i, j) > max_row) {
        max_row = map(i, j);
        visibilities(i, j)++;
      }
    }
  }

  for (int j = 0; j < map.rows; j++) {
    int max_row = std::numeric_limits<int>::min();
    for (int i = map.columns - 1; i >= 0; --i) {
      if (map(i, j) > max_row) {
        max_row = map(i, j);
        visibilities(i, j)++;
      }
    }
  }

  for (int i = 0; i < map.columns; i++) {
    int max_row = std::numeric_limits<int>::min();
    for (int j = 0; j < map.rows; j++) {
      if (map(i, j) > max_row) {
        max_row = map(i, j);
        visibilities(i, j)++;
      }
    }
  }

  for (int i = 0; i < map.columns; i++) {
    int max_row = std::numeric_limits<int>::min();
    for (int j = map.rows - 1; j >= 0; --j) {
      if (map(i, j) > max_row) {
        max_row = map(i, j);
        visibilities(i, j)++;
      }
    }
  }

  return std::count_if(visibilities.data.cbegin(), visibilities.data.cend(),
                       [](int el) { return el > 0; });
}

auto calc_scenic_score(const mdview &map, size_t x, size_t y) {
  int score_up = [&]() {
    for (int j = y - 1; j >= 0; j--) {
      if (map(x, j) >= map(x, y)) {
        return y - j;
      }
    }
    return y;
  }();

  int score_down = [&]() {
    for (int j = y + 1; j < map.rows; j++) {
      if (map(x, j) >= map(x, y)) {
        return j - y;
      }
    }
    return map.rows - 1 - y;
  }();

  int score_left = [&]() {
    for (int i = x - 1; i >= 0; i--) {
      if (map(i, y) >= map(x, y)) {
        return x - i;
      }
    }
    return x;
  }();

  int score_right = [&]() {
    for (int i = x + 1; i < map.columns; i++) {
      if (map(i, y) >= map(x, y)) {
        return i - x;
      }
    }
    return map.columns - 1 - x;
  }();

  return score_down * score_left * score_right * score_up;
}

auto second_question(const mdview &map) -> int {
  int max_score = -1;
  for (int j = 0; j < map.rows; j++) {
    for (int i = 0; i < map.columns; i++) {
      auto score = calc_scenic_score(map, i, j);
      if (max_score < score) {
        max_score = score;
      }
    }
  };

  return max_score;
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

  auto root = make_forest(infile);
  std::cout << "First question: " << first_question(root) << "\n";
  std::cout << "Second question: " << second_question(root) << "\n";

  return EXIT_SUCCESS;
}