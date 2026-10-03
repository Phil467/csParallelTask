#pragma once

#if defined _WIN32 || defined __CYGWIN__
  #ifdef BUILDING_CSPARALLEL_DLL
    #define CS_PARALLEL_TASK_API __declspec(dllexport)
  #else
    #define CS_PARALLEL_TASK_API __declspec(dllimport)
  #endif
#else
  #ifdef BUILDING_CSPARALLEL_DLL
    #define CS_PARALLEL_TASK_API __attribute__ ((visibility ("default")))
  #else
    #define CS_PARALLEL_TASK_API
  #endif
#endif

#ifndef CSPERF_CHECKER_H
#define CSPERF_CHECKER_H

#include <chrono>
#include <iostream>

#define CSTIME_UNIT_HOUR            0
#define CSTIME_UNIT_MINUTE          1
#define CSTIME_UNIT_SECOND          2
#define CSTIME_UNIT_MILLISECOND     3
#define CSTIME_UNIT_MICROSECOND     4
#define CSTIME_UNIT_NANOSECOND      5

class CS_PARALLEL_TASK_API CSPERF_CHECKER
{
  private:
  std::chrono::time_point<std::chrono::high_resolution_clock>
  strt, stp;
  size_t ellapsed;
  int unit = CSTIME_UNIT_MICROSECOND;
  mutable const char*unitName = " microseconds\0";

  public:
/**
 * @brief Constructs a performance checker.
 * @param unit Time unit used for measurements (see CSTIME_UNIT_* constants).
 */
  CSPERF_CHECKER(int unit = CSTIME_UNIT_MICROSECOND);
/**
 * @brief Sets the time unit used for reporting.
 * @param unit Time unit (see CSTIME_UNIT_* constants).
 */
  void setTimeUnit(int unit);
/**
 * @brief Starts measuring execution time.
 */
  void start();
/**
 * @brief Stops measuring execution time and records the elapsed time.
 */
  void stop();
/**
 * @brief Prints the execution time for a code block.
 * @param title Text prefix printed before the measured time.
 */
  void printReport(const char* title);
/**
 * @brief Returns the last measured execution time.
 * @return Execution time in the configured time unit.
 */
  size_t getEllapsedTime();
};

#endif // CSPERF_CHECKER_H
