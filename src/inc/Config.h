#pragma once

#include <unordered_map>
#include <mutex>
#include <ptree.hpp>
#include <json_parser.hpp>

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

  int getImageWidth() const;

  int getImageHeight() const;

  int getMode() const;

  float getFPS() const;

  void quit();

  bool isTimeToQuit(); 

  void setMode(Mode m);
 
 private:
  boost::property_tree::ptree cfg;
  mutable std::mutex mtx;
  Mode mode;
};
