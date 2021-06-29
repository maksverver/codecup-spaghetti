#ifndef GAME_H_INCLUDED
#define GAME_H_INCLUDED

#include "common.h"

constexpr int H = 9;
constexpr int W = 7;
constexpr int RANDOM_TILE_COUNT = 2;

enum Tile { NO_TILE = 0, LEFT = 1, STRAIGHT = 2, RIGHT = 3 };

enum Player { NO_PLAYER = 0, BLUE = 1, RED = 2, RANDOM = 3 };

static_assert(RED == BLUE + 1);

inline Player Other(Player player) {
  assert(player == BLUE || player == RED);
  return Player(RED + BLUE - player);
}


std::string FormatPlayer(Player player);
Player NextPlayer(int moveIndex);

struct Move {
  int row;
  int col;
  Tile tile;
};

struct Path {
  static Path Merged(const Path &p, const Path &q) {
    Path r;
    r.length = p.length + q.length;
    r.connects_side[0] = p.connects_side[0] || q.connects_side[0];
    r.connects_side[1] = p.connects_side[1] || q.connects_side[1];
    r.connects_outside = p.connects_outside || q.connects_outside;
    return r;
  }

  bool ConnectsOutside() const {
    return connects_outside;
  }

  bool ConnectsSide(Player player) const {
    assert(player == BLUE || player == RED);
    return connects_side[player - BLUE];
  }

  int Length() const {
    return length;
  }

private:
  int length = 0;
  bool connects_side[2] = {false, false};  // blue side / red side
  bool connects_outside = false;  // bottom or top
};

struct State {
  int Score(Player player) const  {
    assert(player == BLUE || player == RED);
    return scores[player - BLUE];
  }

  bool IsOccupied(int r, int c) const {
    assert(0 <= r && r < H && 0 <= c && c < W);
    return occupied[r][c];
  }

  Player NextPlayer() const {
    return ::NextPlayer(moves_played);
  }

  void Execute(const Move &m);

private:
  bool occupied[H][W] = {};
  int scores[2] = {0, 0};
  int moves_played = 0;
  std::array<std::array<int, W>, H> path_index = {};
  std::vector<Path> paths = {Path()};
};

bool ParseMove(const std::string &s, Move &move);
std::string FormatMove(const Move &move);

#endif  // GAME_H_INCLUDED
