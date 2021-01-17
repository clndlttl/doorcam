#include <iostream>
#include <iomanip>
#include <ctime>
#include <sstream>
#include <boost/algorithm/string.hpp>

std::string get_timestamp() {
    auto t = std::time(nullptr);
    auto tm = *std::localtime(&t);
    std::ostringstream oss;
    oss << std::put_time(&tm, "%d-%m-%Y %H-%M-%S");
    
    std::string daytime_str = oss.str();
    std::vector<std::string> daytime_vec;
    
    boost::split(daytime_vec, daytime_str, boost::is_any_of(" "));
    if (daytime_vec.size() == 2) {
      return daytime_vec[0]+"_"+daytime_vec[1];
    } else {
      return std::string("unknown");
    }
}
