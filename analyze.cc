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
    std::cout << "Current value: " << Evaluate(state, next_player) << std::endl;

    const int depth = 2;
    Move best_move;
    int value = MinimaxSearch(state, depth, best_move);
    std::cout << "Minimax value at depth=" << depth << ": " << value << "; best move: " << FormatMove(best_move) << "." << std::endl;

    for (const Move &move : state.GenerateMoves()) {
      UndoState undo_state;
      state.Execute(move, &undo_state);
      std::cout << "  " << FormatMove(move) << ": " << MinimaxSearch(state, depth - 1, next_player) <<std::endl;
      state.Undo(move, undo_state);
    }
  }
}
