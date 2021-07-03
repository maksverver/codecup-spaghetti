#ifndef RANDOM_H_INCLUDED
#define RANDOM_H_INCLUDED

#include "common.h"

#include <random>

// TODO: support seed from command line to make player deterministic?
// Note: access is not threadsafe!
extern std::mt19937 rng;

// Returns a random integer between 0 (inclusive) and limit (exclusive).
inline int RandInt(int limit) {
  assert(limit > 0);
  std::uniform_int_distribution<int> dist(0, limit - 1);
  return dist(rng);
}

template<class T>
void Shuffle(std::vector<T> &v) {
  for (int n = v.size(); n > 1; --n) {
    int i = RandInt(n);
    if (i < n - 1) std::swap(v[i], v[n - 1]);
  }
}

#endif  // ndef RANDOM_H_INCLUDED
