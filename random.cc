#include "random.h"

namespace {

std::mt19937 CreateRng() {
  std::random_device r;
  std::seed_seq s{r(), r(), r(), r(), r(), r(), r(), r()};
  return std::mt19937(s);
}

}  // namespace

std::mt19937 rng = CreateRng();
