#ifndef MINIMAX_H_INCLUDED
#define MINIMAX_H_INCLUDED

#include "game.h"

int Evaluate(const State &state, Player target_player);

int MinimaxSearch(State &state, int depth, Player target_player);

int MinimaxSearch(State &state, int depth, Move &best_move);

inline int MaxSearchDepth(const State &state) {
  int moves_left = state.MovesLeft();
  return std::min(static_cast<int>(log(25e6) / log(moves_left*3)), moves_left);
}

#endif  // ndef MINIMAX_H_INCLUDED
