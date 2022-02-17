#pragma once
#ifdef USING_DLL
#define DECLSPEC __declspec(dllimport)
#else
#ifdef LINUX
#define DECLSPEC __attribute__((visibility("default")))
#else
#ifdef STDCPP
#define DECLSPEC
#else
#define DECLSPEC __declspec(dllexport)
#endif
#endif
#endif
#include <cassert>
#include <chrono>

namespace spgxyz {
/**
 * @brief stopwatch start, stop, read, read... start, stop, read, read... or start, stop, read, resume, stop, read, resume
 *
 */
class DECLSPEC stopwatch {

  bool started;
  bool readyToRead;
  std::chrono::steady_clock::time_point start_t;
  std::chrono::duration<double> elapsed_seconds;

public:
  stopwatch();
  /**
   * @brief starts/resets the timer, calling start on a running timer has no effect
  */
  void start();
  /**
   * @brief stop the timer before a read, or before a restart
  */
  void stop();
  /**
   * @brief resume the timer after a stop,read, no reset
  */
  void resume();

  /**
   * @brief need start, stop calls before read is available
   *
   * @return time in s
   */
  double read();
};
} // namespace spgxyz
