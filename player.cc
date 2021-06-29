#include "common.h"
#include "game.h"

#include <iostream>
#include <random>

namespace {

// TODO: support seed from command line to make player deterministic?
std::mt19937 CreateRng() {
  std::random_device r;
  std::seed_seq s{r(), r(), r(), r(), r(), r(), r(), r()};
  return std::mt19937(s);
}

std::mt19937 rng = CreateRng();

std::string GetLine() {
  std::string line;
  if (!std::getline(std::cin, line)) {
    std::cerr << "Failed to read move!" << std::endl;
    exit(EXIT_FAILURE);
  }
  if (line == "Quit") {
    std::cerr << "Quit received. Exiting." << std::endl;
    exit(EXIT_SUCCESS);
  }
  return line;
}

// Returns a random integer between 0 (inclusive) and limit (exclusive).
int RandInt(int limit) {
  assert(limit > 0);
  std::uniform_int_distribution<int> dist(0, limit - 1);
  return dist(rng);
}

Move ParseAndValidateMove(const State &state, const std::string &line) {
  Move move;
  if (!ParseMove(line, move)) {
    std::cerr << "Failed to parse move: [" << line << "]" << std::endl;
    exit(EXIT_FAILURE);
  }
  if (state.IsOccupied(move.row, move.col)) {
    std::cerr << "Invalid move: [" << line << "]" << std::endl;
    exit(EXIT_FAILURE);
  }
  return move;
}

Move SelectRandomMove(const State &state) {
  std::vector<Move> moves;
  for (int r = 0; r < H; ++r) {
    for (int c = 0; c < W; ++c) {
      if (!state.IsOccupied(r, c)) {
        for (Tile tile : {LEFT, STRAIGHT, RIGHT}) {
          moves.push_back({r, c, tile});
        }
      }
    }
  }
  return moves[RandInt(moves.size())];
}

}  // namespace

int main() {
  State state;
  Player my_player = NO_PLAYER;
  for (Player next_player; (next_player = state.NextPlayer()) != NO_PLAYER; ) {
    std::string move_string;
    if (next_player == my_player) {
      move_string = FormatMove(SelectRandomMove(state));
      std::cerr << "Sent [" << move_string << "]" << std::endl;
      std::cout << move_string << std::endl;
    } else {
      move_string = GetLine();
      if (my_player == NO_PLAYER && next_player != RANDOM) {
        if (move_string == "Start") {
          my_player = next_player;
          std::cerr << "I am player " << my_player << std::endl;
          continue;
        } else {
          my_player = Other(next_player);
          std::cerr << "I am player " << my_player << std::endl;
        }
      }
      std::cerr << "Received [" << move_string << "]" << std::endl;
    }
    state.Execute(ParseAndValidateMove(state, move_string));
    std::cerr << "Scores " << state.Score(BLUE) << '-' << state.Score(RED) << "\n";
  }
  std::cerr << "Game is over. Exiting." << std::endl;
}
