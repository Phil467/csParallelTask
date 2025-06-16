#include "csPERF_MEASUREMENT.h"

csPERF_MEASUREMENT::csPERF_MEASUREMENT(int _unit)
{
    setTimeUnit(_unit);
}

void csPERF_MEASUREMENT::setTimeUnit(int _unit)
{
    unit = _unit;
}

void csPERF_MEASUREMENT::start()
{
   strt = std::chrono::high_resolution_clock::now();
}

void csPERF_MEASUREMENT::stop()
{
  stp = std::chrono::high_resolution_clock::now();
  // Calculer la dur�e en microsecondes (ou autre unit�)

  switch (unit)
  {
  case CSTIME_UNIT_HOUR:
    ellapsed = std::chrono::duration_cast<std::chrono::hours>(stp - strt).count();
    unitName = " hours\0";
    break;
  case CSTIME_UNIT_MINUTE:
    ellapsed = std::chrono::duration_cast<std::chrono::minutes>(stp - strt).count();
    unitName = " minutes\0";
    break;
  case CSTIME_UNIT_SECOND:
    ellapsed = std::chrono::duration_cast<std::chrono::seconds>(stp - strt).count();
    unitName = " seconds\0";
    break;
  case CSTIME_UNIT_MILLISECOND:
    ellapsed = std::chrono::duration_cast<std::chrono::milliseconds>(stp - strt).count();
    unitName = " milliseconds\0";
    break;
  case CSTIME_UNIT_MICROSECOND:
    ellapsed = std::chrono::duration_cast<std::chrono::microseconds>(stp - strt).count();
    unitName = " microseconds\0";
    break;
  case CSTIME_UNIT_NANOSECOND:
    ellapsed = std::chrono::duration_cast<std::chrono::nanoseconds>(stp - strt).count();
    unitName = " nanoseconds\0";
    break;

  default:
    ellapsed = std::chrono::duration_cast<std::chrono::microseconds>(stp - strt).count();
    unitName = " microseconds\0";
    break;
  }

}

void csPERF_MEASUREMENT::printReport(const char* title)
{
  std::cout<< title;
  std::cout << "Execution time : " << ellapsed << unitName << " \n";
}

size_t csPERF_MEASUREMENT::getEllapsedTime()
{
  return ellapsed;
}
