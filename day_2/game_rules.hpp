#pragma once

namespace game {
enum Play { Rock = 0, Paper, Scissors, NumPlays };
enum Result { Lose = 0, Draw = 3, Win = 6 };

template <typename T> T convert(char c) { return T{}; }

template <> Play convert(char c) {
  if (c == 'A' || c == 'X')
    return Play::Rock;
  if (c == 'B' || c == 'Y')
    return Play::Paper;
  if (c == 'C' || c == 'Z')
    return Play::Scissors;
  throw;
}

template <> Result convert(char c) {
  switch (c) {
  case 'X':
    return Result::Lose;
  case 'Y':
    return Result::Draw;
  case 'Z':
    return Result::Win;
  default:
    throw;
  }
}

// Returns the play that needs to be played against p
// to get the result r
Play match_play(Play p, Result r) {
  switch (r) {
  case Result::Draw:
    return p;
  case Result::Win:
    return static_cast<Play>((p + 1) % NumPlays);
  case Result::Lose:
    return static_cast<Play>((p + 2) % NumPlays);
  }
}

// Returns score for rhs
int score(Play lhs, Play rhs) {
  if (lhs == rhs)
    return 3;

  int beats_rhs = match_play(rhs, Result::Win);
  if (lhs == beats_rhs)
    return 0;

  return 6;
}

int value(Play p) { return p + 1; }

} // namespace game
