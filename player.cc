#include "common.h"
#include "game.h"
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
  if (state.IsOccupied(move.row, move.col)) {
    std::cerr << "Invalid move: [" << line << "]" << std::endl;
    exit(EXIT_FAILURE);
  }
  return move;
}

constexpr int inf = 999999999;

int Evaluate(const State &state, Player target_player) {
  return state.Score(target_player) * 100 + state.Score(Other(target_player));
}

int MinimaxSearch(State &state, int depth, Player target_player) {
  Player next_player;
  if (depth <= 0|| (next_player = state.NextPlayer()) == NO_PLAYER) {
    return Evaluate(state, target_player);
  }
  assert(next_player != RANDOM);
  int best_value = next_player == target_player ? -inf : +inf;
  for (int r = 0; r < H; ++r) {
    for (int c = 0; c < W; ++c) {
      if (!state.IsOccupied(r, c)) {
        for (Tile tile : {LEFT, STRAIGHT, RIGHT}) {
          Move move = {.row = r, .col = c, .tile = tile};
          UndoState undo_state;
          state.Execute(move, &undo_state);
          int value = MinimaxSearch(state, depth - 1, target_player);
          state.Undo(move, undo_state);
          if (next_player == target_player ? value > best_value : value < best_value) {
            best_value = value;
          }
        }
      }
    }
  }
  return best_value;
}

int MinimaxSearch(State &state, int depth, Move &best_move) {
  assert(depth > 0);
  Player next_player = state.NextPlayer();
  assert(next_player == BLUE || next_player == RED);
  int best_value = -inf;
  std::vector<Move> moves = state.GenerateMoves();
  Shuffle(moves);
  for (const Move &move : moves) {
    UndoState undo_state;
    state.Execute(move, &undo_state);
    int value = MinimaxSearch(state, depth - 1, next_player);
    state.Undo(move, undo_state);
    if (value > best_value) {
      best_value = value;
      best_move = move;
    }
  }
  return best_value;
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
