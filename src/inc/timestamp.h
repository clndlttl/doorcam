#include <iostream>
#include <ctime>
#include <chrono>
#include <vector>
#include <boost/algorithm/string.hpp>

std::string get_timestamp() {
  auto timenow = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
  std::vector<std::string> timevec;
  std::string timestr = ctime(&timenow);
  boost::split( timevec, timestr, boost::is_any_of(" "));
  return timevec[1]+"_"+timevec[2]+"_"+timevec[3];
}
