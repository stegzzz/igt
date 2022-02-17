#include "stopwatch.h"
namespace spgxyz {

stopwatch::stopwatch() : started(false), readyToRead(false) {}

void stopwatch::start() {
  if (!started) {
    start_t = std::chrono::steady_clock::now();
    started = true;
    readyToRead = false;
  }
}

void stopwatch::resume() {
    started = true;
    readyToRead = false;
}

void stopwatch::stop() {
  if (started) {
    auto end = std::chrono::steady_clock::now();
    elapsed_seconds = end - start_t;
    started = false;
    readyToRead = true;
  }
}

double stopwatch::read() {
  assert(readyToRead && "reading stopwatch without start/stop completed");
  return elapsed_seconds.count();
}
} // namespace spgxyz
