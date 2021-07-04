#ifndef IO_H_INCLUDED
#define IO_H_INCLUDED

#include "common.h"
#include "game.h"

std::string FormatPlayer(Player player);

bool ParseMove(const std::string_view &s, Move &move);

std::string FormatMove(const Move &move);

bool ParseMoves(const std::string_view &s, std::vector<Move> &moves);

std::string FormatMoves(const std::vector<Move> &moves);

#endif  // ndef IO_H_INCLUDED
