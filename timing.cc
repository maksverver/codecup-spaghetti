#include "common.h"
#include "timing.h"

#include <time.h>

namespace {

timespec GetTime() {
  timespec tp;
  int res = clock_gettime(CLOCK_MONOTONIC, &tp);
  assert(res == 0);
  return tp;
}

static timespec time_start = GetTime();

}  // namespace

#include <iostream>

double TimeElapsed() {
  timespec time_now = GetTime();
  time_t delta_sec = time_now.tv_sec - time_start.tv_sec;
  long delta_nsec = time_now.tv_nsec - time_start.tv_nsec;
  return delta_sec + 1e-9 * delta_nsec;
}
