#include <fstream>
#include <iostream>
#include <charconv>
#include <unordered_set>

using input_t = std::vector<std::pair<char, std::size_t>>;

struct knot_state_t {
    std::int16_t dx = 0;
    std::int16_t dy = 0;

    bool operator==(const knot_state_t &other) const {
        return other.dx == dx && other.dy == dy;
    }
};

namespace std {
    template<>
    struct hash<knot_state_t> {
        size_t operator()(knot_state_t const &state) const noexcept {
            return (state.dx << 16) | state.dy;
        }
    };
}

template<std::size_t NumKnots>
struct rope_state_t {
    std::array<knot_state_t, NumKnots> knots{};
};

knot_state_t move_head(knot_state_t state, char step) {
    switch (step) {
        case 'U':
            state.dy++;
            break;
        case 'D':
            state.dy--;
            break;
        case 'L':
            state.dx--;
            break;
        case 'R':
            state.dx++;
            break;
        default:
            throw std::invalid_argument("Command letter not recognized");
    }
    return state;
}

knot_state_t follow(knot_state_t head, knot_state_t tail) {
    auto delta = [](int x0, int x1) -> std::int16_t {
        return x0 > x1 ? -1 : 1;
    };

    if (std::abs(head.dx - tail.dx) < 2 && std::abs(head.dy - tail.dy) < 2) {
        return tail;
    }

    if (std::abs(head.dx - tail.dx) > 1) {
        head.dx += delta(head.dx, tail.dx);
    }
    if (std::abs(head.dy - tail.dy) > 1) {
        head.dy += delta(head.dy, tail.dy);
    }
    return head;
}

template<std::size_t NumKnots>
rope_state_t<NumKnots> next_step(rope_state_t<NumKnots> state, char step) {
    auto head = state.knots.begin();
    auto tail = std::next(head);

    *head = move_head(*head, step);
    while (tail != std::end(state.knots)) {
        *tail = follow(*head, *tail);
        head = tail++;
    }

    return state;
}

template<std::size_t NumKnots>
void draw_rope(rope_state_t<NumKnots> rope) {
    constexpr std::int16_t grid_x = 12;
    constexpr std::int16_t grid_y = 12;

    for (std::int16_t y = grid_y; y > 0; y--) {
        for (std::int16_t x = 0; x < grid_x; x++) {
            auto position = knot_state_t{static_cast<int16_t>(x - grid_x / 2), static_cast<int16_t>(y - grid_y / 2)};
            auto element = std::find(rope.knots.cbegin(), rope.knots.cend(), position);
            if (element != rope.knots.cend()) {
                std::cout << std::to_string(std::distance(rope.knots.cbegin(), element));
            } else if (x == grid_x / 2 && y == grid_y / 2) {
                std::cout << "s";
            } else {
                std::cout << ".";
            }

        }
        std::cout << "\n";
    }
}

std::string first_question(const input_t &input) {
    rope_state_t<2> rope;
    std::unordered_set<knot_state_t> visited;

    for (auto &[command, repetitions]: input) {
        for (std::size_t i = 0; i < repetitions; i++) {
            rope = next_step(rope, command);
            visited.insert(rope.knots.back());
        }
    }

    return std::to_string(visited.size());
}

std::string second_question(const input_t &input) {
    rope_state_t<10> rope;
    std::unordered_set<knot_state_t> visited;

    for (auto &[command, repetitions]: input) {
        for (std::size_t i = 0; i < repetitions; i++) {
            rope = next_step(rope, command);
            visited.insert(rope.knots.back());
        }
    }

    return std::to_string(visited.size());
}


input_t make_input(std::ifstream &input) {
    std::string line;
    input_t result;

    while (!input.eof()) {
        getline(input, line);
        std::size_t value;
        const auto res = std::from_chars(line.data() + 2, line.data() + line.size(), value);
        if (res.ec != std::errc()) {
            throw std::invalid_argument("Failed to parse input.");
        }
        result.emplace_back(std::make_pair(line[0], value));
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