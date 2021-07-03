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

void State::Execute(const Move &m, UndoState *undo_state) {
  assert(!IsOccupied(m.row, m.col));
  const Player player = NextPlayer();
  occupied[m.row][m.col] = true;
  ++moves_played;

  int a = (2*W + 1)*m.row + m.col;  // top
  int b = a + (2*W + 1);            // bottom
  int c = a + W;                    // left
  int d = c + 1;                    // right
  switch (m.tile) {
    case LEFT:
      std::swap(b, c);
      // top--left, bottom--right
      break;
    case STRAIGHT:
      // top--bottom, left--right
      break;
    case RIGHT:
      // top--right, left--bottom
      std::swap(b, d);
      break;
    case NO_TILE:
      assert(false);
  }
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
  // Important to not write Connect(...) + Connect(...) because we need these
  // method calls to happen in the correct order!
  int score = 0;
  score += Connect(a, b, player);
  score += Connect(c, d, player);
  if (player == RANDOM) {
    score = 0;
  } else {
    scores[PlayerIndex(player)] += score;
  }

  if (undo_state != nullptr) {
    undo_state->a = a;
    undo_state->b = b;
    undo_state->c = c;
    undo_state->d = d;
    undo_state->score = score;
  }
}

void State::Undo(const Move &m, const UndoState &undo_state) {
  assert(moves_played > 0);
  --moves_played;
  assert(IsOccupied(m.row, m.col));
  occupied[m.row][m.col] = false;
  if (undo_state.score != 0) {
    scores[PlayerIndex(NextPlayer())] -= undo_state.score;
  }
  // Undo connections in reverse order. This matters when the two connections
  // created a single path.
  Disconnect(undo_state.c, undo_state.d);
  Disconnect(undo_state.a, undo_state.b);
}

int State::Connect(int a, int b, Player player) {
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
    // Connecting two different paths.
    assert(far_b != a);
    Player side_a = PlayerSide(far_a);
    Player side_b = PlayerSide(far_b);
    if (side_a != NO_PLAYER && side_b != NO_PLAYER) {
      if (side_a == side_b) {
        // Path loops back to same side.
        score = LOOPBACK_SCORE;
      } else if (side_a == player) {
        // Winning path created! Path a connect to player's side.
        assert(side_b == Other(player));
        score = path_a.length + 1;
      } else {
        // Winning path created! Path b connect to player's side.
        assert(side_b == player && side_a == Other(player));
        score = path_b.length + 1;
      }
    } else {
      assert(path_a.length == path_index[far_a].length);
      assert(path_b.length == path_index[far_b].length);
      // Maybe optimize: if far_a or far_b connect to the top/bottom edge, we
      // don't have to update the path index. But is checking this cheaper than
      // just updating it unconditionally? It might help if we only store the
      // real assignments in the UndoState, so undoing becomes cheaper.
      path_index[far_a].other_end = far_b;
      path_index[far_b].other_end = far_a;
      path_index[far_a].length = path_index[far_b].length = path_a.length + path_b.length + 1;
    }
  }
  return score;
}

void State::Disconnect(int a, int b) {
  // The logic here is more complicated than I'd like. It could be
  // simplified by modeling each singleton vertex as a pair over vertices
  // (i.e. for a horiozontal vertex, a left and right side). That would
  // remove a lot of special cases here. Otherwise, we might just store
  // the assignments done in Connect() explicitly, which takes slightly
  // more space in the UndoState, but makes undoing easier.
  assert(a != b);
  int far_a = path_index[a].other_end;
  int far_b = path_index[b].other_end;
  if (a == far_b && b == far_a) {
    assert(path_index[a].length == path_index[b].length);
    if (path_index[a].length > 1) {
      // This move created a cycle originally; nothing to undo.
    } else {
      // a--b  (a and b were both singletons)
      path_index[a].other_end = a;
      path_index[b].other_end = b;
      path_index[a].length = 0;
      path_index[b].length = 0;
    }
  } else if (far_a == far_b) {
    // One of a or b was a singleton
    if (b == path_index[far_a].other_end) {
      // far_a--a--b  (b was a singleton)
      assert(far_a != a);
      path_index[far_a].other_end = a;
      path_index[far_a].length = path_index[a].length;
      path_index[b].other_end = b;
      path_index[b].length = 0;
    } else {
      assert(a == path_index[far_a].other_end);
      // a--b--far_b  (a was a singleton)
      assert(far_b != b);
      path_index[far_b].other_end = b;
      path_index[far_b].length = path_index[b].length;
      path_index[a].other_end = a;
      path_index[a].length = 0;
    }
  } else {
    // Optional: early-out that detects the points were not connected. This
    // allows the assertion below to pass.
    //if (path_index[far_a].other_end == a && path_index[far_b].other_end == b) {
    //  return;
    //}
    // This assertion doesn't pass because there are cases where the connection
    // wasn't actually made (e.g., when creating a loopback or winning path).
    //assert(a != far_a && a != far_b && b != far_a && b != far_b);
    int a_length = path_index[a].length;
    int b_length = path_index[b].length;
    path_index[far_a].other_end = a;
    path_index[far_b].other_end = b;
    path_index[far_a].length = a_length;
    path_index[far_b].length = b_length;
  }
}

int State::GenerateMoves(Move (&moves)[MAX_MOVE_COUNT]) {
  int n = 0;
  for (int r = 0; r < H; ++r) {
    for (int c = 0; c < W; ++c) {
      if (!IsOccupied(r, c)) {
        for (Tile tile : {LEFT, STRAIGHT, RIGHT}) {
          moves[n++] = Move{.row = r, .col = c, .tile = tile};
        }
      }
    }
  }
  return n;
}

std::vector<Move> State::GenerateMoves() {
  Move moves[MAX_MOVE_COUNT];
  return std::vector<Move>(&moves[0], &moves[GenerateMoves(moves)]);
}
