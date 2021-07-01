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

Player PlayerSide(int v) {
  assert(v >= 0 && v < VERTEX_COUNT);
  unsigned short u = static_cast<unsigned short>(v) % (2*W + 1);
  if (u == W) return BLUE;
  if (u == 2*W) return RED;
  return NO_PLAYER;
}

/*
// Returns whether the vertex v connects to the player's side of the board
// (i.e., left for BLUE, right for RED).
//
// Possible performance optimization: memoize this in table to avoid division?
bool VertexOnPlayerSide(int v, Player player) {
  static_assert(BLUE == 1);
  static_assert(RED == 2);
  assert(v >= 0 && v < VERTEX_COUNT);
  return static_cast<unsigned short>(v) % (2*W + 1) == W*player;
}

// Returns whether the vertex is at the top or bottom side of the grid.
bool VertexOnOutside(int v) {
  assert(v >= 0 && v < VERTEX_COUNT);
  return v < W || v >= H*(2*W + 1);
}
*/

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

  int top    = (2*W + 1)*m.row + m.col;
  int left   = (2*W + 1)*m.row + W + m.col;
  int right  = (2*W + 1)*m.row + W + m.col + 1;
  int bottom = (2*W + 1)*(m.row + 1) + m.col;
  switch (m.tile) {
    case LEFT:
      Connect(left, top, right, bottom, player);
      break;
    case STRAIGHT:
      Connect(left, right, top, bottom, player);
      break;
    case RIGHT:
      Connect(left, bottom, right, top, player);
      break;
    case NO_TILE:
      assert(false);
  }
}

void State::Connect(int a, int b, int c, int d, Player player) {
  // We will connect vertex a to b, and c to d. If these connect independent
  // paths, we can make the connections in any order. There is one case where
  // order matters: if the two pairs together create a single path from
  // left-to-right, e.g.:  L----a==b----c==d----R. Then, we need to make sure
  // that the connection closest to the player's side of the board is made
  // first, so that player maximizes its score.
  if (PlayerSide(c) == player || PlayerSide(d) == player) {
    std::swap(a, c);
    std::swap(b, d);
  }
  Connect(a, b, player);
  Connect(c, d, player);
}

void State::Connect(int a, int b, Player player) {
  Path &path_a = path_index[a];
  Path &path_b = path_index[b];
  int far_a = path_a.other_end;
  int far_b = path_b.other_end;
  assert(far_a != far_b);
  int score = 0;
  if (far_a == b) {
    // Cycle created!
    assert(far_b == a);
    assert(path_a.length == path_b.length);
    score = CYCLE_SCORE;
  } else {
    // Connecting two separate paths.
    assert(far_b != a);
    Player side_a = PlayerSide(far_a);
    Player side_b = PlayerSide(far_b);
    if (side_a != NO_PLAYER && side_b != NO_PLAYER) {
      if (side_a == side_b) {
        // Path loops back to same side.
        score = LOOPBACK_SCORE;
      } else if (side_a == player) {
        assert(side_b == Other(player));
        score = path_a.length + 1;
      } else {
        assert(side_b == player && side_a == Other(player));
        score = path_b.length + 1;
      }
    } else {
      // Maybe optimize: if far_a or far_b connect to the top/bottom edge, we
      // don't have to update the path index. But is checking this cheaper than
      // just updating it unconditionally? Maybe it reduces undo state?
      path_index[far_a].other_end = far_b;
      path_index[far_b].other_end = far_a;
      path_index[far_a].length = path_index[far_b].length = path_a.length + path_b.length + 1;
    }
  }
  if (player != RANDOM) {
    scores[PlayerIndex(player)] += score;
  }
}
