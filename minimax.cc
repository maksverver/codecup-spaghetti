#include "common.h"
#include "minimax.h"
#include "random.h"

constexpr int inf = 999999999;

int Evaluate(const State &state, Player target_player) {
  return state.Score(target_player) * 100 +
      (100 - state.Score(Other(target_player)));
}

int MinimaxSearch(State &state, int depth, Player target_player) {
  Player next_player;
  if (depth <= 0|| (next_player = state.NextPlayer()) == NO_PLAYER) {
    return Evaluate(state, target_player);
  }
  assert(IsRegularPlayer(next_player));
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
  assert(IsRegularPlayer(next_player));
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
