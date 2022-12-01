#include <fstream>
#include <iostream>
#include <queue>
#include <string>
#include <type_traits>

template <typename ResultContainer, typename UpdateStrategy,
          typename ResultStrategy>
auto for_each_elf(std::ifstream &stream, ResultContainer &&container,
                  UpdateStrategy &&elf_handler,
                  ResultStrategy &&result_handler) {
  std::string line;
  int total_calories = 0;

  while (!stream.eof()) {
    getline(stream, line);
    if (line.empty()) {
      elf_handler(total_calories, container);
      total_calories = 0;
    } else {
      total_calories += std::stoi(line);
    }
  }

  return result_handler(container);
}

int first_question(std::ifstream &stream) {
  std::string line;
  int current_calories = 0;
  int max_calories = 0;

  auto handle_empty = [&current_calories, &max_calories] {
    if (current_calories > max_calories) {
      max_calories = current_calories;
    }
    current_calories = 0;
  };

  auto handle_food = [&current_calories](int calories) {
    current_calories += calories;
  };

  while (!stream.eof()) {
    getline(stream, line);

    if (line.empty()) {
      handle_empty();
    } else {
      handle_food(std::stoi(line));
    }
  }

  return max_calories;
}

auto second_question(std::ifstream &stream) {
  std::string line;
  int current_calories = 0;
  std::priority_queue<int, std::vector<int>, std::greater<int>> top_three;

  auto handle_empty = [&current_calories, &top_three] {
    top_three.push(current_calories);
    if (top_three.size() > 3) {
      top_three.pop();
    }
    current_calories = 0;
  };

  auto handle_food = [&current_calories](int calories) {
    current_calories += calories;
  };

  auto reduce = [](decltype(top_three) &priority_queue) {
    decltype(top_three)::value_type sum{};

    while (!priority_queue.empty()) {
      sum += priority_queue.top();
      priority_queue.pop();
    }
    return sum;
  };

  while (!stream.eof()) {
    getline(stream, line);

    if (line.empty()) {
      handle_empty();
    } else {
      handle_food(std::stoi(line));
    }
  }

  return reduce(top_three);
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

  // First question
  std::cout << "First question: " << first_question(infile) << "\n";

  // Reset input file stream
  infile.clear();
  infile.seekg(0, std::ios::beg);

  // Second question
  std::cout << "Second question: " << second_question(infile) << "\n";

  // Reset input file stream
  infile.clear();
  infile.seekg(0, std::ios::beg);

  // First question as generic code
  std::cout << "First question -- generic version: "
            << for_each_elf(
                   infile, int{0},
                   [](auto calories, auto &max_container) {
                     if (calories > max_container) {
                       max_container = calories;
                     }
                   },
                   [](auto &max_container) -> int { return max_container; })
            << "\n";

  // Reset input file stream
  infile.clear();
  infile.seekg(0, std::ios::beg);

  // Second question as generic code
  std::cout << "Second question -- generic version: "
            << for_each_elf(
                   infile,
                   std::priority_queue<int, std::vector<int>,
                                       std::greater<int>>{},
                   [](int calories, auto &max_container) {
                     max_container.push(calories);
                     if (max_container.size() > 3) {
                       max_container.pop();
                     }
                   },
                   [](auto &max_container) -> int {
                     int sum{};
                     while (!max_container.empty()) {
                       sum += max_container.top();
                       max_container.pop();
                     }
                     return sum;
                   })
            << "\n";

  return EXIT_SUCCESS;
}
