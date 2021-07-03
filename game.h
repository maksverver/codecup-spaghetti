#ifndef GAME_H_INCLUDED
#define GAME_H_INCLUDED

#include "common.h"

constexpr int H = 9;
constexpr int W = 7;
constexpr int RANDOM_TILE_COUNT = 2;
constexpr int CYCLE_SCORE = -5;
constexpr int LOOPBACK_SCORE = -3;
constexpr int VERTEX_COUNT = (H + 1)*W + (W + 1)*H;
constexpr int MAX_MOVE_COUNT = H*W*3;

enum Tile { NO_TILE = 0, LEFT = 1, STRAIGHT = 2, RIGHT = 3 };

enum Player { NO_PLAYER = 0, BLUE = 1, RED = 2, RANDOM = 3 };

static_assert(RED == BLUE + 1);

inline Player Other(Player player) {
  assert(player == BLUE || player == RED);
  return Player(RED + BLUE - player);
}

inline int PlayerIndex(Player player) {
  assert(player == BLUE || player == RED);
  return player - BLUE;
}

std::string FormatPlayer(Player player);
Player NextPlayer(int moveIndex);

struct Move {
  int row;
  int col;
  Tile tile;
};

class State;

struct UndoState {
  int a, b, c, d, score;
};

class State {
public:
  State() {
    for (int v = 0; v < VERTEX_COUNT; ++v) {
      path_index[v] = Path{.other_end = v, .length = 0};
    }
  }

  int Score(Player player) const  {
    return scores[PlayerIndex(player)];
  }

  bool IsOccupied(int r, int c) const {
    assert(0 <= r && r < H && 0 <= c && c < W);
    return occupied[r][c];
  }

  Player NextPlayer() const {
    return ::NextPlayer(moves_played);
  }

  int GenerateMoves(Move (&moves)[MAX_MOVE_COUNT]);
  std::vector<Move> GenerateMoves();

  void Execute(const Move &m, UndoState *undo_state);
  void Undo(const Move &m, const UndoState &undo_state);

private:
  struct Path {
    int other_end;
    int length;
  };

  int Connect(int a, int b, Player player);
  void Disconnect(int a, int b);

  bool occupied[H][W] = {};
  int scores[2] = {50, 50};
  int moves_played = 0;
  Path path_index[VERTEX_COUNT];
};

bool ParseMove(const std::string &s, Move &move);
std::string FormatMove(const Move &move);

#endif  // GAME_H_INCLUDED
