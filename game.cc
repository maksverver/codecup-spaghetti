#include "game.h"

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

}  // namespace

Player NextPlayer(int moveIndex) {
  if (moveIndex >= H * W) return NO_PLAYER;
  if (moveIndex < RANDOM_TILE_COUNT) return RANDOM;
  return Player(((moveIndex - RANDOM_TILE_COUNT) & 1) + BLUE);
}

bool ParseMove(const std::string &s, Move &move) {
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

void State::Execute(const Move &m) {
  assert(!IsOccupied(m.row, m.col));
  const Player player = NextPlayer();
  occupied[m.row][m.col] = true;
  ++moves_played;
  // TODO: connect paths, update scores!
}
