#include <Config.h>

Config::Config(const std::string& filename) :
    update_flags(0x0) {		 
    std::lock_guard<std::mutex> lock(mtx);
    boost::property_tree::json_parser::read_json(
           filename, cfg);

    // get mode from json spec, default to server mode
    std::string modestr = cfg.get("mode", "server");
    
    if (modeCode.find(modestr) != modeCode.end()) {
      mode = modeCode[modestr];
    }
}

void Config::update(uint32_t flags) {
  std::lock_guard<std::mutex> lock(mtx);
  update_flags |= flags;
}

uint32_t Config::updateNeeded() {
  std::lock_guard<std::mutex> lock(mtx);
  uint32_t temp = update_flags;
  update_flags = 0x0;
  return temp;
}

int Config::getImageWidth() const {
    std::lock_guard<std::mutex> lock(mtx);
    return std::stoi( cfg.get("imgWidth", "320") );
}

void Config::setImageWidth(std::string width) {
    std::lock_guard<std::mutex> lock(mtx);
    cfg.put("imgWidth", width);
}

int Config::getImageHeight() const {
    std::lock_guard<std::mutex> lock(mtx);
    return std::stoi( cfg.get("imgHeight", "240") );
}

void Config::setImageHeight(std::string height) {
    std::lock_guard<std::mutex> lock(mtx);
    cfg.put("imgHeight", height);
}

int Config::getMode() const {
    std::lock_guard<std::mutex> lock(mtx);
    return mode;
}

float Config::getFPS() const {
    std::lock_guard<std::mutex> lock(mtx);
    return std::stof( cfg.get("fps", "10.0") );
}

void Config::quit() {
    std::lock_guard<std::mutex> lock(mtx);
    mode = QUIT;
}

bool Config::isTimeToQuit() { 
    std::lock_guard<std::mutex> lock(mtx);
    return mode == QUIT; 
}

void Config::setMode(Mode m) {
    std::lock_guard<std::mutex> lock(mtx);
    mode = m; 
}
