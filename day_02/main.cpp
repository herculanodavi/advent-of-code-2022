#include <fstream>
#include <iostream>

#include "game_rules.hpp"

template <typename... Args> using Fun = void (*)(Args...);

template <typename ResultContainer, typename Lhs, typename Rhs>
auto for_each_match(std::ifstream &stream, ResultContainer &&container,
                    Fun<ResultContainer &, Lhs, Rhs> handler) {
  std::string line;
  while (!stream.eof()) {
    getline(stream, line);
    handler(container, game::convert<Lhs>(line[0]),
            game::convert<Rhs>(line[2]));
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

  std::cout << "First question: " << [&infile]() {
    return for_each_match<int, game::Play, game::Play>(
        infile, int{},
        [](int &container, game::Play lhs, game::Play rhs) -> void {
          container += game::value(rhs);
          container += game::score(lhs, rhs);
        });
  }() << "\n";

  // Reset input file stream
  infile.clear();
  infile.seekg(0, std::ios::beg);

  std::cout << "Second question: " << [&infile]() {
    return for_each_match<int, game::Play, game::Result>(
        infile, int{},
        [](int &container, game::Play lhs, game::Result result) -> void {
          game::Play rhs = game::match_play(lhs, result);
          container += game::value(rhs);
          container += game::score(lhs, rhs);
        });
  }() << "\n";

  return EXIT_SUCCESS;
}