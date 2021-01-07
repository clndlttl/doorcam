#pragma once

#include <unordered_map>
#include <mutex>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

// update flags
#define RESOLUTION 0x1

typedef enum {
    QUIT = 0,
    CLOSE,
    SERVER,
    MOTION,
} Mode;

static std::unordered_map<std::string, Mode> modeCode = {
  {"quit", QUIT},
  {"close", CLOSE},
  {"server", SERVER},
  {"motion", MOTION},
};

class Config {
 
 public:

  Config(const std::string& filename);		 

  void update(uint32_t flags);
  uint32_t updateNeeded();

  int getImageWidth() const;
  void setImageWidth(std::string width);

  int getImageHeight() const;
  void setImageHeight(std::string height);

  int getMode() const;
  void setMode(Mode m);

  float getFPS() const;

  void quit();

  bool isTimeToQuit(); 

 
 private:
  boost::property_tree::ptree cfg;
  mutable std::mutex mtx;
  
  Mode mode;
  uint32_t update_flags;
};
