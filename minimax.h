#ifndef MINIMAX_H_INCLUDED
#define MINIMAX_H_INCLUDED

#include "game.h"

int Evaluate(const State &state, Player target_player);

int MinimaxSearch(State &state, int depth, Player target_player);

int MinimaxSearch(State &state, int depth, Move &best_move);

#endif  // ndef MINIMAX_H_INCLUDED
