#include "common.h"
#include "game.h"
#include "random.h"

#include <cstring>
#include <cstdlib>
#include <iostream>

// Verifies that undoing a move correctly restores a game state.
bool run_single_undo_test() {
  State state;
  for (Player next_player; (next_player = state.NextPlayer()) != NO_PLAYER; ) {
    // Select random move.
    std::vector<Move> moves = state.GenerateMoves();
    Move move = moves[RandInt(moves.size())];
    UndoState undo_state;
    State old_state = state;
    if (memcmp(&state, &old_state, sizeof(State)) != 0) {
      std::cerr << "State assignment didn't copy all bytes!\n";
      return false;
    }
    state.Execute(move, &undo_state);
    state.Undo(move, undo_state);
    if (memcmp(&state, &old_state, sizeof(State)) != 0) {
      std::cerr << "Undo test failed!\n";
      return false;
    }
    state.Execute(move, nullptr);
  }
  return true;
}

bool run_undo_test() {
  for (int attempt = 0; attempt < 1000; ++attempt) {
    if (!run_single_undo_test()) return false;
  }
  return true;
}

int main() {
  if (!run_undo_test()) return EXIT_FAILURE;
}
