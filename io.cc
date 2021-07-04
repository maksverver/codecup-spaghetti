#include "common.h"
#include "io.h"

namespace {

char TileToChar(Tile tile) {
  switch (tile) {
    case LEFT:
      return 'l';
    case STRAIGHT:
      return 's';
    case RIGHT:
      return 'r';
    default:
      return '?';
  }
}

Tile TileFromChar(char ch) {
  switch (ch) {
    case 'l':
      return LEFT;
    case 's':
      return STRAIGHT;
    case 'r':
      return RIGHT;
    default:
      return NO_TILE;
  }
}

}  // namespace;

std::string FormatPlayer(Player player) {
  switch (player) {
    case NO_PLAYER:
      return "NO_PLAYER";
    case BLUE:
      return "BLUE";
    case RED:
      return "RED";
    case RANDOM:
      return "RANDOM";
  }
  assert(false);
  return "";
}


bool ParseMove(const std::string_view &s, Move &move) {
  if (s.size() != 3) return false;
  int row = s[0] - 'a';
  if (row < 0 || row >= H) return false;
  int col = s[1] - 'a';
  if (col < 0 || col >= W) return false;
  Tile tile = TileFromChar(s[2]);
  if (tile == NO_TILE) return false;
  move.row = s[0] - 'a';
  move.col = s[1] - 'a';
  move.tile = tile;
  return true;
}

std::string FormatMove(const Move &move) {
  char buf[4];
  buf[0] = 'a' + move.row;
  buf[1] = 'a' + move.col;
  buf[2] = TileToChar(move.tile);
  buf[3] = '\0';
  return std::string(buf);
}

bool ParseMoves(const std::string_view &s, std::vector<Move> &moves) {
  std::vector<Move> result;
  if (!s.empty()) {
    if (s.size() % 4 != 3) return false;
    int n = (s.size() + 1)/4;
    result.resize(n);
    for (int i = 0; i < n; ++i) {
      if (i != 0 && s[4*i - 1] != ',') return false;
      if (!ParseMove(std::string_view(&s[4*i], 3), result[i])) return false;
    }
  }
  moves.swap(result);
  return true;
}

std::string FormatMoves(const std::vector<Move> &moves) {
  std::string s;
  if (!moves.empty()) {
    s.reserve(moves.size()*4 - 1);
    s += FormatMove(moves[0]);
    for (int i = 1; i < moves.size(); ++i) {
      s += ',';
      s += FormatMove(moves[i]);
    }
  }
  return s;
}
