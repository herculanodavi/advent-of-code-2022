#include <fstream>
#include <iostream>
#include <queue>
#include <string>
#include <type_traits>

template <typename ResultContainer, typename UpdateStrategy>
auto for_each_elf(std::ifstream &stream, ResultContainer &&container,
                  UpdateStrategy &&elf_handler) {
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

  return container;
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

  // First question as generic code
  std::cout << "First question: " << [&infile]() {
    return for_each_elf(infile, int{},
                        [](int calories, auto &container) -> void {
                          if (calories > container) {
                            container = calories;
                          }
                        });
  }() << "\n";

  // Reset input file stream
  infile.clear();
  infile.seekg(0, std::ios::beg);

  // Second question as generic code
  std::cout << "Second question: " << [&infile]() {
    using ValueType = int;
    ValueType sum{};
    auto queue =
        for_each_elf(infile,
                     std::priority_queue<ValueType, std::vector<ValueType>,
                                         std::greater<ValueType>>{},
                     [](int calories, auto &container) -> void {
                       container.push(calories);
                       if (container.size() > 3) {
                         container.pop();
                       }
                     });

    while (!queue.empty()) {
      sum += queue.top();
      queue.pop();
    }
    return sum;
  }() << "\n";

  return EXIT_SUCCESS;
}
