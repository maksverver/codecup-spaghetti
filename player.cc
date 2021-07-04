#include "common.h"
#include "game.h"
#include "minimax.h"
#include "random.h"

#include <iostream>

namespace {

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

Move ParseAndValidateMove(const State &state, const std::string &line) {
  Move move;
  if (!ParseMove(line, move)) {
    std::cerr << "Failed to parse move: [" << line << "]" << std::endl;
    exit(EXIT_FAILURE);
  }
  if (!state.IsValid(move)) {
    std::cerr << "Invalid move: [" << line << "]" << std::endl;
    exit(EXIT_FAILURE);
  }
  return move;
}

}  // namespace

int main() {
  State state;
  Player my_player = NO_PLAYER;
  for (Player next_player; (next_player = state.NextPlayer()) != NO_PLAYER; ) {
    std::string move_string;
    if (next_player == my_player) {
      Move my_move;
      if (0) {
        // Select random move.
        std::vector<Move> moves = state.GenerateMoves();
        my_move = moves[RandInt(moves.size())];
      } else {
        std::cerr << "Value " << MinimaxSearch(state, 2, my_move) << std::endl;
      }
      move_string = FormatMove(my_move);
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
    state.Execute(ParseAndValidateMove(state, move_string), nullptr);
    if (next_player != RANDOM) {
      std::cerr << "Scores " << state.Score(BLUE) << ' ' << state.Score(RED) << "\n";
    }
  }
  std::cerr << "Game is over. Exiting." << std::endl;
}
