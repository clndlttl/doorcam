#pragma once

#include <mutex>
#include <ptree.hpp>
#include <json_parser.hpp>

class Config {

  bool quitting = false;
  boost::property_tree::ptree cfg;
  mutable std::mutex mtx;

 public:
  Config(const std::string& filename) {
    std::lock_guard<std::mutex> lock(mtx);
    boost::property_tree::json_parser::read_json(
           filename, cfg);
  }

  
  int getImageWidth() const {
    std::lock_guard<std::mutex> lock(mtx);
    return std::stoi( cfg.get("imgWidth", "320") );
  }

  int getImageHeight() const {
    std::lock_guard<std::mutex> lock(mtx);
    return std::stoi( cfg.get("imgHeight", "240") );
  }

  std::string getMode() const {
    std::lock_guard<std::mutex> lock(mtx);
    return cfg.get("mode", "server");
  }

  float getFPS() const {
    std::lock_guard<std::mutex> lock(mtx);
    return std::stof( cfg.get("fps", "10.0") );
  }

  void quit() {
    std::lock_guard<std::mutex> lock(mtx);
    quitting = true;
  }

  bool isTimeToQuit() { 
    std::lock_guard<std::mutex> lock(mtx);
    return quitting; 
  }
};
