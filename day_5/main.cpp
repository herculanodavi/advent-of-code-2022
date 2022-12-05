#include <algorithm>
#include <charconv>
#include <fstream>
#include <iostream>
#include <numeric>
#include <vector>

auto parse_state(std::ifstream &stream) {
  std::string line = "start";
  std::vector<std::string> state_text;

  while (!stream.eof() && line != "") {
    getline(stream, line);
    state_text.push_back(line);
  }

  // last line is blank, and the one before is indices
  // so we can get rid of those
  state_text.pop_back();
  state_text.pop_back();

  // the line size hints the number of stacks
  // crates have width 3 and spaces have width 1
  // so n stacks occupy size 3 * n + (n - 1) -> 4n - 1
  size_t line_size = state_text[0].size();
  size_t number_of_stacks = (line_size + 1) / 4;
  auto position_of_stack = [](size_t idx) { return 4 * idx + 1; };

  std::vector<std::vector<char>> state(number_of_stacks);
  std::for_each(state_text.crbegin(), state_text.crend(),
                [&](const std::string &line) {
                  for (int i = 0; i < number_of_stacks; i++) {
                    char crate = line[position_of_stack(i)];
                    if (crate != ' ') {
                      state[i].push_back(crate);
                    }
                  }
                });
  return state;
}

auto operate_crane(std::ifstream &stream, auto &&handler) {
  std::string line;
  std::string dummy;
  int quantity{};
  size_t origin{};
  size_t target{};

  auto state = parse_state(stream);

  while (!stream.eof()) {
    stream >> dummy >> quantity >> dummy >> origin >> dummy >> target;
    handler(state, quantity, origin - 1, target - 1);
  }
  return std::accumulate(
      state.cbegin(), state.cend(), std::string{},
      [](auto accumulator, auto &stack) { return accumulator + stack.back(); });
}

auto first_question(std::ifstream &stream) {
  return operate_crane(
      stream, [&](auto &state, int quantity, size_t origin, size_t target) {
        for (int i = 0; i < quantity; ++i) {
          char crate = state.at(origin).back();
          if (!state.at(origin).empty()) {
            state.at(origin).pop_back();
            state.at(target).push_back(crate);
          }
        }
      });
}

auto second_question(std::ifstream &stream) {
  return operate_crane(
      stream, [&](auto &state, int quantity, size_t origin, size_t target) {
        auto &o = state.at(origin);
        auto &t = state.at(target);

        t.insert(t.end(), std::next(o.begin(), o.size() - quantity), o.end());
        o.erase(std::next(o.begin(), o.size() - quantity), o.end());
      });
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