#include <algorithm>
#include <charconv>
#include <fstream>
#include <iostream>

struct range {
  int begin;
  int end;
};

bool contains(const range &lhs, const range &rhs) {
  return (lhs.begin >= rhs.begin) && (lhs.end <= rhs.end);
}

bool intersects(const range &lhs, const range &rhs) {
  return lhs.end >= rhs.begin && lhs.begin <= rhs.end;
}

auto parse(std::string_view line) {
  auto parse_range = [](auto begin, auto end) {
    range r{};
    auto lhs_result = std::from_chars(begin, end, r.begin);
    auto rhs_result = std::from_chars(std::next(lhs_result.ptr, 1), end, r.end);
    return r;
  };

  auto comma = std::find(std::begin(line), std::end(line), ',');
  auto r1 = parse_range(line.begin(), comma);
  auto r2 = parse_range(std::next(comma, 1), line.end());
  return std::make_pair(r1, r2);
}

template <typename Criteria>
auto count_range_pair(std::ifstream &stream, Criteria &&criteria) {
  std::string line;
  int count{};

  while (!stream.eof()) {
    getline(stream, line);
    auto [r1, r2] = parse(line);
    count += criteria(r1, r2);
  }
  return count;
}

int first_question(std::ifstream &infile) {
  return count_range_pair(infile, [](range &r1, range &r2) {
    return contains(r1, r2) || contains(r2, r1);
  });
}

int second_question(std::ifstream &infile) {
  return count_range_pair(infile, intersects);
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