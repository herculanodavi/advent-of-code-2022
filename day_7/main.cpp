#include <algorithm>
#include <charconv>
#include <fstream>
#include <iostream>
#include <numeric>
#include <unordered_set>
#include <variant>
#include <vector>

template <class... Ts> struct overloaded : Ts... {
  using Ts::operator()...;
};
template <class... Ts> overloaded(Ts...) -> overloaded<Ts...>;

struct cd {
  std::string_view dir;
};

struct ls {};

struct node {
  std::string name;
  int size{};
  std::vector<node> children{};
  node *parent = nullptr;
};

bool is_directory(const node &target) { return target.size == 0; }

template <typename Callback> int value(const node &target, Callback &&c) {
  auto result = std::accumulate(
      target.children.begin(), target.children.end(), target.size,
      [&](int acc, const auto &child) { return acc + value(child, c); });
  c(target, result);
  return result;
};

std::variant<cd, ls, node> parse_line(const std::string &line) {
  // Check if it's a command or result
  if (line.starts_with("$ cd")) {
    return cd{std::string_view{line.data() + 5, line.size() - 5}};
  } else if (line.starts_with("$ ls")) {
    return ls{};
  } else {
    int size{};
    auto separator = std::find(line.begin(), line.end(), ' ');
    auto separator_idx = std::distance(line.begin(), separator);

    if (line.starts_with("dir")) {
      return node{
          std::string{std::next(line.begin(), separator_idx + 1), line.end()}};
    } else {
      std::from_chars(line.data(), line.data() + separator_idx, size);
      return node{
          std::string{std::next(line.begin(), separator_idx + 1), line.end()},
          size};
    }
  }
}

auto make_tree(std::ifstream &input) {
  std::string line;
  node root{"/"};
  node *p = &root;
  int i = 0;

  while (!input.eof()) {
    i++;
    getline(input, line);
    std::visit(overloaded{
                   [](ls) {},
                   [&](cd command) {
                     if (command.dir == "..") {
                       if (p->parent != nullptr) {
                         p = p->parent;
                       }
                     } else if (command.dir == "/") {
                       p = &root;
                     } else {
                       for (auto &child : p->children) {
                         if (command.dir == child.name) {
                           p = &child;
                         }
                       }
                     }
                   },
                   [&](node descriptor) {
                     p->children.push_back(descriptor);
                     p->children.back().parent = p;
                   },
               },
               parse_line(line));
  }
  return root;
}

auto first_question(const node &root) -> int {
  static constexpr const int SIZE_LIMIT = 100000;
  int sum{};

  value(root, [&](const node &n, int calculated_value) {
    if (calculated_value < SIZE_LIMIT && is_directory(n)) {
      sum += calculated_value;
    }
  });

  return sum;
}

auto second_question(const node &root) -> int {
  static constexpr const int TOTAL_MEMORY = 70000000;
  static constexpr const int TARGET_MEMORY = 30000000;

  std::vector<int> directory_sizes{};
  int total_value = value(root, [&](const node &n, int calculated_value) {
    if (is_directory(n)) {
      directory_sizes.push_back(calculated_value);
    }
  });

  int size_to_delete = TARGET_MEMORY - (TOTAL_MEMORY - total_value);
  int closest_value = std::numeric_limits<int>::max();
  const node *p = &root;

  std::for_each(directory_sizes.begin(), directory_sizes.end(),
                [&](int calculated_value) {
                  if (calculated_value >= size_to_delete &&
                      calculated_value < closest_value) {
                    closest_value = calculated_value;
                  }
                });

  return closest_value;
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

  auto root = make_tree(infile);
  std::cout << "First question: " << first_question(root) << "\n";
  std::cout << "Second question: " << second_question(root) << "\n";

  return EXIT_SUCCESS;
}