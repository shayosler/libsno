#include <time_utils.h>

//////////////////////////////////////////////////////////////////////////////

/**
 * @brief Return the current unix time in decimal seconds
 * @return Time since 00:00 UTC on Jan 1 1970
 */
double so::Time_utils::Unix_time()
{
  using namespace std::chrono;
  auto unix_timestamp = std::chrono::seconds(std::time(nullptr));
  size_t unix_us = std::chrono::microseconds(unix_timestamp).count();
  double sec = static_cast<double>(unix_us) / 1.0e6;
  return sec;
}

//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
