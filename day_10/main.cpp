#include <fstream>
#include <iostream>
#include <string_view>
#include <charconv>
#include <variant>

struct noop {
    static constexpr int num_cycles = 1;

    int operator()(int register_value) const {
        return register_value;
    }
};

struct addx {
    static constexpr int num_cycles = 2;
    int value;

    int operator()(int register_value) const {
        return register_value + value;
    }
};

using command_t = std::variant<addx, noop>;
using input_t = std::vector<command_t>;

void execute_instructions(const input_t &commands, auto callback) {
    int num_clocks = 0;
    int register_value = 1;

    for (auto element: commands) {
        std::visit([&]<typename T>(T command) {
            callback(num_clocks, num_clocks + T::num_cycles, register_value);
            register_value = command(register_value);
            num_clocks += T::num_cycles;
        }, element);
    }
}

std::string first_question(const input_t &input) {
    std::vector<int> values{220, 180, 140, 100, 60, 20};
    int sum = 0;

    execute_instructions(input, [&](int clock_from, int clock_to, int register_value) {
        if (values.empty()) {
            return;
        }

        if (clock_to >= values.back()) {
            sum += register_value * values.back();
            values.pop_back();
        }
    });

    return std::to_string(sum);
}

std::string second_question(const input_t &input) {
    execute_instructions(input, [&](int from, int to, int register_value) {
        for(int clock = from; clock < to; clock++) {
            int normalised_clock = clock % 40;

            if(normalised_clock  == 0) {
                printf("\n");
            }

            if(register_value - 1 <= normalised_clock && normalised_clock <= register_value + 1) {
                printf("#");
            } else {
                printf(" ");
            }
        }
    });
    return "";
}

command_t parse(std::string_view line) {
    using namespace std::string_view_literals;

    if (line.starts_with("noop"sv)) {
        return noop{};
    }

    if (line.starts_with("addx"sv)) {
        int value;
        auto result = std::from_chars(line.data() + 5, line.data() + line.size(), value);
        if (result.ec != std::errc()) {
            throw std::invalid_argument("addx argument is invalid");
        }
        return addx{value};
    }

    throw std::invalid_argument("Command could not be parsed");
}

std::vector<command_t> make_input(std::ifstream &input) {
    std::string line;
    std::vector<command_t> result;

    while (!input.eof()) {
        getline(input, line);
        result.emplace_back(parse(line));
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

    auto input = make_input(infile);
    std::cout << "First question: " << first_question(input) << "\n";
    std::cout << "Second question: " << second_question(input) << "\n";

    return EXIT_SUCCESS;
}