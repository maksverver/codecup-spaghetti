#include "common.h"
#include "game.h"
#include "io.h"
#include "minimax.h"

#include <cstdlib>
#include <iostream>

int main(int argc, char *argv[]) {
  if (argc != 2) {
    std::cout << "Usage: analyze <transcript>" << std::endl;
    return EXIT_FAILURE;
  }

  std::vector<Move> moves;
  if (!ParseMoves(argv[1], moves)) {
    std::cout << "Invalid moves list: [" << argv[1] << "]" << std::endl;
    return EXIT_FAILURE;
  }

  State state;
  for (const Move &move : moves) {
    if (!state.IsValid(move)) {
      std::cout << "Invalid move: [" << FormatMove(move) << "]" << std::endl;
      return EXIT_FAILURE;
    }
    state.Execute(move, nullptr);
  }

  Player next_player = state.NextPlayer();
  std::cout << "Next player: " << FormatPlayer(next_player) << std::endl;
  if (IsRegularPlayer(next_player)) {
    const int max_depth = MaxSearchDepth(state);
    std::cout << "Max depth: " << max_depth << std::endl;
    std::cout << "Values: (" << Evaluate(state, next_player) << ")";
    for (int depth = 1; depth <= max_depth; ++depth) {
      Move best_move;
      int value = MinimaxSearch(state, depth, best_move);
      std::cout << ' ' << FormatMove(best_move) << " (" << value << ")";
    }
    std::cout << std::endl;

    for (const Move &move : state.GenerateMoves()) {
      UndoState undo_state;
      state.Execute(move, &undo_state);
      std::cout << "  " << FormatMove(move) << ":";
      for (int depth = 0; depth <= max_depth; ++depth) {
        std::cout << ' ' << MinimaxSearch(state, depth, next_player);
      }
      std::cout << std::endl;
      state.Undo(move, undo_state);
    }
  }
}
